#include "Application.h"
#include "Sys/Debugger/Debugger.h"
#include "Script/Modules/System.h"
#include "Config.h"
#include "Utils.h"
#include <stdio.h>
#include <string>

Application::Application() :
     _release(false), 
     _version(VERSION_NUMBER),
     _scriptRunTime(0),
     _script(SPTR_MAKE(Script)())
{   
   _script->init();
   if (!_release)
   {
      //в оладке попадем сюда и можно переопределить параметры командной строки
      //_listing = true; 
      //_tracing = true;
      //_scriptFileName = "room/unitTestsM.ds";
      //_scriptFileName = "room/test1.ds";
      _scriptFileName = "in_on_models_genarator.ds";
      //_scriptFileName = "test.ds";
      //_scriptFileName = "in_models_calc1.ds";
      //_scriptFileName = "history01.ds";
      //_debuggerClient = "vscode";
      //_debuggerPort   = 4711;
   }
   initManagers();
   System::instance()->version(_version);
}

Application::~Application()
{
   clearManagers();   
}

int Application::start(int argc, char * argv[])
{
   //парсим аргументы командной строки
   parseArguments(argc, argv);
   
   _scriptRunTime = 0;

   //нет команд, показываем справку в релизе
   if (_showHelp && _release)
   {
      showHelp();
      return 0;
   }

   //если указан порт отладчика, передаем ему управление программой
   if (_debuggerPort > 0)
   {
      Debugger::instance().run(this, _debuggerClient, _debuggerPort);
      return 0;
   } 

   //запускаем интерпретатор
   runInterpreter();

   //если нужно, ждем от пользователя нажатия клавиши
   if (!_quitAfterComplete)
   {
#ifdef __WINDOWS__
      system("pause");
#else 
      system("read");
#endif
   }

   return 0;
}

bool Application::runInterpreter()
{
   //TRACE << "<start> version: " << System::instance()->versionStr() << ", path: "<< _appPath << endl;
   _script->_listing = _listing;
   _script->_trace   = _tracing?1:0;

   //TRACE << "<parse>" << endl;
   //загружаем и компилируем файл скрипта
   if (!_script->load(_scriptFileName, "", true))
   {
      //при парсинге были ошибки, поэтому выходим
      TRACE_CRITICAL << "Were parsing errors :(\n";
      return false;
   }
      
   //запускаем интерпретатор
   //TRACE << "<run>" << endl;
   _script->run();

   //выводим время работы скрипта
   printRunTime();

   //TRACE << "<complete>" << endl;

   //сообщаем отладчику о завершении выполнения
   if (Debugger::instance().debugging())
   {
      Debugger::instance().onComplete();
      _quitAfterComplete = true;
   }     

   return true;
}

void Application::printRunTime()
{
   _scriptRunTime = (clock() - _script->_time_start) * 1000 / CLOCKS_PER_SEC;
   //TRACE << "\nRun time = "<< _scriptRunTime << " ms." << endl;
}

void Application::initManagers()
{
}

void Application::clearManagers()
{
   GraphManager::instance().clear();
}

void Application::parseArguments(int argc, char * argv[])
{
   if (argc <= 1)
   {
      // параметров нет, показываем хелп и выходим
      _showHelp = true;
      return;
   }

   if (argc == 0)
      return;

   _appPath = argv[0];

   bool paramStarted = false;
   int i = 1;   
   while (i < argc - 1)
   {
      if (string(argv[i]) == "-l" || string(argv[i]) == "--listing")
      {
         paramStarted = false;
         _listing = true;  // включен листинг скрипта
         i++;
      }
      else if (string(argv[i]) == "-t" || string(argv[i]) == "--tracing")
      {
         paramStarted = false;
         _tracing = true;  // включенa трассировка скрипта
         i++;
      }
      else if (string(argv[i]) == "-q" || string(argv[i]) == "--quitProgramAfterComplete")
      {
         paramStarted = false;
         _quitAfterComplete = true; // выйти после завершения скрипта
         i++;
      }
      else if ((string(argv[i]) == "-dc" || string(argv[i]) == "--debuggerClient" ) && (i + 1 < argc))
      {
         paramStarted = false;
         _debuggerClient = argv[i + 1];         // установлен имя отладчика
         i += 2;
      }
      else if ((string(argv[i]) == "-dp"  || string(argv[i]) == "--debuggerPort") && (i + 1 < argc))
      {
         paramStarted = false;
         _debuggerPort = std::stoi(argv[i+1]);  // установлен порт отладчика
         i+=2;
      }
      else if ((string(argv[i]) == "-p" || string(argv[i]) == "--params") && (i + 1 < argc))
      {
         paramStarted = true;
         i++;  
      }
      //else if ((string(argv[i]) == "-ir" || string(argv[i]) == "--ignoreRuntimeErrors"))
      //{
      //   _script.ignoreRuntimeErrors();
      //   i ++;
      //}
      else
      {
         if (paramStarted)
         {
            SystemFuncArgs::args->push(make_shared<ValueStr>(argv[i]));
         }
         i++;
      }
   }

   // последним параметром если задан идет имя стартового скрипта
   _scriptFileName = argv[argc - 1];
}

void Application::showHelp()
{
   TRACE << "Version: " << System::instance()->versionStr() << "\n";
   TRACE << "Usage: ds.exe [option] ... file\n";
   TRACE << "Options: \n";
   TRACE << "file                             - script file_name\n";
   TRACE << "-l,  --listing                   - listing of script\n";
   TRACE << "-t,  --tracing                   - tracing of script\n";
   TRACE << "-q,  --quitProgramAfterComplete  - quit the program after complete\n";
   TRACE << "-dc, --debuggerClient name       - debugger client name (only \"vscode\" is supported)\n";
   TRACE << "-dp, --debuggerPort   port       - debugger port number\n";
   //TRACE << "-ir, --ignoreRuntimeErrors       - don't interrupt execution on errors\n";
}


