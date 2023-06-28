/*!            Module - базовый класс модулей скрипта

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef ModuleH
#define ModuleH

#include "ModuleFunction.h"

using namespace std;

#define	INTERFACE_MODULE(Class)	   	                     \
public:												                  \
   /* получить имя модуля */                                \
   string name() {return _name;}                            \
   /* получить экземпляр модуля */                          \
   static Class* instance();                                \
private:											                     \
   static bool				_registered;     /* флаг регистрации функции */                \
   static const char*   _name;           /* имя модуля */

#define	IMPLEMENTATION_MODULE(Class,Name)	                                          \
Class*       Class::instance() { static Class inst; return &inst;}                     \
const char*  Class::_name = Name;                                                      \
bool 	       Class::_registered = ModuleManager::instance()->registerModule(Class::instance());

//=======================================================================================
//! Интерфейс модуля скрипта
// 
class Module
{
public:
   /*!
      Получить имя текущего модуля, доступное из скрипта
      \return имя текущего модуля
   */
   virtual string name() = 0;
   /*!
      Тип функции создания модуля
   */
   typedef Module* (*TModuleCreator)();
   /*!
   Регистратор функций модуля
   \return результат регистрации
   */
   bool  registerFunction(string name, ModuleFunction::TModuleFunctionCreator creator);
   /*!
   Регистратор константы модуля
   \return результат регистрации
   */
   bool  registerConstant(string name, const Value &val);
   /*!
   Получить экземпляр функции по имени. Если экземпляр еще не создан - создать его.
   \name имя функции
   \return экземпляр функции
   */
   ModuleFunctionPtr   getFunction(string name);
   /*!
   Получить константу по имени.
   \name имя константы
   \val значение константы
   \return результат поиска константы
   */
   bool   getConstant(string name, Value &val);
   /*!
   Установить текущий скрипт модуля
   \name указатель на скрипт
   */
   void   setScript(Script* script) { _script = script; }

protected:
   Script*                                   _script = 0;   //!< ссылка на скрипт
private:
   class ModuleFunctionRegisterInfo {
   public:
      ModuleFunctionRegisterInfo() {}
      ModuleFunctionRegisterInfo(string &name, ModuleFunction::TModuleFunctionCreator& creator) : _name(name), _creator(creator) {}
      string                                 _name;         //!< имя функции
      ModuleFunction::TModuleFunctionCreator _creator;      //!< метод создания экземпляра функции
      ModuleFunctionPtr                      _function;     //!< экземпляр функции
   };
   class ModuleConstantRegisterInfo {
   public:
      ModuleConstantRegisterInfo() {}
      ModuleConstantRegisterInfo(string &name, const Value& val) : _name(name), _val(val) {}
      string                                 _name;          //!< имя константы
      Value                                  _val;           //!< значение константы
   };
   vector<ModuleFunctionRegisterInfo>        _funcRegInfos;  //!< регистрационные данные функций 
   vector<ModuleConstantRegisterInfo>        _constRegInfos; //!< регистрационные данные констант 
   
};

#endif
