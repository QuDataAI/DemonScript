/*!                        Менеджер графов

Хранит ссылки на все созданные в текущий момент графы с привязкой их к номерам классов.
При добавлении нового узла в граф. Менеджер определяет номер класса текущего графа
и добавляет узел во все графы текущего класса

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef GraphManagerH
#define GraphManagerH
#include "OTypes.h"
#include "Sys/Singletone.h"
#include "GraphClass.h"
#include "ValueBase.h"
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stack>
#include <string>

using namespace std;

class Graph;
class Value;

class GraphManager
{
   IMPLEMENTATION_SINGLETONE(GraphManager)
public:
   /*!
   Инициализировать менеджер графов
   */
   void init();
   /*!
   Получить ссылку на значение текущего графа
   \return ссылка на значение текущего графа
   */
   Value* currentGraphValue();
   /*!
   Получить ссылку на текущий граф
   \return ссылка на текущий граф
   */
   Graph* currentGraph();
   /*!
   Получить идентификатор класса для текущего графа
   \return идентификатор класса для текущего графа
   */
   UInt currentGraphClass();
   /*!
   Получить идентификатор нового класса
   \return идентификатор нового класса
   */
   Int getNewClassID();
   /*!
   Зарегистрировать ссылку на новый граф. Добавляет ссылку в соответствующий 
   класс и проверяет корректность набора узлов.
   \param graph ссылка на новый граф
   */
   void registerGraph(Graph* graph);
   /*!
   Удалить ссылку на указанный граф. Удаляет ссылку в соответствующем
   классе.
   \param graph ссылка на удаляемый граф
   */
   void unregisterGraph(Graph* graph);
   /*!
   Добавить новый узел в указанный класс графов.
   \param classID класс графов
   \param name имя нового узла
   */
   UInt addNode(UInt classID, const string &name);
   /*!
   Добавить имя узла в справочник имен, для контроля на этапе компиляции
   \param name имя узла
   */
   void addNodeName(const string &name);
   /*!
   Проверить, есть ли имя узла в справочнике имен, для контроля на этапе компиляции
   \param name имя узла
   \return результат проверки
   */
   bool isNodeName(const string &name);
   /*!
   Добавить N безыменных узлов графов
   \param classID класс графов
   \param N число безымянных узлов
   \return идентификатор последнего добавленного узла
   */
   UInt addNodes(UInt classID, UInt n);
   /*!
   Удалить все узлы графов указанного класса
   \param classID класс графов
   */
   void deleteNodes(UInt classID);
   /*!
   Добавить новый тип ребра по имени
   \param name имя нового типа узла
   */
   Int addEdgeName(const string &name);
   /*!
   Получить имя типа ребра по его идентификатору
   \param edgeID идентификатор типа ребра
   */
   string edgeName(Int edgeID);
   /*!
   Получить имя узла по номеру класса и его идентификатору
   \param classID идентификатор класса графов
   \param nodeID идентификатор узла в классе графов
   */
   string nodeName(UInt classID, UInt nodeID);
   /*!
   Получить все имена узлов графа указанного класса
   \param classID идентификатор класса графов
   \return имена узлов
   */
   const map<string, UInt>& nodeNames(UInt classID);
   /*!
   Получить идентификатор типа ребра по имени (0 если нет)
   \param name имя типа ребра
   */
   Int edge(const string &name);
   /*!
   Получить число типов ребер
   \param name имя типа ребра
   */
   UInt edges();
   /*!
   Получить все имена типов ребер графа
   \return имена типов рёбер
   */
   const map<string, Int>& edgeNames();
   /*!
   Получить идентификатор узла по имени 
   \param classID класс графов
   \param name имя узла
   */
   UInt node(UInt classID, const string &name);
   /*!
   Получить идентификатор узла по имени в классе текущего графа
   \param name имя узла
   */
   UInt node(const string &name);
   /*!
   Получить число имен узлов для указанного класса
   \param classID класс графов
   */
   UInt nodes(UInt classID);
   /*!
   Проверить зарегистрирован ли тип ребра с таким именем
   \param name имя типа ребра
   */
   Bool isEdgeName(const string &name);
   /*!
   Добавить ребро типа knd между узлами x и y с истинностью val во все графы указанного класса
   \param classID идентификатор класса графов
   \param knd идентификатор типа ребра
   \param x   идентификатор узла из которого устанавливаем ребро
   \param y   идентификатор узла в  который  устанавливаем ребро
   \param val значение истинности ребра
   \param yGraph ссылка на граф в котором расположен узел y (если не указана, используется текущий граф)
   \return результат добавления
   */
   Bool addEdge(UInt classID, Int knd, UInt xID, UInt yID, Logic val, Graph* yGraph = 0);
   /*!
   Установить значение узла  во все графы указанного класса
   \param classID идентификатор класса графов
   \param graph граф для которого разрешено менять значение узла
   \param id идентификатор узла
   \param val новое значение
   */
   void nodeValue(UInt classID, Graph* graph, UInt id, shared_ptr<ValueBase> val);
   /*!
   Установить значение атрибута для узла всех графов указанного класса
   \param classID идентификатор класса графов
   \param obNodeID идентификатор узла для которого устанавливаем значение атрибута
   \param graph граф для которого разрешено менять значение атрибута
   \param attrNodeID идентификатор узла, который описывает имя атрибута
   \param attrGraph граф в котором находится узел, описывающий имя атрибута
   \param change разрешено ли менять значение атрибута если уже существует
   \param[out] attrValue значение атрибута узла
   */
   void setAttr(UInt classID, UInt obNodeID, Graph* obGraph, UInt attrNodeID, Graph* attrGraph, const shared_ptr<ValueBase> &attrValue);
   /*!
   Проверить, является ли идентификатор класса графов корректным
   \return результат проверки
   */
   Bool wrongClass(UInt classID);
   /*!
   Проверить, является ли идентификатор типа ребра корректным
   \return результат проверки
   */
   Bool wrongEdge(Int edgeID);
   /*!
   Проверить, является ли идентификатор узла корректным
   \param classID класс графов
   \param nodeID идентификатор узла в классе графов
   \return результат проверки
   */
   Bool wrongNode(UInt classID, UInt nodeID);
   /*!
   Установить новое значение флага close для класса графов
   \param classID класс графов
   \param val новое значение
   \return новое значение
   */
   Bool close(UInt classID, Bool val);
   /*!
   Очистить данные менеджера
   */
   void clear();
private:
   /*!
   Изменить ссылку на значение текущего графа
   \param graph ссылка на текущий граф
   */
   void currentGraphValue(Value* graph);

   Value*                           _currentGraph = NULL;   //!< текущий граф (из переменной GRAPH)
   vector<shared_ptr<GraphClass>>   _graphClasses;          //!< классы графов
   map<string, Int>                 _edgeNames;             //!< имена типов рёбер и их номера (идентификаторы)
   set<string>                      _nodesNames;            //!< имена узлов
   vector<string>                   _edges;                 //!< список имен типов рёбер 

   friend class Script;
};

///Вспомогательный класс для сохранения текущего значения графа и восстановления его при вызове деструктора
class CurrentGraph
{
public:
   /*!
   Сохраняет ссылку на текущий граф для последующего восстановления в restore или деструкторе
   */
   CurrentGraph();                                               
   /*!
   Сохраняет ссылку на текущий граф для последующего восстановления в restore или деструкторе
   и меняет значение текущего графа на newGraph
   \param newGraph новое значение графа
   */
   CurrentGraph(const Value &newGraph);
   /*!
   Восстанавливает сохраненное значение графа
   */
   ~CurrentGraph();                                               
   /*!
   Меняет значение текущего графа на newGraph
   */
   void set(const Value &newGraphVal);
   /*!
   Восстанавливает сохраненное значение графа
   */
   void restore();                               
private:
   /*!
   Сохраняет ссылку на текущий граф для последующего восстановления в restore или деструкторе
   */
   void save();
   shared_ptr<ValueBase>   _savedGraph;       //!< ссылка на сохраненный граф
};
//=======================================================================================
#endif
