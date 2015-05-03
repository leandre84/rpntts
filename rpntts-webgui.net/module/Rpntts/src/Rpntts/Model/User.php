<?php
namespace Rpntts\Model;

class User
{
    public $primaryKey;
    public $userName;
    public $passWord;
    public $staffNumber;
    public $firstName;
    public $lastName;
    public $active;
    public $timeBalance;
    public $timeModelForeignKey;

    public function exchangeArray($data)
    {
        $this->primaryKey = (!empty($data['pk'])) ? $data['pk'] : NULL;
        $this->userName = (!empty($data['username'])) ? $data['username'] : NULL;
        $this->passWord = (!empty($data['password'])) ? $data['password'] : NULL;
        $this->staffNumber = (!empty($data['persno'])) ? $data['persno'] : NULL;
        $this->firstName = (!empty($data['firstname'])) ? $data['firstname'] : NULL;
        $this->lastName = (!empty($data['lastname'])) ? $data['lastname'] : NULL;
        $this->active = (!empty($data['active'])) ? $data['active'] : NULL;
        $this->timeBalance = (!empty($data['timebalance'])) ? $data['timebalance'] : NULL;
        $this->timeModelForeignKey = (!empty($data['timeModel_fk'])) ? $data['timeModel_fk'] : NULL;
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
                'name'     => 'userName',
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
            
            $inputFilter->add(array(
                'name'     => 'passWord',
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
