<?php
namespace Rpntts\Model;

use Zend\InputFilter\InputFilter;
use Zend\InputFilter\InputFilterAwareInterface;
use Zend\InputFilter\InputFilterInterface;

class Booking implements InputFilterAwareInterface
{
    public $primaryKey;
    public $timeStamp;
    public $type;
    public $text;
    public $userForeignKey;
    protected $inputFilter;

    public function exchangeArray($data)
    {
        $this->primaryKey = (!empty($data['pk'])) ? $data['pk'] : NULL;
        $this->timeStamp = (!empty($data['timestamp'])) ? $data['timestamp'] : NULL;
        $this->type = (!empty($data['type'])) ? $data['type'] : NULL;
        $this->text = (!empty($data['text'])) ? $data['text'] : NULL;
        $this->userForeignKey = (!empty($data['user_fk'])) ? $data['user_fk'] : NULL;
    }
    
    public function setInputFilter(InputFilterInterface $inputFilter)
    {
        throw new \Exception("Not used");
    }

    public function getInputFilter()
    {
        if (!$this->inputFilter) {
            $inputFilter = new InputFilter();

            $inputFilter->add(array(
                'name'     => 'timeStamp',
                'required' => true,
                'filters'  => array(
                    array('name' => 'StripTags'),
                    array('name' => 'StringTrim'),
                ),
                'validators' => array(
                    array(
                        'name'    => 'StringLength',
                        'options' => array(
                            'encoding' => 'UTF-8',
                            'min'      => 10,
                            'max'      => 19,
                        ),
                    ),
                ),
            ));

            $inputFilter->add(array(
                'name'     => 'type',
                'required' => true,
                'filters'  => array(
                    array('name' => 'StripTags'),
                    array('name' => 'StringTrim'),
                ),
                'validators' => array(
                    array(
                        'name'    => 'StringLength',
                        'options' => array(
                            'encoding' => 'UTF-8',
                            'min'      => 2,
                            'max'      => 2,
                        ),
                    ),
                ),
            ));
            
            $inputFilter->add(array(
                'name'     => 'text',
                'required' => true,
                'filters'  => array(
                    array('name' => 'StripTags'),
                    array('name' => 'StringTrim'),
                ),
                'validators' => array(
                    array(
                        'name'    => 'StringLength',
                        'options' => array(
                            'encoding' => 'UTF-8',
                            'min'      => 1,
                            'max'      => 100,
                        ),
                    ),
                ),
            ));

            $this->inputFilter = $inputFilter;
        }

        return $this->inputFilter;
    }
}
