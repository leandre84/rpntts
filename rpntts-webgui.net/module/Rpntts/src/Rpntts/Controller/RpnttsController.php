<?php
namespace Rpntts\Controller;

use Rpntts\Model\Booking;
use Rpntts\Form\BookingForm;
use Rpntts\Form\LoginForm;
use Zend\Mvc\Controller\AbstractActionController;
use Zend\Session\Container;
use Zend\View\Model\ViewModel;

class RpnttsController extends AbstractActionController
{
    protected $timeModelTable;
    protected $userTable;
    protected $cardTable;
    protected $bookingTable;
    private $errorMessage;
    private $successMessage;

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

    public function loginAction()
    {     
		$form = new LoginForm();
        $form->get('submit')->setValue('Anmelden');

        $request = $this->getRequest();
        if ($request->isPost()) {
            $form->setData($request->getPost());

            // TODO: user auf active prüfen
            if ($form->isValid()) {
                $formContent = $form->getData();
                $clearTextPass = $formContent['passWord'];
                $userNameFromForm = $formContent['userName'];
                $hashPass = hash('sha256', $clearTextPass);
                try {
                    $user = $this->getUserTable()->getUserMatchingNameAndPassword($userNameFromForm, $hashPass);
                } catch (\Exception $e) {
                    return $this->redirect()->toRoute('loginerror');
                }
                $user_session = new Container('user');
                $user_session->userName = $user->userName;
                $user_session->userPrimaryKey = $user->primaryKey;
                
                return $this->redirect()->toRoute('booking');
            }
        }
        return array('form' => $form);
    }

        
    public function bookingAction()
    {
		$user_session = new Container('user');
		
        $bookings = [];
        try {
            $user_session = new Container('user');
            $bookings = $this->getBookingTable()->getBookingsMatchingUserId($user_session->userPrimaryKey);
			$user_session->errorMessage = '';
        } catch (\Exception $e) {
			$user_session->successMessage = '';
            $user_session->errorMessage = $e->getMessage();
        }
        
        return new ViewModel(array(
        'bookings' => $bookings,
        'errorMessage' => $user_session->errorMessage,
        'successMessage' => $user_session->successMessage,
        'userName' => $user_session->userName,
        'id' => $user_session->userPrimaryKey,
        ));
    }
    
    public function logoutAction()
    {
        // Unset user session
        $user_session = new Container('user');
        $user_session->getManager()->getStorage()->clear('user');
    }
    
    public function loginerrorAction()
    {
        
    }

    public function addAction()
    {
		$user_session = new Container('user');
		$user_session->errorMessage = '';
		$user_session->successMessage = '';

        $form = new BookingForm();
        $form->get('submit')->setValue('Hinzufügen');
        
        $request = $this->getRequest();
        if ($request->isPost()) {
            $booking = new Booking();
            $form->setInputFilter($booking->getInputFilter());
            $form->setData($request->getPost());

            if ($form->isValid()) {
                $booking->exchangeArray($form->getData());
                try {
                    $booking->userForeignKey = $user_session->userPrimaryKey;
                    $this->getBookingTable()->saveBooking($booking);
                    $user_session->successMessage = 'Buchung erfolgreich gespeichert.';
                } catch (\Exception $e) {
                    $user_session->errorMessage = $e->getMessage();
                }
                
                // Redirect to list of bookings
                return $this->redirect()->toRoute('booking');
            }
        }
        
        return array('form' => $form, 'errorMessage' => $user_session->errorMessage, 'successMessage' => $user_session->successMessage);
    }

    public function editAction()
    {
		$user_session = new Container('user');
		$user_session->errorMessage = '';
		$user_session->successMessage = '';
		
        $id = (int) $this->params()->fromRoute('id', 0);
		
        if (!$id) {
            return $this->redirect()->toRoute('booking', array(
                'action' => 'add'
            ));
        }

        // Get the Booking with the specified id. An exception is thrown
        // if it cannot be found, in which case go to the booking page.
        try {
            $booking = $this->getBookingTable()->getBookingMatchingBookingId($id);
        }
        catch (\Exception $e) {
			$user_session->errorMessage = $e->getMessage();
            return $this->redirect()->toRoute('booking', array(
                'action' => 'booking',
				'errorMessage' => $user_session->errorMessage,
				'successMessage' => $user_session->successMessage
            ));
        }
		
        $form  = new BookingForm();
        $form->bind($booking);
        $form->get('submit')->setAttribute('value', 'Übernehmen');
		
        $request = $this->getRequest();		
        if ($request->isPost()) {			
            $form->setInputFilter($booking->getInputFilter());
            $form->setData($request->getPost());			

			if ($form->isValid()) {
				try {
					$booking->userForeignKey = $user_session->userPrimaryKey;
					$this->getBookingTable()->saveBooking($booking);
					$user_session->successMessage = 'Buchung erfolgreich bearbeitet.';
				} catch (\Exception $e) {
					$user_session->errorMessage = $e->getMessage();
				}

                // Redirect to list of bookings
                return $this->redirect()->toRoute('booking');
            }
        }
		
        return array(
            'id' => $id,
            'form' => $form,
			'errorMessage' => $user_session->errorMessage,
			'successMessage' => $user_session->successMessage
        );
    }
    
   public function deleteAction()
   {
		$user_session = new Container('user');
		$user_session->errorMessage = '';
		$user_session->successMessage = '';
		
        $id = (int) $this->params()->fromRoute('id', 0);
        if (!$id) {
            return $this->redirect()->toRoute('booking');
        }
        
        $request = $this->getRequest();
        if ($request->isPost()) {
            $del = $request->getPost('del', 'Nein');

            if ($del == 'Ja') {
                $id = (int) $request->getPost('id');
				
                $this->getBookingTable()->deleteBooking($id);
				$user_session->successMessage = 'Buchung erfolgreich gelöscht.';
            }

            // Redirect to list of bookings
            return $this->redirect()->toRoute('booking');
        }

        return array(
            'id'    => $id,
            'booking' => $this->getBookingTable()->getBookingMatchingBookingId($id),
			'errorMessage' => $user_session->errorMessage,
			'successMessage' => $user_session->successMessage
        );
    }
}
