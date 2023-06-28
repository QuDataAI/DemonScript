/*!            Mind - модуль логических выводов из множества аксиом

Доступ к функциям модуля осуществляется через вызов Mind.<имя функции>(<аргументы>)

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/
#ifndef MindModuleH
#define MindModuleH

#include "Mind.h"
#include "Modules/Module.h"

#include <vector>
#include <map>
#include <algorithm>    // std::sort

//=======================================================================================
//! Добавление аксиом, которые выполняются всегда
//
class MindFuncAdd : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   /*!
   Разрешать использовать непроинициализированные переменные в аргументах функции
   \return результат проверки разрешения
   */
   Bool allowNewVarsInArgs() { return true; }
};

//=======================================================================================
//! Добавление аксиом, которые выполняются обычно
//
class MindFuncAddUsual : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   /*!
   Разрешать использовать непроинициализированные переменные в аргументах функции
   \return результат проверки разрешения
   */
   Bool allowNewVarsInArgs() { return true; }
};

//=======================================================================================
//! Вывод всех аксиом
//
class MindFuncOut : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Логический вывод
//
class MindFuncSetGraph : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//=======================================================================================
//! Проверка, был ли конфликт аксиом
//
class MindFuncWasConflict : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

//============================================================================================
//! Разделить граф на две части в которых неопределнное ребро исходного графа будет определено
//
class MindFuncSplitGraph : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   MindFuncSplitGraph();
};

//============================================================================================
//! Разделить граф на две части в которых неопределнное ребро исходного графа будет определено с помощью "usual" аксиом
//
class MindFuncSplitGraphUsual : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   MindFuncSplitGraphUsual();
};

//=======================================================================================
//! Установка моды вычислений
//
class MindFuncSetMode : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Очистка аксиом
//
class MindFuncClear : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Очистка аксиом
//
class MindFuncVerbose : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Очистка аксиом
//
class MindFuncGetModels : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Получить правдоподобные модели
//
class MindFuncGetUsualModels : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   MindFuncGetUsualModels();
};
//=======================================================================================
//! Объединить два графа
//
class MindFuncMerge : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
public:
   MindFuncMerge();
};
//=======================================================================================
//! Очистка аксиом
//
class MindFuncCountModels : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Вычисляем значение выржения по всем моделям. Если оно одно и тоже, то возвращаем его
//! Иначе возвращем Undef. Учитывает возможность многозначности логических значений.
//
class MindFuncValModels : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Установить валидатор
//
class MindFuncValidator : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Сбросить статистику использования моделей
//
class MindFuncClearUsed : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Установить группу аксиом
//
class MindFuncGroup : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Установить режим различных переменных
//
class MindFuncDifferntVals : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};
//=======================================================================================
//! Установить режим вывода неопределённых выражений
//
class MindFuncOutUndef : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
};

#endif
