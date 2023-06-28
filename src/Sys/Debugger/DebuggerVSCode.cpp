#include "DebuggerVSCode.h"
#include "DebuggerMessenger.h"
#include "Sys/Net/TCPServer.h"
#include <regex>
#include <algorithm>  
#include <cmath> 
#include <thread>

#define DEBUGGER_VSCODE_TRACE

#define TWO_CRLF "\r\n\r\n"

DebuggerVSCode::DebuggerVSCode(Int port, DebuggerMessenger * messenger):
   DebuggerClient(messenger),
   _port(port),
   _sequence(0),
   _clientID(-1)
{
}


void DebuggerVSCode::run()
{
   _server = TCPServer::create();

   if (!_server->init(_port, 1, this))
      return;

   _server->start();

   if (_clientID < 0)
      return;

   const int sleepMs = 50;

   while (_server->connected(_clientID) && !_terminated)
   {
        send();
        receive();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
   }
}

void DebuggerVSCode::onClientConnect(int clientID)
{
   _clientID = clientID;
}

void DebuggerVSCode::onClientDisconnect(int clientID)
{
   clientID = clientID;
}

void DebuggerVSCode::dispatchAppMessages()
{
   Json::Value message;
   while (_messenger->popMessageForDebugger(message))
   {
      string messageType = message["type"].asString();
      string messageName = message["name"].asString();
      if (messageType == "response")
      {
         if (messageName == "launch")
            launchAppResponse(message);
         else if(messageName == "setBreakPoints")
            setBreakPointsAppResponse(message);
         else if (messageName == "configurationDone")
            configurationDoneAppResponse(message);
         else if (messageName == "stackTrace")
            stackTraceAppResponse(message);
         else if (messageName == "scopes")
            scopesAppResponse(message);
         else if (messageName == "variables")
            variablesAppResponse(message);
         else if (messageName == "pause")
            pauseAppResponse(message);
         else if (messageName == "continue")
            continueAppResponse(message);
         else if (messageName == "stepOver")
            nextAppResponse(message);
         else if (messageName == "stepIn")
            stepInAppResponse(message);
         else if (messageName == "stepOut")
            stepOutAppResponse(message);
      }
      else if (messageType == "event")
      {
         if (messageName == "step")
            stepAppEvent(message);
         else if (messageName == "entry")
            entryAppEvent(message);
         else if (messageName == "breakPoint")
            breakPointAppEvent(message);
         else if (messageName == "stepIn")
            stepInAppEvent(message);
         else if (messageName == "stepOver")
            stepOverAppEvent(message);
         else if (messageName == "stepOut")
            stepOutAppEvent(message);
         else if (messageName == "pause")
            pauseAppEvent(message);
         else if (messageName == "output")
            outputAppEvent(message);
         else if (messageName == "exception")
            exceptionAppEvent(message);
         else if (messageName == "terminated")
            terminatedAppEvent(message);
      }
   }
}

void DebuggerVSCode::sendStoppedEvent(string reason, string* exceptionText/* = NULL*/)
{
   Json::Value body;
   body["reason"]   = reason;
   body["threadId"] = 1;
   if (exceptionText)
   {
      body["text"] = *exceptionText;
   }
   sendEvent("stopped", &body);
}

void DebuggerVSCode::stepAppEvent(Json::Value & message)
{
   sendStoppedEvent("step");
}

void DebuggerVSCode::entryAppEvent(Json::Value & message)
{
   sendStoppedEvent("entry");
}

void DebuggerVSCode::stepInAppEvent(Json::Value & message)
{
   sendStoppedEvent("step");
}

void DebuggerVSCode::stepOverAppEvent(Json::Value & message)
{
   sendStoppedEvent("step");
}

void DebuggerVSCode::stepOutAppEvent(Json::Value & message)
{
   sendStoppedEvent("step");
}

void DebuggerVSCode::pauseAppEvent(Json::Value & message)
{
   sendStoppedEvent("step");
}

void DebuggerVSCode::breakPointAppEvent(Json::Value & message)
{
   Json::Value breakpoint;
   breakpoint["verified"] = true;
   breakpoint["line"]     = 3;
   breakpoint["column"] = 0;

   Json::Value body;
   body["reason"]     = "breakpoint";
   body["breakpoint"] = breakpoint;
   body["threadId"]   = 1;


   sendEvent("stopped", &body);

   sendEvent("breakpoint", &body);
}

void DebuggerVSCode::outputAppEvent(Json::Value & message)
{
   string sss = message["msg"].asString();


   Json::Value body;
   body["category"]   = "output";
   body["output"]     = message["msg"];
   body["source"]     = message["source"];
   body["line"]       = message["line"];
   body["column"]     = message["column"];

   sendEvent("output", &body);
}

void DebuggerVSCode::exceptionAppEvent(Json::Value & message)
{
   string msg = message["msg"].asString();
   sendStoppedEvent("exception", &msg);
}

void DebuggerVSCode::terminatedAppEvent(Json::Value & ev)
{
   sendEvent("terminated");
}

void DebuggerVSCode::concatRawData(char * data, size_t dataSize)
{
   //!< если размера буфера недостаточно, расширяем
   if (_rawDataSize + dataSize > _rawDataCapacity)
   {
      _rawDataCapacity = _rawDataSize + dataSize;
      char * newBuffer = new char[_rawDataCapacity];
      memset(newBuffer, 0, _rawDataCapacity);     
      if (_rawDataSize > 0)
         memcpy(newBuffer, _rawData, _rawDataSize);
      memcpy(&newBuffer[_rawDataSize], data, dataSize);
      if (_rawData)
         delete[] _rawData;
      _rawDataSize += dataSize;
      _rawData = newBuffer;
   }
   else
   {
      memcpy(&_rawData[_rawDataSize], data, dataSize);
      _rawDataSize += dataSize;
   }
}

void DebuggerVSCode::sliceRawData(size_t dataSize)
{
   if (dataSize > _rawDataSize)
      return;

   size_t sizeForSlice = _rawDataSize - dataSize;

   memcpy(_rawData, &_rawData[dataSize], sizeForSlice);
   memset(&_rawData[sizeForSlice], 0, _rawDataSize - sizeForSlice);

   _rawDataSize = sizeForSlice;
}

int DebuggerVSCode::rawDataIndexOf(const char * str)
{
   size_t strLen = strlen(str);

   if (_rawDataSize < strLen)
      return -1;

   for (size_t i = 0; i < _rawDataSize - strLen; i++)
   {
      bool found = true;
      for (size_t j = 0; j < strLen; j++)
         if (_rawData[i+j] != str[j])
         {
            found = false;
            break;
         }
      if (found)
         return (Int)i;
   }
   return -1;
}

void DebuggerVSCode::handleData(char* data, size_t dataSize)
{
   concatRawData(data, dataSize);
   while (true) {
      if (_contentLength >= 0) {
         if ((Int)_rawDataSize >= _contentLength) {
            string message(_rawData, _contentLength);
            sliceRawData(_contentLength);
            _contentLength = -1;
            if (message.length() > 0) {
               dispatch(message);
            }
            continue; // there may be more complete messages to process
         }
         else
         {
            //данный пакет, полностью не передался, ждем
            break;
         }
      }
      else {
         int idx = rawDataIndexOf(TWO_CRLF);
         if (idx != -1) {
            string header(_rawData, idx);
            vector<string> lines = split(header,"\r\n");
            for (size_t i = 0; i < lines.size(); i++) {
               //vector<string> pair  = split(lines[i],"/:+/");
               vector<string> pair = split(lines[i], ": ");
               if (pair[0] == "Content-Length") {
                  _contentLength = stoi(pair[1]);
               }
            }
            sliceRawData(idx + strlen(TWO_CRLF));
            continue;
         }
      }
      break;
   }

}

void DebuggerVSCode::send()
{
   dispatchAppMessages();
}

void DebuggerVSCode::receive()
{
   const int TmpBufSize = 1024;
   char tmpBuf[TmpBufSize];

   int received = _server->receiveFromClient(_clientID, tmpBuf, TmpBufSize);

   if (received == -1)
   {
      //произошла ошибка, закрываем соединение
      _server->disconnect(_clientID);
      return;
   }

   if (received > 0)
      handleData(tmpBuf, received);
}

void DebuggerVSCode::dispatch(string & str)
{
   Json::Reader reader;
   Json::Value  value;
   bool res = reader.parse(str, value);
   if (!res)
      return;

   if (value["type"].isNull())
      return;

   if (value["type"] == "request" && !value["command"].isNull())
   {
#ifdef DEBUGGER_VSCODE_TRACE
      printf("From client: %s(%s)\n", value["command"].asCString(), str.c_str());
#endif
      //обрабатываем запрос
      dispatchRequest(value["command"].asString(), value);
   }
}


vector<string> DebuggerVSCode::split(const string & input, const string & regex)
{
   // passing -1 as the submatch index parameter performs splitting
   std::regex re(regex);
   std::sregex_token_iterator
      first{ input.begin(), input.end(), re, -1 },
      last;
   return{ first, last };
}

void DebuggerVSCode::dispatchRequest(const string & command, Json::Value & request)
{  
   //\node_modules\vscode-debugadapter\lib 
   Json::Value response;

   response["seq"]         = 0;
   response["type"]        = "response";
   response["request_seq"] = request["seq"];
   response["command"]     = command;
   response["success"]     = true;

   if (command == "initialize") {
      Json::Value args = request["arguments"];
      if (request["linesStartAt1"].isBool()) {
         _clientLinesStartAt1 = request["linesStartAt1"].asBool();
      }
      if (request["columnsStartAt1"].isBool()) {
         _clientColumnsStartAt1 = request["columnsStartAt1"].isBool();
      }
      if (request["pathFormat"].asString() == "path") {
         sendErrorResponse(response, 2018, "debug adapter only supports native paths", 0, ERROR_DESTINATION_TELEMETRY);
      }
      else {
         Json::Value initializeResponse = response;
         initializeResponse["body"] = Json::Value();
         initializeRequest(initializeResponse, args);
      }
   }
   else if (command == "launch") {
      launchRequest(response, request["arguments"]);
   }
   else if (command == "attach") {
      attachRequest(response, request["arguments"]);
   }
   else if (command == "disconnect") {
      disconnectRequest(response, request["arguments"]);
   }
   else if (command == "restart") {
      restartRequest(response, request["arguments"]);
   }
   else if (command == "setBreakpoints") {
      setBreakPointsRequest(response, request["arguments"]);
   }
   else if (command == "setFunctionBreakpoints") {
      setFunctionBreakPointsRequest(response, request["arguments"]);
   }
   else if (command == "setExceptionBreakpoints") {
      setExceptionBreakPointsRequest(response, request["arguments"]);
   }
   else if (command == "configurationDone") {
      configurationDoneRequest(response, request["arguments"]);
   }
   else if (command == "continue") {
      continueRequest(response, request["arguments"]);
   }
   else if (command == "next") {
      nextRequest(response, request["arguments"]);
   }
   else if (command == "stepIn") {
      stepInRequest(response, request["arguments"]);
   }
   else if (command == "stepOut") {
      stepOutRequest(response, request["arguments"]);
   }
   else if (command == "stepBack") {
      stepBackRequest(response, request["arguments"]);
   }
   else if (command == "reverseContinue") {
      reverseContinueRequest(response, request["arguments"]);
   }
   else if (command == "restartFrame") {
      restartFrameRequest(response, request["arguments"]);
   }
   else if (command == "goto") {
      gotoRequest(response, request["arguments"]);
   }
   else if (command == "pause") {
      pauseRequest(response, request["arguments"]);
   }
   else if (command == "stackTrace") {
      stackTraceRequest(response, request["arguments"]);
   }
   else if (command == "scopes") {
      scopesRequest(response, request["arguments"]);
   }
   else if (command == "variables") {
      variablesRequest(response, request["arguments"]);
   }
   else if (command == "setVariable") {
      setVariableRequest(response, request["arguments"]);
   }
   else if (command == "source") {
      sourceRequest(response, request["arguments"]);
   }
   else if (command == "threads") {
      threadsRequest(response);
   }
   else if (command == "evaluate") {
      evaluateRequest(response, request["arguments"]);
   }
   else if (command == "stepInTargets") {
      stepInTargetsRequest(response, request["arguments"]);
   }
   else if (command == "gotoTargets") {
      gotoTargetsRequest(response, request["arguments"]);
   }
   else if (command == "completions") {
      completionsRequest(response, request["arguments"]);
   }
   else if (command == "exceptionInfo") {
      exceptionInfoRequest(response, request["arguments"]);
   }
   else if (command == "loadedSources") {
      loadedSourcesRequest(response, request["arguments"]);
   }
   else {
      customRequest(command, response, request["arguments"]);
   }
}



void DebuggerVSCode::sendResponse(Json::Value & response)
{  
   response["type"]      = "response";
   response["seq"]       = 0;
   
   Json::FastWriter jsonWriter;

   std::string jsonString = jsonWriter.write(response);
   size_t   jsonStringLen = jsonString.length();

#ifdef DEBUGGER_VSCODE_TRACE
   printf("To client: %s\n", jsonString.c_str());
#endif

   string requestString = "Content-Length: " + std::to_string(jsonStringLen) + TWO_CRLF + jsonString;
   _sequence++;

   _server->sendToClient(_clientID, requestString.c_str(), requestString.length());
}

void DebuggerVSCode::sendEvent(const string name, Json::Value* body /*= NULL*/)
{
   Json::Value eventJson;
   eventJson["type"] = "event";
   eventJson["seq"]  = 0;
   eventJson["event"] = name;
   if (body)
      eventJson["body"] = *body;

   Json::FastWriter jsonWriter;

   std::string jsonString = jsonWriter.write(eventJson);
   size_t   jsonStringLen = jsonString.length();

#ifdef DEBUGGER_VSCODE_TRACE
   printf("To client: %s\n", jsonString.c_str());
#endif

   string requestString = "Content-Length: " + std::to_string(jsonStringLen) + TWO_CRLF + jsonString;
   _sequence++;

   _server->sendToClient(_clientID, requestString.c_str(), requestString.length());
}

void DebuggerVSCode::sendErrorResponse(Json::Value & response, UInt code, string text, Json::Value *variables, ErrorDestination dest/* = ERROR_DESTINATION_USER*/)
{
   Json::Value msg;
   msg["id"]     = (unsigned int)code;
   msg["format"] = text;
   if (variables)
   {
      msg["variables"] = *variables;
   }
   switch (dest)
   {
      case ERROR_DESTINATION_USER:      msg["showUser"]      = true; break;
      case ERROR_DESTINATION_TELEMETRY: msg["sendTelemetry"] = true; break;
   }
   response["success"] = false;
   //response["message"] = DebugSession.formatPII(msg.format, true, msg.variables);
   response["message"]   = text;
   response["body"]["error"] = msg;
   sendResponse(response);
}

void DebuggerVSCode::shutdown()
{
   _server->disconnect(_clientID);
}

void DebuggerVSCode::initializeRequest(Json::Value & response, Json::Value & args)
{
   /*
   // default parameters

   // This default debug adapter does not support conditional breakpoints.
   response["body"]["supportsConditionalBreakpoints"] = false;
   // This default debug adapter does not support hit conditional breakpoints.
   response["body"]["supportsHitConditionalBreakpoints"] = false;
   // This default debug adapter does not support function breakpoints.
   response["body"]["supportsFunctionBreakpoints"] = false;
   // This default debug adapter implements the 'configurationDone' request.
   response["body"]["supportsConfigurationDoneRequest"] = true;
   // This default debug adapter does not support hovers based on the 'evaluate' request.
   response["body"]["supportsEvaluateForHovers"] = false;
   // This default debug adapter does not support the 'stepBack' request.
   response["body"]["supportsStepBack"] = false;
   // This default debug adapter does not support the 'setVariable' request.
   response["body"]["supportsSetVariable"] = false;
   // This default debug adapter does not support the 'restartFrame' request.
   response["body"]["supportsRestartFrame"] = false;
   // This default debug adapter does not support the 'stepInTargetsRequest' request.
   response["body"]["supportsStepInTargetsRequest"] = false;
   // This default debug adapter does not support the 'gotoTargetsRequest' request.
   response["body"]["supportsGotoTargetsRequest"] = false;
   // This default debug adapter does not support the 'completionsRequest' request.
   response["body"]["supportsCompletionsRequest"] = false;
   // This default debug adapter does not support the 'restart' request.
   response["body"]["supportsRestartRequest"] = false;
   // This default debug adapter does not support the 'exceptionOptions' attribute on the 'setExceptionBreakpointsRequest'.
   response["body"]["supportsExceptionOptions"] = false;
   // This default debug adapter does not support the 'format' attribute on the 'variablesRequest', 'evaluateRequest', and 'stackTraceRequest'.
   response["body"]["supportsValueFormattingOptions"] = false;
   // This debug adapter does not support the 'exceptionInfoRequest' request.
   response["body"]["supportsExceptionInfoRequest"] = false;
   // This debug adapter does not support the 'TerminateDebuggee' attribute on the 'disconnect' request.
   response["body"]["supportTerminateDebuggee"] = false;
   // This debug adapter does not support delayed loading of stack frames.
   response["body"]["supportsDelayedStackTraceLoading"] = false;
   */

   // custom parameters

   // the adapter implements the configurationDoneRequest.
   response["body"]["supportsConfigurationDoneRequest"] = true;

   // make VS Code to use 'evaluate' when hovering over source
   response["body"]["supportsEvaluateForHovers"] = true;

   sendResponse(response);

   sendEvent("initialized");
}

void DebuggerVSCode::disconnectRequest(Json::Value & response, Json::Value & args)
{
   sendResponse(response);
   shutdown();
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "disconnect";
   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::launchRequest(Json::Value & response, Json::Value & args)
{
   if (!args["trace"].isNull())
   {
      // setup trace commands
   }

   string program     = "";
   bool   stopOnEntry = false;

   if (!args["program"].isNull())
   {
      program = args["program"].asString();
   }

   if (!args["stopOnEntry"].isNull())
   {
      stopOnEntry = args["stopOnEntry"].asBool();
   }
  
   Json::Value msgForApp;
   msgForApp["type"]       = "request";
   msgForApp["name"]       = "launch";
   msgForApp["fileName"]   = program;
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::launchAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::setBreakPointsRequest(Json::Value & response, Json::Value & args)
{   
   Json::Value msgForApp;
   msgForApp["type"]        = "request";
   msgForApp["name"]        = "setBreakPoints";
   msgForApp["sourceName"]  = args["source"]["name"];
   msgForApp["sourcePath"]  = args["source"]["path"];
   msgForApp["lines"]       = args["lines"];
   msgForApp["clientData"]  = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::setBreakPointsAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   if (!message["lines"].isNull() && message["lines"].isArray())
   {
      int breakPoints = message["lines"].size();
      for (int i = 0; i < breakPoints; i++)
      {
         Json::Value breakPointJson;
         breakPointJson["verified"] = true;
         breakPointJson["line"] = message["lines"][i].asInt();
         breakPointJson["id"]   = message["ids"][i].asInt();
         response["body"]["breakpoints"][i] = breakPointJson;
      }
   }
   sendResponse(response);
}

void DebuggerVSCode::configurationDoneRequest(Json::Value & response, Json::Value & args)
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "configurationDone";
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::configurationDoneAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::continueRequest(Json::Value & response, Json::Value & args)
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "continue";
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::continueAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::pauseRequest(Json::Value & response, Json::Value & args)
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "pause";
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::pauseAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::nextRequest(Json::Value &response, Json::Value &args)
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "stepOver";
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::nextAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::stepInRequest(Json::Value &response, Json::Value &args) 
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "stepIn";
   msgForApp["clientData"] = response;   
   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::stepInAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::stepOutRequest(Json::Value &response, Json::Value &args) 
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "stepOut";
   msgForApp["clientData"] = response;
   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::stepOutAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   sendResponse(response);
}

void DebuggerVSCode::threadsRequest(Json::Value & response)
{
   Json::Value threadInfo;

   threadInfo["id"]   = 1;
   threadInfo["name"] = "Thread 1";

   response["body"]["threads"][0] = threadInfo;
   sendResponse(response);
}

void DebuggerVSCode::stackTraceRequest(Json::Value & response, Json::Value & args)
{
   Json::Value msgForApp;
   msgForApp["type"] = "request";
   msgForApp["name"] = "stackTrace";
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::stackTraceAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];

   int totalFrames = message["totalFrames"].asInt();

   for (int i = 0; i < totalFrames; i++)
   {
      Json::Value &appStackFrame = message["stackFrames"][i];
      Json::Value clientStackFrame;

      clientStackFrame["id"]                         = appStackFrame["id"];
      clientStackFrame["line"]                       = appStackFrame["line"];
      clientStackFrame["column"]                     = appStackFrame["column"];
      clientStackFrame["name"]                       = appStackFrame["name"];
      clientStackFrame["source"]["name"]             = appStackFrame["source"]["name"];
      clientStackFrame["source"]["path"]             = appStackFrame["source"]["path"];
      clientStackFrame["source"]["sourceReference"]  = 0;
      clientStackFrame["source"]["adapterData"]      = "mock-adapter-data";
      response["body"]["stackFrames"][i]             = clientStackFrame;
   }
   response["body"]["totalFrames"] = totalFrames;

   sendResponse(response);
}

void DebuggerVSCode::scopesRequest(Json::Value & response, Json::Value & args)
{  
   Json::Value msgForApp;
   msgForApp["type"]    = "request";
   msgForApp["name"]    = "scopes";
   msgForApp["frameId"] = args["frameId"];
   msgForApp["clientData"] = response;

   _messenger->pushMessageForApp(msgForApp);
}

void DebuggerVSCode::scopesAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];
   size_t scopesTotal = message["scopes"].size();
   for (int i = 0; i < scopesTotal; i++)
   {
      Json::Value &appScopeJson = message["scopes"][i];
      Json::Value scopeJson;

      scopeJson["name"]               = appScopeJson["name"];
      scopeJson["variablesReference"] = appScopeJson["variablesReference"];
      scopeJson["expensive"]          = appScopeJson["expensive"];
      response["body"]["scopes"][i]   = scopeJson;
   }

   sendResponse(response);
}

void DebuggerVSCode::variablesRequest(Json::Value & response, Json::Value & args)
{
   Json::Value msgForApp;
   msgForApp["type"]               = "request";
   msgForApp["name"]               = "variables";
   msgForApp["variablesReference"] = args["variablesReference"];
   msgForApp["clientData"]         = response;

   _messenger->pushMessageForApp(msgForApp);   
}
  
void DebuggerVSCode::variablesAppResponse(Json::Value & message)
{
   Json::Value response = message["clientData"];

   int variablesTotal = message["variablesTotal"].asInt();
   for (int i = 0; i < variablesTotal; i++)
   {
      Json::Value &appVarJson = message["variables"][i];
      Json::Value varJson;

      varJson["name"]               = appVarJson["name"];
      varJson["type"]               = appVarJson["type"];
      varJson["value"]              = appVarJson["value"];
      varJson["variablesReference"] = appVarJson["variablesReference"];

      response["body"]["variables"][i] = varJson;
   }

   sendResponse(response);
}



