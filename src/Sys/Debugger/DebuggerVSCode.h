/*!               DebuggerVSCode - клиент отладки кода Visual Studio Code

протокол позаимствован из TypeScript    
//vscode-mock-debug\node_modules\vscode-debugadapter-testsupport\lib\debugClient.js

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef DebuggerVSCodeH
#define DebuggerVSCodeH

#include "DebuggerClient.h"
#include "Sys/Net/TCPServer.h"
#include "Sys/ThirdParty/Jsoncpp/json/json.h"
#include <functional>

using namespace std;
//=======================================================================================
//! Отладчик
class DebuggerVSCode : public DebuggerClient, TCPServerListener
{
public:
   DebuggerVSCode(Int port, DebuggerMessenger *messenger);
   /*!
   Запустить отладчик
   */
   void run();
   // события от TCPServerListener
   /*!
   Соединение с клиентом успешно установлено
   */
   void onClientConnect(int clientID);
   /*!
   Соединение с клиентом прервано
   */
   void onClientDisconnect(int clientID);
private:
   /*!
   Обрабатываем сообщения от приложения
   */
   void dispatchAppMessages();

   //определяем тип функции, которая будет вызвана при приеме ответа от сервера
   typedef std::function<void(DebuggerVSCode& vsdebugger, Json::Value args, Json::Value &response)> ResponseCallback;
   //определяем тип функции, которая будет вызвана при приеме события от сервера
   typedef std::function<void(DebuggerVSCode& vsdebugger, Json::Value &response)> EventCallback;

   // ----------- Основные функции петли потока --------------------  
   /*!
   Отправляем необходимые запросы на сервер
   */
   void send();
   /*!
   Получаем ответы от сервера
   */
   void receive();

   // ----------- Протокол работы с сервером Visual Studio Code --------------------  
   /*!
   Парсим поступившие данные из сокета
   */
   void handleData(char* data, size_t dataSize);
   /*!
   Парсим пакет сокета
   */
   void dispatch(string &str);
   /*!
   Соединяем текущий буфкр с новым
   */
   void concatRawData(char* data, size_t dataSize);
   /*!
   Обрезаем данные
   */
   void sliceRawData(size_t dataSize);
   /*!
   Ищем последовательность в сырых данных
   */
   int rawDataIndexOf(const char*);
   /*!
   Разрезаем строку на части используя регулярное выражение
   */
   vector<string> split(const string& input, const string& regex);
   /*!
   Обрабатываем запрос клиента
   */
   void dispatchRequest(const string& command, Json::Value &request);
   /*!
   Отправляем ответ на запрос
   */
   void sendResponse(Json::Value &response);
   /*!
   Отправляем событие
   */
   void sendEvent(const string name, Json::Value* body = NULL);

   enum ErrorDestination {
      ERROR_DESTINATION_USER,
      ERROR_DESTINATION_TELEMETRY
   };
   /*!
   Отправляем ошибочный ответ на запрос
   */
   void sendErrorResponse(Json::Value &response, UInt code, string text, Json::Value *variables, ErrorDestination dest = ERROR_DESTINATION_USER);
   /*!
   Закрываем приложение
   */
   void shutdown();

   /// ----- Ниже перечислены запросы от клиента Visual Studio Code ---
   void initializeRequest(Json::Value &response, Json::Value &args);

   void disconnectRequest(Json::Value &response, Json::Value &args);

   void launchRequest(Json::Value &response, Json::Value &args);

   void attachRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void restartRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void setBreakPointsRequest(Json::Value &response, Json::Value &args);

   void setFunctionBreakPointsRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void setExceptionBreakPointsRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void configurationDoneRequest(Json::Value &response, Json::Value &args);

   void continueRequest(Json::Value &response, Json::Value &args);

   void nextRequest(Json::Value &response, Json::Value &args);

   void stepInRequest(Json::Value &response, Json::Value &args);

   void stepOutRequest(Json::Value &response, Json::Value &args);

   void stepBackRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void reverseContinueRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void restartFrameRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void gotoRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void pauseRequest(Json::Value &response, Json::Value &args);

   void sourceRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void threadsRequest(Json::Value &response);

   void stackTraceRequest(Json::Value &response, Json::Value &args);

   void scopesRequest(Json::Value &response, Json::Value &args);

   void variablesRequest(Json::Value &response, Json::Value &args);

   void setVariableRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void evaluateRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void stepInTargetsRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void gotoTargetsRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void completionsRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void exceptionInfoRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }
   void loadedSourcesRequest(Json::Value &response, Json::Value &args) {
      sendResponse(response);
   }

   /**
   * Override this hook to implement custom requests.
   */
   void customRequest(const string &command, Json::Value &response, Json::Value &args) {
      sendErrorResponse(response, 1014, "unrecognized request",0, ERROR_DESTINATION_TELEMETRY);
   }

   void sendStoppedEvent(string reason, string* exceptionText = NULL);

   /// - ответы на запросы от приложения
   void launchAppResponse(Json::Value & message);
   void setBreakPointsAppResponse(Json::Value & message);
   void configurationDoneAppResponse(Json::Value & message);
   void stackTraceAppResponse(Json::Value & message);
   void scopesAppResponse(Json::Value & message);
   void variablesAppResponse(Json::Value & message);
   void pauseAppResponse(Json::Value & message);
   void continueAppResponse(Json::Value & message);
   void nextAppResponse(Json::Value & message);
   void stepInAppResponse(Json::Value & message);
   void stepOutAppResponse(Json::Value & message);
   /// - события от приложения
   void stepAppEvent(Json::Value &message);
   void entryAppEvent(Json::Value &message);
   void stepInAppEvent(Json::Value &message);
   void stepOverAppEvent(Json::Value &message);
   void stepOutAppEvent(Json::Value &message);
   void pauseAppEvent(Json::Value &message);
   void breakPointAppEvent(Json::Value &message);
   void outputAppEvent(Json::Value &message);
   void exceptionAppEvent(Json::Value &message);
   void terminatedAppEvent(Json::Value &message);

   string                              _hostName;                                   //!< имя хоста 
   Int                                 _port;                                       //!< порт
   int                                 _sequence;                                   //!< номер последовательности команд отправляемых на сервер
   shared_ptr<TCPServer>               _server;                                     //!< объект сервера, слушающий подключения
   UInt                                _clientID;                                   //!< идентификатор клиента, подключенного к серверу
   std::map<int, ResponseCallback>     _pendingRequests;                            //!< ожидающие ответа запросы
   std::map<string, EventCallback>     _pendingEvents;                              //!< ожидаемые события от сервера
   bool                                _supportsConfigurationDoneRequest = false;   //!< флаг завершения конфигурации
   Int                                 _defaultTimeout = 5000;                      //!< таймаут по умолчанию
   char*                               _rawData = NULL;                             //!< данные из сокета до парсинга
   size_t                              _rawDataSize = 0;                            //!< размер сырых данных
   size_t                              _rawDataCapacity = 0;                        //!< размер буфера для сырых данных
   Int                                 _contentLength = -1;                         //!< размер полученного контента
   bool                                _terminated = false;                         //!< флаг прерывания потока

   bool                                _clientLinesStartAt1 = false;                
   bool                                _clientColumnsStartAt1 = false;
};
#endif
