<?php
namespace Rpntts\Model;

class TimeModel
{
    public $primaryKey;
    public $modelName;
    public $monday;
    public $tuesday;
    public $wednesday;
    public $thursday;
    public $friday;
    public $saturday;
    public $sunday;

    public function exchangeArray($data)
    {
        $this->primaryKey = (isset($data['primaryKey'])) ? $data['primaryKey'] : NULL;
        $this->modelName = (isset($data['modelName'])) ? $data['modelName'] : NULL;
        $this->monday = (isset($data['monday'])) ? $data['monday'] : NULL;
        $this->tuesday = (isset($data['tuesday'])) ? $data['tuesday'] : NULL;
        $this->wednesday = (isset($data['wednesday'])) ? $data['wednesday'] : NULL;
        $this->thursday = (isset($data['thursday'])) ? $data['thursday'] : NULL;
        $this->friday = (isset($data['friday'])) ? $data['friday'] : NULL;
        $this->saturday = (isset($data['saturday'])) ? $data['saturday'] : NULL;
        $this->sunday = (isset($data['sunday'])) ? $data['sunday'] : NULL;
    }
}
