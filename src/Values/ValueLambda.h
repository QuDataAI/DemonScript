/*!                           Лямбда-выражение (анонимная функция)


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueLambdaH
#define ValueLambdaH

#include "ValueBase.h"
#include "Script/Function.h"

SPTR_DEF(ValueLambda);
SPTR_DEF(FunLambda);

//=======================================================================================
//! Значение, хранящее лямбда-выражение
// 
class ValueLambda : public ValueBase
{
   FunLambdaPtr _val;
public:
   ValueLambda(const FunLambdaPtr &val) { _val = val; }
   ValueLambda(vector<string> &vars, ScriptPtr script = 0);
   ostream& print(ostream& out);
   string   type_str() { return "lambda"; }
   Kind     type() { return _LAMBDA; }
   SetType  setType() const { return ST_REFERENCE; }
   virtual shared_ptr<ValueBase> copy() const { return make_shared<ValueLambda>(_val); }
   /*!
   Присвоить переменной Value
   \param val значение
   */
   virtual void operator=(const shared_ptr<ValueBase> &val) {}
   /*!
   Конвертировать значение в строку
   \return строковое значение
   */
   virtual string toString();
   /*!
   Получить лямбда функцию
   \return лямбда функция
   */
   virtual FunLambdaPtr  get_Lambda() { return _val; }
};

#endif