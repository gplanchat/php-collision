#ifndef PHP_COLLISION_H
#define PHP_COLLISION_H 1

#include "php.h"
#include "collision_functions.h"

#define PHP_COLLISION_VERSION "1.0.0.dev0"
#define PHP_COLLISION_API_VERSION "1.0"
#define PHP_COLLISION_EXTNAME "collision"

#define COLLISION_NS      "Collision"
#define EVENTS_MANAGER_NS "EventManager"

PHPAPI zend_module_entry collision_module_entry;
#define phpext_collision_ptr &collision_module_entry

/*
 * namespace Collision\EventManager;
 *
 * interface EventInterface
 * {
 *     public function getName();
 *     public function stop();
 *     public function isStopped();
 * }
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventInterface_getName, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventInterface_stop, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventInterface_isStopped, 0)
ZEND_END_ARG_INFO();
/* }}} */

/* {{{ methods */
PHP_METHOD(Collision_EventManager_EventInterface, getName);
PHP_METHOD(Collision_EventManager_EventInterface, stop);
PHP_METHOD(Collision_EventManager_EventInterface, isStopped);
/* }}} */

PHPAPI zend_function_entry collision_fe_Collision_EventManager_EventInterface_methods[] = {
    PHP_ME(Collision_EventManager_EventInterface, getName,   arginfo_Collision_EventManager_EventInterface_getName,   ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventInterface, stop,      arginfo_Collision_EventManager_EventInterface_stop,      ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventInterface, isStopped, arginfo_Collision_EventManager_EventInterface_isStopped, ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHPAPI zend_class_entry * collision_ce_Collision_EventManager_EventInterface;

/*
 * namespace Collision\EventManager;
 *
 * trait EventTrait
 * {
 *     protected $name = null;
 *     protected $isStopped = false;
 *
 *     public function getName();
 *     public function stop();
 *     public function isStopped();
 * }
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventTrait_getName, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventTrait_stop, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventTrait_isStopped, 0)
ZEND_END_ARG_INFO();
/* }}} */

/* {{{ methods */
PHP_METHOD(Collision_EventManager_EventTrait, getName);
PHP_METHOD(Collision_EventManager_EventTrait, stop);
PHP_METHOD(Collision_EventManager_EventTrait, isStopped);
/* }}} */

PHPAPI zend_function_entry collision_fe_Collision_EventManager_EventTrait_methods[] = {
    PHP_ME(Collision_EventManager_EventTrait, getName,   arginfo_Collision_EventManager_EventTrait_getName,   ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventTrait, stop,      arginfo_Collision_EventManager_EventTrait_stop,      ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventTrait, isStopped, arginfo_Collision_EventManager_EventTrait_isStopped, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHPAPI zend_class_entry * collision_ce_Collision_EventManager_EventTrait;

/*
 * namespace Collision\EventManager;
 *
 * class Event
 *     implements EventInterface
 * {
 *     use EventTrait;
 *
 *     public function __construct(string $name);
 * }
 */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_Event_construct, 0)
    ZEND_ARG_INFO(0, name) // IS_STRING
ZEND_END_ARG_INFO();
/* }}} */

/* {{{ methods */
PHP_METHOD(Collision_EventManager_Event, __construct);
/* }}} */

PHPAPI zend_function_entry collision_fe_Collision_EventManager_Event_methods[] = {
    PHP_ME(Collision_EventManager_Event, __construct,    arginfo_Collision_EventManager_Event_construct,      ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventTrait, getName,   arginfo_Collision_EventManager_EventTrait_getName,   ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventTrait, stop,      arginfo_Collision_EventManager_EventTrait_stop,      ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventTrait, isStopped, arginfo_Collision_EventManager_EventTrait_isStopped, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHPAPI zend_class_entry * collision_ce_Collision_EventManager_Event;

/*
 * namespace Collision\EventManager;
 *
 * interface EventEmitterInterface
 * {
 *     public function on($event, callable $listener);
 *     public function once($event, callable $listener);
 *     public function removeListener($event, callable $listener);
 *     public function removeAllListeners($event = null);
 *     public function getListeners($event);
 *     public function emit($event, array $arguments = []);
 * }
 */

/* {{{ arginfo */
/*
ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventEmitterInterface_on, 0)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, listener, IS_CALLABLE, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventEmitterInterface_once, 0)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, listener, IS_CALLABLE, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventEmitterInterface_removeListener, 0)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, listener, IS_CALLABLE, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_Collision_EventManager_EventEmitterInterface_removeAllListeners, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_Collision_EventManager_EventEmitterInterface_getListeners, 0)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_Collision_EventManager_EventEmitterInterface_emit, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, arguments, IS_ARRAY, 0)
ZEND_END_ARG_INFO();
/* }}} */

/* {{{ methods */
/*
PHP_METHOD(Collision_EventManager_EventEmitterInterface, on);
PHP_METHOD(Collision_EventManager_EventEmitterInterface, once);
PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeListener);
PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeAllListeners);
PHP_METHOD(Collision_EventManager_EventEmitterInterface, getListeners);
PHP_METHOD(Collision_EventManager_EventEmitterInterface, emit);
/* }}} */
/*
PHPAPI zend_function_entry collision_fe_Collision_EventManager_EventEmitterInterface_methods[] = {
    PHP_ME(Collision_EventManager_EventEmitterInterface, on,                 arginfo_Collision_EventManager_EventEmitterInterface_on,                 ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventEmitterInterface, once,               arginfo_Collision_EventManager_EventEmitterInterface_once,               ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventEmitterInterface, removeListener,     arginfo_Collision_EventManager_EventEmitterInterface_removeListener,     ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventEmitterInterface, removeAllListeners, arginfo_Collision_EventManager_EventEmitterInterface_removeAllListeners, ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventEmitterInterface, getListeners,       arginfo_Collision_EventManager_EventEmitterInterface_getListeners,       ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_ME(Collision_EventManager_EventEmitterInterface, emit,               arginfo_Collision_EventManager_EventEmitterInterface_emit,               ZEND_ACC_ABSTRACT | ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHPAPI zend_class_entry * collision_ce_Collision_EventManager_EventEmitterInterface;
*/
#endif
