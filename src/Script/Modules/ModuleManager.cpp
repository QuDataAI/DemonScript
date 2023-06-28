#include "ModuleManager.h"

ModuleManager * ModuleManager::instance()
{
   static ModuleManager manager;
   return &manager;
}

bool ModuleManager::registerModule(Module *module)
{
   if (getModule(module->name()))
      return true;

   _modules.push_back(module);
   return true;
}

Module * ModuleManager::getModule(const string &name)
{
   for (int i = 0; i < _modules.size(); i++)
   {
      if (_modules[i]->name() == name)
         return _modules[i];
   }
   return 0;
}

void ModuleManager::setScript(Script * script)
{
   for (auto module: _modules)
   {
      module->setScript(script);
   }
}

