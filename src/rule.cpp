#include <rule.h>
#include <chrono>
#include <iostream>
#include "event.h"


namespace EventEngine {

    Rule::Rule() : _persist(false), _millis(0) {

    }

    Rule::~Rule() {

    }

    /**
     * Function to match all criteria and if all criteria satisfies then 
     * calls output function and return true.
     * 
     * Multiple match criteria helps to break up the match condition into 
     * multiple reusable functions.
     * 
     * All Match criteria must return true to execute the output.
     * 
     * @param eventPtr
     * @return true : if matchCriteria is set and all matchCriteria matches
     *          false : if no matchCriteria set or any one of the matchCriteria return false
     */
    bool Rule::execute(Event::Ptr eventPtr) {

        if (matchCriteriaList.size() == 0) {
            cout << "No match criteria set for this rule. Returning false" << endl;
            return false;
        }
        
        /** 
         * Set the pointer to the rule so that it is available to match 
         * function and outputevent through the event
         */
        eventPtr->setRule(this);
        for (auto f : matchCriteriaList) {
            if (f(eventPtr) == false) {
                return false;
            }
        }

        for (auto event : outputEventList) {
            event->trigger(eventPtr);
        }

        return true;
    }

    void Rule::addCriteria(MatchFunction func) {
        matchCriteriaList.push_back(func);
    }

    void Rule::addOutputEvent(OutputEvent::Ptr output) {
        outputEventList.push_back(output);
    }
    
    void Rule::addTimeoutEvent(TimeoutFunction func) {
        timeoutEventList.push_back(func);
    }


    void Rule::setPersistance(bool persist) {
        _persist = persist;
    }

    void Rule::setLifespan(int millis) {
        _millis = milliseconds(millis);
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        _expiryTime = now + _millis;
    }

    bool Rule::expired() {
        if (_millis.count() == 0) {
            return false;
        }
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        if (now > _expiryTime) {
            cout  << "Calling timeout callbacks" << endl;
            for(auto f : timeoutEventList){
                f();
            }
            return true;
        }
        return false;
    }

    Rule::Ptr Rule::createRule(MatchFunction func, OutputEvent::Ptr outputEvent, bool persistant, long lifespan) {
        Rule::Ptr rule = make_shared<Rule>();
        rule->addCriteria(func);
        rule->addOutputEvent(outputEvent);
        rule->setPersistance(persistant);
        rule->setLifespan(lifespan);
        return rule;
    }

    void Rule::setExpired(){
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        _expiryTime = now;
    }

}
