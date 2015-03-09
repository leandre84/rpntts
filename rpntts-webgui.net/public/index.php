<?php
require_once(
__DIR__ .
'/../vendor/zendframework/library/Zend/Loader/AutoloaderFactory.php'
);
Zend/Loader/AutoloaderFactory::factory(array(
    'Zend/Loader/StandardAutoloader' => array(
    'autoregister_zf' => true
    )
));
?>
