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
            'type' => 'Zend\Form\Element\DateTime',
            'name' => 'timeStamp',
            'options' => array(
                    'label' => 'Datum/Uhrzeit: ',
                    'format' => 'd.m.Y H:i'
            ),
            /* 'attributes' => array(
                    'min' => '01.01.2010 00:00:00',
                    'max' => '01.01.2020 00:00:00',
                    'step' => '1', // minutes; default step interval is 1 min
            ) */
        ));
        $this->add(array(
            'name' => 'type',
            'type' => 'Zend\Form\Element\Select',
            'options' => array(
                'label' => 'Buchungstyp: ',
                'options' => array(
                             'A' => 'A',
                             'U' => 'U',
                             'UH' => 'UH',
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
