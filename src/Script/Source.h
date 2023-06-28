/*!            Source - исходник скрипта

Хранит имя файла и полный путь к исходнику

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef SourceH
#define SourceH

#include <string>

using namespace std;

class Source {
public:
   Source(string fileName, string fileDir = "");
   /*!
   Получить короткое имя файла с исходниками
   \return короткое имя файла с исходниками
   */
   string shortFileName() { return _shortFileName; }
   /*!
   Получить полный путь к файлу с исходниками
   \return полный путь к файлу с исходниками
   */
   string filePath() { return _fileDir + _shortFileName; }
   /*!
   Получить путь к директории в которой расположен файл с исходниками
   \return путь к директории в которой расположен файл с исходниками
   */
   string fileDir() { return _fileDir; }

private:
   string _shortFileName = "";   //!< короткое имя файла с исходниками
   string _fileDir = "";         //!< имя директории в которой расположен файл
};

#endif