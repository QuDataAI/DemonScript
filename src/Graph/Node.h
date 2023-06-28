/*!                        Узел графов

Хранит связи с другими узлами как текущего так и внешних графов

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef NodeH
#define NodeH
#include "OTypes.h"
#include "Logic.h"
#include "Edge.h"

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <memory>
#include <algorithm>       

using namespace std;

class ValueBase;
class ValueGraph;

struct EdgeList
{
   Int          _kind = -1;
   vector<Edge> _edges;
   //! Как сейчас отсортированы ребра
   Int          _sort = 0; // Node::sortNone;

   //! Отортировать массив ребер тем или иным способом
   void sort(Int mode);
   /*!
   Установить граф-владелец всем ребрам (нужно для копирования графа)
   */
   void setOwner(shared_ptr<ValueGraph>  &valGraph1);
};

//=======================================================================================
//! Узел графа характеризуется уникальным идентификаторм _id и может иметь имя _name
/*! Хранит список всех выходящих из него рёбер (_kind > 0) и входящих рёбер (_kind < 0)
*/
class Node
{
public:
   enum
   {
      sortNone = 0,
      sortTargetNode,
      sortTargetName
   };
   UInt                    _id;           //!< уникальный числовой идентификатор узла
   shared_ptr<ValueBase>   _value;        //!< значение узла графа

   Node() : _id(0) {}
   Node(UInt id) : _id(id) {}
   /*!
   Получить числовой идентификатор объекта
   \return числовой идентификатор объекта
   */
   UInt id() { return _id; }
   /*!
   Добавить в узел ребро типа knd со значением  val, направленное в узел n2
   \param knd тип ребра
   \param n2 номер узла к которому добавляем ребро
   \param val значение узла
   \param n1Graph указатель на граф в котором находится данный узел
   \param n2Graph указатель на граф в котором находится узел к которому добавляем ребро
   \return число добавленных ребер
   */
   Int add(Int knd, UInt n2, Logic val, shared_ptr<ValueGraph> valGraph1, shared_ptr<ValueGraph> valGraph2 = 0);
   /*!
   Сравнить два узла
   \param n1 первый узел
   \param n2 второй узел
   \return результат сравнения
   */
   friend Bool operator == (const Node &n1, const Node &n2) { return n1._id == n2._id; }
   /*!
   Установить значение узла графа
   \param val новое значение
   */
   void value(shared_ptr<ValueBase> val) { _value = val; }
   /*!
   Получить значение узла графа
   \return значение узла графа
   */
   shared_ptr<ValueBase> value() { return _value; }
   /*!
   Получить все исходящие ребра
   \return список списков ребер ребер
   */
   vector<EdgeList> & edgesOut() { return _edgesOut; }
   /*!
   Получить все входящие ребра
   \return список списков ребер ребер
   */
   vector<EdgeList> & edgesIn() { return _edgesIn; }
   /*!
   \param kind тип ребра
   \param добавить ли пустой массив, если его нет на месте
   Получить исходящие ребра по заданному k
   \return список списков ребер ребер
   */
   EdgeList & edgesOut(int kind, bool add=false);
   /*!
   \param kind тип ребра
   \param add добавить ли пустой массив, если его нет на месте
   Получить входящте ребра по заданному k
   \return список списков ребер ребер
   */
   EdgeList & edgesIn(int kind, bool add = false);
   /*!
   Очистить списки ребер
   */
   void clear() { _edgesIn.clear(); _edgesOut.clear(); }
   /*!
   Очистить заданный тип ребра (нужно для удаления пустых)
   \param kind тип ребра
   */
   void clearEdge(int kind);
   /*!
   Отсортировать списки ребер
   */
   void sort();
   /*!
   Установить граф-владелец всем ребрам (нужно для копирования графа)
   */
   void setOwner(shared_ptr<ValueGraph>  &valGraph1);
protected:
   /*!
   \param edges вектор ребер
   \param kind тип ребра
   \param add добавить ли пустой массив, если его нет на месте
   Получить ребра из вектора по заданному k
   \return список списков ребер ребер
   */
   EdgeList & edgesHelp(vector<EdgeList> &edges, int kind, bool add = false);
   /*!
   Очистить заданный тип ребра в заданном векторе ребер (нужно для удаления пустых)
   \param edges вектор ребер
   \param kind тип ребра
   */
   void clearEdgeHelp(vector<EdgeList> &edges, int kind);
   //! Входящие ребра
   vector<EdgeList>        _edgesIn;
   //! Исходящие ребра
   vector<EdgeList>        _edgesOut;   
   //! Заглушка для edgeHelp
   static EdgeList dummy;
};
//=======================================================================================
#endif
