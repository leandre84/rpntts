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

    public function getBooking($id)
    {
        $id  = (int) $id;
        $rowset = $this->tableGateway->select(array('primaryKey' => $id));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row "  . $id);
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
        } else {
            if ($this->getBooking($id)) {
                $this->tableGateway->update($data, array('pk' => $id));
            } else {
                throw new \Exception("Booking id " . $id . " does not exist");
            }
        }
    }

    public function deleteBooking($id)
    {
        $this->tableGateway->delete(array('pk' => (int) $id));
    }
 }
 