Event Engine that accepts events and executes programmed rules, that consist of
list of match functions, output events and timeout events.

Event Engine Rules match event against list of match functions. If matched then 
EventEngine triggers output event. 

Rules may be persistent or non persistant. If persistent then it may exist forever or
for a limited amount of time. Else if non persistant it is removed on first match 
event.  
