<?php
namespace Rpntts\Controller;

use Rpntts\Model\Booking;
use Rpntts\Form\BookingForm;
use Rpntts\Form\LoginForm;
use Zend\Mvc\Controller\AbstractActionController;
use Zend\View\Model\ViewModel;

class RpnttsController extends AbstractActionController
{
    protected $timeModelTable;
    protected $userTable;
    protected $cardTable;
    protected $bookingTable;
    
    public function getTimeModelTable()
    {
        if (!$this->timeModelTable) {
            $sm = $this->getServiceLocator();
            $this->timeModelTable = $sm->get('Rpntts\Model\TimeModelTable');
        }
        return $this->timeModelTable;
    }
    
    public function getUserTable()
    {
        if (!$this->userTable) {
            $sm = $this->getServiceLocator();
            $this->userTable = $sm->get('Rpntts\Model\UserTable');
        }
        return $this->userTable;
    }
    
    public function getCardTable()
    {
        if (!$this->cardTable) {
            $sm = $this->getServiceLocator();
            $this->cardTable = $sm->get('Rpntts\Model\CardTable');
        }
        return $this->cardTable;
    }
    
    public function getBookingTable()
    {
        if (!$this->bookingTable) {
            $sm = $this->getServiceLocator();
            $this->bookingTable = $sm->get('Rpntts\Model\BookingTable');
        }
        return $this->bookingTable;
    }
    
    /* public function loginAction()
    {
        return new ViewModel(array(
        'user' => $this->getUserTable()->fetchAll(),
        ));
    } */
	
	public function loginAction()
    {
        /* $allUsers = $this->getUserTable()->fetchAll();
		foreach ($allUsers as $user) {
			var_dump($user);
		} */
		
		$form = new LoginForm();
        $form->get('submit')->setValue('Anmelden');

        $request = $this->getRequest();
        if ($request->isPost()) {
			#$form->setInputFilter($user->getInputFilter());
            var_dump(get_object_vars($form->setData($request->getPost())));
            
			if ($form->isValid()) {
				#$user->exchangeArray($form->getData());
				var_dump($form->getData());
				$allUsers = $this->getUserTable()->fetchAll();
				#var_dump($allUsers);

				// Redirect to list of bookings
				#return $this->redirect()->toRoute('booking');
			}
        }
        return array('form' => $form);
    }

        
    public function indexAction()
    {
        return new ViewModel(array(
        'bookings' => $this->getBookingTable()->fetchAll(),
        ));
    }

    public function addAction()
    {
        $form = new BookingForm();
        $form->get('submit')->setValue('Hinzufügen');

        $request = $this->getRequest();
        if ($request->isPost()) {
            $booking = new Booking();
            $form->setInputFilter($booking->getInputFilter());
            $form->setData($request->getPost());

            if ($form->isValid()) {
                $booking->exchangeArray($form->getData());
                $this->getBookingTable()->saveBooking($booking);

                // Redirect to list of bookings
                return $this->redirect()->toRoute('booking');
            }
        }
        return array('form' => $form);
    }

    public function editAction()
    {
        $id = (int) $this->params()->fromRoute('id', 0);
        if (!$id) {
            return $this->redirect()->toRoute('booking', array(
                'action' => 'add'
            ));
        }

        // Get the Booking with the specified id. An exception is thrown
        // if it cannot be found, in which case go to the index page.
        try {
            $booking = $this->getBookingTable()->getBooking($id);
        }
        catch (\Exception $ex) {
            return $this->redirect()->toRoute('booking', array(
                'action' => 'index'
            ));
        }

        $form  = new BookingForm();
        $form->bind($booking);
        $form->get('submit')->setAttribute('value', 'Edit');

        $request = $this->getRequest();
        if ($request->isPost()) {
            $form->setInputFilter($booking->getInputFilter());
            $form->setData($request->getPost());

            if ($form->isValid()) {
                $this->getBookingTable()->saveBooking($booking);

                // Redirect to list of bookings
                return $this->redirect()->toRoute('booking');
            }
        }

        return array(
            'id' => $id,
            'form' => $form,
        );
    }
    
   public function deleteAction()
   {
        $id = (int) $this->params()->fromRoute('id', 0);
        if (!$id) {
            return $this->redirect()->toRoute('booking');
        }
        
        $request = $this->getRequest();
        if ($request->isPost()) {
            $del = $request->getPost('del', 'No');

            if ($del == 'Yes') {
                $id = (int) $request->getPost('id');
                $this->getBookingTable()->deleteBooking($id);
            }

            // Redirect to list of bookings
            return $this->redirect()->toRoute('booking');
        }

        return array(
            'id'    => $id,
            'booking' => $this->getBookingTable()->getBooking($id)
        );
    }
}

