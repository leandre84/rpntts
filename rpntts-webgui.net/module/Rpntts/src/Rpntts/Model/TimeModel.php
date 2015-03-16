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
        $this->primaryKey = (!empty($data['primaryKey'])) ? $data['primaryKey'] : NULL;
        $this->modelName = (!empty($data['modelName'])) ? $data['modelName'] : NULL;
        $this->monday = (!empty($data['monday'])) ? $data['monday'] : NULL;
        $this->tuesday = (!empty($data['tuesday'])) ? $data['tuesday'] : NULL;
        $this->wednesday = (!empty($data['wednesday'])) ? $data['wednesday'] : NULL;
        $this->thursday = (!empty($data['thursday'])) ? $data['thursday'] : NULL;
        $this->friday = (!empty($data['friday'])) ? $data['friday'] : NULL;
        $this->saturday = (!empty($data['saturday'])) ? $data['saturday'] : NULL;
        $this->sunday = (!empty($data['sunday'])) ? $data['sunday'] : NULL;
    }
}
