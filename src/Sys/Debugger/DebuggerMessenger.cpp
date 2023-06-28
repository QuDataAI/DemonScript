#include "DebuggerMessenger.h"

void DebuggerMessenger::pushMessageForDebugger(Json::Value & ev)
{
   MessengerLocker locker(_eventQueueMutex);
   _messagesForDebugger.push(ev);
}

bool DebuggerMessenger::popMessageForDebugger(Json::Value & ev)
{
   MessengerLocker locker(_eventQueueMutex);
   if (_messagesForDebugger.empty())
      return false;

   ev = _messagesForDebugger.front();
   _messagesForDebugger.pop();

   return true;
}

void DebuggerMessenger::pushMessageForApp(Json::Value & ev)
{
   MessengerLocker locker(_eventQueueMutex);
   _messagesForApp.push(ev);
}

bool DebuggerMessenger::popMessageForApp(Json::Value & ev)
{
   MessengerLocker locker(_eventQueueMutex);
   if (_messagesForApp.empty())
      return false;

   ev = _messagesForApp.front();
   _messagesForApp.pop();

   return true;
}
