#include "Module.h"

bool Module::registerFunction(string name, ModuleFunction::TModuleFunctionCreator creator)
{
   for (int i = 0; i < _funcRegInfos.size(); i++)
   {
      if (_funcRegInfos[i]._name == name) // уже с таким именем регистрация есть
         return true;
   }

   _funcRegInfos.push_back(ModuleFunctionRegisterInfo(name, creator));
   return true;
}

bool Module::registerConstant(string name, const Value & val)
{
   for (int i = 0; i < _constRegInfos.size(); i++)
   {
      if (_constRegInfos[i]._name == name) // уже с таким именем регистрация есть
         return true;
   }

   _constRegInfos.push_back(ModuleConstantRegisterInfo(name, val));
   return true;
}

ModuleFunctionPtr Module::getFunction(string name)
{
   for (int i = 0; i < _funcRegInfos.size(); i++)
   {
      ModuleFunctionRegisterInfo &regInfo = _funcRegInfos[i];
      if (regInfo._name == name)
      {
         if (!regInfo._function)
         {
            //первый запрос функции, создаем экземпляр
            regInfo._function = regInfo._creator();
            regInfo._function->_module = this;
            regInfo._function->_script = _script;
         }
        		   
         return regInfo._function;
      }
   }

   return nullptr;
}

bool Module::getConstant(string name, Value & val)
{
   for (int i = 0; i < _constRegInfos.size(); i++)
   {
      ModuleConstantRegisterInfo &regInfo = _constRegInfos[i];
      if (regInfo._name == name)
      {
         val = regInfo._val;
         return true;
      }
   }

   return false;
}
