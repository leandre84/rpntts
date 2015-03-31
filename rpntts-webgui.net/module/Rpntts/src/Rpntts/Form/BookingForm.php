<?php
namespace Rpntts\Form;

use Zend\Form\Form;

class BookingForm extends Form
{
    public function __construct($name = null)
    {
        // ignore the name passed
        parent::__construct('booking');

        $this->add(array(
            'name' => 'id',
            'type' => 'Hidden',
        ));
        $this->add(array(
            'name' => 'timeStamp',
            'type' => 'Text',
            'options' => array(
                'label' => 'Datum/Uhrzeit',
            ),
        ));
        $this->add(array(
            'name' => 'type',
            'type' => 'Text',
            'options' => array(
                'label' => 'Buchungstyp',
            ),
        ));
        $this->add(array(
            'name' => 'text',
            'type' => 'Text',
            'options' => array(
                'label' => 'Buchungstext',
            ),
        ));
        $this->add(array(
            'name' => 'submit',
            'type' => 'Submit',
            'attributes' => array(
                'value' => 'Go',
                'id' => 'submitbutton',
            ),
        ));
    }
}
