#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php/Zend/zend_API.h>
#include <php/ext/spl/spl_functions.h>
#include <php/Zend/zend_exceptions.h>

#include "php_collision.h"

PHP_MINFO_FUNCTION(collision)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Collision Framework", "enabled");
    php_info_print_table_row(2, "Collision Framework version", PHP_COLLISION_VERSION);
    php_info_print_table_row(2, "Collision Framework API Version", PHP_COLLISION_API_VERSION);
    php_info_print_table_end();
}

PHP_MINIT_FUNCTION(collision)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventInterface",
            collision_fe_Collision_EventManager_EventInterface_methods);
    collision_ce_Collision_EventManager_EventInterface = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventTrait",
            collision_fe_Collision_EventManager_EventTrait_methods);
    collision_ce_Collision_EventManager_EventTrait = zend_register_internal_class(&ce TSRMLS_CC);
    collision_ce_Collision_EventManager_EventTrait->ce_flags |= ZEND_ACC_TRAIT;
    zend_declare_property_long(collision_ce_Collision_EventManager_EventTrait, "name",      sizeof("name") - 1,      0, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_long(collision_ce_Collision_EventManager_EventTrait, "isStopped", sizeof("isStopped") - 1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "Event",
            collision_fe_Collision_EventManager_Event_methods);
    collision_ce_Collision_EventManager_Event = zend_register_internal_class(&ce TSRMLS_CC);
    zend_do_implement_interface(collision_ce_Collision_EventManager_Event, collision_ce_Collision_EventManager_EventInterface);
    zend_do_implement_trait(collision_ce_Collision_EventManager_Event, collision_ce_Collision_EventManager_EventTrait);
/*
    INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventEmitterInterface",
            collision_fe_Collision_EventManager_EventEmitterInterface_methods);
    collision_ce_Collision_EventManager_EventEmitterInterface = zend_register_internal_interface(&ce TSRMLS_CC);
*/
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(collision)
{
    return SUCCESS;
}

zend_module_entry collision_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_COLLISION_EXTNAME,
    NULL,
    PHP_MINIT(collision),
    PHP_MSHUTDOWN(collision),
    NULL,
    NULL,
    PHP_MINFO(collision),
    PHP_COLLISION_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_COLLISION
ZEND_GET_MODULE(collision)
#endif

/* {{{ methods */
PHP_METHOD(Collision_EventManager_EventInterface, getName)
{
}

PHP_METHOD(Collision_EventManager_EventInterface, stop)
{
}

PHP_METHOD(Collision_EventManager_EventInterface, isStopped)
{
}

PHP_METHOD(Collision_EventManager_EventTrait, getName)
{
    zval * value;

    value = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "name", sizeof("name") - 1, 1 TSRMLS_CC);

    if (value == NULL) {
        RETVAL_NULL();
    } else {
        RETURN_ZVAL(value, 1, 0);
    }
}

PHP_METHOD(Collision_EventManager_EventTrait, stop)
{
    zval * value;

    ALLOC_ZVAL(value);
    ZVAL_TRUE(value);

    zend_update_property(Z_OBJCE_P(getThis()), getThis(), "isStopped", sizeof("isStopped") - 1, value TSRMLS_CC);

    FREE_ZVAL(value);

    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventTrait, isStopped)
{
    zval * value;

    value = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "isStopped", sizeof("isStopped") - 1, 1 TSRMLS_CC);

    if (value == NULL) {
        RETVAL_NULL();
    } else {
        RETURN_ZVAL(value, 1, 0);
    }
}

PHP_METHOD(Collision_EventManager_Event, __construct)
{
    zval * name = NULL;

    if (zend_parse_parameters(1 TSRMLS_CC, "z", &name) == FAILURE) {
        zend_error(E_ERROR, "Wrong parameters for Collision\EventManager\Event(string $name)");
    }

    zend_update_property(Z_OBJCE_P(getThis()), getThis(), "name", sizeof("name") - 1, name TSRMLS_CC);
}
/*
PHP_METHOD(Collision_EventManager_EventEmitterInterface, on)
{
    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventEmitterInterface, once)
{
    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeListener)
{
    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeAllListeners)
{
    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventEmitterInterface, getListeners)
{
    RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(Collision_EventManager_EventEmitterInterface, emit)
{
    RETURN_ZVAL(getThis(), 0, 0);
}
*/
/* }}} */
