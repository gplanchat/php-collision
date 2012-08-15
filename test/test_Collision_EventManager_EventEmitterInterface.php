<?php

namespace Test
{
    use Collision\EventManager\EventEmitterInterface;
    use Collision\EventManager\EventInterface;
    use Collision\EventManager\EventTrait;
    use Collision\EventManager\Event;
/*
    class Emitter
        implements EventEmitterInterface
    {
        public function on(string $event, callable $listener)
        {
            return $this;
        }

        public function once(string $event, callable $listener)
        {
            return $this;
        }

        public function removeListener(string $event, callable $listener)
        {
            return $this;
        }

        public function removeAllListeners(string $event = null)
        {
            return $this;
        }
    }
*/
    class MyInterfacedEvent
        implements EventInterface
    {
        protected $name = null;

        public function __construct($name)
        {
            $this->name = $name;
        }

        public function isStopped()
        {
            return false;
        }

        public function getName()
        {
            return $this->name;
        }

        public function stop()
        {
            return $this;
        }

        public function getName2()
        {
            return $this->name;
        }
    }

    class MyTraitedEvent
    {
        use EventTrait;

        public function __construct($name)
        {
            $this->name = $name;
        }

        public function getName2()
        {
            return $this->name;
        }

        public function setName($name)
        {
            $this->name = $name;

            return $this;
        }
    }

    class MyInterfacedTraitedEvent
        implements EventInterface
    {
        use EventTrait;

        public function __construct($name)
        {
            $this->name = $name;
        }

        public function getName2()
        {
            return $this->name;
        }
    }

    class MyEvent
        extends Event
    {
/*
        public function __construct($name)
        {
            parent::__construct($name);
            var_dump($this->name);
        }
*/
    }

    class MyEvent2
        extends MyInterfacedTraitedEvent
    {
/*
        public function __construct($name)
        {
            parent::__construct($name);
            var_dump($this->name);
        }
*/
    }

}

namespace 
{
    if (!in_array('collision', get_loaded_extensions())) { 
        throw new Exception('Collision Framework not loaded');
    }

    $event = new Collision\EventManager\Event('test');

    var_dump($event->getName());
    var_dump($event instanceof Collision\EventManager\EventInterface);
    $reflection = new ReflectionObject($event);
    var_dump($reflection->getTraits());

    $event = new Test\MyInterfacedEvent('test');

    var_dump($event->getName());
    var_dump($event->getName2());

    $event = new Test\MyTraitedEvent(uniqid('test_'));

    var_dump($event->getName());
    $event->setName('Woot');
    var_dump($event->getName());
    var_dump($event->getName2());

    $event = new Test\MyInterfacedTraitedEvent('test');

    var_dump($event->getName());
    var_dump($event->getName2());
    $reflection = new ReflectionObject($event);
    var_dump($reflection->getTraits());

    $event = new Test\MyEvent('test');

    var_dump($event->getName());
    var_dump($event instanceof Collision\EventManager\EventInterface);
    $reflection = new ReflectionObject($event);
    var_dump($reflection->getTraits());

    $event = new Test\MyEvent2('test');

    var_dump($event->getName());
    var_dump($event instanceof Collision\EventManager\EventInterface);
    $reflection = new ReflectionObject($event);
    var_dump($reflection->getTraits());
}
