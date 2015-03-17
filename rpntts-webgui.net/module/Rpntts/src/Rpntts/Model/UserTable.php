<?php
namespace Rpntts\Model;

use Zend\Db\TableGateway\TableGateway;

class UserTable
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

    public function getUser($id)
    {
        $id  = (int) $id;
        $rowset = $this->tableGateway->select(array('primaryKey' => $id));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row "  . $id);
        }
        
        return $row;
    }

    public function saveUser(User $user)
    {
        $data = array(
            'primaryKey' => $user->primaryKey,
            'userName' => $user->userName,
            'passWord' => $user->passWord,
            'staffNumber' => $user->staffNumber,
            'firstName' => $user->firstName,
            'lastName' => $user->lastName,
            'active' => $user->active,
            'timeBalance' => $user->timeBalance,
            'timeModelForeignKey' => $user->timeModelForeignKey,
        );

        $id = (int) $user->primaryKey;
        if ($id == 0) {
            $this->tableGateway->insert($data);
        } else {
            if ($this->getUser($id)) {
                $this->tableGateway->update($data, array('primaryKey' => $id));
            } else {
                throw new \Exception("User id " . $id . " does not exist");
            }
        }
    }

    public function deleteUser($id)
    {
        $this->tableGateway->delete(array('primaryKey' => (int) $id));
    }
 }
 