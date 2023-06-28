/*!                        Граф с рёбрами вероятностной логики

Граф состоит из узлов Node, соединённых направленными рёбрами Edge.
С каждым ребром связывается логическое значение Logic _val.
Если _val = (0,1) - ребро гарантированно есть. 
Но могут быть и промежуточные значения истинности наличия ребра.

                            (с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef GraphH
#define GraphH
#include "OTypes.h"
#include "Logic.h"
#include "Node.h"
#include "Trace.h"
#include "Axioms/Axiom.h"
#include <string>
#include <vector>
#include <map>
#include <queue>

#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

class GraphManager;
class GraphInvariants;
class ValueMap;
class ValueArr;
class ValueGraph;
class GraphNode;
class Expr;

//=======================================================================================
//! Граф узлов, соединённых направленными рёбрами
/*! Нулевой узел это несуществующий узел (аналогично нулевому указателю)
*/
class Graph
{
public:
   vector<Node>                  _nodes;                 //!< список узлов по их номерам
   UInt                          _changes;               //!< число сделанных изменений графа
   Int                           _edges_blocked;         //!< не позволяет менять определённые значения рёбер
   UInt                          _edges_changes;         //!< сколько изменилось определённых значений рёбер
   string                        _graph_name;            //!< имя графа
   UInt                          _classID;               //!< идентификатор класса графов
   weak_ptr<ValueGraph>          _valGraph;              //!< слабая ссылка на значение, содержащее текущий объект
   shared_ptr<ValueGraph>        _valAttrGraph;          //!< ссылка на значение, содержащее атрибутный граф для текущего
   shared_ptr<ValueGraph>        _valDefSensesGraph;     //!< ссылка на значение, содержащее граф смыслов по умолчанию для текущего графа
   vector<Axiom>                 _axioms;                //!< список аксиом графа
   shared_ptr<GraphInvariants>   _invariants;            //!< инвариантные признаки графа

   Graph();
   Graph(const Graph & graph);
   ~Graph();

   //! Передаем графу ссылку на значение, содержащее текущий объект
   void    setValueGraph(const std::shared_ptr<ValueGraph> &valGraph) { _valGraph = valGraph; }
   //! Передаем графу ссылку на значение, содержащее граф смыслов по умолчанию для текущего графа
   void    setValueDefSensesGraph(Graph* graph) { _valDefSensesGraph = graph->_valGraph.lock(); }
   //! Очистить (полностью) текущий граф, кроме $UNKNOWN_NODE, @UNKNOWN_EDGE 
   Graph & clear();
   //! Скопировать в текущий граф, граф graph
   Graph & copy(const Graph & graph);
   //! Скопировать в текущий граф, схему graph
   Graph & copyScheme(const Graph & graph);
   /*!
   Операция присваивания
   \param graph присваиваемое значение
   \return результат операции
   */
   Graph & operator = (const Graph &graph) { return copy(graph); } 
   /*!
   Операция сложения +
   \param y слогаемое значение
   \return результат операции
   */
   shared_ptr<ValueGraph> operator + (const Graph &y);
   /*!
   Операция вычитания -
   \param y вычитаемое значение
   \return результат операции
   */
   shared_ptr<ValueGraph> operator - (const Graph &y);
   /*!
   Операция равенства x == y
   \param x левое значение операции
   \param y правое значение операции
   \return результат операции
   */
   friend Logic  operator == (const Graph &x, const Graph &y);
   /*!
   Операция неравенства x != y
   \param x левое значение операции
   \param y правое значение операции
   \return результат операции
   */
   friend Logic  operator != (const Graph &x, const Graph &y);


   UInt node(const string &name);    //!< идентификатор узла по имени (0 если нет)
   /*!
   Получить идентификатор типа ребра по имени (0 если нет)
   \param edgeID идентификатор типа ребра
   */
   Int  edge(const string &name);
   /*!
   Получить имя типа ребра по его идентификатору
   \param edgeID идентификатор типа ребра
   */
   string edge_name(Int id);
   //! Получение имени ребра по идентификатору с проверкой границ
   string  node_name(UInt id);
   /*!
   Установить значение узла графа
   \param id идентификатор узла
   \param val новое значение
   \param change разрешено ли менять значение если ранее установлено
   */
   void nodeValue(UInt id, shared_ptr<ValueBase> val, Bool change = true);
   /*!
   Получить значение узла графа
   \param id идентификатор узла
   \return значение узла графа
   */
   shared_ptr<ValueBase> nodeValue(UInt id);
   /*!
   Установить значение атрибута для узла
   \param obNodeID идентификатор узла для которого устанавливаем значение атрибута
   \param attrNodeID идентификатор узла, который описывает имя атрибута
   \param attrGraph граф в котором находится узел, описывающий имя атрибута
   \param change разрешено ли менять значение атрибута если уже существует
   \param[out] attrValue значение атрибута узла
   \return результат установки атрибута
   */
   Bool set_attr(UInt obNodeID, UInt attrNodeID, Graph* attrGraph, Bool change, const shared_ptr<ValueBase> &attrValue);
   /*!
   Получить значение атрибута для узла
   \param obNodeID идентификатор узла для которого получаем значение атрибута
   \param attrNodeID идентификатор узла, который описывает имя атрибута
   \param attrGraph ссылка на графа в котором находится узел, описывающий имя атрибута
   \param localSearch атрибут ищем только в текущем узле
   \return значение атрибута узла
   */
   shared_ptr<ValueBase> get_attr(UInt obNodeID, UInt attrNodeID, Graph* attrGraph, Bool localSearch = true);
   /*!
   Найти узел хранящий значение атрибута для узла с идентификатором obNodeID
   \param obNodeID идентификатор узла для которого получаем идентификатор узла атрибута
   \param attrNodeID идентификатор узла, который описывает имя атрибута
   \param attrGraphVal граф в котором находится узел, описывающий имя атрибута
   \param localSearch атрибут ищем только в текущем узле
   \param[out] attrValueNodeID идентификатор узла атрибута, если найден
   \param[out] attrValueGraph граф узла атрибута, если найден
   \return успешность поиска
   */
   Bool find_attr_node(UInt                     obNodeID, 
                       UInt                     attrNodeID, 
                       shared_ptr<ValueGraph>   attrGraphVal,
                       Bool                     localSearch,
                       UInt  &                  attrValueNodeID,
                       shared_ptr<ValueGraph>&  attrValueGraphVal);
   /*!
   Получить список всех атрибутов узла
   \param obNodeID идентификатор узла для которого получаем идентификатор узла атрибута
   \param localSearch атрибут ищем только в текущем узле
   \param[out] attrs список узлов-атрибутов
   \return успешность поиска
   */
   void attr_list(UInt                    obNodeID,
                  Bool                    localSearch,
                  ValueArr                &attrs);
   /*!
   Получить список всех узлов в которые из заданного есть ребро нужного типа и значения
   */
   void node_list(UInt                    obNodeID,
                  Int                     edgeKind,
                  ValueArr                &attrs,
                  Logic                   value
                 );
   //! Получить объект по его id
   Node & get(UInt id) { return _nodes[id]; }
   //! Получить объект по его имени без проверки диапазона
   Node & operator [](const string &name) { return _nodes[node(name)]; }

   //! Добавить ребро типа knd между узлами x и y с истинностью val
   void add(Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph = 0);
   //! Удалить ребра типа knd исходящие из узла xID, при необходимости - рекурсивно
   void clear_edges(Int knd, UInt xID, bool recursion);

   //! Добавить ребро 
   void add(const Edge &edge);

   //! Вычислить инварианты для текущего графа
   void setInvariants();

   //! Проверить, является ли указанный граф изоморфным к текущему
   Bool isomorphic(Graph* g);
   
   //! Проверить, является ли указанный граф изоморфным к текущему (используя метод перебора ребер)
   Bool isomorphicSimple(Graph* g);

   //! Удалить все узлы
   void delete_nodes();
   //! Убрать все рёбра (но не из справочника)
   void clear_edges();
   //! Ошибочные идентификаторы
   Bool wrong_id(Int k, UInt x, UInt y);
   //! Ошибочный тип ребра
   Bool wrong_kind(Int k);
   //! Ошибочный узел ребра
   Bool wrong_node(UInt x);

   //! Вывести граф в поток out
   ostream & print(ostream & out, Int show_inverse = 1, Int show_value = 1);
   //! Вывести одно ребро в поток
   void print_edge(EdgeList edge, ostream & out, Int show_inverse, Int cnt, Int show_value = 1);
   //! Вывести объект c идентификатором id в поток out
   void print_obj(UInt id, ostream & out, Int show_inverse=1, Int show_value = 1);
   /*!
   Вывести граф в поток out в dot формате
   \param out поток в который необходимо вывести файл
   \param edgesFilter список типов ребер которые нужно выводить
                      если параметр не указан выводим все ребра
   \param showLoop отображеть петли
   \param mode режим отображения ребер
   \return поток в который вывели граф
   */
   ostream &  print_dot(ostream & out, vector<Int> *edgesFilter = NULL, bool showLoop = true, Int mode = 0);
   //! Вывести граф в поток out в формате ds
   ostream & print_ds(ostream & out);
   //! Вывести объект c идентификатором id в поток out в формате ds
   void print_obj_ds(UInt id, ostream & out);

   //! Получить значение ребра knd(x,y) между узлами x и y
   Logic val(Int knd, UInt x, UInt y, Graph* xGraph = NULL, Graph* yGraph = NULL, bool safe = true);
   //! Получить значение ребра knd(x,y) между узлами x и y
   Logic val(const string & rel, Node &x, Node & y){return val(edge(rel),x.id(),y.id());}
   Logic val(const string & rel, Edge &x, Node & y){return val(edge(rel),x._n2, y.id());}
   Logic val(const string & rel, Node &x, Edge & y){return val(edge(rel),x.id(),y._n2); }

   //! Есть ли ребро типа kind (с любым значением) от узла x в узел y
   Logic is_edge(Int knd, UInt x, UInt y, Graph* xGraph = NULL, Graph* yGraph = NULL);

   //! Получить список рёбер objs типа knd, выходящих из узла x 
   void get_out(const string &knd, Node &x, vector<Edge> &objs);
   //! Получить список рёбер objs типа knd, входящих в узел x 
   void get_in (const string &knd, Node &x,  vector<Edge> &objs);

   //! Получить список всех выходящих рёбер 
   void get_out(vector<Edge> &objs);
   //! Получить список всех входящих рёбер 
   void get_in(vector<Edge> &objs);

   //! Является ли данный узел изолированным от остальных
   Bool  isolated(UInt id) 
   {
       if(id ==0 || id >= _nodes.size())
         return true;
      return (_nodes[id].edgesOut().size() == 0 && _nodes[id].edgesIn().size() == 0);
   }

   //! Записать в txt граф в виде текста
   void get_str(string & txt);

   //!  Есть ли путь от x к y по рёбрам типа knds
   Logic path(vector<Int> & knds, Node &x, Node &y, bool loop = true, bool back = true);
   Logic path(vector<Int> & knds, UInt  x, UInt  y, Graph* xGraph, Graph* yGraph, bool loop = true, bool back = true)
   {
      xGraph = xGraph ? xGraph : this;
      yGraph = yGraph ? yGraph : this;
      return path(knds, xGraph->_nodes[x], yGraph->_nodes[y], loop, back);
   }
   //!  Есть ли путь от x к y по рёбру типа knd
   Logic path(Int knd, Node &x, Node &y, bool loop = true, bool back = true);
   Logic path(Int knd, UInt  x, UInt  y, Graph* xGraph, Graph* yGraph, bool loop = true, bool back = true)
   {
      xGraph = xGraph ? xGraph : this;
      yGraph = yGraph ? yGraph : this;
      return path(knd, xGraph->_nodes[x], yGraph->_nodes[y], loop, back);
   }
   //! Проверка существования у узла или его ближайших по isa узлов нужного отношения
   Logic get(Int knd, Node &x, Node &y, bool loop = true, bool back = true);
   //! Проверка существования у узла или его ближайших по isa узлов нужного отношения
   Logic get(Int knd, UInt  x, UInt  y, Graph* xGraph, Graph* yGraph, bool loop = true, bool back = true)
   {
      xGraph = xGraph ? xGraph : this;
      yGraph = yGraph ? yGraph : this;
      return get(knd, xGraph->_nodes[x], yGraph->_nodes[y], loop, back);
   }
   //! Поиск всех связанных по isa узлов, у которых есть нужное отношение
   void getAll(Int knd, Node &x, shared_ptr<ValueArr> out, bool loop = true, bool back = true);
   //! Поиск всех связанных по isa узлов, у которых есть нужное отношение
   void getAllHelper(Int knd, UInt x, Graph* xGraph, shared_ptr<ValueArr> out)
   {
      xGraph = xGraph ? xGraph : this;
      getAll(knd, xGraph->_nodes[x], out);
   }
   //! Проверка isa отношения с использованием функции get
   Logic isa(Node &x, Node &y);

   //! Есть ли общий узел при движении по рёбрам knd от узлов x,y
   Logic common(vector<Int> & knds, Node &x, Node &y);
   Logic common(vector<Int> & knds, UInt  x, UInt  y) { return common(knds, _nodes[x], _nodes[y]);}
   /*!
   Получить идентификатор класса графа
   \return идентификатор класса графа
   */
   UInt  classID() const { return _classID; }
   /*!
   Получить имя графа
   \return  имя графа
   */
   string  name() { return _graph_name; }
   /*!
   Получить число входящих/исходящих ребер 
   \name x идентификатор ребра
   \name knd тип ребра (<0 - входящие, >0 - исходящие)
   \return число ребер
   */
   UInt  count(UInt x, Int knd);  
   /*
   Получить число входящих/исходящих ребер с указанным значением вероятности
   \name x идентификатор ребра
   \name knd тип ребра (<0 - входящие, >0 - исходящие)
   \name val вероятность ребра
   \return число ребер
   */
   UInt  count(UInt x, Int knd, Logic val);
   /*!
   Получить число входящих ребер
   \name x идентификатор ребра
   \name knd тип ребра
   \return  имя графа
   */
   UInt  countIn(UInt x, Int knd) { return count(x, -knd); }
   /*!
   Получить число входящих ребер с указанным значением вероятности
   \name x идентификатор ребра
   \name knd тип ребра
   \name val вероятность ребра
   \return  имя графа
   */
   UInt  countIn(UInt x, Int knd, Logic val) { return count(x, -knd, val); }
   /*!
   Получить число входящих ребер
   \name x идентификатор ребра
   \name knd тип ребра
   \return  имя графа
   */
   UInt  countOut(UInt x, Int knd) { return count(x, knd); }
   /*!
   Получить число входящих ребер с указанным значением вероятности
   \name x идентификатор ребра
   \name knd тип ребра
   \name val вероятность ребра
   \return  имя графа
   */
   UInt  countOut(UInt x, Int knd, Logic val) { return count(x, knd, val); }
   /*!
   Загрузить граф из файла
   \name fileName имя файла
   \name mode режим открытия файла 0-не загружаем значения, 1-загружаем значения
   \return  результат успешности загрузки
   */
   Logic  load(string &fileName, Int mode = 0);
   /*!
   Загрузить граф из файла базы смыслов
   \name fileName имя файла базы смыслов
   \name mode режим открытия файла 0-не загружаем значения, 1-загружаем значения
   \return  результат успешности загрузки
   */
   Logic  loadFromSenses(string &fileName, Int mode = 0);
   /*!
   Добавить строковое значение поля в мэп
   \param map мэп в который нужно добавить пару
   \param name имя поля
   \param valStr строковое значение
   */
   void   addSenseFieldToMap(shared_ptr<ValueMap> &map, const char* name, const string &valStr);
   /*!
   Добавить массив строк поля в мэп
   \param map мэп в который нужно добавить пару
   \param name имя поля
   \param valStr массив строк
   */
   void   addSenseFieldToMap(shared_ptr<ValueMap> &map, const char* name, const vector<string> &valStrArr);
   /*!
   Вернуть настройки логирования
   \return  уровень логирования
   */
   Int verbose() { return _verbose; }
   /*!
   Установить настройки логирования
   \param v уровень логирования
   */
   void verbose(Int v) { _verbose = v; }
   /*!
   Вернуть настройки автосортировки
   \return  настройка автосортировки
   */
   Int autoSort() { return _autoSort; }
   /*!
   Установить настройки автосортировки
   \param v автосортировка
   */
   void autoSort(Int v) { _autoSort = v; }
   /*!
   Вернуть настройки изменения ребер (0 -  разрешить менять значение с True на False и наоборот, 1 - запретить)
   \return  настрока изменения ребер
   */
   Int locked() { return _locked; }
   /*!
   Установить настройки изменения ребер (0 -  разрешить менять значение с True на False и наоборот, 1 - запретить)
   \param v настрока изменения ребер
   */
   void locked(Int v) { _locked = v; }
   /*!
   Получить ссылку на атрибутный граф
   \return ссылка на атрибутный граф
   */
   Graph* attrGraph();
   /*!
   Получить ссылку на значение атрибутного граф
   \return ссылка на значение атрибутного граф
   */
   shared_ptr<ValueGraph> attrGraphValue();
   /*!
   Отсортировать все ребра всех узлов графа
   */
   void sort();
   /*!
   Добавить аксиому в граф
   \param ex выражение, содержащее аксиому
   */
   void addAxiom(ExprPtr ex);
   /*!
   Проверить аксиомы графа
   */
   void checkAxioms();
   /*!
   Установить граф-владелец всем ребрам всех узлов (нужно для копирования графа)
   */
   void setOwner();
   /*!
   Граф изменен
   */
   void updated();
   /*!
   Получить значение флага полноты графа
   */
   bool close() { return _close; }
   /*!
   Установить значение флага полноты графа 
   */
   void close(bool v) { _close = v; }
   /*!
   Число запросов на получение значения ребра
   */
   static UInt getEdgeCnt() { return _getEdgeCnt; }   
   /*!
   Число запросов на установку значения ребра
   */
   static UInt setEdgeCnt() { return _setEdgeCnt; }

private:

   //! Уровень логирования 0 - выключить логи кроме самых критических, 1 - отображать (пока все)
   Int _verbose = 1;
   //! Настройки изменения ребер (0 -  разрешить менять значение с True на False и наоборот, 1 - запретить)
   Int _locked = 0;
   //! Настройка автосортировки ребер
   Int _autoSort = 0;
   //! Начиная с какой длины вектора ребер пытаться в бинарный поиск
   Int _lengthForBinary = 3;
   static UInt _getEdgeCnt;   //!< число запросов на получение значения ребра
   static UInt _setEdgeCnt;   //!< число запросов на установку значения ребра
   //! Флаг полноты графа
   bool _close = false;

   /*!
   Добавить логически узел в граф с именем и идентификатором (данную функцию можно вызывать только из менеджера классов)
   \name name имя узла
   \id   идентификатор узла
   */   
   void add_node(UInt id);     

   GraphManager &_manager;          //!< менеджер графов

   friend class GraphClass;
};

//вспомогательный класс для хранения связки графа и его узла
class GraphNode {
public:
   GraphNode(Graph* graph = 0, Node *node = 0, Node * from = 0, Logic val = Logic::True, int len = 0) :
      _graph(graph), _node(node), _from(from), _path(val), _len(len) { }

   Graph * _graph; //!< ссылка на граф
   Node  * _node;  //!< ссылка на узел графа
   Node  * _from;  //!< узел с которого мы попали на этот
   Logic   _path;  //!< логическое значение вдоль пути
   int     _len;   //!< длина пути в рёбрах

   friend bool  operator <  (const GraphNode &x, const GraphNode &y)
   {
      //return x._path < y._path;        // по более истинным
      return x._len < y._len;            // в глубину
   }
};

//вспомогательный класс для хранения связки значения графа и его узла
class ValueGraphPtrNodeID 
{
public:
   ValueGraphPtrNodeID(const shared_ptr<ValueGraph> &valGraph, Node* node) : _valGraph(valGraph), _node(node) {}
   shared_ptr<ValueGraph>        _valGraph;   //!< ccылка на значение графа
   Node  *                       _node;       //!< ссылка идентификатора узла
};


//=======================================================================================
#endif
