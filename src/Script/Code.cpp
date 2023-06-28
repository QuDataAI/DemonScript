#include "Code.h"
#include "Variable.h"
#include "Command.h"
#include "Stack/StackFrame.h"

Code::Code():
   _parent(0),
   _shift(0)
{
   add_permitted_global_variable("GRAPH");     //!< переменная GRAPH видна везде
}

Code::Code(CodePtr par):
   Code()
{ 
   _parent = par; 
   if (par) 
      _shift = par->_shift + 3; 
}


Code::~Code()
{
   _local_variables.clear();
   _static_variables.clear();
   _commands.clear();
}

//=======================================================================================
//                                   Code
//=======================================================================================
// Выполнить последовательность комманд,  ret=1, если был return   
//
void Code::run(Int &ret, Value &retVal, ScriptPtr script)
{
   StackFrame frame(*this);
   ret = 0;
   for (UInt i = 0; i < _commands.size(); i++) {
      if (!frame.step(i))
      {
         ret = Command::WAS_RETURN;
         return;
      }         
      _commands[i]->run(ret, retVal, script);        // выполняем каждую команду
      if (ret)                                       // если это return
         return;                                     // прерываемся
   }   
}
//=======================================================================================
// Вывести в поток out код code в виде текста (программы)
//
ostream& Code::print(ostream& out, shared_ptr<Script> script)
{
   string shift; shift.assign(_shift, ' ');          // _shift штук пробелов
   for (UInt i = 0; i < _commands.size(); i++)
      if (_commands[i]) {
         if (i > 0)
            out << "\n";
         out << shift; _commands[i]->print(out, script);
      }
   return out;
}
//=======================================================================================
// Вывести информацию о вычислениях кода
//
ostream& Code::out_info(ostream& out, UInt shift)
{
   for (UInt i = 0; i < _commands.size(); i++)
      if (_commands[i])
         _commands[i]->out_info(out, shift);
   return out;
}
void Code::add_permitted_global_variable(const string & name)
{
   _permittedGlobalVariables.push_back(name);
}

//=======================================================================================
// Найти переменную в данном блоке или в его родителях _parent (если local=0)
//
VariablePtr Code::find_variable(const string &name, Int local)
{
   for (UInt i = 0; i < _local_variables.size(); i++)
      if (_local_variables[i]->name() == name)
         return _local_variables[i];                    // есть в этом блоке как локальная

   for (UInt i = 0; i < _static_variables.size(); i++)
      if (_static_variables[i]->name() == name)
         return _static_variables[i];                    // есть в этом блоке как статическая

   if (!local && _parent)                          // есть родитель
      return _parent->find_variable(name);        // ищем в нём

   return 0;                                      // не нашли и корневой блок
}

bool Code::is_global_variable_permitted(const string & name)
{
   for (size_t i = 0; i < _permittedGlobalVariables.size(); i++)
      if (_permittedGlobalVariables[i] == name)
         return true;

   if (_parent)                                             // есть родитель
      return _parent->is_global_variable_permitted(name);   // ищем в нём

   return false;                                 // не нашли и в корневом блоке
}

void Code::get_variables(ScriptPtr script, vector<VariablePtr>& vars)
{ 
   for (size_t i = 0; i < script->_code->_permittedGlobalVariables.size(); i++)
   {
      VariablePtr globalVariable = script->_code->get_variable(script->_code->_permittedGlobalVariables[i]);
      if (!globalVariable)
         continue;
      if (find(vars.begin(),vars.end(), globalVariable) == vars.end())
         vars.push_back(globalVariable);
   }

   for (size_t i = 0; i < _permittedGlobalVariables.size(); i++)
   {
      VariablePtr globalVariable = script->_code->get_variable(_permittedGlobalVariables[i]);
      if (!globalVariable)
         continue;
      if (find(vars.begin(), vars.end(), globalVariable) == vars.end())
         vars.push_back(globalVariable);
   }

   for (size_t i = 0; i < _local_variables.size(); i++)
   {
      if (find(vars.begin(), vars.end(), _local_variables[i]) == vars.end())
         vars.push_back(_local_variables[i]);
   }

   for (size_t i = 0; i < _static_variables.size(); i++)
   {
      if (find(vars.begin(), vars.end(), _static_variables[i]) == vars.end())
         vars.push_back(_static_variables[i]);
   }

   if (_parent)                                             // есть родитель
      return _parent->get_variables(script, vars);          // забираем и его переменные
}

//=======================================================================================
// Добавить локальную переменную с именем name и получить на неё ссылку 
//
VariablePtr Code::add_local_variable(const string &name)
{
   auto var = SPTR_MAKE(Variable)(name);                      // создаём и добавляем новую
   _local_variables.push_back(var);
   return _local_variables[_local_variables.size() - 1];      // возвращаем на неё ссылку
}
//=======================================================================================
// Добавить статическую переменную с именем name и получить на неё ссылку 
//
VariablePtr Code::add_static_variable(const string &name)
{
   auto var = SPTR_MAKE(Variable)(name);                       // создаём и добавляем новую
   _static_variables.push_back(var);
   return _static_variables[_static_variables.size() - 1];     // возвращаем на неё ссылку
}
//=======================================================================================
// Получить ссылку на переменную с именем name, в *add=1, если это новая
//
VariablePtr Code::get_variable(const string &name, Int * add)
{
   VariablePtr var = find_variable(name);
   if (var) {
      if (add) *add = 0;
      return var;                                 // такая уже существует
   }
   if (add) *add = 1;
   return add_local_variable(name);                     // добавляем новую
}
