<?php

return array(
    'controllers' => array(
        'invokables' => array(
            'Rpntts\Controller\Rpntts' => 'Rpntts\Controller\RpnttsController',
        ),
    ),
    
    'router' => array(
        'routes' => array(
			'login' => array(
					'type'    => 'segment',
					'options' => array(
						'route'    => '/rpntts/login',
						'defaults' => array(
							'controller' => 'Rpntts\Controller\Rpntts',
							'action'     => 'login',
						),
					),
			),
			'logout' => array(
                'type'    => 'segment',
                'options' => array(
                    'route'    => '/rpntts/logout',
                    'defaults' => array(
                        'controller' => 'Rpntts\Controller\Rpntts',
                        'action'     => 'logout',
                    ),
                ),
            ),
			'loginerror' => array(
                'type'    => 'segment',
                'options' => array(
                    'route'    => '/rpntts/loginerror',
                    'defaults' => array(
                        'controller' => 'Rpntts\Controller\Rpntts',
                        'action'     => 'loginerror',
                    ),
                ),
            ),
            'booking' => array(
                'type'    => 'segment',
                'options' => array(
                    'route'    => '/rpntts[/:action][/:id]',
                    'constraints' => array(
                        'action' => '[a-zA-Z][a-zA-Z0-9_-]*',
                        'id'     => '[0-9]+',
                    ),
                    'defaults' => array(
                        'controller' => 'Rpntts\Controller\Rpntts',
                        'action'     => 'booking',
                    ),
                ),
            ),
        ),
    ),
    
    'view_manager' => array(
        'template_path_stack' => array(
            'rpntts' => __DIR__ . '/../view',
        ),
    ),
);
 