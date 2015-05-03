<?php
namespace Rpntts\Form;

use Zend\Form\Element\Password;
use Zend\Form\Element\Submit;
use Zend\Form\Element\Text;
use Zend\Form\Form;

class LoginForm extends Form
{
    public function __construct()
    {
        parent::__construct('login');
		
		$nameElement = new Text('name');
		$nameElement->setLabel('Benutzername');
		$passElement = new Password('pass');
		$passElement->setLabel('Passwort');
		$submitElement = new Submit('login');
		$submitElement->setValue('Anmelden');
		$submitElement->setAttribute('class', 'btn');
		
		$this->add($nameElement);
		$this->add($passElement);
		$this->add($submitElement);
	}
}