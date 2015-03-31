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
        $this->primaryKey = (!empty($data['pk'])) ? $data['pk'] : NULL;
        $this->timeStamp = (!empty($data['timestamp'])) ? $data['timestamp'] : NULL;
        $this->type = (!empty($data['type'])) ? $data['type'] : NULL;
        $this->text = (!empty($data['text'])) ? $data['text'] : NULL;
        $this->userForeignKey = (!empty($data['user_fk'])) ? $data['user_fk'] : NULL;
    }
}