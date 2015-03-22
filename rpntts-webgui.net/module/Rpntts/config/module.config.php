<?php
return array(
    'controllers' => array(
        'invokables' => array(
            'Rpntts\Controller\Rpntts' => 'Rpntts\Controller\RpnttsController',
        ),
    ),
    
    'router' => array(
        'routes' => array(
            'rpntts' => array(
                'type'    => 'segment',
                'options' => array(
                    'route'    => '/rpntts[/:action][/:id]',
                    'constraints' => array(
                        'action' => '[a-zA-Z][a-zA-Z0-9_-]*',
                        'id'     => '[0-9]+',
                    ),
                    'defaults' => array(
                        'controller' => 'Rpntts\Controller\Rpntts',
                        'action'     => 'index',
                    ),
                ),
            ),
        ),
    ),
    
    'view_manager' => array(
        'template_path_stack' => array(
            'rpntts' => __DIR__ . '/../view/rpntts/rpntts',
        ),
    ),
);
 