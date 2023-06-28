/*!                        Класс графов
Графы одного класса имеют одинаковый набор узлов и их идентификаторов, 
соответственно задачей данного модуля является добавление узлов в графы одного класса,
а также хранение ссылок на активные графы текущего класса
(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef GraphClassH
#define GraphClassH
#include "OTypes.h"
#include "Sys/Singletone.h"
#include "Logic.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
using namespace std;

class Graph;
class ValueGraph;
class ValueBase;

class GraphClass {
public:
   GraphClass(UInt &id);
   /*!
   Зарегистрировать ссылку на новый граф.
   \param graph ссылка на новый граф
   */
   void registerGraph(Graph* graph);
   /*!
   Удалить ссылку на указанный граф. 
   \param graph ссылка на удаляемый граф
   */
   void unregisterGraph(Graph* graph);
   /*!
   Добавить новый узел во все графы текущего класса
   \param name имя нового узла
   */
   UInt addNode(string name);
   /*!
   Добавить N безыменных узлов графов
   \param N число безымянных узлов
   \return идентификатор последнего добавленного узла
   */
   UInt addNodes(UInt n);
   /*!
   Удалить все узлы графов текущего класса
   */
   void deleteNodes();
   /*!
   Получить идентификатор узла по имени
   \param name имя узла
   */
   UInt node(const string &name);
   /*!
   Получить число имен узлов для текущего класса
   \param classID класс графов
   */
   UInt nodes();
   /*!
   Получить имя узла по идентификатору
   \param id идентификатор узла
   \return имя узла
   */
   string nodeName(const UInt id);
   /*!
   Получить все имена узлов графа
   \return имя узла
   */
   const map<string, UInt>& nodeNames();
   /*!
   Проверить, является ли идентификатор узла корректным
   \param nodeID идентификатор узла в классе графов
   \return результат проверки
   */
   Bool wrongNode(UInt id);
   /*!
   Добавить ребро типа knd между узлами x и y с истинностью val во все графы текущего класса
   \param knd идентификатор типа ребра
   \param x   идентификатор узла из которого устанавливаем ребро
   \param y   идентификатор узла в  который  устанавливаем ребро
   \param val значение истинности ребра
   \param yGraph ссылка на граф в котором расположен узел y (если не указана, используется текущий граф)
   \return результат добавления
   */
   Bool addEdge(Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph = 0);
   /*!
   Установить значение узла  во все графы указанного класса если ранее не установлены
   иначе обновить значение, но только для графа "graph"
   \param graph граф для которого разрешено менять значение узла
   \param id идентификатор узла
   \param val новое значение
   */
   void nodeValue(Graph* graph, UInt id, shared_ptr<ValueBase> val);
   /*!
   Установить значение атрибута для узла всех графов текущего класса
   \param obNodeID идентификатор узла для которого устанавливаем значение атрибута
   \param graph граф для которого разрешено менять значение атрибута
   \param attrNodeID идентификатор узла, который описывает имя атрибута
   \param attrGraph граф в котором находится узел, описывающий имя атрибута
   \param[out] attrValue значение атрибута узла
   */
   void setAttr(UInt obNodeID, Graph* obGraph, UInt attrNodeID, Graph* attrGraph, const shared_ptr<ValueBase> &attrValue);
   /*!
   Установить значение флага close у всех графов класса
   \param val новое значение флага
   \return новое значение флага
   */
   Bool close(Bool val);

private:
   /*!
   Получить индекс указанного графа
   \param  graph ссылка на граф, который необходимо найти
   \return если ссылка присутствует, возвращаем ее индекс, иначе -1
   */
   //int graphIndex(Graph* graph);
   /*!
   Синхронизировать граф
   \param  добавить недостающие узлы
   */
   void synhronizeGraph(Graph* graph);
   UInt              _id;              //!< идентификатор текущего класса
   vector<string>    _nodes;           //!< имена узлов всех графов текущего класса
   map<string, UInt> _nodeNames;       //!< имена узлов и их номера (идентификаторы)
   //vector<Graph*>    _graphs;          //!< ссылки на активные графы текущего класса
   set<Graph*>         _graphs;          //!< ссылки на активные графы текущего класса
};

//=======================================================================================
#endif
