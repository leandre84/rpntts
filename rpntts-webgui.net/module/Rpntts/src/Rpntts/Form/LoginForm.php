<?php
namespace Rpntts\Form;

use Zend\Form\Form;

class LoginForm extends Form
{
    public function init()
    {
        $this->setName("login");
        $this->setMethod('post');

        $this->addElement('text', 'username', array(
            'filters' => array('StringTrim'),
            'validators' => array(
            array('StringLength', FALSE, array(0, 50)),
            ),
            'required' => TRUE,
            'label' => 'Username:',
        ));

        $this->addElement('password', 'password', array(
            'filters' => array('StringTrim'),
            'validators' => array(
            array('StringLength', FALSE, array(0, 50)),
            ),
            'required' => TRUE,
            'label' => 'Password:',
        ));

        $this->addElement('submit', 'login', array(
            'required' => FALSE,
            'ignore' => TRUE,
            'label' => 'Login',
        ));
    }
}