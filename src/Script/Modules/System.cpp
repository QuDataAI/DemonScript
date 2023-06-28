#include "System.h"
#include "ModuleManager.h"
#include "ErrorManager.h"
#include "Stack/StackFrame.h"

//регистрируем модуль
IMPLEMENTATION_MODULE(System, "System")

//регистрируем функции модуля
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncVersion,    "version")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncVersionStr, "versionStr")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncError,      "error")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncErrorMsg,   "errorMsg")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncTime,       "time")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncStat,       "stat")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncLine,       "line")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncArgs,       "args")
IMPLEMENTATION_MODULE_FUNCTION(System, SystemFuncTraceLevel, "traceLevel")

/*static*/ shared_ptr<ValueArr> SystemFuncArgs::args = make_shared<ValueArr>();

System::System():
   _version(0)
{
   _lastTimer = clock();
}


Value SystemFuncVersion::run()
{
   System* module = (System*)_module;
   return Value(module->version());
}

Value SystemFuncVersionStr::run()
{
   System* module = (System*)_module;
   return Value(module->versionStr());
}

Value SystemFuncError::run()
{
   return Value(Logic(ErrorManager::instance().wasError()));
}

Value SystemFuncErrorMsg::run()
{
   return Value(ErrorManager::instance().lastErrorMsg());
}

string System::versionStr()
{
   Int versionMajor, versionMinor, versionRevision;

   versionMajor    = _version / 1000000;
   versionMinor    = (_version - versionMajor * 1000000) / 1000;
   versionRevision = (_version - versionMajor * 1000000) - versionMinor * 1000;

   string ver = to_string(versionMajor) + "." + to_string(versionMinor) + "." + to_string(versionRevision);
   return ver;
}

SystemFuncTime::SystemFuncTime()
{
   _minArgs = 0;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_LOGIC);
}

Value SystemFuncTime::run()
{
   if (!initRun())
      return Value();

   clock_t timeNow = clock();

   System* module = (System*)_module;

   Float timeSpent = (timeNow - module->lastTimer()) * 1000 / CLOCKS_PER_SEC;

   bool needReset = true;

   if (_vals.size() > 0 && _vals[0].get_Logic() == Logic::False)
      needReset = false;
   
   if (needReset)
      module->lastTimer(timeNow);

   return Value(timeSpent);
}

Value SystemFuncStat::run()
{
   _num_calls++;                               // число вызовов  
   stringstream str;
   str << "Functions calls stat:\n";
   multimap<Int, string> sortedByCalls;
   stringstream name;
   name << std::right << std::setw(8) << Graph::getEdgeCnt() << "    : "
        << std::left  << std::setw(15) << "Graph::val" << endl;
   sortedByCalls.insert(pair<Int, string>(-(Int)(Graph::getEdgeCnt()), name.str()));
   name.str(std::string());
   name << std::right << std::setw(8) << Graph::setEdgeCnt() << "    : "
        << std::left << std::setw(15) << "Graph::add" << endl;
   sortedByCalls.insert(pair<Int, string>(-(Int)(Graph::setEdgeCnt()), name.str()));
   for (UInt i = 0; i < _script->_functions.size(); i++) {
      FunctionPtr fun = _script->_functions[i];
      name.str(std::string());
      name << std::right << std::setw(8) << fun->_num_calls << "    : "
           << std::left << std::setw(15) << fun->_name << endl;
      fun->out_info(name);
      sortedByCalls.insert(pair<Int, string>(-(Int)(fun->_num_calls), name.str()));
   }
   for (auto &pair : sortedByCalls) {
      if (pair.first == 0)
         continue;
      str << pair.second;
   }
   /*
   for (UInt i = 0; i < _script->_functions.size(); i++) {
   Function * fun = _script->_functions[i];
   str << std::right << std::setw(8) << fun->_num_calls << "    : "
   << std::left << std::setw(15) << fun->_name << endl;
   fun->out_info(str);
   }
   */
   _script->_code->out_info(str, 0);
   TRACE << str.str();
   return Logic();
}

Value SystemFuncLine::run()
{
   if (!initRun())
      return Value();

   UInt stackOffset = 0;
   if (_vals.size() > 0)
   {
      stackOffset = (UInt)_vals[0].get_Float();
   }
   return Value(Float(StackFrame::currentLine(stackOffset)));
}

Value SystemFuncArgs::run()
{
   return (shared_ptr<ValueBase>)args;
}

SystemFuncTraceLevel::SystemFuncTraceLevel()
{
   _minArgs = 0;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_STR);
}

Value SystemFuncTraceLevel::run()
{
   if (!initRun())
      return Value();

   if (_vals.size() > 0)
   {
      string logLevelStr = _vals[0].get_Str();
      TraceStream::traceLevel(logLevelStr);
      return Value(logLevelStr);
   }
   else
   {
      return Value(TraceStream::traceLevel());
   }
}
