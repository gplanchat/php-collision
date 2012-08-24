php-collision
=============

An experimental PHP extension for building event-driven network servers in PHP

# Installation

To install collision, you must first compile it :

    $ phpize
    $ ./configure --enable-collision && make
    $ sudo make install

Then, in your php.ini, add this line near others modules declarations :

    extension=php_collision.so

# Current API

## The Event Manager

There are currently 6 classes in the Event Manager of `php-collision`, grouped into th namespace `Collision\EventManager`.

They could be defined as :


    interface EventInterface
    {
        string public function getName();
        EventInterface public function stop();
        bool public function isStopped();
    }

    trait EventTrait
    {
        string public function getName();
        EventInterface public function stop();
        bool public function isStopped();
    }

    class Event
        implements EventInterface
    {
        use EventTrait;
        
        public function __construct(string $name);
    }
    
    interface EventEmitterInterface
    {
        EventEmitterInterface public function on(string $eventName, callable $listener);
        EventEmitterInterface public function once(string $eventName, callable $listener);
        EventEmitterInterface public function removeListener(string $eventName, callable $listener);
        EventEmitterInterface public function removeAllListeners(string $eventName = null);
        array public function getListeners(string $eventName);
        EventEmitterInterface public function emit(EventInterface $event, array $arguments = []);
    }
    
    trait EventEmitterTrait
    {
        EventEmitterInterface public function on(string $eventName, callable $listener);
        EventEmitterInterface public function once(string $eventName, callable $listener);
        EventEmitterInterface public function removeListener(string $eventName, callable $listener);
        EventEmitterInterface public function removeAllListeners(string $eventName = null);
        array public function getListeners(string $eventName);
        EventEmitterInterface public function emit(EventInterface $event, array $arguments = []);
    }

    class SharedEventEmitter
        implements EventEmitterInterface
    {
        use EventEmitterTrait;
    }