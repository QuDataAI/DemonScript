/*!               DebuggerMessenger - обменник сообщениями между потоком приложения и клиентом отладки


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef DebuggerMessengerH
#define DebuggerMessengerH

#include <mutex>
#include <queue>
#include "Sys/ThirdParty/Jsoncpp/json/json.h"

//=======================================================================================
//! Обменник сообщениями между потоком приложения и клиентом отладки
//
class DebuggerMessenger {
public:
   /*!
   Добавить сообщение отладчику
   \param ev параметры события в JSON
   */
   void pushMessageForDebugger(Json::Value &ev);
   /*!
   Получить событие для отладчика
   \param ev параметры события в JSON
   */
   bool popMessageForDebugger(Json::Value &ev);
   /*!
   Добавить событие для приложения
   \param ev параметры события в JSON
   */
   void pushMessageForApp(Json::Value &ev);
   /*!
   Получить событие для приложения
   \param ev параметры события в JSON
   */
   bool popMessageForApp(Json::Value &ev);
private:
   class MessengerLocker
   {
   public:
      MessengerLocker(std::mutex &mutex) :_mutex(mutex) { _mutex.lock(); }
      ~MessengerLocker() { _mutex.unlock(); }
   private:
      std::mutex &_mutex;
   };
   std::queue<Json::Value>             _messagesForDebugger;   //!< сообщения от приложения к отладчику
   std::queue<Json::Value>             _messagesForApp;        //!< сообщения от отладчика к приложению
   std::mutex                          _eventQueueMutex;       //!< блокировка доступа к общей памяти
};

#endif