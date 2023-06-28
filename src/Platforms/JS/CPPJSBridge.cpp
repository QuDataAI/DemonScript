#include "CPPJSBridge.h"
#include "Application.h"
                                                              
#ifndef VS_COMPILE_EMSCRIPTEN_PROJ
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>
#endif

void ds_onMessage(const string &msg)
{
   CPPJSBridge::instance().onMessage(msg);
}


#ifndef VS_COMPILE_EMSCRIPTEN_PROJ

EMSCRIPTEN_BINDINGS(viewer) {
   emscripten::function("postMessage", &ds_onMessage);
}

#endif


void CPPJSBridge::postMessage(const string &msg)
{
   int retVal = EM_ASM_INT({
      return global_DemonScript_instance.onMessage(Pointer_stringify($0));
   }, msg.c_str());
}


void CPPJSBridge::postMessage(const Json::Value &msg)
{
   Json::FastWriter jsonWriter;

   std::string jsonString = jsonWriter.write(msg);

   postMessage(jsonString);
}


void CPPJSBridge::onMessage(const string &msg)
{
   Json::Reader reader;
   Json::Value  value;
   bool res = reader.parse(msg, value);
   if (!res)
   {
      TRACE_CRITICAL << "Can't parse message: " << msg << "endl";
      return;
   }
   if (value["kind"].isNull())
      return;

   string msgKind = value["kind"].asString();
   if (msgKind == "start")
   {
      //старт приложения
      _appl = 0; //вызов деструкторов раньше нового конструктора
      _appl = make_shared<Application>();

      vector<string> args;
      args.push_back(""); 

      if (!value["listing"].isNull() && value["listing"].asBool())
         args.push_back("-l");
      if (!value["tracing"].isNull() && value["tracing"].asBool())
         args.push_back("-t");
      if (!value["file"].isNull())
         args.push_back(value["file"].asString());
      const char **argsC = new const char*[args.size()];
      for (int i = 0; i < args.size(); i++)
         argsC[i] = args[i].c_str();

      _appl->start(args.size(), (char**)argsC);

      delete [] argsC;

      sendCompleteMessage();
   }
}

void CPPJSBridge::sendCompleteMessage()
{
   Json::Value  msgJSON;
   msgJSON["kind"] = "complete";
   msgJSON["time"] = (int)_appl->scriptRunTime();

   postMessage(msgJSON);
}

void CPPJSBridge::onTrace(TraceLevel level, size_t line, size_t column, string &msg)
{
   Json::Value  msgJSON;
   msgJSON["kind"]   = "trace";
   msgJSON["level"]  = level;
   msgJSON["line"]   = line;
   msgJSON["column"] = column;
   msgJSON["msg"]    = msg;

   postMessage(msgJSON);

#ifdef VS_COMPILE_EMSCRIPTEN_PROJ
   printf(msg.c_str());
#endif
}
