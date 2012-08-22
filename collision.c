#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php/Zend/zend_API.h>
#include <php/Zend/zend_exceptions.h>

#include "php_collision.h"

static zend_object_handlers collision_event_manager_eventemitter_object_handlers;

typedef struct _collision_event_entry_t {
	zend_uchar   is_called_once;
	zval	   * listener;
} collision_event_entry_t;

typedef struct _collision_event_emitter_t {
	zend_object object;
	HashTable * events;
} collision_event_emitter_t;


static void collision_eventmanager_eventemitter_object_dtor(void * object, zend_object_handle handle TSRMLS_DC)
{
	collision_event_emitter_t * intern = (collision_event_emitter_t *) object;

	FREE_HASHTABLE(intern->events);

	zend_objects_free_object_storage(object TSRMLS_CC);
}

zend_object_value collision_eventmanager_eventemitter_object_new(zend_class_entry * ce TSRMLS_DC)
{
	zend_object_value retval;
	collision_event_emitter_t * intern;

	intern = ecalloc(1, sizeof(collision_event_emitter_t));
	intern->object.ce = ce;

	zend_object_std_init(&intern->object, ce TSRMLS_CC);
	object_properties_init(&intern->object, ce);

	retval.handle = zend_objects_store_put(intern, NULL, collision_eventmanager_eventemitter_object_dtor, NULL TSRMLS_CC);
	retval.handlers = &collision_event_manager_eventemitter_object_handlers;

	ALLOC_HASHTABLE(intern->events);
	zend_hash_init(intern->events, 0, NULL, NULL, 0);

	return retval;
}

static int collision_eventmanager_eventemitter_listener_add(zval * event_emitter, char * event_name, uint event_name_len, zval * listener TSRMLS_DC)
{
	collision_event_emitter_t * emitter;
	HashTable * event_entries = NULL;
	char * error = NULL;

	if (!zend_is_callable(listener, 0, &error TSRMLS_CC)) {
		zend_error(E_ERROR, "Listener error: listener argument is not a callable.");
		if (error) {
			efree(error);
		}
		return FAILURE;
	}
	if (error) {
		efree(error);
	}

	emitter = (collision_event_emitter_t *) zend_object_store_get_object(event_emitter TSRMLS_CC);
	if (emitter == NULL) {
		zend_error(E_ERROR, "Could not retrieve event manager object");
		return FAILURE;
	}

	if (!zend_hash_exists(emitter->events, event_name, event_name_len)) {
		ALLOC_HASHTABLE(event_entries);
		zend_hash_init(event_entries, 16, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_add(emitter->events, event_name, event_name_len, event_entries, sizeof(HashTable *), NULL);
	} else if (zend_hash_find(emitter->events, event_name, event_name_len, (void **)&event_entries) == FAILURE) {
		zend_error(E_ERROR, "Could not retrieve event entries handler");
		return FAILURE;
	}

	if (zend_hash_next_index_insert(event_entries, &listener, sizeof(zval *), NULL) == SUCCESS) {
		zval_add_ref(&listener);
	} else {
		zend_error(E_ERROR, "Could not add event listener");
		return FAILURE;
	}

	return SUCCESS;
}

static int collision_eventmanager_eventemitter_trigger(zval * event_emitter, zval * event, zval * params TSRMLS_DC)
{
	collision_event_emitter_t * emitter = NULL;
	HashTable * event_entries = NULL;
	HashPosition pos;
	zval * item = NULL;
	zval retval;
	zval * name_property = NULL;
	zval ** params_array = NULL;

	int params_count = 0;
	int index = 0;
	int event_name_len = 0;
	char * error = NULL;
	char * event_name = NULL;

	name_property = zend_read_property(Z_OBJCE_P(event), event, "name", sizeof("name") - 1, 1 TSRMLS_CC);
	event_name = Z_STRVAL_P(name_property);
	event_name_len = strlen(event_name);

	emitter = (collision_event_emitter_t *) zend_object_store_get_object(event_emitter TSRMLS_CC);
	if (emitter == NULL) {
		zend_error(E_ERROR, "Could not retrieve event manager object");
		return FAILURE;
	}

	if (!zend_hash_exists(emitter->events, event_name, event_name_len)) {
		ALLOC_HASHTABLE(event_entries);
		zend_hash_init(event_entries, 16, NULL, ZVAL_PTR_DTOR, 0);
		zend_hash_add(emitter->events, event_name, event_name_len, event_entries, sizeof(HashTable *), NULL);
	} else if (zend_hash_find(emitter->events, event_name, event_name_len, (void **)&event_entries) == FAILURE) {
		zend_error(E_ERROR, "Could not retrieve event entries handler");
		return FAILURE;
	}

	if (Z_TYPE_P(params) != IS_ARRAY) {
		zend_error(E_ERROR, "Extra paramaters should be an array");
		return FAILURE;
	}
	params_count = zend_hash_num_elements(params);
	params_array = ecalloc(params_count + 1, sizeof(zval *));
	params_array[0] = event;

	zend_hash_internal_pointer_reset_ex(params, &pos);
	for (index = 0;; zend_hash_move_forward_ex(event_entries, &pos)) {
		if (zend_hash_get_current_data_ex(event_entries, (void **) &item, &pos) == FAILURE) {
			break;
		}

		params_array[++index] = item;
	}

	zend_hash_internal_pointer_reset_ex(event_entries, &pos);
	for (index = 0;; zend_hash_move_forward_ex(event_entries, &pos)) {
		if (zend_hash_get_current_data_ex(event_entries, (void **) &item, &pos) == FAILURE) {
			break;
		}

		if (!zend_is_callable(item, 0, &error TSRMLS_CC)) {
			zend_error(E_WARNING, "Listener error: listener is not a callable.");
			if (error) {
				efree(error);
			}
			return FAILURE;
		}
		if (error) {
			efree(error);
		}

		if (call_user_function(EG(function_table), NULL, item, &retval, params_count, params_array TSRMLS_CC ) == SUCCESS) {
			zval_dtor(&retval);
		}
	}

	efree(params_array);

	return SUCCESS;
}

/*
 * namespace Collision\EventManager;
 *
 * interface EventInterface
 * {
 *	 public function getName();
 *	 public function stop();
 *	 public function isStopped();
 * }
 */

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
/* }}} */

/*
 * namespace Collision\EventManager;
 *
 * trait EventTrait
 * {
 *	 protected $name = null;
 *	 protected $isStopped = false;
 *
 *	 public function getName();
 *	 public function stop();
 *	 public function isStopped();
 * }
 */

/* {{{ methods */
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
	zval * self;
	zend_update_property_bool(Z_OBJCE_P(getThis()), getThis(), "isStopped", sizeof("isStopped") - 1, 1 TSRMLS_CC);

	RETURN_ZVAL(getThis(), 1, 0);
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
/* }}} */

/*
 * namespace Collision\EventManager;
 *
 * class Event
 *	 implements EventInterface
 * {
 *	 use EventTrait;
 *
 *	 public function __construct(string $name);
 * }
 */

/* {{{ methods */
PHP_METHOD(Collision_EventManager_Event, __construct)
{
	char * name = NULL;
	int name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Collision\EventManager\Event(string $name)");
	}

	zend_update_property_stringl(Z_OBJCE_P(getThis()), getThis(), "name", sizeof("name") - 1, name, name_len TSRMLS_CC);
}
/* }}} */

/*
 * namespace Collision\EventManager;
 *
 * interface EventEmitterInterface
 * {
 *	 EventEmitterInterface public function on(string $eventName, callable $listener);
 *	 EventEmitterInterface public function once(string $eventName, callable $listener);
 *	 EventEmitterInterface public function removeListener(string $eventName, callable $listener);
 *	 EventEmitterInterface public function removeAllListeners(string $eventName = null);
 *	 array public function getListeners(string $eventName);
 *	 EventEmitterInterface public function emit(EventInterface $event, array $arguments = []);
 * }
 */

/* {{{ methods */

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::on(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, on)
{
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::once(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, once)
{
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::removeListener(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeListener)
{
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::removeAllListeners([string eventName = NULL])
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, removeAllListeners)
{
}

/* {{{ proto array Collision\EventManager\EventEmitterInterface::getListeners(string eventName)
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, getListeners)
{
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::emit(Collision\EventManager\EventInterface event[, array arguments = array()])
*/
PHP_METHOD(Collision_EventManager_EventEmitterInterface, emit)
{
}
/* }}} */

/*
 * namespace Collision\EventManager;
 *
 * interface EventEmitterTrait
 * {
 *	 EventEmitterInterface public function on(string $eventName, callable $listener);
 *	 EventEmitterInterface public function once(string $eventName, callable $listener);
 *	 EventEmitterInterface public function removeListener(string $eventName, callable $listener);
 *	 EventEmitterInterface public function removeAllListeners(string $eventName = null);
 *	 array public function getListeners(string $eventName);
 *	 EventEmitterInterface public function emit(EventInterface $event, array $arguments = []);
 * }
 */

/* {{{ methods */

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::on(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, on)
{
	char * event_name = NULL;
	int event_name_len = 0;
	zval * listener = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &event_name, &event_name_len, &listener) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Collision\EventManager\Event(string $name)");
	}

	collision_eventmanager_eventemitter_listener_add(getThis(), event_name, event_name_len, listener TSRMLS_CC);

	RETURN_ZVAL(getThis(), 1, 0);
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::once(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, once)
{
	char * event_name = NULL;
	int event_name_len = 0;
	zval * listener = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &event_name, &event_name_len, &listener) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Collision\EventManager\Event(string $name)");
	}

	collision_eventmanager_eventemitter_listener_add(getThis(), event_name, event_name_len, listener TSRMLS_CC);

	RETURN_ZVAL(getThis(), 1, 0);
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::removeListener(string eventName, callable listener)
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, removeListener)
{
	// TODO
	RETURN_ZVAL(getThis(), 1, 0);
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::removeAllListeners([string eventName = NULL])
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, removeAllListeners)
{
	// TODO
	RETURN_ZVAL(getThis(), 1, 0);
}

/* {{{ proto array Collision\EventManager\EventEmitterInterface::getListeners(string eventName)
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, getListeners)
{
	/*
	zval * retval;
	MAKE_STD_ZVAL(retval);

	array_init(retval);


	add_next_index_zval(retval, listener);

	RETURN_ZVAL(retval, 0, 1);
	*/
}

/* {{{ proto Collision\EventManager\EventEmitterInterface Collision\EventManager\EventEmitterInterface::emit(Collision\EventManager\EventInterface event[, array arguments = array()])
*/
PHP_METHOD(Collision_EventManager_EventEmitterTrait, emit)
{
	zval * event = NULL;
	zval * args = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", event, args) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Collision\EventManager\Event(string $name)");
	}

	collision_eventmanager_eventemitter_trigger(getThis(), event, args TSRMLS_CC);

	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

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

	memcpy(&collision_event_manager_eventemitter_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	collision_event_manager_eventemitter_object_handlers.clone_obj = NULL;

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventInterface",
			collision_fe_Collision_EventManager_EventInterface_methods);
	collision_ce_Collision_EventManager_EventInterface = zend_register_internal_interface(&ce TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventTrait",
			collision_fe_Collision_EventManager_EventTrait_methods);
	collision_ce_Collision_EventManager_EventTrait = zend_register_internal_class(&ce TSRMLS_CC);
	collision_ce_Collision_EventManager_EventTrait->ce_flags |= ZEND_ACC_TRAIT;
	zend_declare_property_long(collision_ce_Collision_EventManager_EventTrait, "name",	  sizeof("name") - 1,	  0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(collision_ce_Collision_EventManager_EventTrait, "isStopped", sizeof("isStopped") - 1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "Event",
			collision_fe_Collision_EventManager_Event_methods);
	collision_ce_Collision_EventManager_Event = zend_register_internal_class(&ce TSRMLS_CC);
	zend_do_implement_interface(collision_ce_Collision_EventManager_Event, collision_ce_Collision_EventManager_EventInterface TSRMLS_CC);
	zend_do_implement_trait(collision_ce_Collision_EventManager_Event, collision_ce_Collision_EventManager_EventTrait TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventEmitterInterface",
			collision_fe_Collision_EventManager_EventEmitterInterface_methods);
	collision_ce_Collision_EventManager_EventEmitterInterface = zend_register_internal_interface(&ce TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "EventEmitterTrait",
			collision_fe_Collision_EventManager_EventEmitterTrait_methods);
	collision_ce_Collision_EventManager_EventEmitterTrait = zend_register_internal_class(&ce TSRMLS_CC);
	collision_ce_Collision_EventManager_EventEmitterTrait->ce_flags |= ZEND_ACC_TRAIT;
	collision_ce_Collision_EventManager_EventEmitterTrait->create_object = collision_eventmanager_eventemitter_object_new;
//	zend_declare_property_long(collision_ce_Collision_EventManager_EventEmitterTrait, "eventListeners", sizeof("eventListeners") - 1, 0, ZEND_ACC_PRIVATE TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(ce, ZEND_NS_NAME(COLLISION_NS, EVENTS_MANAGER_NS), "SharedEventEmitter",
			collision_fe_Collision_EventManager_SharedEventEmitter_methods);
	collision_ce_Collision_EventManager_SharedEventEmitter = zend_register_internal_class(&ce TSRMLS_CC);
	collision_ce_Collision_EventManager_SharedEventEmitter->create_object = collision_eventmanager_eventemitter_object_new;
	zend_do_implement_interface(collision_ce_Collision_EventManager_SharedEventEmitter, collision_ce_Collision_EventManager_EventEmitterInterface TSRMLS_CC);
	zend_do_implement_trait(collision_ce_Collision_EventManager_SharedEventEmitter, collision_ce_Collision_EventManager_EventEmitterTrait TSRMLS_CC);

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

