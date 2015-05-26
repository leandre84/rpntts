<?php
namespace Rpntts\Model;

class User
{
    public $primaryKey;
    public $userName;
    public $passWord;
    public $staffNumber;
    public $firstName;
    public $lastName;
    public $active;
    public $timeBalance;
    public $timeModelForeignKey;

    public function exchangeArray($data)
    {
        $this->primaryKey = (isset($data['pk'])) ? $data['pk'] : NULL;
        $this->userName = (isset($data['username'])) ? $data['username'] : NULL;
        $this->passWord = (isset($data['password'])) ? $data['password'] : NULL;
        $this->staffNumber = (isset($data['persno'])) ? $data['persno'] : NULL;
        $this->firstName = (isset($data['firstname'])) ? $data['firstname'] : NULL;
        $this->lastName = (isset($data['lastname'])) ? $data['lastname'] : NULL;
        $this->active = (isset($data['active'])) ? $data['active'] : NULL;
        $this->timeBalance = (isset($data['timebalance'])) ? $data['timebalance'] : NULL;
        $this->timeModelForeignKey = (isset($data['timemodel_fk'])) ? $data['timemodel_fk'] : NULL;
    }
}
