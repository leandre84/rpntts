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
        $this->primaryKey = (!empty($data['primaryKey'])) ? $data['primaryKey'] : NULL;
        $this->userName = (!empty($data['userName'])) ? $data['userName'] : NULL;
        $this->passWord = (!empty($data['passWord'])) ? $data['passWord'] : NULL;
        $this->staffNumber = (!empty($data['staffNumber'])) ? $data['staffNumber'] : NULL;
        $this->firstName = (!empty($data['firstName'])) ? $data['firstName'] : NULL;
        $this->lastName = (!empty($data['lastName'])) ? $data['lastName'] : NULL;
        $this->active = (!empty($data['active'])) ? $data['active'] : NULL;
        $this->timeBalance = (!empty($data['timeBalance'])) ? $data['timeBalance'] : NULL;
        $this->timeModelForeignKey = (!empty($data['timeModelForeignKey'])) ? $data['timeModelForeignKey'] : NULL;
    }
}
