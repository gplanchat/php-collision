#ifndef COLLISION_FUNCTIONS_H
#define COLLISION_FUNCTIONS_H 1

#define COLLISION_CLASS(namespace, class_name) collision_ce_Collision_ ## namespace ## _ ## class_name
#define COLLISION_INTERFACE(namespace, interface_name) COLLISION_CLASS(namespace, interface_name)

#define DECLARE_COLLISION_CLASS(namespace, class_name) \
	PHPAPI zend_class_entry * COLLISION_CLASS(namespace, class_name)
#define DECLARE_COLLISION_INTERFACE(namespace, class_name) DECLARE_COLLISION_CLASS(namespace, class_name)


#define COLLISION_METHOD(namespace, class_name, method) \
	PHP_METHOD(collision_ce_Collision_ ## namespace ## _ ## class_name, method);

#define COLLISION_METHOD_ARGINFO(namespace, class_name, method) arginfo_Collision_ ## namespace ## _ ## class_name ## _ ## method

#define COLLISION_ME(namespace, class_name, method, arginfo, flags) \
	PHP_ME(Collision_ ## namespace ## _ ## class_name, method, arginfo, flags);

#define COLLISION_CLASS_FE(namespace, class_name) collision_fe_Collision_ ## namespace ## class_name ## _methods;





#define REGISTER_COLLISION_STD_CLASS(class_handler, class_name, obj_ctor) \
	spl_register_std_class(&class_handler, class_name, obj_ctor, NULL TSRMLS_CC);

#define REGISTER_COLLISION_STD_CLASS_EX(class_handler, class_name, obj_ctor, funcs) \
	spl_register_std_class(&class_handler, class_name, obj_ctor, funcs TSRMLS_CC);

#define REGISTER_COLLISION_SUB_CLASS_EX(class_handler, class_name, parent_class_handler, obj_ctor, funcs) \
	spl_register_sub_class(&class_name, parent_class_handler, class_name, obj_ctor, funcs TSRMLS_CC);

#define REGISTER_COLLISION_INTERFACE(class_handler, class_name, funcs) \
	spl_register_interface(&class_handler, class_name, funcs TSRMLS_CC);

#define REGISTER_COLLISION_IMPLEMENTS(class_handler, class_name, interface_handler) \
	zend_class_implements(class_handler TSRMLS_CC, 1, interface_handler);

#define REGISTER_COLLISION_ITERATOR(class_handler) \
	zend_class_implements(class_handler TSRMLS_CC, 1, zend_ce_iterator);

#define REGISTER_COLLISION_PROPERTY(class_handler, prop_name, prop_flags) \
	collision_register_property(class_handler, prop_name, sizeof(prop_name)-1, prop_flags TSRMLS_CC);

#define REGISTER_COLLISION_CLASS_CONST_LONG(class_handler, const_name, value) \
	zend_declare_class_constant_long(class_handler, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

#endif
