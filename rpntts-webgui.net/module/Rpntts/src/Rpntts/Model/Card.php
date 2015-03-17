<?php
namespace Rpntts\Model;

class Card
{
    public $cardUid;
    public $text;
    public $userForeignKey;
    public $active;

    public function exchangeArray($data)
    {
        $this->cardUid = (!empty($data['cardUid'])) ? $data['cardUid'] : NULL;
        $this->text  = (!empty($data['text'])) ? $data['text'] : NULL;
        $this->userForeignKey  = (!empty($data['userForeignKey'])) ? $data['userForeignKey'] : NULL;
        $this->active  = (!empty($data['active'])) ? $data['active'] : NULL;
    }
}
