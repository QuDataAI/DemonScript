#include "ErrorManager.h"

void ErrorManager::error(string msg)
{
   _lastErrorProcessed = false;
   _lastErrorMsg       = msg;
}