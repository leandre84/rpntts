<?php
namespace Rpntts\Form;

use Zend\Form\Element\Password;
use Zend\Form\Element\Submit;
use Zend\Form\Element\Text;
use Zend\Form\Form;

class LogoutForm extends Form
{
    public function __construct()
    {
        parent::__construct('logout');
		
		$submitElement = new Submit('logout');
		$submitElement->setValue('Abmelden');
		$submitElement->setAttribute('class', 'btn');
		
		$this->add($submitElement);
	}
}