<?php
namespace Rpntts;

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
                }
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
                    return new TableGateway('timeModel', $dbAdapter, NULL, $resultSetPrototype);
                },
                'Rpntts\Model\UserModelTable' => function($sm) {
                    $tableGateway = $sm->get('UserModelTableGateway');
                    $table = new UserModelTable($tableGateway);
                    return $table;
                },
                'UserModelTableGateway' => function($sm) {
                    $dbAdapter = $sm->get('Zend\Db\Adapter\Adapter');
                    $resultSetPrototype = new ResultSet();
                    $resultSetPrototype->setArrayObjectPrototype(new User());
                    return new TableGateway('user', $dbAdapter, NULL, $resultSetPrototype);
                },
            ),
         );
     }
 }
 