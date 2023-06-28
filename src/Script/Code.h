/*!            Code - Последовательность команд Command скрипта

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef CodeH
#define CodeH

#include "Value.h"          // Определение базовых типов

//=======================================================================================
// Предварительное объявление классов:
SPTR_DEF(Variable);      //!< переменная скрипта
SPTR_DEF(Command);       //!< команда скрипта
SPTR_DEF(Script);        //!< скрипт
SPTR_DEF(Code)

//=======================================================================================
//! Последовательность команд скрипта
/*!
*/
class Code: SPTR_ENABLE_FROM_THIS(Code)
{
public:
   CodePtr              _parent;            //!< родительский (вышестоящий) блок кода
   vector<VariablePtr>  _local_variables;   //!< локальные переменные блока
   vector<VariablePtr>  _static_variables;  //!< статические переменные блока
   vector<CommandPtr>   _commands;          //!< последовательность команд блока скрипта
   Int                  _shift;             //!< размер сдвига кода вправо при выводе на консоль

   Code();
   Code(CodePtr par);
   ~Code();

   //! Выполнить последовательность комманд,  ret=1, если был return   
   void run(Int &ret, Value &retVal, ScriptPtr script);
   //! Добавить команду
   void add_command(CommandPtr cmd) { _commands.push_back(cmd); };
   //! Получить ссылку на переменную с именем name, в *add=1, если это новая
   VariablePtr get_variable(const string &name, Int * add = 0);
   //! Добавить локальную переменную с именем name и получить на неё ссылку 
   VariablePtr add_local_variable(const string &name);
   //! Добавить локальную переменную с именем name и получить на неё ссылку 
   VariablePtr add_static_variable(const string &name);
   //! Найти переменную в данном блоке или в его родителях _parent (если local=0)
   VariablePtr find_variable(const string &name, Int local = 0);
   //! Вывести в поток out код code в виде текста (программы)
   ostream & print(ostream & out, shared_ptr<Script> script = 0);
   //! Вывести информацию о вычислениях кода
   ostream & out_info(ostream & out, UInt shift = 0);
   //! Добавить имя глобальной переменной, к которой разрешили обращаться из блока командой global
   void add_permitted_global_variable(const string& name);
   //! Проверить, разрешено ли использовать глобальную переменную с таким именем в текущем блоке кода
   bool is_global_variable_permitted(const string &name);
   //! Получить список всех переменных, доступных из текущего кода
   void get_variables(ScriptPtr script, vector<VariablePtr> &vars);
private:
   vector<string> _permittedGlobalVariables;   //!< имена глобальных переменных, к которым разрешен доступ из текущего блока
};

#endif