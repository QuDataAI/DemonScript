/*!               Debugger - менеджер внешней отладки скрипта


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef DebuggerH
#define DebuggerH
#include "Sys/Debugger/DebuggerMessenger.h"          
#include "Sys/Singletone.h"          
#include "OTypes.h"
#include "Trace.h"
#include <string>
#include <thread>

//если необходимо отключить использование отладчика, коментирем следующую строку
#define USE_DEBUGGER

using namespace std;

class DebuggerClient;   //!< клиент отладки 
class Application;      //!< для доступа к приложению
class Code;             //!< для доступа к текущему исполняемому коду
class StackFrame;       //!< кадр стека
class Graph;            //!< граф
class Node;             //!< узел графа
class Edge;             //!< ребро графа

//=======================================================================================
//! Отладчик
class Debugger
{
   IMPLEMENTATION_SINGLETONE(Debugger)
public:
   enum DebuggerCommand {
      DEBUGGER_COMMAND_NONE,      //!< нет команды
      DEBUGGER_COMMAND_WAIT,      //!< ждем команды
      DEBUGGER_COMMAND_PAUSE,     //!< нужно остановить выполнение
      DEBUGGER_COMMAND_CONTINUE,  //!< продолжаем выполнение
      DEBUGGER_COMMAND_STEP_OVER, //!< не заходим во внутрь функций
      DEBUGGER_COMMAND_STEP_IN,   //!< заходим во внутрь функций
      DEBUGGER_COMMAND_STEP_OUT,  //!< выходим из функции
      DEBUGGER_COMMAND_QUIT       //!< выходим из приложения
   };
   //---- события основного потока приложения
   /*!
   Запустить отладчик
   \param отлаживаемое приложение
   \param protocolName имя протокола по которому нужно обмениваться с сервером
   \param port     порт сервера
   */
   void  run(Application* application, string protocolName, Int port);
   /*!
   Запускаем приложение
   */
   void  onLaunch();
   /*!
   Трассировка
   */
   void  onTrace(TraceLevel level, size_t line, size_t column, string &msg);
   /*!
   Исключение
   */
   void  onException(size_t line, size_t column, string &msg);
   /*!
   Измененена команда скрипта
   \return разрешено ли выполнять команду
   */
   bool  onStep();
   /*!
   Завершено выполнение скрипта
   */
   void  onComplete();
   /*!
   Есть ли отладка в текущий момент
   */
   bool  debugging() { return _debugging; }
private:
   class BreakPoint {
   public:
      BreakPoint(string srcPath, size_t line): _srcPath(srcPath), _line(line) {}
      string _srcPath  = "";     //!< путь к файлу с исходниками (актуален до запуска)
      size_t _sourceID = 0;      //!< идентификатор исходника
      size_t _line     = 0;      //!< номер строки
      bool   _valid    = false;  //!< точка остановки не проверена, либо некорректна
   };
   class VariablesReference {
   public:
      /*!
      Получить переменные по указанной ссылке
      \param debugger текущий отладчик
      \param[out] json объект с переменными
      \return результат выполнения запроса
      */
      virtual bool getVariables(Debugger* debugger, Json::Value &json) = 0;
   };
   class VariablesReferenceScope: public VariablesReference {
   public:
      VariablesReferenceScope(size_t frameID) :_frameID(frameID) {}
      /*!
      Получить переменные по указанной ссылке
      \param debugger текущий отладчик
      \param[out] json объект с переменными
      \return результат выполнения запроса
      */
      bool getVariables(Debugger* debugger, Json::Value &json);
   private:      
      size_t _frameID = 0;    //!< идентификатор текущего кадра стека
   };
   class VariablesReferenceGraph : public VariablesReference {
   public:
      VariablesReferenceGraph(Graph &graph) :_graph(graph) {}
      /*!
      Получить переменные по указанной ссылке
      \param debugger текущий отладчик
      \param[out] json объект с переменными
      \return результат выполнения запроса
      */
      bool getVariables(Debugger* debugger, Json::Value &json);
   private:      
      Graph &_graph;    //!< ссылка на граф
   };

   class VariablesReferenceGraphNode : public VariablesReference {
   public:
      VariablesReferenceGraphNode(Graph &graph, Node &node) :_graph(graph), _node(node) {}
      /*!
      Получить переменные по указанной ссылке
      \param debugger текущий отладчик
      \param[out] json объект с переменными
      \return результат выполнения запроса
      */
      bool getVariables(Debugger* debugger, Json::Value &json);
   private:
      /*!
      Вспомогательная функция для getVariables(Debugger* debugger, Json::Value &json); которая обрабатывает 1 вектор ребер
      \param list список ребер
      \param debugger текущий отладчик
      \param[out] json объект с переменными
      */
      void getVariables(vector<Edge> & list, string edgeName, string edgeDirection, Debugger* debugger, Json::Value &json, int &variablesTotal);

      Graph &_graph;    //!< ссылка на граф
      Node  &_node;     //!< ссылка на узел графа
   };

   /*!
   Запустить отладчик в потоке
   \param DebuggerClient клиент отладки
   */
   static void runThread(DebuggerClient* client);
   /*!
   Обрабатываем сообщения клиента отладчика
   */
   void dispatchDebuggerClientMessages();
   /*!
   Обрабатываем команды
   */
   void dispatch();
   /*!
   Обрабатываем сообщения отладчика
   */
   void dispatchDebuggerCommands();
   /*!
   Отправить событие отладчику
   \param name имя события
   */
   void pushEventForDebugger(string name);
   /*!
   Проверяем точки остановок на корректность
   */
   void validateBreakPoints();
   /*!
   Проверяем не попали ли мы в точку остановки
   */
   void checkBreakPoints();
   /*!
   Проверяем не попали ли мы в целевую точку STEP_OVER или STEP_IN
   */
   bool checkTarget();
   /*!
   Запрос на запустк приложения
   */
   void launchDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на установку точек остановок
   */
   void setBreakPointsDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на завершение конфигурации
   */
   void configurationDoneDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на остановку кода
   */
   void pauseDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на продолжение выполнения кода
   */
   void continueDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на переход к следующей команде
   */
   void stepOverDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на вход во внутрь функции
   */
   void stepInDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на вход во внутрь функции
   */
   void stepOutDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос получение текущего стека
   */
   void stackTraceDebuggerClientRequest(Json::Value &message);
   /*!
   Отправить стек из парсера
   */
   void sendStackTraceFromParsing(Json::Value &message);
   /*!
   Отправить стек из интерпретатора
   */
   void sendStackTraceFromRun(Json::Value &message);
   /*!
   Запрос на получение области видимости
   */
   void scopesDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на получение переменных
   */
   void variablesDebuggerClientRequest(Json::Value &message);
   /*!
   Запрос на отключение отладчика
   */
   void disconnectDebuggerClientRequest(Json::Value &message);
   /*!
   Отправить ответ на запрос клиенту
   */
   void sendResponse(Json::Value &message);
   /*!
   Установить новую команду
   */
   void setCommand(DebuggerCommand command);
   /*!
   Получить текущую позицию в исходниках
   */
   bool currentSourcePos(string & srcName, string & srcPath, size_t & line);

   DebuggerCommand          _command = DEBUGGER_COMMAND_NONE;        //!< текущая команда отладки
   thread                   _thread;            
   DebuggerClient*          _client             = NULL;              //!< клиент отладки (Visual Studio Code, ...)
   Application*             _application        = NULL;              //!< отлаживаемое приложение
   bool                     _debugging          = false;             //!< идет ли отладка в текущий момент
   DebuggerMessenger        _messenger;                              //!< обменник сообщениями между потоком приложения и клиентом отладки
   vector<BreakPoint>       _breakPoints;                            //!< список точек остановок
   bool                     _needValidateBreakPonts = false;         //!< флаг необходимости проверки точек остановок
   StackFrame*              _currentStackFrame  = NULL;              //!< текущий исполняемый кадр кода
   map<DebuggerCommand, Json::Value> _pendingRequests;               //!< ожидающие выполнения команды
   size_t                   _targetSourceID = 0;                     //!< целевая точка для команд STEP_OVER и STEP_OUT
   size_t                   _targetLine     = 0;                     //!< целевая точка для команд STEP_OVER и STEP_OUT
   vector<StackFrame*>      _stack;                                  //!< текущий стек
   size_t                                      _variableReference = 1000;       //!< ссылка на переменные
   map<size_t, shared_ptr<VariablesReference>> _variableReferences;             //!< идентификаторы ссылок на переменные
   size_t                                      _exceptionLine = 1;              //!< строка парсинга, на которой произошло исключение
};

#endif
