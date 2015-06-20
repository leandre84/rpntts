<?php
namespace Rpntts\Model;

use Zend\Db\TableGateway\TableGateway;

class BookingTable
{
    protected $tableGateway;
    
    public function __construct(TableGateway $tableGateway)
    {
        $this->tableGateway = $tableGateway;
    }

    public function fetchAll()
    {
        $resultSet = $this->tableGateway->select();
        
        return $resultSet;
    }

    public function getBookingsMatchingUserId($id)
    {
        $id  = (int) $id;
        $rowSet = $this->tableGateway->select(array('user_fk' => $id));
        $row = $rowSet->current();
        if (!$row) {
            throw new \Exception('Keine Buchungen für Benutzerkennung '  . $id);
        }
        
        $rowSet = $this->tableGateway->select(array('user_fk' => $id));
        
        /* $dbAdapter = $this->tableGateway->getAdapter();
        $query = "SELECT * FROM booking WHERE user_fk = " . $id . " ORDER BY timestamp DESC";
        $statement = $dbAdapter->query($query);
        $rowSet = $this->tableGateway->select($statement); */   
        
        return $rowSet;
    }
    
    public function getBookingMatchingBookingId($id)
    {
        $id  = (int) $id;
        $rowSet = $this->tableGateway->select(array('pk' => $id));
        $row = $rowSet->current();
        if (!$row) {
            throw new \Exception('Keine Buchung für Buchungs-ID ' . $id);
        }
        
        return $row;
    }

    public function saveBooking(Booking $booking)
    {
        $data = array(
            'pk' => $booking->primaryKey,
            'timestamp' => $booking->timeStamp,
            'type' => $booking->type,
            'text' => $booking->text,
            'user_fk' => $booking->userForeignKey,
        );
        
        $id = (int) $booking->primaryKey;
        if ($id != NULL) {
            $this->deleteBooking($id);
            $data['pk'] = NULL;
        }
   
        if ($this->isHolidayBooking($data['type'])) {
            // Format date from booking for stored procedure
            $date = $data['timestamp'];
            $date = preg_replace('/^(.*)\s(.*:.*)$/isum', '\1', $date);
            $date = date('Y-m-d', strtotime($date));
            $date = str_replace('-', '', $date);
            
            // Call stored procedure for holiday booking
            $dbAdapter = $this->tableGateway->getAdapter();
            $stmt = $dbAdapter->createStatement();
            $stmt->prepare('CALL rpntts_holiday_booking(?, ?, ?, ?)');
            $stmt->getResource()->bindParam(1, $data['user_fk'], \PDO::PARAM_INT);
            $stmt->getResource()->bindParam(2, $date, \PDO::PARAM_STR); 
            $stmt->getResource()->bindParam(3, $data['type'], \PDO::PARAM_STR); 
            $stmt->getResource()->bindParam(4, $data['text'], \PDO::PARAM_STR); 
            $stmt->execute();
        } else {
            $this->tableGateway->insert($data);
            
            // Call stored procedure for updating global saldo
            $dbAdapter = $this->tableGateway->getAdapter();
            $stmt = $dbAdapter->createStatement();
            $stmt->prepare('CALL rpntts_update_global_saldo()');
            $stmt->execute();     
        }
    }

    public function deleteBooking($id)
    {
        // Call stored procedure for delete booking
        $dbAdapter = $this->tableGateway->getAdapter();
        $stmt = $dbAdapter->createStatement();
        $stmt->prepare('CALL rpntts_delete_booking(?)');
        $stmt->getResource()->bindParam(1, $id, \PDO::PARAM_INT); 
        $stmt->execute();
    }
    
    private function isHolidayBooking($type)
    {
        return strpos($type, 'U') !== FALSE || strpos($type, 'UH') !== FALSE;
    }
 }
 