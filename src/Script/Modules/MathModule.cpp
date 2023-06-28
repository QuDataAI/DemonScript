#include "MathModule.h"
#include "ModuleManager.h"

//регистрируем модуль
IMPLEMENTATION_MODULE(MathModule,"Math")

//регистрируем функции модуля
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAbs,        "abs")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAcos,       "acos")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAcosh,      "acosh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAsin,       "asin")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAsinh,      "asinh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAtan,       "atan")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAtan2,      "atan2")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncAtanh,      "atanh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncCbrt,       "cbrt")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncCeil,       "ceil")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncCos,        "cos")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncCosh,       "cosh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncExp,        "exp")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncExpm1,      "expm1")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncFloor,      "floor")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncHypot,      "hypot")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncLog,        "log")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncLog10,      "log10")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncLog1p,      "log1p")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncLog2,       "log2")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncMax,        "max")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncMin,        "min")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncPow,        "pow")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncRandom,     "random")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncRandomSeed, "randomSeed")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncRandRange,  "randrange")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncRound,      "round")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncSign,       "sign")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncSin,        "sin")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncSinh,       "sinh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncSqrt,       "sqrt")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncTan,        "tan")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncTanh,       "tanh")
IMPLEMENTATION_MODULE_FUNCTION(MathModule, MathFuncTrunc,      "trunc")

MathModule::MathModule() :
   _randomDistr0_1(0, 1)
{
   registerConstant("PI", Value(3.141592653589793));
   registerConstant("E", Value(2.718281828459045));
   _randomGenerator.seed((unsigned int)time(0));  // встряхиваем генератор 
}

//=======================================================================================
//! Вернуть случайное число 0..1
//
Float MathModule::random()
{
   return _randomDistr0_1(_randomGenerator);
}
//=======================================================================================
//! Установить seed генератору
//
void MathModule::randomSeed(UInt seed)
{
   _randomGenerator.seed((unsigned int)seed);
}
//=======================================================================================
Int MathModule::randomRange(Int start, Int stop, Int step /*= 1*/)
{
   if (step < 1)
      step = 1;

   uniform_int_distribution<Int>  distr(start, stop);

   Int res = distr(_randomGenerator);
   res -= (res % step);
   if (res < start)
      res = start;

   return res;
}
//=======================================================================================
//! Возвращает абсолютное значение числа.
// 
MathFuncAbs::MathFuncAbs()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает абсолютное значение числа.
// 
Value MathFuncAbs::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = abs(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает арккосинус числа.
// 
MathFuncAcos::MathFuncAcos()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает арккосинус числа.
// 
Value MathFuncAcos::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = acos(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический арккосинус числа.
// 
MathFuncAcosh::MathFuncAcosh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический арккосинус числа.
// 
Value MathFuncAcosh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = acosh(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает арксинус числа.
// 
MathFuncAsin::MathFuncAsin()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает арксинус числа.
// 
Value MathFuncAsin::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = asin(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический арксинус числа.
// 
MathFuncAsinh::MathFuncAsinh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический арксинус числа.
// 
Value MathFuncAsinh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = asinh(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает арктангенс числа.
// 
MathFuncAtan::MathFuncAtan()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает арктангенс числа.
// 
Value MathFuncAtan::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = atan(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает арктангенс от частного своих аргументов.
// 
MathFuncAtan2::MathFuncAtan2()
{
   _minArgs = 2;
   _maxArgs = 2;
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает арктангенс от частного своих аргументов.
// 
Value MathFuncAtan2::run()
{
   if (!initRun())
      return Value();

   Float flVal1 = _vals[0].get_Float();
   Float flVal2 = _vals[1].get_Float();
   Float res = atan2(flVal1, flVal2);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический арктангенс числа.
// 
MathFuncAtanh::MathFuncAtanh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический арктангенс числа.
// 
Value MathFuncAtanh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = atanh(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает кубический корень числа.
// 
MathFuncCbrt::MathFuncCbrt()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает кубический корень числа.
// 
Value MathFuncCbrt::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = cbrt(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает наименьшее целое число, большее, либо равное указанному числу.
// 
MathFuncCeil::MathFuncCeil()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает наименьшее целое число, большее, либо равное указанному числу.
// 
Value MathFuncCeil::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = ceil(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает косинус числа.
// 
MathFuncCos::MathFuncCos()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает косинус числа.
// 
Value MathFuncCos::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = cos(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический косинус числа.
// 
MathFuncCosh::MathFuncCosh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический косинус числа.
// 
Value MathFuncCosh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = cosh(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает Ex, где x — аргумент, а E — число Эйлера (2,718…), основание натурального логарифма.
// 
MathFuncExp::MathFuncExp()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает Ex, где x — аргумент, а E — число Эйлера (2,718…), основание натурального логарифма.
// 
Value MathFuncExp::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = exp(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает exp(x), из которого вычли единицу.
// 
MathFuncExpm1::MathFuncExpm1()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает exp(x), из которого вычли единицу.
// 
Value MathFuncExpm1::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = expm1(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает наибольшее целое число, меньшее, либо равное указанному числу.
// 
MathFuncFloor::MathFuncFloor()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает наибольшее целое число, меньшее, либо равное указанному числу.
// 
Value MathFuncFloor::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = floor(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает квадратный корень из суммы квадратов своих аргументов.
// 
MathFuncHypot::MathFuncHypot()
{
   _minArgs = 1;
}
//=======================================================================================
//! Возвращает квадратный корень из суммы квадратов своих аргументов.
// 
Value MathFuncHypot::run()
{
   if (!initRun())
      return Value();

   Float flVal = 0;
   for (size_t i = 0; i < _vals.size(); i++)
      flVal += _vals[i].get_Float() * _vals[i].get_Float();
   Float res = sqrt(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает натуральный логарифм числа (loge, также известен как ln).
// 
MathFuncLog::MathFuncLog()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает натуральный логарифм числа (loge, также известен как ln).
// 
Value MathFuncLog::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = log(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает десятичный логарифм числа.
// 
MathFuncLog10::MathFuncLog10()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает десятичный логарифм числа.
// 
Value MathFuncLog10::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = log10(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает натуральный логарифм числа 1 + x (loge, также известен как ln).
// 
MathFuncLog1p::MathFuncLog1p()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает натуральный логарифм числа 1 + x (loge, также известен как ln).
// 
Value MathFuncLog1p::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = log1p(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает двоичный логарифм числа.
// 
MathFuncLog2::MathFuncLog2()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает двоичный логарифм числа.
// 
Value MathFuncLog2::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = log2(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает наибольшее число из своих аргументов.
// 
MathFuncMax::MathFuncMax()
{
   _minArgs = 1;
}
//=======================================================================================
//! Возвращает наибольшее число из своих аргументов.
// 
Value MathFuncMax::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   for (size_t i = 1; i < _vals.size(); i++)
      if (_vals[i].get_Float() > flVal)
         flVal = _vals[i].get_Float();
   return Value(flVal);
}
//=======================================================================================
//! Возвращает наименьшее число из своих аргументов.
// 
MathFuncMin::MathFuncMin()
{
   _minArgs = 1;
}
//=======================================================================================
//! Возвращает наименьшее число из своих аргументов.
// 
Value MathFuncMin::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   for (size_t i = 1; i < _vals.size(); i++)
      if (_vals[i].get_Float() < flVal)
         flVal = _vals[i].get_Float();
   return Value(flVal);
}
//=======================================================================================
//! Возведение в степень
// 
MathFuncPow::MathFuncPow()
{
   _minArgs = 2;
   _maxArgs = 2;
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_FLOAT);
   _argTypes[1].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возведение в степень
// 
Value MathFuncPow::run()
{
   if (!initRun())
      return Value();

   Float flVal1 = _vals[0].get_Float();
   Float flVal2 = _vals[1].get_Float();
   Float res = pow(flVal1, flVal2);
   return Value(res);
}
//=======================================================================================
//! Установить начальную позицию генератора
// 
MathFuncRandomSeed::MathFuncRandomSeed()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Установить начальную позицию генератора
// 
Value MathFuncRandomSeed::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   MathModule* module = (MathModule*)_module;
   module->randomSeed((UInt)flVal);
   return Value();
}
//=======================================================================================
//! Получить вещественное число в диапазоне [0,1)
// 
MathFuncRandom::MathFuncRandom()
{
   _minArgs = 0;
   _maxArgs = 0;
}
//=======================================================================================
//! Получить вещественное число в диапазоне [0,1)
// 
Value MathFuncRandom::run()
{
   if (!initRun())
      return Value();
   MathModule* module = (MathModule*)_module;
   return Value(Float(module->random()));
}
//=======================================================================================
//! Получить целое случайное число в диапазоне
// 
MathFuncRandRange::MathFuncRandRange()
{
   _minArgs = 1;
   _maxArgs = 3;
}
//=======================================================================================
//! Получить целое случайное число в диапазоне
// 
Value MathFuncRandRange::run()
{
   if (!initRun())
      return Value();

   Int start = 0;
   Int stop = 0;
   Int step = 1;

   if (_vals.size() == 1)
   {
      stop = _vals[0].get_Int();
   }
   else if(_vals.size() == 2)
   {
      start = _vals[0].get_Int();
      stop = _vals[1].get_Int();
   }
   else if (_vals.size() >= 3)
   {
      start = _vals[0].get_Int();
      stop = _vals[1].get_Int();
      step = _vals[2].get_Int();
   }

   MathModule* module = (MathModule*)_module;
   return Value(Float(module->randomRange(start, stop, step)));
}
//=======================================================================================
//! Возвращает значение числа, округлённое до ближайшего целого.
//
MathFuncRound::MathFuncRound()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает значение числа, округлённое до ближайшего целого.
// 
Value MathFuncRound::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = round(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает знак числа, указывающий, является ли число положительным, отрицательным или нулём.
// 
MathFuncSign::MathFuncSign()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает знак числа, указывающий, является ли число положительным, отрицательным или нулём.
// 
Value MathFuncSign::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = flVal > 0 ? 1 : flVal < 0 ? -1 : 0;
   return Value(res);
}
//=======================================================================================
//! Возвращает синус числа.
// 
MathFuncSin::MathFuncSin()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает синус числа.
// 
Value MathFuncSin::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = sin(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический синус числа.
// 
MathFuncSinh::MathFuncSinh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический синус числа.
// 
Value MathFuncSinh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = sinh(flVal);
   return Value(res);
}
//=======================================================================================
//! Взятие корня
// 
MathFuncSqrt::MathFuncSqrt()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Взятие корня
// 
Value MathFuncSqrt::run()
{
   if (!initRun())
      return Value();

   Float flVal  = _vals[0].get_Float();
   Float res    = sqrt(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает тангенс числа.
// 
MathFuncTan::MathFuncTan()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает тангенс числа.
// 
Value MathFuncTan::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = tan(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает гиперболический тангенс числа.
// 
MathFuncTanh::MathFuncTanh()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает гиперболический тангенс числа.
// 
Value MathFuncTanh::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = tanh(flVal);
   return Value(res);
}
//=======================================================================================
//! Возвращает целую часть числа, убирая дробные цифры.
// 
MathFuncTrunc::MathFuncTrunc()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FLOAT);
}
//=======================================================================================
//! Возвращает целую часть числа, убирая дробные цифры.
// 
Value MathFuncTrunc::run()
{
   if (!initRun())
      return Value();

   Float flVal = _vals[0].get_Float();
   Float res = trunc(flVal);
   return Value(res);
}
