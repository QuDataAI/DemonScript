#include "Debugger.h"
#include "Script/Stack/StackFrame.h"
#include "Utils.h"
#include "Application.h"
#include "Graph.h"
#include "Command.h"
#include "Function.h"
#include "DebuggerVSCode.h"
#include <thread>
#include <locale>
#include "Graph/GraphManager.h"

void  Debugger::run(Application * application, string protocolName, Int port)
{
   _application = application;

   if (protocolName == "vscode")
      _client = new DebuggerVSCode(port, &_messenger);

   if (!_client)
      return;

   _thread = thread(runThread, _client);
   _thread.detach();

   _debugging = true;

   setCommand(DEBUGGER_COMMAND_WAIT);

   while (true) {
      dispatchDebuggerClientMessages();
      if (_command != DEBUGGER_COMMAND_WAIT)
         break;
      else
         Utils::sleep(50);
   }
}

void Debugger::onLaunch()
{
   _application->runInterpreter();
}

void Debugger::onTrace(TraceLevel level, size_t line, size_t column, string & msg)
{
   string srcName, srcFile;

   currentSourcePos(srcName, srcFile, line);

   Json::Value message;
   message["type"]   = "event";
   message["name"]   = "output";
   //message["source"] = srcFile;
   message["source"] = "";
   message["line"]   = (unsigned int)line;
   message["column"] = (unsigned int)column;
   message["msg"]    = msg;
   _messenger.pushMessageForDebugger(message);
}

void Debugger::onException(size_t line, size_t column, string & msg)
{
   if (_command == DEBUGGER_COMMAND_QUIT)
      return;

   if (_application->_script->state() == Script::ST_RUN)
   {
      _currentStackFrame = StackFrame::currentStackFrame();
   }

   _exceptionLine = line;

   Json::Value message;
   message["type"]    = "event";
   message["name"]    = "exception";
   message["msg"]     = msg;
   _messenger.pushMessageForDebugger(message);

   setCommand(DEBUGGER_COMMAND_WAIT);

   dispatch();

   _currentStackFrame = NULL;
}

bool Debugger::onStep()
{
   if (!_debugging)
      return true;

   _currentStackFrame   = StackFrame::currentStackFrame();

   dispatch();

   //закрываем доступ, чтоб случайно не обратиться из другого потока
   _currentStackFrame = NULL;
   return _command != DEBUGGER_COMMAND_QUIT;
}

void Debugger::onComplete()
{
   if (_command != DEBUGGER_COMMAND_QUIT)
   {
      // скрипт завершился и теперь отключаемся от отладчика
      pushEventForDebugger("terminated");
      setCommand(DEBUGGER_COMMAND_WAIT);
   }
   else
   {
      // скрипт прерван отладчиком, от которого уже отключились
   }
}

void Debugger::runThread(DebuggerClient * client)
{
   client->run();
}

void Debugger::dispatchDebuggerClientMessages()
{
   Json::Value message;

   while (_messenger.popMessageForApp(message))
   {
      string messageType = message["type"].asString();
      string messageName = message["name"].asString();
      if (messageType == "request")
      {
         if (messageName == "launch")
            launchDebuggerClientRequest(message);
         else if (messageName == "setBreakPoints")
            setBreakPointsDebuggerClientRequest(message);
         else if (messageName == "configurationDone")
            configurationDoneDebuggerClientRequest(message);
         else if (messageName == "stackTrace")
            stackTraceDebuggerClientRequest(message);
         else if (messageName == "scopes")
            scopesDebuggerClientRequest(message);
         else if (messageName == "variables")
            variablesDebuggerClientRequest(message);
         else if (messageName == "continue")
            continueDebuggerClientRequest(message);
         else if (messageName == "pause")
            pauseDebuggerClientRequest(message);
         else if (messageName == "stepOver")
            stepOverDebuggerClientRequest(message);
         else if (messageName == "stepIn")
            stepInDebuggerClientRequest(message);
         else if (messageName == "stepOut")
            stepOutDebuggerClientRequest(message);
         else if (messageName == "disconnect")
            disconnectDebuggerClientRequest(message);
      }
   }
}

void Debugger::dispatch()
{
   while (true) {
      dispatchDebuggerCommands();
      dispatchDebuggerClientMessages();
      if (_command != DEBUGGER_COMMAND_WAIT)
         break;
      else
         Utils::sleep(50);
   }
}

void Debugger::dispatchDebuggerCommands()
{
   switch (_command)
   {
      case DEBUGGER_COMMAND_WAIT:                                                       break;
      case DEBUGGER_COMMAND_PAUSE:     setCommand(DEBUGGER_COMMAND_WAIT);               
                                       pushEventForDebugger("pause");
                                       break;
      case DEBUGGER_COMMAND_CONTINUE:  checkBreakPoints();  break;
      case DEBUGGER_COMMAND_STEP_OVER: if (!checkTarget())
                                          checkBreakPoints();  
                                       break;
      case DEBUGGER_COMMAND_STEP_IN:   setCommand(DEBUGGER_COMMAND_WAIT);                                                           
                                       pushEventForDebugger("stepIn");
                                       break;
      case DEBUGGER_COMMAND_STEP_OUT:  if (!checkTarget())
                                          checkBreakPoints();
                                       break;
   }
}

void Debugger::pushEventForDebugger(string name)
{
   Json::Value message;
   message["type"] = "event";
   message["name"] = name;
   _messenger.pushMessageForDebugger(message);
}

void Debugger::validateBreakPoints()
{
   for (size_t i = 0; i < _breakPoints.size(); i++)
   {
      BreakPoint &bp = _breakPoints[i];
      if (!bp._valid)
      {
         bp._valid = _application->_script->srcID(bp._srcPath, bp._sourceID);
      }
   }

   _needValidateBreakPonts = false;
}


void Debugger::checkBreakPoints()
{   
   if (_breakPoints.size() == 0)
      return;

   CommandPtr currentCommand = _currentStackFrame->_code->_commands[_currentStackFrame->_command];

   size_t currentSourceID = currentCommand->sourceID();
   size_t currentLine = currentCommand->line();

   if (_needValidateBreakPonts)
      validateBreakPoints();

   for (size_t i = 0; i < _breakPoints.size(); i++)
   {
      BreakPoint &bp = _breakPoints[i];

      if (bp._valid && bp._line == currentLine && bp._sourceID == currentSourceID)
      {
         Json::Value message;
         message["type"] = "event";
         message["name"] = "breakPoint";
         message["id"]   = (unsigned int)(i + 1);
         _messenger.pushMessageForDebugger(message);
         _targetSourceID = _targetLine = 0;
         setCommand(DEBUGGER_COMMAND_WAIT);
         break;
      }         
   }
}

bool Debugger::checkTarget()
{
   CommandPtr currentCommand = _currentStackFrame->_code->_commands[_currentStackFrame->_command];

   size_t currentSourceID = currentCommand->sourceID();
   size_t currentLine = currentCommand->line();

   if (currentSourceID != _targetSourceID || currentLine != _targetLine)
      return false;

   _targetSourceID = _targetLine = 0;

   pushEventForDebugger(_command == DEBUGGER_COMMAND_STEP_OUT ? "stepOut" : "stepOver");

   setCommand(DEBUGGER_COMMAND_WAIT);

   return true;
}

void Debugger::launchDebuggerClientRequest(Json::Value & message)
{
   //sendResponse(message);
   _application->_scriptFileName = message["fileName"].asString();   
}

void Debugger::setBreakPointsDebuggerClientRequest(Json::Value & message)
{
   string sourceName = message["sourceName"].asString();
   string sourcePath = message["sourcePath"].asString();

   string sourcePathTrue = sourcePath;
   if (sourcePathTrue.length() > 2 && sourcePathTrue[1] == ':')
   {
      std::locale loc;
      sourcePathTrue[0] = std::toupper(sourcePathTrue[0], loc);
   }

   vector<BreakPoint> oldbreakPoints = _breakPoints;
   _breakPoints.clear();

   for (size_t i = 0; i < oldbreakPoints.size(); i++)
   {
      if (oldbreakPoints[i]._srcPath == sourcePathTrue)
         continue;

      _breakPoints.push_back(oldbreakPoints[i]);
   }

   if (!message["lines"].isNull() && message["lines"].isArray())
   {
      int breakPoints = message["lines"].size();
      for (int i = 0; i < breakPoints; i++)
      {
         message["ids"][i] = (unsigned int)(_breakPoints.size() + 1);
         _breakPoints.push_back(BreakPoint(sourcePathTrue, message["lines"][i].asInt()));
      }
   }

   _needValidateBreakPonts = true;

   sendResponse(message);
}

void Debugger::configurationDoneDebuggerClientRequest(Json::Value & message)
{
   sendResponse(message);
   setCommand(DEBUGGER_COMMAND_CONTINUE);
   onLaunch();
}

void Debugger::pauseDebuggerClientRequest(Json::Value & message)
{
   //_pendingRequests[DEBUGGER_COMMAND_PAUSE] = message;
   sendResponse(message);
   setCommand(DEBUGGER_COMMAND_WAIT);   
}

void Debugger::continueDebuggerClientRequest(Json::Value & message)
{
   sendResponse(message);
   setCommand(DEBUGGER_COMMAND_CONTINUE);
}

void Debugger::stepOverDebuggerClientRequest(Json::Value & message)
{   
   //_pendingRequests[DEBUGGER_COMMAND_STEP_OVER] = message;

   StackFrame* stackFrame = _currentStackFrame;
   _targetSourceID = _targetLine = 0;

   size_t sourceID = stackFrame->_code->_commands[stackFrame->_command]->sourceID();

   while (true)
   {
      if (stackFrame->_type != StackFrame::STACK_FRAME_TYPE_CODE)
      {
         if (stackFrame->_parent)
         {
            stackFrame = stackFrame->_parent;
            continue;
         }
         else break;
      }
      
      //пропускаем команды инклудов
      UInt command = stackFrame->_command;
      while (stackFrame->_code->_commands.size() > (size_t)command + 1 &&
            sourceID != stackFrame->_code->_commands[command + 1]->sourceID())
            command++;

      if (stackFrame->_code->_commands.size() > (size_t)command + 1)
      {
         CommandPtr target = stackFrame->_code->_commands[command + 1];
         _targetSourceID = target->sourceID();
         _targetLine     = target->line();
         break;
      }

      sourceID = stackFrame->_code->_commands[stackFrame->_command]->sourceID();
      //поднимаемся на верх если есть куда
      if (!stackFrame->_parent)
         break;
      stackFrame = stackFrame->_parent;
   }

   sendResponse(message);

   if (_targetLine>0)
      setCommand(DEBUGGER_COMMAND_STEP_OVER);
   else
      setCommand(DEBUGGER_COMMAND_CONTINUE);
}  

void Debugger::stepInDebuggerClientRequest(Json::Value & message)
{
   //_pendingRequests[DEBUGGER_COMMAND_STEP_IN] = message;
   setCommand(DEBUGGER_COMMAND_STEP_IN);
   sendResponse(message);
}

void Debugger::stepOutDebuggerClientRequest(Json::Value & message)
{
   //_pendingRequests[DEBUGGER_COMMAND_STEP_OUT] = message;
   StackFrame* stackFrame = _currentStackFrame;
   _targetSourceID        = _targetLine = 0;
   bool wasOut            = true;
   while (true)
   {
      if (!wasOut)
      {
         while (stackFrame->_parent && stackFrame->_type != StackFrame::STACK_FRAME_TYPE_DEMON_FUNC)
            stackFrame = stackFrame->_parent;

         if (stackFrame)
         {
            stackFrame = stackFrame->_parent;
            wasOut = true;
         }            
         else
            break;
      }

      if (stackFrame->_type != StackFrame::STACK_FRAME_TYPE_CODE)
      {
         if (stackFrame->_parent)
         {
            stackFrame = stackFrame->_parent;
            continue;
         }
         else break;
      }

      if (stackFrame->_code->_commands.size() > stackFrame->_command + 1)
      {
         CommandPtr target = stackFrame->_code->_commands[stackFrame->_command + 1];
         _targetSourceID = target->sourceID();
         _targetLine = target->line();
         break;
      }

      //поднимаемся на верх если есть куда
      if (!stackFrame->_parent)
         break;
      stackFrame = stackFrame->_parent;
   }

   sendResponse(message);

   if (_targetLine>0)
      setCommand(DEBUGGER_COMMAND_STEP_OUT);
   else
      setCommand(DEBUGGER_COMMAND_CONTINUE);
}

void Debugger::stackTraceDebuggerClientRequest(Json::Value & message)
{
   switch (_application->_script->state())
   {
      case Script::ST_PARSING: sendStackTraceFromParsing(message); break;
      case Script::ST_RUN:     sendStackTraceFromRun(message);     break;
   }
}

void Debugger::sendStackTraceFromParsing(Json::Value & message)
{
   string srcName = "", srcFile = "";
   size_t line = (size_t)_exceptionLine;

   currentSourcePos(srcName, srcFile, line);

   Json::Value stackFrameJSON;
   stackFrameJSON["id"]             = 0;
   stackFrameJSON["line"]           = (unsigned int)line;
   stackFrameJSON["column"]         = 0;
   stackFrameJSON["source"]["name"] = srcName;
   stackFrameJSON["source"]["path"] = srcFile;
   stackFrameJSON["name"]  = string("__parse__");

   message["stackFrames"][0] = stackFrameJSON;
   message["totalFrames"]    = 1;

   sendResponse(message);
}

void Debugger::sendStackTraceFromRun(Json::Value & message)
{
   if (!_currentStackFrame)
   {
      sendResponse(message);
      return;
   }

   int totalFrames = 0;

   _stack.clear();

   StackFrame* stackFrame = _currentStackFrame;

   while (stackFrame)
   {
      StackFrame* stackFrameFirst = stackFrame;

      CommandPtr currentCommand = stackFrame->_code->_commands[stackFrame->_command];

      size_t currentLine = currentCommand->line();
      size_t currentSourceID = currentCommand->sourceID();
      string srcShortFileName = _application->_script->srcShortFileName(currentSourceID);
      string srcFilePath = _application->_script->srcFilePath(currentSourceID);

      Json::Value stackFrameJSON;
      stackFrameJSON["id"] = totalFrames;
      stackFrameJSON["line"] = (unsigned int)currentLine;
      stackFrameJSON["column"] = 0;
      stackFrameJSON["source"]["name"] = srcShortFileName;
      stackFrameJSON["source"]["path"] = srcFilePath;

      while (stackFrame)
      {
         if (stackFrame->_type == StackFrame::STACK_FRAME_TYPE_DEMON_FUNC)
            break;

         stackFrame = stackFrame->_parent;
      }

      if (!stackFrame)
      {
         stackFrameJSON["name"] = string("__main__");
      }
      else
         stackFrameJSON["name"] = stackFrame->_func->_name;

      _stack.push_back(stackFrameFirst);
      message["stackFrames"][totalFrames++] = stackFrameJSON;

      if (!stackFrame)
         break;

      stackFrame = stackFrame->_parent;
   }

   message["totalFrames"] = totalFrames;

   sendResponse(message);
}

void Debugger::scopesDebuggerClientRequest(Json::Value & message)
{
   //_variableReferences.clear();
   _variableReference = 1000;
   size_t frameId = message["frameId"].asInt();

   shared_ptr<VariablesReferenceScope> varRefs(new VariablesReferenceScope(frameId));
   
   _variableReferences[_variableReference] = varRefs;

   Json::Value localScopeJson;

   localScopeJson["name"]               = "Local";
   localScopeJson["variablesReference"] = (unsigned int)(_variableReference++);
   localScopeJson["expensive"]          = false;

   message["scopes"][0] = localScopeJson;

   if (frameId != 0)
   {
      shared_ptr<VariablesReferenceScope> varRefs(new VariablesReferenceScope(0));

      _variableReferences[_variableReference] = varRefs;

      Json::Value globalScopeJson;

      globalScopeJson["name"] = "Global";
      globalScopeJson["variablesReference"] = (unsigned int)(_variableReference++);
      globalScopeJson["expensive"] = false;

      message["scopes"][1] = globalScopeJson;
   }

   sendResponse(message);
}

void Debugger::variablesDebuggerClientRequest(Json::Value & message)
{
   int variablesReference = message["variablesReference"].asInt();

   shared_ptr<VariablesReference> variablesReferenceObj = _variableReferences[variablesReference];

   Json::Value variablesJson;

   if (variablesReferenceObj->getVariables(this, variablesJson))
   {
      message["variables"]      = variablesJson["variables"];
      message["variablesTotal"] = variablesJson["variablesTotal"];
   }

   sendResponse(message);
}

void Debugger::disconnectDebuggerClientRequest(Json::Value & message)
{
   setCommand(DEBUGGER_COMMAND_QUIT);
}

void Debugger::sendResponse(Json::Value & message)
{
   message["type"] = "response";
   _messenger.pushMessageForDebugger(message);
}

void Debugger::setCommand(DebuggerCommand command)
{
   _command = command;
}

bool Debugger::currentSourcePos(string & srcName, string & srcPath, size_t & line)
{
   if (_application->_script->state() == Script::ST_PARSING)
   {
      if (_application->_script->srcSize() > 0)
      {
         size_t parseSourceID = _application->_script->parseSourceID();
         srcName = _application->_script->srcShortFileName(parseSourceID);
         srcPath = _application->_script->srcFilePath(parseSourceID);
         return true;
      }
   }
   else if (_application->_script->state() == Script::ST_RUN)
   {
      if (StackFrame::currentStackFrame())
      {
         StackFrame* stackFrame = StackFrame::currentStackFrame();
         CommandPtr  currentCommand = stackFrame->_code->_commands[stackFrame->_command];
         line = currentCommand->line();
         size_t currentSourceID = currentCommand->sourceID();
         srcName = _application->_script->srcShortFileName(currentSourceID);
         srcPath = _application->_script->srcFilePath(currentSourceID);
         return true;
      }
   }
   return false;
}

bool Debugger::VariablesReferenceScope::getVariables(Debugger* debugger, Json::Value &json)
{
   StackFrame* selectedStackFrame = _frameID < debugger->_stack.size() ? debugger->_stack[_frameID] : NULL;

   if (!selectedStackFrame)
      return false;

   CodePtr code = selectedStackFrame->_code->SPTR_FROM_THIS;
   vector<VariablePtr> codeVars;
   code->get_variables(debugger->_application->_script, codeVars);

   int varIndex = 0;
   while (code)
   {
      for (size_t i = 0; i < codeVars.size(); i++)
      {
         Json::Value varJson;
         VariablePtr var = codeVars[i];
         string   valStr = "", valType = "";
         Value    &val = var->val();
         size_t   variablesReference = 0;
         switch (val.type())
         {
         case ValueBase::_FLOAT:
            valType = "float";
            valStr = val.toString();
            break;
         case ValueBase::_NONE:
         case ValueBase::_STR:
         case ValueBase::_LOGIC:
         case ValueBase::_NODE:
         case ValueBase::_ARRAY:
         case ValueBase::_EDGE:
         case ValueBase::_FUSNUMBER:
         case ValueBase::_FILE:
            valType = "string";
            valStr = val.toString();
            break;
         case ValueBase::_GRAPH:
            {
               valType = "object";
               Graph &graph = ((ValueGraph*)(var->val().ptr().get()))->_val;
               valStr = graph._graph_name;
               variablesReference = debugger->_variableReference++;
               shared_ptr<VariablesReferenceGraph> varRefs(new VariablesReferenceGraph(graph));
               debugger->_variableReferences[variablesReference] = varRefs;
            }
            break;
         default:
            valType = "string";
            valStr = val.toString();
            break;
         }
         varJson["name"]               = var->name();
         varJson["type"]               = valType;
         varJson["value"]              = valStr + " (" + var->val().type_str() + ")";
         varJson["variablesReference"] = (unsigned int)variablesReference;

         json["variables"][varIndex++] = varJson;
      }
      code = code->_parent;
   }

   json["variablesTotal"] = varIndex;

   return true;
}

bool Debugger::VariablesReferenceGraph::getVariables(Debugger* debugger, Json::Value &json)
{
   int variablesTotal = 0;
   //выводим узлы в отсортированном виде

   auto &node_names = GraphManager::instance().nodeNames(_graph.classID());

   for (auto &nodePair : node_names)
   {
      if (nodePair.second == 0)
         continue;

      Node& node = _graph._nodes[nodePair.second];
      
      bool isEdges = node.edgesOut().size() > 0;

      Json::Value varJson;

      varJson["name"] = _graph.node_name(nodePair.second);
      varJson["value"] = "";

      if (isEdges)
      {
         size_t variablesReference = debugger->_variableReference++;
         shared_ptr<VariablesReferenceGraphNode> varRefs(new VariablesReferenceGraphNode(_graph, node));
         debugger->_variableReferences[variablesReference] = varRefs;
         varJson["type"] = "object";
         varJson["variablesReference"] = (unsigned int)variablesReference;
      }
      else
      {
         varJson["type"] = "string";
      }

      json["variables"][variablesTotal++] = varJson;
   }

   json["variablesTotal"] = variablesTotal;

   return true;
}

bool Debugger::VariablesReferenceGraphNode::getVariables(Debugger* debugger, Json::Value &json)
{
   int variablesTotal = 0;

   for (size_t i = 0; i < _node.edgesOut().size(); i++)
   {
      string edgeName = GraphManager::instance().edgeName(_node.edgesOut()[i]._kind);
      getVariables(_node.edgesOut()[i]._edges, edgeName, string(""), debugger, json, variablesTotal);
   }

   // 20181112 В оригинале перед входящими ребрами стоял continue;
   //for (size_t i = 0; i < _node.edgesIn().size(); i++)
   //{
   //   string edgeName = GraphManager::instance().edgeName(_node.edgesIn()[i]._kind);
   //   getVariables(_node.edgesIn()[i]._edges, edgeName, string("<-"), debugger, json, variablesTotal);
   //}

   json["variablesTotal"] = variablesTotal;

   return true;
}

void Debugger::VariablesReferenceGraphNode::getVariables(vector<Edge> & list, string edgeName, string edgeDirection, Debugger* debugger, Json::Value &json, int &variablesTotal)
{
   for (size_t i = 0; i < list.size(); i++)
   {
      string varName = "";
      string varValue = "";
      Edge & e = list[i];
      Graph* graphN1 = &_graph;
      Graph* graphN2 = e._valGraph2 ? &e._valGraph2->_val : &_graph;
      string external = (graphN1 != graphN2) ? (graphN2->_graph_name + "=>") : "";
      if (e._val._p0 == 0 && e._val._p1 == 1)
      {
         varName = edgeName;
         varValue = external + graphN2->node_name(e._n2);
      }
      else if (e._val._p0 == 1 && e._val._p1 == 0)
      {
         varName = "!" + edgeName;
         varValue = external + graphN2->node_name(e._n2);
      }
      else
      {
         varName = edgeName;
         varValue = external + graphN2->node_name(e._n2) + e._val.to_str();
      }

      Node& node = graphN2->_nodes[e._n2];

      if (node._value)
      {
         varValue += "(" + node._value->toString() + ")";
      }

      Json::Value varJson;
      varJson["name"] = edgeDirection + varName + "." + varValue;
      varJson["value"] = varValue;

      bool isEdges = node.edgesOut().size() > 0;

      if (isEdges)
      {
         size_t variablesReference = debugger->_variableReference++;
         shared_ptr<VariablesReferenceGraphNode> varRefs(new VariablesReferenceGraphNode(*graphN2, node));
         debugger->_variableReferences[variablesReference] = varRefs;
         varJson["type"] = "object";
         varJson["variablesReference"] = (unsigned int)variablesReference;
      }
      else
      {
         varJson["type"] = "string";
      }


      json["variables"][variablesTotal++] = varJson;
   }
}