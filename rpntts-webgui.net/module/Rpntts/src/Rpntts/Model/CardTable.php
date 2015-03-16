<?php
namespace Rpntts\Model;

use Zend\Db\TableGateway\TableGateway;

class CardTable
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

    public function getCard($id)
    {
        $id  = (int) $id;
        $rowset = $this->tableGateway->select(array('cardUid' => $id));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row "  . $id);
        }
        
        return $row;
    }

    public function saveCard(Card $card)
    {
        $data = array(
            'cardUid' => $card->cardUid,
            'text' => $card->text,
            'userForeignKey' => $card->userForeignKey,
            'active' => $card->active,
        );

        $id = (int) $card->cardUid;
        if ($id == 0) {
            $this->tableGateway->insert($data);
        } else {
            if ($this->getCard($id)) {
                $this->tableGateway->update($data, array('cardUid' => $id));
            } else {
                throw new \Exception("Card id " . $id . " does not exist");
            }
        }
    }

    public function deleteCard($id)
    {
        $this->tableGateway->delete(array('cardUid' => (int) $id));
    }
 }
 