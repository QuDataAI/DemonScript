/*!                           Ссылка на переменную по индексу

Так как не всегда можно получить ссылку на индексный элемент одним типом
например:
  ValueLogic()[0] - Float, 
  ValueArr()[0]   - shared_ptr<ValueBase>
То в данном контейнере будет хранится ссылка на объект и индекс на его элемент,
что позволит без проблем изменять значения по индексу для объектов содержащих элементы разных типов:
  Value a = ValueArr([0,1,2,3])
  Value b = ValueLogic([0,1])
  a[0] = 0.2
  b[0] = 0.2

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueIndexRefH
#define ValueIndexRefH

#include "ValueBase.h"

class Value;
//=======================================================================================
//! Ссылка на переменную по индексу
// 
class ValueIndexRef
{   
public:
   ValueIndexRef() {}
   ValueIndexRef(const shared_ptr<ValueBase> &ptr, const shared_ptr<ValueBase> &key) : _ptr(ptr), _key(key) {}

   shared_ptr<ValueBase> operator->();
   ValueIndexRef & operator = (const Value & val);
   shared_ptr<ValueBase>  value() const;
   shared_ptr<ValueBase>& ptr() { return _ptr; }
private:
   shared_ptr<ValueBase> _ptr;            //!< ссылка на переменную к которой собираемся обратиться по индексу
   shared_ptr<ValueBase> _key;            //!< ключ элемента в переменной
};

#endif


