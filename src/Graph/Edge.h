/*!                        Связь между узлами графа

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef EdgeH
#define EdgeH
#include "OTypes.h"
#include "Logic.h"

#include <string>
#include <vector>
#include <map>
#include <queue>

#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
using namespace std;

class ValueGraph;

//=======================================================================================
//! Направленное ребро типа _knd между двумя узлами (от _n1 к _n2) со значением _val
/*! Идентификаторы узлов _n1, _n2 начинаются с 1 (0 - это не существующий цзел).
Для рёбер _knd > 0 это исходное ребро, а _knd < 0 - входящее ребро типа -_kind
*/
class Edge
{
public:
   UInt                    _n1;        //!< идентификатор узла из которого идет ребро
   UInt                    _n2;        //!< идентификатор узла в который идет ребро
   weak_ptr<ValueGraph>    _valGraph1; //!< ссылка на граф из которого идет ребро (нужен для сортировки по именам нод)
   shared_ptr<ValueGraph>  _valGraph2; //!< ссылка на граф в который идет ребро если связь внешняя иначе 0
   Int                     _knd;       //!< тип ребра
   Logic                   _val;       //!< логическое значение, свзяанное с ребром

   Edge() : _knd(0), _n1(0), _n2(0) {}
   Edge(Int knd, UInt n1, UInt n2, Logic & val, shared_ptr<ValueGraph> valGraph1, shared_ptr<ValueGraph> valGraph2);
   /*!
   Операция равенства x == y
   \param x левое значение операции
   \param y правое значение операции
   \return результат операции
   */
   friend bool  operator == (const Edge &x, const Edge &y);
   /*!
   Операция неравенства x != y
   \param x левое значение операции
   \param y правое значение операции
   \return результат операции
   */
   friend bool  operator != (const Edge &x, const Edge &y);
};
#endif
