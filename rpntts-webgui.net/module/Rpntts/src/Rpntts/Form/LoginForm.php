<?php
namespace Rpntts\Form;

use Zend\Form\Element\Password;
use Zend\Form\Element\Submit;
use Zend\Form\Element\Text;
use Zend\Form\Form;

class LoginForm extends Form
{
    public function __construct($name = NULL)
    {
        // ignore the name passed
        parent::__construct('login');

        $this->add(array(
            'name' => 'userName',
            'type' => 'Text',
            'options' => array(
                'label' => 'Benutzername',
            ),
        ));
        $this->add(array(
            'name' => 'passWord',
            'type' => 'Text',
            'options' => array(
                'label' => 'Passwort',
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