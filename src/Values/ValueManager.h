/*!                           Менеджер переменных

правила создания переменных, их конвертации между субой


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ValueManagerH
#define ValueManagerH

#include "ValueNone.h"
#include "ValueLogic.h"
#include "ValueNode.h"
#include "ValueEdge.h"
#include "ValueFloat.h"
#include "ValueStr.h"
#include "ValueGraph.h"
#include "ValueArr.h"
#include "ValueIndexRef.h"
#include "ValueFile.h"
#include "ValueFus.h"
#include "ValueMap.h"
#include "ValueSVG.h"

class ValueManager {
public:
   template <typename  T> static void   changeValue(shared_ptr<ValueBase> &ptr, T &val, bool checkIfExists = true);
   static shared_ptr<ValueBase> createValue() {                     return ValueNone::ptr(); }
   static shared_ptr<ValueBase> createValue(const Logic     &val) { return make_shared<ValueLogic>(val); }
   static shared_ptr<ValueBase> createValue(const UInt      &val) { return make_shared<ValueNode>(val);  }
   static shared_ptr<ValueBase> createValue(const Int       &val) { return make_shared<ValueEdge>(val);  }
   static shared_ptr<ValueBase> createValue(const Float     &val) { return make_shared<ValueFloat>(val); }
   static shared_ptr<ValueBase> createValue(const string    &val) { return make_shared<ValueStr>(val);   }
   static shared_ptr<ValueBase> createValue(const char      *val) { return make_shared<ValueStr>(val);   }
   static shared_ptr<ValueBase> createValue(const Graph     &val) { return make_shared<ValueGraph>(val); }
   static shared_ptr<ValueBase> createValue(const ValueNode &val) { return make_shared<ValueNode>(val);  }
   static shared_ptr<ValueBase> createValue(const ValueArr  &val) { return make_shared<ValueArr>(val);   }
   static shared_ptr<ValueBase> createValue(const ValueFile &val) { return make_shared<ValueFile>(val);  }
   static shared_ptr<ValueBase> createValue(const ValueFus  &val) { return make_shared<ValueFus>(val);   }
   static shared_ptr<ValueBase> createValue(const ValueMap  &val) { return make_shared<ValueMap>(val);   }
   static shared_ptr<ValueBase> createValue(const ValueSVG  &val) { return make_shared<ValueSVG>(val);   }
   static shared_ptr<ValueBase> createValue(const shared_ptr<ValueBase> & val) { return val->copy(); }
   /*!
      Получить ссылку на значение или ссылку на копию значения
      Для сложных типов GRAPH и ARRAY возвращается ссылка, 
      в остальных случаях создается копия
      \param val ссылка на текущее значение
      \return исходная ссылка или ссылка на копию значения
   */
   static shared_ptr<ValueBase> getValue(const shared_ptr<ValueBase> &val);
   /*!
      Изменить значение leftVal на значение rightVal
      Для сложных типов GRAPH и ARRAY копируется ссылка,
      в остальных случаях создается копия и устанавливается ссылка на ее значение
      \param leftVal ссылка на текущее значение
      \param rightVal ссылка на новое значение
   */
   static void                  copyValueToValue(shared_ptr<ValueBase> &leftVal, const shared_ptr<ValueBase> & rightVal);
};


template<typename  T> void ValueManager::changeValue(shared_ptr<ValueBase>& ptr, T &val, bool checkIfExists/* = true*/)
{
   if (checkIfExists && ptr->checkType(val))
      *ptr = val;
   else
      ptr = createValue(val);
}

#endif