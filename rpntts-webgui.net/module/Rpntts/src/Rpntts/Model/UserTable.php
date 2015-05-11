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
    
    public function getUserMatchingNameAndPassword($userName, $passWord)
    {
		$rowset = $this->tableGateway->select(array('username' => $userName, 'password' => $passWord));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row for user name " . $userName);
        }
        
        return $row;
    }

    public function fetchAll()
    {
        $resultSet = $this->tableGateway->select();
        
        return $resultSet;
    }

    public function getUser($id)
    {
        $id  = (int) $id;
        $rowset = $this->tableGateway->select(array('pk' => $id));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row "  . $id);
        }
        
        return $row;
    }

    public function saveUser(User $user)
    {
        $data = array(
            'pk' => $user->primaryKey,
            'username' => $user->userName,
            'password' => $user->passWord,
            'persno' => $user->staffNumber,
            'firstname' => $user->firstName,
            'lastname' => $user->lastName,
            'active' => $user->active,
            'timebalance' => $user->timeBalance,
            'timemodel_fk' => $user->timeModelForeignKey,
        );

        $id = (int) $user->primaryKey;
        if ($id == 0) {
            $this->tableGateway->insert($data);
        } else {
            if ($this->getUser($id)) {
                $this->tableGateway->update($data, array('pk' => $id));
            } else {
                throw new \Exception("User id " . $id . " does not exist");
            }
        }
    }

    public function deleteUser($id)
    {
        $this->tableGateway->delete(array('pk' => (int) $id));
    }
 }
 