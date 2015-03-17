<?php
namespace Rpntts\Model;

class Booking
{
    public $primaryKey;
    public $timeStamp;
    public $type;
    public $text;
    public $userForeignKey;

    public function exchangeArray($data)
    {
        $this->primaryKey = (!empty($data['primaryKey'])) ? $data['primaryKey'] : NULL;
        $this->timeStamp = (!empty($data['timeStamp'])) ? $data['timeStamp'] : NULL;
        $this->type = (!empty($data['type'])) ? $data['type'] : NULL;
        $this->text = (!empty($data['text'])) ? $data['text'] : NULL;
        $this->userForeignKey = (!empty($data['userForeignKey'])) ? $data['userForeignKey'] : NULL;
    }
}