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
        if ($id == 0) {
            $this->tableGateway->insert($data);
            // Typen: A = Standard, U = Urlaub
            // Nach Insert global_update() aufrufen
            // Vorher Überprüfung auf Urlaub, wenn ja Urlaubs-Procedure
            // Update deaktvieren
        } else {
            if ($this->getBookingMatchingBookingId($id)) {
                $this->tableGateway->update($data, array('pk' => $id));
            } else {
                throw new \Exception("Buchung mit der ID " . $id . " existiert nicht.");
            }
        }
    }

    public function deleteBooking($id)
    {
		// Löschen über Procedure rpntts_delete_booking (Param booking PK)
        var_dump($id);
        $this->tableGateway->delete(array('pk' => (int) $id));
    }
 }
 