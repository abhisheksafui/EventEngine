#include <event.h>

namespace EventEngine {

    OutputEvent::OutputEvent(void *data, OutputEventCallback call)
    : context(data), callback(call) {

    }

    void OutputEvent::trigger(Event::Ptr inputEvent) {
        callback(inputEvent, context);
    }

}
