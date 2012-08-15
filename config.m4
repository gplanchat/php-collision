PHP_ARG_ENABLE(collision, whether to enable Collision Framework support, [ --enable-collision   Enable Collision Framework support])

if test "$PHP_COLLISION" = "yes"; then
  AC_DEFINE(HAVE_COLLISION, 1, [Whether you have Collision Framework support])
  PHP_NEW_EXTENSION(collision, src/php_collision.c, $ext_shared)
fi