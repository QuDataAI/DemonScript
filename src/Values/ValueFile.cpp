#include "ValueFile.h"
#include "ValueManager.h"
#include "GraphManager.h"
#include "Script/Modules/ModuleManager.h"
#include "Script/Modules/FileModule.h"

string ValueFile::toString()
{
   return _name;
}

UInt ValueFile::size()
{
   FileModule * module = (FileModule *)(ModuleManager::instance()->getModule("File"));
   if (module)
   {
      return module->size(_fileId);
   }
   return 0;
}