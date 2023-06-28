/*!               Логирование происходящего в интерпретаторе

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
*/

#ifndef LogH
#define LogH

#include "OTypes.h"

#include <iostream>
#include <sstream>

using namespace std;

enum TraceLevel
{
   L_INFO     = 1,      //!< информационное сообщение
   L_DEBUG    = 2,      //!< отладочное сообщение
   L_WARNING  = 4,      //!< предупреждающее сообщение
   L_ERROR    = 8,      //!< ошибка, но продолжать можем
   L_CRITICAL = 16      //!< критическая ошибка, останавливаем работу интерпретатора       
};

#define TRACE                        TraceStream(L_INFO,    __FUNCTION__)
#define TRACE_POS(Level,Line,Column) TraceStream(Level,     __FUNCTION__,-1,Line,Column)
#define TRACE_FILE_POS(Level,SourceID,Line,Column) TraceStream(Level, __FUNCTION__,SourceID,Line,Column)
#define TRACE_INFO                   TraceStream(L_INFO,    __FUNCTION__)
#define TRACE_DEBUG                  TraceStream(L_DEBUG,   __FUNCTION__)
#define TRACE_WARNING                TraceStream(L_WARNING, __FUNCTION__)
#define TRACE_ERROR                  TraceStream(L_ERROR,   __FUNCTION__)
#define TRACE_CRITICAL               TraceStream(L_CRITICAL,__FUNCTION__)

#define TRACE_STREAM(obj)            TraceStream objTrace(L_INFO,    __FUNCTION__); std::stringstream &obj = objTrace.stream();
#define TRACE_STREAM_DEBUG(obj)      TraceStream objTrace(L_DEBUG,   __FUNCTION__); std::stringstream &obj = objTrace.stream();
#define TRACE_STREAM_WARNING(obj)    TraceStream objTrace(L_WARNING, __FUNCTION__); std::stringstream &obj = objTrace.stream();
#define TRACE_STREAM_ERROR(obj)      TraceStream objTrace(L_ERROR,   __FUNCTION__); std::stringstream &obj = objTrace.stream();
#define TRACE_STREAM_CRITICAL(obj)   TraceStream objTrace(L_CRITICAL,__FUNCTION__); std::stringstream &obj = objTrace.stream();

class TraceStream
{
public:
   TraceStream(TraceLevel level = L_INFO, const char* function = "", int sourceID = -1, size_t line = 0, size_t column = 0);
   ~TraceStream();

   std::stringstream strStream;

   /*!
   Тип std::cout
   */
   typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
   /*!
   Сигнатура std::endl
   */
   typedef CoutType& (*StandardEndLine)(CoutType&);
   /*!
   Перехватываем <<std::endl
   */
   TraceStream& operator<<(StandardEndLine manip);
   /*!
   Перехватываем <<
   */
   template <class T>
   TraceStream& operator<< (T val)
   {
      strStream << val; // отправляем в буфер трассировки
      _empty = false;
      //cout         << val; // отправляем на экран
      return *this;
   }
   /*!
   Отправляем лог на экран и в отладчик
   */
   void flush();
   /*!
   Получить ссылку на поток
   */
   std::stringstream &stream() { _empty = false; return strStream; }
   /*!
   Установить уровень трассировки
   */
   static void traceLevel(const string &level);
   /*!
   Получить текущий уровень трассировки
   */
   static string traceLevel();
private:
   TraceLevel   _level;                //!< уровень трассировки
   const char * _cppFunction;          //!< имя с++ функции в которой сформировалась строка лога
   int          _dsSourceID;           //!< идентификатор исходника
   size_t       _dsLine;               //!< строка кода 
   size_t       _dsColumn;             //!< колонка в строке кода
   bool         _empty;                //!< пустой ли буфер строки
   static int   _traceLevels;          //!< включенные уровни трассировки
};

#endif