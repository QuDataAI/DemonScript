/*!                           Application - стартовый модуль приложения


(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef ApplicationH
#define ApplicationH

#include "Compiler.h"
#include "OTypes.h"

using namespace std;

//=======================================================================================
//! Cтартовый модуль приложения
// 
class Application
{
public:
   Application();
   ~Application();
   /*!
   Запустить приложение
   \param argc число параметров запуска
   \param argv значения параметров запуска
   \return результат выполнения
   */
   int start(int argc, char *argv[]);
   /*!
   Время выполнения скрипта в мс
   \return время выполнения скрипта в мс
   */
   clock_t scriptRunTime() { return _scriptRunTime; }
private:
   /*!
   Парсим аргументы коммандной строки
   \param argc число параметров запуска
   \param argv значения параметров запуска
   \return результат выполнения
   */
   void parseArguments(int argc, char *argv[]);
   /*!
   Выводим справку
   */
   void showHelp();
   /*!
   Запустить интерпретатор
   */
   bool runInterpreter();
   /*!
   Выводим время работы скрипта
   */
   void printRunTime();
   /*!
   Инициализация менеджеров
   */
   void initManagers();
   /*!
   Очистка менеджеров
   */
   void clearManagers();


   bool           _listing             = false;       //!< включен листинг скрипта
   bool           _tracing             = false;       //!< включенa трассировка скрипта
   bool           _quitAfterComplete   = false;       //!< выйти после завершения скрипта
   string         _debuggerClient      = "";          //!< клиент отладки
   int            _debuggerPort        = -1;          //!< порт отладчика
   string         _scriptFileName      = "";          //!< имя стартового скрипта
   string         _appPath             = "";          //!< путь к файлу приложения
   bool           _showHelp            = false;       //!< нужно показать справку
   ScriptPtr      _script;                            //!< скрипт программы
   clock_t        _scriptRunTime;                     //!< время выполнения скрипта
   bool           _release;                           //!< признак релиза
   const Int      _version;                           //!< текущая версия программы

   friend class Debugger;
};

#endif 