<?php

namespace Test\EventManager
{
    interface EventInterface
    {
        public function getName();
        public function stop();
        public function isStopped();
    }

    trait EventTrait
    {
        protected $name= null;
        protected $isStopped = false;

        public function getName()
        {
            return $this->name;
        }

        public function stop()
        {
            $this->isStopped = true;

            return $this;
        }

        public function isStopped()
        {
            return (bool) $this->isStopped;
        }
    }

    class Event
        implements EventInterface
    {
        use EventTrait;

        public function __construct($name)
        {
            $this->name = $name;
        }
    }

    interface EventEmitterInterface
    {
        public function on($eventName, callable $listener);
        public function once($eventName, callable $listener);
        public function removeListener($eventName, callable $listener);
        public function removeAllListeners($eventName);
        public function getListeners($eventName);
        public function emit(EventInterface $event, Array $params = []);
    }

    class EventEntry
    {
        public $listener;
        public $isCalledOnce;
    }

    trait EventEmitterTrait
    {
        private $eventListeners = array();

        private function _registerEvent($eventName, callable $listener, $isCalledOnce = false)
        {
            if (!isset($this->eventListeners[$eventName])) {
                $this->eventListeners[$eventName] = array();
            }

            $eventEntry = new EventEntry();
            $this->eventListeners[$eventName][] = $eventEntry;

            $eventEntry->listener = $listener;
            $eventEntry->isCalledOnce = (bool) $isCalledOnce;
        }

        public function on($eventName, callable $listener)
        {
            $eventName = (string) $eventName;
            if (empty($eventName)) {
                throw new \RuntimeException('Could not register event, event name should be a non-empty string.');
            }

            $this->_registerEvent($eventName, $listener, false);

            return $this;
        }

        public function once($eventName, callable $listener)
        {
            $eventName = (string) $eventName;
            if (empty($eventName)) {
                throw new \RuntimeException('Could not register event, event name should be a non-empty string.');
            }

            $this->_registerEvent($eventName, $listener, true);

            return $this;
        }

        public function removeListener($eventName, callable $listener)
        {
            $eventName = (string) $eventName;
            if (empty($eventName)) {
                throw new \RuntimeException('Could not remove event listeners, event name should be a non-empty string.');
            }

            if (!isset($this->eventListeners[$eventName])) {
                return $this;
            }

            $key = array_search($listener, $this->eventListeners[$eventName]);
            if ($key === false) {
                return $this;
            }
            unset($this->eventListeners[$eventName][$key]);

            return $this;
        }

        public function removeAllListeners($eventName)
        {
            $eventName = (string) $eventName;
            if (empty($eventName)) {
                throw new \RuntimeException('Could not remove event listeners, event name should be a non-empty string.');
            }

            if (!isset($this->eventListeners[$eventName])) {
                return $this;
            }

            unset($this->eventListeners[$eventName]);

            return $this;
        }

        public function getListeners($eventName)
        {
            $eventName = (string) $eventName;
            if (empty($eventName)) {
                throw new \RuntimeException('Could not get event listeners, event name should be a non-empty string.');
            }

            if (!isset($this->eventListeners[$eventName])) {
                return [];
            }

            $result = [];
            foreach ($this->eventListeners[$eventName] as $eventEntry) {
                $result[] = $eventEntry->listener;
            }

            return $result;
        }

        public function emit(EventInterface $event, Array $params = [])
        {
            $eventName = $event->getName();
            if (empty($eventName)) {
                throw new \RuntimeException('Could not trigger event, event name should be a non-empty string.');
            }

            if (!isset($this->eventListeners[$eventName])) {
                return $this;
            }

            array_unshift($params, $event);
            foreach ($this->eventListeners[$eventName] as $eventIndex => $eventEntry) {
                call_user_func_array($eventEntry->listener, $params);

                if ($eventEntry->isCalledOnce) {
                    unset($this->eventListeners[$eventName][$eventIndex]);
                }

                if ($event->isStopped()) {
                    break;
                }
            }

            return $this;
        }
    }

    class SharedEventEmitter
        implements EventEmitterInterface
    {
        use EventEmitterTrait;
    }
}

namespace
{
    use Test\EventManager;

    const EXECUTIONS = 1000000;

/*
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $event = new EventManager\Event('test');
        
        //var_dump($event->getName());
    }
*/
/*
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $event = new EventManager\Event('test');

        $event->stop();
        if (!$event->isStopped()) {
            die('Event Should be stopped');
        }
    }
*/

    $event = new EventManager\Event('test');
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $eventEmitter = new EventManager\SharedEventEmitter();
        $counter = 0;

        $eventEmitter->on('test', function(EventManager\EventInterface $event) use(&$counter) {$counter++;});

        $eventEmitter->emit($event);
        if ($counter != 1) {
            die('Event Should have been triggered');
        }
    }
}
