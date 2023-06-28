/*!            Math - модуль математических функций используемых скриптом

Доступ к функциям модуля осуществляется через вызов Math.<имя функции>(<аргументы>)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef MathModuleH
#define MathModuleH

#include "Module.h"
#include "ModuleFunction.h"
#include <random>

//=======================================================================================
//! Модуль математических функций
// 
class MathModule : public Module
{
   INTERFACE_MODULE(MathModule);
public:
   MathModule();
   /*!
   Получить вещественное число в диапазоне [0,1) 
   \return вещественное число в диапазоне [0,1) 
   */
   Float random();
   /*!
   Установить начальную позицию генератора
   \return вещественное число в диапазоне [0,1)
   */
   void randomSeed(UInt seed);
   /*!
   Получить целое число в заданном диапазаоне с заданным шагом
   \return число
   */
   Int randomRange(Int start, Int stop, Int step = 1);
private:
   std::mt19937                     _randomGenerator;      //!< генератор случайных чисел
   std::uniform_real_distribution<> _randomDistr0_1;       //!< распределение вещественных чисел от 0 до 1
   
};

//=======================================================================================
//! Возвращает абсолютное значение числа.
// 
class MathFuncAbs : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAbs();
};
//=======================================================================================
//! Возвращает арккосинус числа.
// 
class MathFuncAcos : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAcos();
};
//=======================================================================================
//! Возвращает гиперболический арккосинус числа.
// 
class MathFuncAcosh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAcosh();
};
//=======================================================================================
//! Возвращает арксинус числа.
// 
class MathFuncAsin : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAsin();
};
//=======================================================================================
//! Возвращает гиперболический арксинус числа.
// 
class MathFuncAsinh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAsinh();
};
//=======================================================================================
//! Возвращает арктангенс числа.
// 
class MathFuncAtan : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAtan();
};
//=======================================================================================
//! Возвращает арктангенс от частного своих аргументов.
// 
class MathFuncAtan2 : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAtan2();
};
//=======================================================================================
//! Возвращает гиперболический арктангенс числа.
// 
class MathFuncAtanh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncAtanh();
};
//=======================================================================================
//! Возвращает кубический корень числа.
// 
class MathFuncCbrt : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncCbrt();
};
//=======================================================================================
//! Возвращает наименьшее целое число, большее, либо равное указанному числу.
// 
class MathFuncCeil : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncCeil();
};
//=======================================================================================
//! Возвращает косинус числа.
// 
class MathFuncCos : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncCos();
};
//=======================================================================================
//! Возвращает гиперболический косинус числа.
// 
class MathFuncCosh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncCosh();
};
//=======================================================================================
//! Возвращает Ex, где x — аргумент, а E — число Эйлера (2,718…), основание натурального логарифма.
// 
class MathFuncExp : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncExp();
};
//=======================================================================================
//! Возвращает exp(x), из которого вычли единицу.
// 
class MathFuncExpm1 : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncExpm1();
};
//=======================================================================================
//! Возвращает наибольшее целое число, меньшее, либо равное указанному числу.
// 
class MathFuncFloor : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncFloor();
};
//=======================================================================================
//! Возвращает квадратный корень из суммы квадратов своих аргументов.
// 
class MathFuncHypot : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncHypot();
};
//=======================================================================================
//! Возвращает натуральный логарифм числа (loge, также известен как ln).
// 
class MathFuncLog : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncLog();
};
//=======================================================================================
//! Возвращает десятичный логарифм числа.
// 
class MathFuncLog10 : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
      MathFuncLog10();
};
//=======================================================================================
//! Возвращает натуральный логарифм числа 1 + x (loge, также известен как ln).
// 
class MathFuncLog1p : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
      MathFuncLog1p();
};
//=======================================================================================
//! Возвращает двоичный логарифм числа.
// 
class MathFuncLog2 : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncLog2();
};
//=======================================================================================
//! Возвращает наибольшее число из своих аргументов.
// 
class MathFuncMax : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncMax();
};
//=======================================================================================
//! Возвращает наименьшее число из своих аргументов.
// 
class MathFuncMin : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncMin();
};
//=======================================================================================
//! Возведение в степень
// 
class MathFuncPow : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncPow();
};
//=======================================================================================
//! Установить начальную позицию генератора
// 
class MathFuncRandomSeed : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncRandomSeed();
};
//=======================================================================================
//! Получить вещественное число в диапазоне [0,1)
// 
class MathFuncRandom : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncRandom();
};
//=======================================================================================
//! Получить целое случайное число в диапазоне
// 
class MathFuncRandRange : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncRandRange();
};
//=======================================================================================
//! Возвращает значение числа, округлённое до ближайшего целого.
// 
class MathFuncRound : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION
   MathFuncRound();
};
//=======================================================================================
//! Возвращает знак числа, указывающий, является ли число положительным, отрицательным или нулём.
// 
class MathFuncSign : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncSign();
};
//=======================================================================================
//! Возвращает синус числа.
// 
class MathFuncSin : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncSin();
};
//=======================================================================================
//! Возвращает гиперболический синус числа.
// 
class MathFuncSinh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncSinh();
};
//=======================================================================================
//! Взятие корня
// 
class MathFuncSqrt : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncSqrt();
};
//=======================================================================================
//! Возвращает тангенс числа.
// 
class MathFuncTan : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncTan();
};
//=======================================================================================
//! Возвращает гиперболический тангенс числа.
// 
class MathFuncTanh : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncTanh();
};
//=======================================================================================
//! Возвращает целую часть числа, убирая дробные цифры.
// 
class MathFuncTrunc : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   MathFuncTrunc();
};

#endif
