/*!            ModuleManager - менеджер модулей

К данному менеджеру скрипт обращается при компиляции с запросом: "Зарегистрирован ли модуль с таким именем?"
Module * module = ModuleManager::instance()->getModule(<name>)

если зарегистрирован, то у модуля запрашиваем объект функции в который передаем аргументы

ModuleFunction * moduleFunction = module->createFunction(<name>)
if (moduleFunction)
moduleFunction->setArguments(args)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef ModuleManagerH
#define ModuleManagerH

#include "Module.h"
#include <vector>

class Script;
//=======================================================================================
//! Интерфейс модуля скрипта
// 
class ModuleManager
{
public:
   /*!
   Получить экземпляр менеджера модулей
   \return экземпляр менеджера модулей
   */
   static ModuleManager* instance();
   /*!
   Регистратор модулей
   \return имя функции
   */
   bool  registerModule(Module *);

   /*!
   Вывести имя модуля и функции в поток
   \param name имя модуля
   */
   Module * getModule(const string &name);

   /*!
   Установить текущий скрипт для всех модулей
   \name указатель на скрипт
   */
   void   setScript(Script* script);

private:
   vector<Module*>   _modules;   //зарегистрированные модули
};

#endif
