#include <iostream>
#include <event.h>
#include <chrono>
#include <eventengine.h>
#include <Poco/Thread.h>
using namespace std;
using namespace EventEngine;

class IntegerEvent : public EventEngine::Event {
public:

    IntegerEvent(int i) : value(i) {
    }
    typedef Poco::AutoPtr<IntegerEvent> Ptr;
    int value;
};

class IntegerGenerator : public Runnable {
    Engine::Ptr engine;
public:

    IntegerGenerator(Engine::Ptr e) : engine(e) {
    }

    void run() {
        IntegerEvent::Ptr event = new IntegerEvent(1);

        while (true) {
            //event->setEngine(engine->shared_from_this());
            engine->postEvent(event);
            Poco::Thread::sleep(2000);
        }
    }
};

int generateInteger() {

}

int main(int argc, char *argv[]) {

    cout << "Hello world" << endl;
    int count = 1;

    EventEngine::Engine::Ptr enginePtr = make_shared<EventEngine::Engine>();
    enginePtr->registerIdleWork(nullptr, [](void *data) {
        cout << "Idle Work.." << endl;
    });

    Poco::Thread thread("generator");

    IntegerGenerator generator = IntegerGenerator(enginePtr);

    int value = 1;
    
    OutputEvent::Ptr output = make_shared<OutputEvent>(nullptr, [&count](EventEngine::Event::Ptr, void *) ->void {
        cout << "PING: " << count++ << endl; });
        
    Rule::Ptr rule = Rule::createRule([&value](EventEngine::Event::Ptr event){
        IntegerEvent::Ptr integerEvent = event.cast<IntegerEvent>();
        if(integerEvent.isNull()){ return false; }
        if(integerEvent->value == value ){ return true; }
        return false;
    },output, true, 5000);


    enginePtr->addRule(rule);

    thread.start(generator);
    /** Run the event engine here **/
    enginePtr->run();

    return 0;
}
