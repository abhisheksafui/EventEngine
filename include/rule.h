/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   rule.h
 * Author: abhi
 *
 * Created on 16 April, 2018, 3:02 PM
 */

#ifndef RULE_H
#define RULE_H

#include <chrono>
#include <list>
#include <vector>
#include <memory>
#include <event.h>

using namespace std;
using namespace std::chrono;

namespace EventEngine {

  /**
   * Blueprint for a rule object that has two things: 
   * 1. A match criteria for a rule   
   * 2. A output event that triggers if criteria matches
   */
  typedef function<bool(Event::Ptr) > MatchFunction;
  typedef function<void()> TimeoutFunction;
  
  class Rule {
    
    /** List of rule match functions **/ 
    list<MatchFunction> matchCriteriaList;
    /** List of output events **/
    vector<OutputEvent::Ptr> outputEventList;
    /** List of timeout events **/
    list<TimeoutFunction> timeoutEventList;

    /** once or everytime*/
    bool _persist;
    /** timestamp for auto cleanup of this rule*/
    system_clock::time_point _expiryTime;
    milliseconds _millis;

  public:
    typedef shared_ptr<Rule> Ptr;

    Rule();
    virtual ~Rule();

    bool execute(Event::Ptr eventPtr);

    void addCriteria(MatchFunction func);

    void addOutputEvent(OutputEvent::Ptr);
    void addTimeoutEvent(TimeoutFunction func);
    void setPersistance(bool persist);
    void setLifespan(int time);

    /** Mark expired for removal of this rule **/
    void setExpired();
    /** check if rule is expired **/ 
    bool expired();
    

    bool isPersistant() {
      return _persist;
    }

    /**
     * Helper function to create rule shared_ptr with one outputEvent.
     * @param criteria
     * @param outputEvent
     * @param persistant
     * @param lifespan
     * @return 
     */
    static Rule::Ptr createRule(MatchFunction criteria,
                                    OutputEvent::Ptr outputEvent,
                                    bool persistant = false,
                                    long lifespan = 0);
  };
}
#endif /* RULE_H */

