<?php
namespace Rpntts;

 use Rpntts\Model\Booking;
 use Rpntts\Model\BookingTable;
 use Rpntts\Model\Card;
 use Rpntts\Model\CardTable;
 use Rpntts\Model\TimeModel;
 use Rpntts\Model\TimeModelTable;
 use Rpntts\Model\User;
 use Rpntts\Model\UserTable;
 use Zend\Db\ResultSet\ResultSet;
 use Zend\Db\TableGateway\TableGateway;
 use Zend\ModuleManager\Feature\AutoloaderProviderInterface;
 use Zend\ModuleManager\Feature\ConfigProviderInterface;

 class Module implements AutoloaderProviderInterface, ConfigProviderInterface
 {
     public function getAutoloaderConfig()
     {
         return array(
             'Zend\Loader\ClassMapAutoloader' => array(
                 __DIR__ . '/autoload_classmap.php',
             ),
             'Zend\Loader\StandardAutoloader' => array(
                 'namespaces' => array(
                     __NAMESPACE__ => __DIR__ . '/src/' . __NAMESPACE__,
                 ),
             ),
         );
     }

     public function getConfig()
     {
         return include __DIR__ . '/config/module.config.php';
     }
     
     public function getServiceConfig()
     {
         return array(
            'factories' => array(
                'Rpntts\Model\BookingTable' => function($sm) {
                    $tableGateway = $sm->get('BookingTableGateway');
                    $table = new BookingTable($tableGateway);
                    return $table;
                },
                'BookingTableGateway' => function($sm) {
                    $dbAdapter = $sm->get('Zend\Db\Adapter\Adapter');
                    $resultSetPrototype = new ResultSet();
                    $resultSetPrototype->setArrayObjectPrototype(new Booking());
                    return new TableGateway('booking', $dbAdapter, NULL, $resultSetPrototype);
                },
                'Rpntts\Model\CardTable' => function($sm) {
                    $tableGateway = $sm->get('CardTableGateway');
                    $table = new CardTable($tableGateway);
                    return $table;
                },
                'CardTableGateway' => function($sm) {
                    $dbAdapter = $sm->get('Zend\Db\Adapter\Adapter');
                    $resultSetPrototype = new ResultSet();
                    $resultSetPrototype->setArrayObjectPrototype(new Card());
                    return new TableGateway('card', $dbAdapter, NULL, $resultSetPrototype);
                },
                'Rpntts\Model\TimeModelTable' => function($sm) {
                    $tableGateway = $sm->get('TimeModelTableGateway');
                    $table = new TimeModelTable($tableGateway);
                    return $table;
                },
                'TimeModelTableGateway' => function($sm) {
                    $dbAdapter = $sm->get('Zend\Db\Adapter\Adapter');
                    $resultSetPrototype = new ResultSet();
                    $resultSetPrototype->setArrayObjectPrototype(new TimeModel());
                    return new TableGateway('timemodel', $dbAdapter, NULL, $resultSetPrototype);
                },
                'Rpntts\Model\UserTable' => function($sm) {
                    $tableGateway = $sm->get('UserTableGateway');
                    $table = new UserTable($tableGateway);
                    return $table;
                },
                'UserTableGateway' => function($sm) {
                    $dbAdapter = $sm->get('Zend\Db\Adapter\Adapter');
                    $resultSetPrototype = new ResultSet();
                    $resultSetPrototype->setArrayObjectPrototype(new User());
                    return new TableGateway('user', $dbAdapter, NULL, $resultSetPrototype);
                },
            ),
         );
     }
 }
 