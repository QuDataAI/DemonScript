/*!            System - системные функции скрипта

Доступ к функциям модуля осуществляется через вызов System.<имя функции>(<аргументы>)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef MathH
#define MathH

#include "Module.h"
#include "ModuleFunction.h"


//=======================================================================================
//! Модуль системных функций
// 
class System : public Module
{
   INTERFACE_MODULE(System);
public:
   System();
   /*!
   Установить текущую версию приложения
   \param val версия приложения
   */
   void version(Int val) { _version = val; }
   /*!
   Получить текущую версию приложения в строковом виде
   \param val версия приложения
   */
   string versionStr();
   /*!
   Получить текущую версию приложения в числовом виде
   \param val версия приложения
   */
   Int version() { return _version; }
   /*!
   Получить число мс при последнем вызове функции System.time()
   \return число мс при последнем вызове функции System.time()
   */
   clock_t lastTimer() { return _lastTimer; }
   /*!
   Установить число мс при последнем вызове функции System.time()
   \param val число мс при последнем вызове функции System.time()
   */
   void lastTimer(clock_t val) { _lastTimer = val; }
private:
   clock_t  _lastTimer;                      //число мс при последнем вызове функции System.time()
   Int      _version;                        //версия приложения
};

//=======================================================================================
//! Получить текущую версию приложения в числовом виде
// 
class SystemFuncVersion : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Получить текущую версию приложения в строковом виде
// 
class SystemFuncVersionStr : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Получить флаг, была ли ошибка
// 
class SystemFuncError : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Получить сообщение о последней ошибке
// 
class SystemFuncErrorMsg : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Получить число ms после последнего вызова данной функции
// 
class SystemFuncTime : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   SystemFuncTime();
};

//=======================================================================================
//! Вывести статистику работы скрипта
// 
class SystemFuncStat : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Вывести текущую строку скрипта
// 
class SystemFuncLine : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Получить 
// 
class SystemFuncArgs : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public: 
   static shared_ptr<ValueArr> args;
};

//=======================================================================================
//! Установить уровень логирования 
// 
class SystemFuncTraceLevel : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
      SystemFuncTraceLevel();
};

#endif
