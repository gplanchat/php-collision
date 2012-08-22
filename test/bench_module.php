<?php

namespace
{
    use Collision\EventManager\EventInterface;
    use Collision\EventManager\Event;
    use Collision\EventManager\SharedEventEmitter;

    const EXECUTIONS = 1000000;

/*
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $event = new Event('test');
        
        //var_dump($event->getName());
    }
*/
/*
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $event = new Event('test');
        
        //var_dump($event->isStopped());

        $event->stop();
        if (!$event->isStopped()) {
            die('Event Should be stopped');
        }
    }
*/
    $event = new Event('test');
    for ($i = 0; $i < EXECUTIONS; $i++) {
        $eventEmitter = new SharedEventEmitter();
        $counter = 0;

        $eventEmitter->on('test', function(EventInterface $event) use(&$counter) {$counter++;});

        $eventEmitter->emit($event);
        if ($counter != 1) {
            die('Event Should have been triggered');
        }
    }
}
