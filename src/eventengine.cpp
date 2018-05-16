/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <Poco/NotificationQueue.h>
#include "eventengine.h"
#include "event.h"
#include <rule.h>
#include <memory>
#include <iostream>
#include <list>
using namespace std;

namespace EventEngine {

    Engine::Engine() : idleWorkTimeout(1000), eventCounter(1), logger("EventEngine") {
        _processingEvents = false;
    }

    void Engine::run() {

        system_clock::time_point timeStamp = system_clock::now();

        while (!isCancelled()) {

            Notification::Ptr event = eventNotificationQueue.waitDequeueNotification(100);

            if (event) {
                Poco::ScopedLock<Mutex> lock(_ruleOperationMutex);
                _processingEvents = true;

                /** 
                 * At this stage, if new rules are added by event output, then they will
                 * be added to pending list after taking recursive lock on mutex.
                 * 
                 * Other threads will wait on mutex.
                 */

                Event::Ptr abstractEvent = event.cast<Event>();
                logger.log(DEBUG, "Engine Processing Event: %u", abstractEvent->getEventID());
                int count = 1;
                auto it = ruleList.begin();
                while (it != ruleList.end()) {
                    logger.log(DEBUG, "Rule Count: %d", ruleList.size());
                    logger.log(DEBUG, "Trying to match rule number %d", count);
                    bool result = false;
                    
                    /**
                     * If exception occurs in execute()
                     */
                    try{
                        result = (*it)->execute(abstractEvent);
                    }catch(exception &e){
                        logger.log(ERROR, "Exception [%s] caught while executing rule.", e.what());
                    }
                    
                    
                    if (result == true) {
                        logger.log(DEBUG, "HIT Rule number %d", count);
                        if ((*it)->isPersistant() == false) {
                            logger.log(DEBUG, "Rule not persistant, removing rule", count);
                            it = ruleList.erase(it);
                        } else {
                            logger.log(DEBUG, "Rule persistant", count);
                            it++;
                        }
                    } else {
                        logger.log(DEBUG, "NO HIT Rule number %d", count);
                        it++;
                    }

                }
                _processingEvents = false;
            }

            /**
             *  after every event or loop dequeue timeout check if idleWorkTimeOut
             *  has expired if so then do
             * 
             *  1. Remove expired rules 
             *  2. idle work 
             *  3. update timestamp
             * 
             */

            if (system_clock::now() - timeStamp > milliseconds(idleWorkTimeout)) {

                Poco::ScopedLock<Mutex> lock(_ruleOperationMutex);
                _processingEvents = false;

                /** 
                 * At this stage, if new rules are added by event timeout, then they will
                 * be added to pending list after taking recursive lock on mutex.
                 * 
                 * Other threads will wait on mutex.
                 */
                
                list<Rule::Ptr>::iterator it = ruleList.begin();

                
                while (it != ruleList.end()) {
                    bool expired = false;
                    try{
                       expired = (*it)->expired(); 
                    }catch (exception &e){
                        logger.log(ERROR, "Exception [%s] caught by event engine while checking rule expiry. Rule will be removed", e.what());
                        expired = true;
                    }
                    if (expired == true) {
                        logger.log(DEBUG, "Rule expired removing it after calling timeout event");
                        it = ruleList.erase(it);
                    } else {
                        it++;
                    }
                }
                
                _processingEvents = false;



                if (idleWork) {
                    idleWork(workContext);
                }
                timeStamp = system_clock::now();
            }

            //            cout << "rule count: " << ruleList.size() << " pending rule count: "
            //                    << _addPendingList.size() << endl;
            if (_addPendingList.size() > 0) {
                ruleList.splice(ruleList.end(), _addPendingList);
            }
            //            cout << "rule count: " << ruleList.size() << " pending rule count: "
            //                    << _addPendingList.size() << endl;
        }
    }

    void Engine::cancel() {
        cancelled = true;
    }

    void Engine::registerIdleWork(void* context, function<void(void*) > func) {
        workContext = context;
        idleWork = func;
    }

    void Engine::addRule(Rule::Ptr rule) {
        Poco::ScopedLock<Mutex> lock(_ruleOperationMutex);
        if (_processingEvents) {
            _addPendingList.push_back(rule);
        } else {
            ruleList.push_back(rule);
        }
    }

    void Engine::postEvent(Event::Ptr event) {
        /** TODO take the lock*/
        {
            Poco::ScopedLock<FastMutex> lock(_eventCounterMutex);
            event->setEventID(eventCounter++);
        }
        logger.log(DEBUG, "Engine Posting Event: %u", event->getEventID());
        event->setEngine(shared_from_this());
        eventNotificationQueue.enqueueNotification(event);
    }

    int Engine::getRuleCount() {
        Poco::ScopedLock<Mutex> lock(_ruleOperationMutex);
        return ruleList.size();
    }
}
