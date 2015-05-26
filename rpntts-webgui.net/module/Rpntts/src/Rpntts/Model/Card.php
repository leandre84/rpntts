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
        $this->cardUid = (isset($data['cardUid'])) ? $data['cardUid'] : NULL;
        $this->text  = (isset($data['text'])) ? $data['text'] : NULL;
        $this->userForeignKey  = (isset($data['userForeignKey'])) ? $data['userForeignKey'] : NULL;
        $this->active  = (isset($data['active'])) ? $data['active'] : NULL;
    }
}
