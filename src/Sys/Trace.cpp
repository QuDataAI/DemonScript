#include "Trace.h"
#include "time.h"
#include "Sys/Debugger/Debugger.h"
#include "Script/Stack/StackFrame.h"
#include "Script.h"
#include "ErrorManager.h"
#if defined(__EMSCRIPTEN__)
#include "CPPJSBridge.h"
#endif
#include "Utils.h"
#ifdef __WINDOWS__
#include <windows.h>
#endif

//!< включенные уровни трассировки
int   TraceStream::_traceLevels = L_INFO | L_DEBUG | L_WARNING | L_ERROR | L_CRITICAL;           

#ifdef __WINDOWS__
static BOOL init = SetConsoleOutputCP(CP_UTF8) && SetConsoleCP(CP_UTF8);
#endif

TraceStream::TraceStream(TraceLevel level, const char* function, int sourceID, size_t line, size_t column):
   _level(level),
   _cppFunction(function),
   _dsSourceID(sourceID),
   _dsLine(line),
   _dsColumn(column),
   _empty(true)
{

}

TraceStream::~TraceStream()
{
   flush();
}

TraceStream & TraceStream::operator<<(StandardEndLine manip)
{
   // call the function, but we cannot return it's value
   //manip(cout);

   strStream << "\n";
   _empty = false;

   flush();

   return *this;
}

void TraceStream::flush()
{
   //www.cplusplus.com/forum/general/74355/
   //if (!strStream.rdbuf()->in_avail())
   //   return;

   if (_empty)
      return;

   if (!(_traceLevels & _level))
      return;

   bool printHead = (_level != L_INFO);

   stringstream toTrace;

   if (printHead)
   {
      const char* level = "";

      switch (_level)
      {
         case L_INFO:      level = "I"; break;
         case L_DEBUG:     level = "D"; break;
         case L_WARNING:   level = "W"; break;
         case L_ERROR:     level = "E"; break;
         case L_CRITICAL:  level = "C"; break;
      }

      toTrace << level << "/" << _cppFunction << "()";

      if (_dsSourceID < 0)
      {
         _dsSourceID = (int)StackFrame::currentSourceID();
      }

      if (_dsSourceID >= 0)
      {
         toTrace << " " << Script::instance()->srcShortFileName((size_t)_dsSourceID) << ":";
      }

      if (_dsLine == 0)
      {
         //пытаемся получить у интерпретатора
         _dsLine = StackFrame::currentLine();
      }
      if (_dsLine > 0)
      {
         toTrace << " in row " << _dsLine;
         if (_dsColumn > 0)
         {
            toTrace << ", col " << _dsColumn;
         }
      }      
      toTrace << ": ";
   }

   toTrace  << strStream.str();

   string toTraceStr = toTrace.str();

   _empty = true;
   strStream.str(std::string());

#if defined(__EMSCRIPTEN__)
   CPPJSBridge::instance().onTrace(_level, _dsLine, _dsColumn, toTraceStr);
#else
   printf(toTraceStr.c_str());
   static shared_ptr<ofstream> logFile = make_shared<ofstream>(Script::instance()->workPath(string("ds.log")), ofstream::out);
   *logFile << toTraceStr;
   (*logFile).flush();
   //puts(toTraceStr.c_str());
   //cout << toTraceStr;
#endif

   if (_level == L_ERROR || _level == L_CRITICAL)
      ErrorManager::instance().error(toTraceStr);
#ifdef USE_DEBUGGER
   if (Debugger::instance().debugging())
   {
      Debugger::instance().onTrace(_level, _dsLine, _dsColumn, toTraceStr);
      if (_level == L_CRITICAL)
      {
         Debugger::instance().onException(_dsLine, _dsColumn, toTraceStr);
      }
   }
#endif
}

void TraceStream::traceLevel(const string & level)
{
   if (level == "Verbose")
   {
      _traceLevels = L_INFO | L_DEBUG | L_WARNING | L_ERROR | L_CRITICAL;
   }
   else if (level == "Info")
   {
      _traceLevels = L_INFO | L_WARNING | L_ERROR | L_CRITICAL;
   }
   else if (level == "Warnings")
   {
      _traceLevels = L_WARNING | L_ERROR | L_CRITICAL;
   }
   else if (level == "Errors")
   {
      _traceLevels = L_ERROR | L_CRITICAL;
   }
   else if (level == "Disable")
   {
      _traceLevels = 0;
   }
}

string TraceStream::traceLevel()
{
   string level;
   if (_traceLevels == (int)(L_INFO | L_DEBUG | L_WARNING | L_ERROR | L_CRITICAL))
   {
      level = "Verbose";
   }
   else if (_traceLevels == (int)(L_INFO | L_WARNING | L_ERROR | L_CRITICAL))
   {
      level = "Info";
   }
   else if (_traceLevels == (int)(L_WARNING | L_ERROR | L_CRITICAL))
   {
      level = "Warnings";
   }
   else if (_traceLevels == (int)(L_ERROR | L_CRITICAL))
   {
      level = "Errors";
   }
   else
   {
      level = "Disable";
   }
   return level;
}
