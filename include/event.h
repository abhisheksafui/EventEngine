#ifndef __EVENT_H__
#define  __EVENT_H__

#include <functional>
#include <memory>
#include <Poco/Notification.h>


using namespace std;

namespace EventEngine {

  class Engine;
  class Rule;

class Event : public Poco::Notification {
private:

  /** Identifier for this event given by EventEngine **/
  uint32_t eventID;

  /** Engine pointer set on posting event **/
  shared_ptr<EventEngine::Engine> enginePtr;

  /** Rule sets pointer to itself while execution **/
  Rule *rule;

public:

  Event() {
  }

  virtual ~Event() {
  }

  /** Set by engine **/
  void setEngine(shared_ptr<Engine> engine){
    enginePtr = engine;
  }

  /** Used by OutputEvent to post another event **/
  shared_ptr<Engine> getEngine() { return enginePtr; }

  /** Used by Event engine to mark the event **/
  uint32_t setEventID(uint32_t id){ eventID = id; }

  /** Used by Event engine to print event id while matching **/
  uint32_t getEventID(){ return eventID; }
    
  void setRule(Rule *r){ rule = r; }

  typedef Poco::AutoPtr<Event> Ptr;
};

class OutputEvent : public Event {
 
  /** data pointer to pass to callback **/ 
  void *context;
  function<void(Event::Ptr, void *)> callback;

public:
  typedef shared_ptr<OutputEvent> Ptr;
  typedef function<void(Event::Ptr, void *) > OutputEventCallback;
  OutputEvent(void *data, OutputEventCallback call);
  
  /** Triggered by EventEngine if event matches criteria **/
  void trigger(Event::Ptr event);
  
  static Ptr makeOutputEvent(void *data, OutputEventCallback call){
    return make_shared<OutputEvent>(data,call);
  }
  
  template <class T>
  static Ptr makeOutputEvent(void *data,void (T::*func)(Event::Ptr,void *), T* obj){
    return make_shared<OutputEvent>(data, bind(func,obj,placeholders::_1,placeholders::_2));
  }
  
};

}
#endif 
