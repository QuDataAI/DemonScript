#include "Script.h"
#include "Compiler.h"
#include "Function.h"
#include "Command.h"
#include "Modules/System.h"
#include "GraphManager.h"
#include "Modules/ModuleManager.h"
#include "Sys/Debugger/Debugger.h"
#include <locale> 

#include "Modules/SVGModule.h"

ScriptWPtr Script::_instance;

//=======================================================================================
//                                   Script
//=======================================================================================
//
Script::Script():
   _listing(false),
   _trace(0)
{   
   _code = SPTR_MAKE(Code)();
}

void Script::init()
{
   ScriptPtr thisPtr = SPTR_FROM_THIS;
   _instance = thisPtr;
   // регистрируем встроенные функции:
   addFunction(SPTR_MAKE(FunOr)           (thisPtr));
   addFunction(SPTR_MAKE(FunAnd)          (thisPtr));
   addFunction(SPTR_MAKE(FunExists)       (thisPtr));
   addFunction(SPTR_MAKE(FunForall)       (thisPtr));
   addFunction(SPTR_MAKE(FunEdge)         (thisPtr));
   addFunction(SPTR_MAKE(FunIsEdge)       (thisPtr));
   addFunction(SPTR_MAKE(FunSetEdge)      (thisPtr));
   addFunction(SPTR_MAKE(FunIsolated)     (thisPtr));
   addFunction(SPTR_MAKE(FunPath)         (thisPtr));
   addFunction(SPTR_MAKE(FunCommon)       (thisPtr));
   addFunction(SPTR_MAKE(FunGraphDot)     (thisPtr));
   addFunction(SPTR_MAKE(FunGraphJson)    (thisPtr));
   addFunction(SPTR_MAKE(FunGraphStr)     (thisPtr));
   addFunction(SPTR_MAKE(FunGraphChanged) (thisPtr));
   addFunction(SPTR_MAKE(FunEdgesChanges) (thisPtr));
   addFunction(SPTR_MAKE(FunEdgesBlocked) (thisPtr));
   addFunction(SPTR_MAKE(FunDeleteNodes)  (thisPtr));
   addFunction(SPTR_MAKE(FunClearEdges)   (thisPtr));
   addFunction(SPTR_MAKE(FunIsomorphic)   (thisPtr));
   addFunction(SPTR_MAKE(FunFopen)        (thisPtr));
   addFunction(SPTR_MAKE(FunFclose)       (thisPtr));
   addFunction(SPTR_MAKE(FunType)         (thisPtr));
   addFunction(SPTR_MAKE(FunCount)        (thisPtr));
   addFunction(SPTR_MAKE(FunIs)           (thisPtr));
   addFunction(SPTR_MAKE(FunFind)         (thisPtr));
   addFunction(SPTR_MAKE(FunSize)         (thisPtr));
   addFunction(SPTR_MAKE(FunClear)        (thisPtr));
   addFunction(SPTR_MAKE(FunCopy)         (thisPtr));
   addFunction(SPTR_MAKE(FunCopyDeep)     (thisPtr));
   addFunction(SPTR_MAKE(FunName)         (thisPtr));
   addFunction(SPTR_MAKE(FunMix)          (thisPtr));
   addFunction(SPTR_MAKE(FunEmpty)        (thisPtr));
   addFunction(SPTR_MAKE(FunPush)         (thisPtr));
   addFunction(SPTR_MAKE(FunUnshift)      (thisPtr));
   addFunction(SPTR_MAKE(FunPop)          (thisPtr));
   addFunction(SPTR_MAKE(FunShift)        (thisPtr));
   addFunction(SPTR_MAKE(FunToString)     (thisPtr));
   addFunction(SPTR_MAKE(FunGraphConstr)  (thisPtr));
   addFunction(SPTR_MAKE(FunIsa)          (thisPtr));
   addFunction(SPTR_MAKE(FunEdgesCountIn) (thisPtr));
   addFunction(SPTR_MAKE(FunEdgesCountOut)(thisPtr));
   addFunction(SPTR_MAKE(FunLogic)        (thisPtr));
   addFunction(SPTR_MAKE(FunArray)        (thisPtr));
   addFunction(SPTR_MAKE(FunLoad)         (thisPtr));
   addFunction(SPTR_MAKE(FunAddNodes)     (thisPtr));
   addFunction(SPTR_MAKE(FunNodes)        (thisPtr));
   addFunction(SPTR_MAKE(FunEdgesNum)     (thisPtr));
   addFunction(SPTR_MAKE(FunValue)        (thisPtr));
   addFunction(SPTR_MAKE(FunClassID)      (thisPtr));
   addFunction(SPTR_MAKE(FunGraph)        (thisPtr));
   addFunction(SPTR_MAKE(FunRange)        (thisPtr));
   addFunction(SPTR_MAKE(FunGraphChange)  (thisPtr));
   addFunction(SPTR_MAKE(FunId)           (thisPtr));
   addFunction(SPTR_MAKE(FunAddNode)      (thisPtr));
   addFunction(SPTR_MAKE(FunSplice)       (thisPtr));
   addFunction(SPTR_MAKE(FunVerbose)      (thisPtr));
   addFunction(SPTR_MAKE(FunSorted)       (thisPtr));
   addFunction(SPTR_MAKE(FunFusNumber)    (thisPtr));
   addFunction(SPTR_MAKE(FunGetLine)      (thisPtr));
   addFunction(SPTR_MAKE(FunGetLines)     (thisPtr));
   addFunction(SPTR_MAKE(FunIn)           (thisPtr));
   addFunction(SPTR_MAKE(FunEOF)          (thisPtr));
   addFunction(SPTR_MAKE(FunIsOpen)       (thisPtr));
   addFunction(SPTR_MAKE(FunOut)          (thisPtr));
   addFunction(SPTR_MAKE(FunClose)        (thisPtr));
   addFunction(SPTR_MAKE(FunSplit)        (thisPtr));
   addFunction(SPTR_MAKE(FunIndexOf)      (thisPtr));
   addFunction(SPTR_MAKE(FunTrim)         (thisPtr));
   addFunction(SPTR_MAKE(FunLength)       (thisPtr));
   addFunction(SPTR_MAKE(FunSubstr)       (thisPtr));
   addFunction(SPTR_MAKE(FunSubstring)    (thisPtr));
   addFunction(SPTR_MAKE(FunReplace)      (thisPtr));
   addFunction(SPTR_MAKE(FunReplaceRegex) (thisPtr));
   addFunction(SPTR_MAKE(FunRepeat)       (thisPtr));
   addFunction(SPTR_MAKE(FunInclude)      (thisPtr));
   addFunction(SPTR_MAKE(FunStartsWith)   (thisPtr));
   addFunction(SPTR_MAKE(FunEndsWith)     (thisPtr));
   addFunction(SPTR_MAKE(FunSearch)       (thisPtr));
   addFunction(SPTR_MAKE(FunFloat)        (thisPtr));
   addFunction(SPTR_MAKE(FunAttr)         (thisPtr));
   addFunction(SPTR_MAKE(FunGet)          (thisPtr));
   addFunction(SPTR_MAKE(FunEdges)        (thisPtr));
   addFunction(SPTR_MAKE(FunBind)         (thisPtr));
   addFunction(SPTR_MAKE(FunUnbind)       (thisPtr));
   addFunction(SPTR_MAKE(FunSort)         (thisPtr));
   addFunction(SPTR_MAKE(FunSet)          (thisPtr));
   addFunction(SPTR_MAKE(FunCreate)       (thisPtr));
   addFunction(SPTR_MAKE(FunArrayConstr)  (thisPtr));
   addFunction(SPTR_MAKE(FunGraphAdd)     (thisPtr));
   addFunction(SPTR_MAKE(FunGraphClose)   (thisPtr));
   addFunction(SPTR_MAKE(FunGraphClearEdges)(thisPtr));
   addFunction(SPTR_MAKE(FunGraphSensesDef) (thisPtr));

   addFunction(SPTR_MAKE(FunSVGColorLine)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGColorFill)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGColorText)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGWidthLine)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGSizeFont)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGLine)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGRect)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGCircle)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGCircleFill)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGPoint)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGArc)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGText)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGPolygon)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGTransBeg)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGTransEnd)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGTransAll)(thisPtr));
   addFunction(SPTR_MAKE(FunSVGSave)(thisPtr));

   addFunction(SPTR_MAKE(FunEval) (thisPtr));

   //инициализируем менеджер графов
   GraphManager::instance().init();
   // предопределённый граф - глобальная переменная
   _graph = &(_code->add_local_variable("GRAPH")->_val = shared_ptr<ValueBase>(ValueGraph::create()));
   GraphManager::instance().currentGraphValue(_graph);
}


Script::~Script()
{
   _functions.clear();
   if (_fout.is_open())
      _fout.close();
}

bool Script::load(const string & fileName, const string fileDir/* = ""*/, bool main/* = true*/)
{
   state(ST_PARSING);

   ModuleManager::instance()->setScript(this);
   Compiler  compiler(SPTR_FROM_THIS);

   size_t sourceID = registerSource(fileName, fileDir);
   string fileType = main ? "main" : "include";
   string filePath = srcFilePath(sourceID);

   ParseStack parseStack(this, sourceID);      //сохраняем стек идентификаторов исходников, которые парсятся в текущий момент
   
   if (!compiler.load(filePath))
   {
      TRACE_ERROR << "E/Script Script::load can't open " << fileType << " file: " << filePath << endl;
      return false;
   }

   //TRACE << "compile " << fileType << " file: " << filePath << endl;

   clock_t compileStart = clock();
   if (!compiler.parse(sourceID))
   {
      TRACE_ERROR << "E/Script Script::load can't parse " << fileType << " file: " << filePath << endl;
      return false;
   }

   //TRACE << "compile " << "file: " << filePath << " time: " << (clock() - compileStart) << "ms" << endl;

   if (_listing)
   {
      TRACE_STREAM(tout)
      print(tout);
   }
      
   
   return true;
}

//=======================================================================================
// Запустить скрипт на выполнение
//
Value Script::run()
{ 
   state(ST_RUN);
   Int ret = 0; 
   _time_start = clock();
   System::instance()->lastTimer(_time_start);
   Value res;
   _code->run(ret, res, SPTR_FROM_THIS);
   state(ST_COMPLETE);
   return res;
}
//=======================================================================================
// Получить указатель на функцию-демона с именем name
//
FunctionPtr Script::get_function(const string &name)
{
   for(UInt i=0; i < _functions.size(); i++)      // \todo - ускорить через map
      if(_functions[i]->_name == name)
         return _functions[i];
   return 0;
}

//=======================================================================================
// Вывести в поток out скрипт scr в виде текста (программы)
//
ostream& Script::print(ostream& out)
{
   for (UInt i=0; i < _functions.size(); i++)
      _functions[i]->print(out);   
   out << "\n"; _code->print(out, SPTR_FROM_THIS);
   return out << "\n";
}

size_t Script::registerSource(const string & fileName, const string fileDir/* = ""*/)
{
   string fileNameTrue = fileName;
   if (fileNameTrue.length() > 2 && fileNameTrue[1] == ':')
   {
      std::locale loc;
      fileNameTrue[0] = std::toupper(fileNameTrue[0], loc);
   }

   size_t sourceID = _sources.size();

   _sources.push_back(Source(fileNameTrue, fileDir));

   return sourceID;
}

string Script::srcShortFileName(size_t sourceID)
{
   if (sourceID >= _sources.size())
      return "";

   return _sources[sourceID].shortFileName();
}

string Script::srcFilePath(size_t sourceID)
{
   if (sourceID >= _sources.size())
      return "";

   return _sources[sourceID].filePath();
}

string Script::srcFileDir(size_t sourceID)
{
   if (sourceID >= _sources.size())
      return "";

   return _sources[sourceID].fileDir();
}

bool Script::srcID(const string & fileName, size_t & sourceID)
{
   for (size_t i = 0; i < _sources.size(); i++)
      if (_sources[i].filePath() == fileName)
      {
         sourceID = i;
         return true;
      }
   return false;
}

string Script::workDir()
{
#ifdef USE_DEBUGGER
   //при запуске из под отладчика текущая директория находится в папке отладчика
   //поэтому переопределяем ее в папку с основным скриптом
   if (Debugger::instance().debugging())
   {
      return srcFileDir(0);
   }
#endif
   return string();
}

string Script::workPath(string & fileName)
{
   string dir = workDir();
   if (dir.length() > 0)
   {
      return dir + "\\" + fileName;
   }
   else
      return fileName;
}

bool Script::addFunction(FunctionPtr f)
{
   for (size_t i = 0; i < _functions.size(); i++)
   {
      if (_functions[i]->name() == f->name())
      {
         TRACE_CRITICAL << "Initialization Error! Duplicate function name: " << f->name() << endl;
         return false;
      }
   }

   _functions.push_back(f);
   return true;
}

Script::ParseStack::ParseStack(Script * script, size_t sourceID):
   _script(script)
{
   _prevParseSourceID      = _script->_parseSourceID;
   _script->_parseSourceID = sourceID;
}

Script::ParseStack::~ParseStack()
{
   _script->_parseSourceID = _prevParseSourceID;
}
