/*!                           Ссылка на значение

Используется для оптимизации копирования значений по ссылке:
Например
Value a(1.0),b(2.0);
a = b;      // тут будет создан новый объект в a и в него будет скопировано значение из b
a = b.ref() // тут в a будет скопирована ссылка на b

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueRefH
#define ValueRefH

#include "ValueBase.h"

SPTR_DEF(ValueRef);

class Value;
//=======================================================================================
//! Ссылка на переменную по индексу
// 
class ValueRef
{
public:
   ValueRef(const Value &val);
   const shared_ptr<ValueBase>& ptr() const { return _ptr; }
private:
   shared_ptr<ValueBase> _ptr;            //!< ссылка на значение
};

#endif


