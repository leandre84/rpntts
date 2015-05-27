<?php
namespace Rpntts\Form;

use Zend\Form\Form;
use Zend\Form\Element;

class BookingForm extends Form
{
    public function __construct($name = NULL)
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
                'label' => 'Datum/Uhrzeit: ',
            ),
        ));
        $this->add(array(
            'name' => 'type',
            'type' => 'Zend\Form\Element\Select',
            'options' => array(
                'label' => 'Buchungstyp: ',
                'options' => array(
                             'A' => 'A',
                             'U' => 'U',
                )
            ),
        ));
        $this->add(array(
            'name' => 'text',
            'type' => 'Text',
            'options' => array(
                'label' => 'Buchungstext: ',
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
