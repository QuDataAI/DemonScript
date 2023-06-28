/*!            MindGraphModel - модель графа, состоящая из ссылки на граф и его вероятности

(с) 2018-oct: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef MindGraphModelH
#define MindGraphModelH

#include "Value.h"
#include "ValueMap.h"

SPTR_DEF(MindGraphModel)

//=======================================================================================
//! Модель графа, состоящая из ссылки на граф и его вероятности
//
class MindGraphModel
{
public:
   Float        _p;            //!< вероятность графа
   UInt         _n;            //!< глубина ветвления для вычисления среднего геометрического
   Value        _graphValue;   //!< значение графа
   Value        _log;          //!< лог генерации модели
   MindGraphModel();
   MindGraphModel(const Value &gVal, Float p = 1.0, UInt n = 1);
   /*!
   Вывести модель как объект с двумя полями "p" - вероятность и "graph" - значение графа
   \return бъект с двумя полями "p" - вероятность и "graph" - значение графа
   */
   Value get_Value();
   /*!
   Создать копию модели
   \return ссылка на копию
   */
   MindGraphModelPtr copy();
   /*!
   Добавить лог в модель
   \return ссылка на копию
   */
   void pushLog(const string &str);
private:
   ValueMapPtr    _valMap;       //!< значение модели
   ValueMapKey    _valKeyP;      //!< ключ к вероятности
   ValueMapKey    _valKeyN;      //!< ключ к размеру ветвления
   ValueMapKey    _valKeyGraph;  //!< ключ к графу   
   ValueMapKey    _valKeyLog;    //!< ключ к логу транзакций   
};

#endif