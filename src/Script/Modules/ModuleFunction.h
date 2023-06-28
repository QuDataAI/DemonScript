/*!            ModuleFunction - функция модуля и интерфейс регистрации функции на фабрике


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef ModuleFunctionH
#define ModuleFunctionH

#include "Function.h"

class Module;

#define	INTERFACE_MODULE_FUNCTION					            \
public:												                  \
   /* получить имя функции */                               \
   string name() {return _funcName;}                        \
   /* получить имя функции */                               \
   string moduleName() {return _moduleName;}                \
   /* вернуть значение функции */                           \
   Value  run();                                            \
   /* метод создания экземпляра функции*/                   \
   static shared_ptr<ModuleFunction> create();	            \
private:											                     \
   static bool				_registered;     /* флаг регистрации функции */        \
   static const char*   _funcName;       /* имя функции */                     \
   static const char*   _moduleName;     /* имя модуля */

#define	IMPLEMENTATION_MODULE_FUNCTION(Module,Class,Name)	                                    \
shared_ptr<ModuleFunction> Class::create() {return shared_ptr<ModuleFunction>(new Class());}	                                          \
const char*     Class::_funcName   = Name;                                                      \
const char*     Class::_moduleName = #Module;                                                   \
bool		       Class::_registered = Module::instance()->registerFunction(Name, &Class::create);  

//=======================================================================================
//! Интерфейс функции модуля
// 
SPTR_DEF(ModuleFunction)
class ModuleFunction: public Function
{
public:
   vector<ExprPtr>                 _args;         //!< аргументы функции              
   Module*                         _module;       //!< родительский для функции модуль
   Script*                         _script;       //!< ссылка на скрипт в котором запускается модуль
   /*!
      Инициализируем запуск функции:
         - инкрементируем счетчик вызовов _num_calls++
         - вычисляем аргументы и значение объекта
         - проверяем корректность типов
      \return результат инициализации
   */
   virtual Bool initRun();
   /*!
      Вычислить функцию и вернуть значение
      \return результат выполнения функции
   */
   virtual Value run() = 0;
   /*!
      Выполнение функции с аргументами args для объекта obj
      \return результат выполнения функции
   */
   virtual Value run(vector<ExprPtr> & args, ExprPtr _obj = 0) { _args = args; return run(); }
   /*!
      Получить имя функции
      \return имя функции
   */
   virtual string name() = 0;
   /*!
      Получить имя модуля функции
      \return имя модуля функции
   */
   virtual string moduleName() = 0;
   /*!
   Получить полное имя функции
   \return результат инициализации
   */
   virtual string fullName() { return (moduleName() + "." + name()); }
   /*!
   Разрешать использовать непроинициализированные переменные в аргументах функции
   \return результат проверки разрешения
   */
   virtual Bool allowNewVarsInArgs() { return false; }
   /*!
      Вывести имя модуля и функции в поток
      \return имя функции
   */
   ostream & print(ostream & out)
   {
      out << fullName() << "(";
      for (size_t i = 0; i < _args.size(); i++)
      {
         _args[i]->print(out);
         if (i + 1 < _args.size()) out << ",";
      }      
      return out << ")";
   }
   /*!
   Тип функции создания модуля
   */
   typedef shared_ptr<ModuleFunction> (*TModuleFunctionCreator)();
};


#endif
