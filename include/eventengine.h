/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   eventengine.h
 * Author: abhi
 *
 * Created on 16 April, 2018, 5:07 PM
 */

#ifndef EVENTENGINE_H
#define EVENTENGINE_H
#include <Poco/Runnable.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <rule.h>
#include <list>
#include <memory>
#include "MyLogger.h"
using namespace Poco;

namespace EventEngine {
  /**
   * A simple engine to accept events and match against programmed rules. 
   * If matched, execute the rule output
   */
  class Engine : public Runnable,public  std::enable_shared_from_this<Engine> {
  private:
    
    /**
     * Event queue. 
     */
    Poco::NotificationQueue eventNotificationQueue;
    
    

    /**
     * Mutex to allow same thread recursively lock. And to block multiple thread
     * concurrent access.
     */
    Poco::Mutex _ruleOperationMutex;
    /**
     * Flag to mark event processing stage. 
     * So that rules are added to pending list. This prevents rule output or
     * timeout adding rule while iterating the list of rules.
     */
    bool _processingEvents;
    list<Rule::Ptr> ruleList;
    list<Rule::Ptr> _addPendingList;

    Poco::FastMutex _eventCounterMutex;
    uint32_t eventCounter;
    
    int idleWorkTimeout;
    function<void(void *) > idleWork;
    void *workContext;

    bool cancelled;

    bool isCancelled() {
      return cancelled;
    }

    MyLogger logger;

  public:
    
    typedef shared_ptr<Engine> Ptr;
    
    Engine();
    
    void run();
    
    void cancel();
    
    void registerIdleWork(void *context, function<void(void *) >func);

    void addRule(Rule::Ptr rule);

    void postEvent(Event::Ptr event);

    Poco::NotificationQueue& getQueue() {
      return eventNotificationQueue;
    }

    int getRuleCount();
  };
}
#endif /* EVENTENGINE_H */

