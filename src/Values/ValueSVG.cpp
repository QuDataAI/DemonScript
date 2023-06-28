#include "ValueSVG.h"
#include "ValueManager.h"
#include "GraphManager.h"
#include "Script/Modules/ModuleManager.h"
#include "Script/Modules/FileModule.h"

string ValueSVG::toString()
{
   return _val;
}

UInt ValueSVG::size()
{   
   return _val.length();
}