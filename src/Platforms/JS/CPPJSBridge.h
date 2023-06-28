/*!            CPPJSBridge - модуль взаимодействия С++ кода с кодом Java Script

(с) 2018-nov: steps: synset.com, qudata.com
****************************************************************************************/

#ifndef CPPJSBridgeH
#define CPPJSBridgeH

#ifndef VS_COMPILE_EMSCRIPTEN_PROJ
#define JS_API_VERSION
#endif

#ifdef VS_COMPILE_EMSCRIPTEN_PROJ
#define EM_ASM_(code, ...) 
#define EM_ASM(code) 
#define EM_ASM_INT(code) 0
#define EM_ASM_DOUBLE(code) 0
#define EM_ASM_INT_V(code) 0
#define EM_ASM_DOUBLE_V(code) 0
#define EMSCRIPTEN_KEEPALIVE
#else
#include <emscripten.h>
#endif

#include "Trace.h"
#include "SingleTone.h"
#include "Application.h"
#include "Sys/ThirdParty/Jsoncpp/json/json.h"
#include <memory.h>

class CPPJSBridge
{
   IMPLEMENTATION_SINGLETONE(CPPJSBridge);
public:
   /*!
   Вывод отладочной строки из демон скрипта
   \param level уровень критичности сообщения
   \param line строка скрипта в которой сейчас находимся
   \param column колонка строки скрипта в которой сейчас находимся
   \param msg строка
   */
   void onTrace(TraceLevel level, size_t line, size_t column, string &msg);

   /*!
   Отправить сообщение в JavaScript
   \param msg текст сообщения
   */
   void postMessage(const string &msg);
   /*!
   Отправить сообщение в JavaScript
   \param msg json сообщения
   */
   void postMessage(const Json::Value &msg);
   /*!
   Получено сообщение из JavaScript
   \param msg текст сообщения
   */
   void onMessage(const string &msg);
private:
   /*!
   Отправить сообщение о завершении выполнения
   */
   void sendCompleteMessage();

	shared_ptr<Application>       _appl;
};

#endif
