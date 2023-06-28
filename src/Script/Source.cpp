#include "Source.h"

Source::Source(string fileName, string fileDir /*= ""*/)
{
   if (fileName.length() == 0)
      return;

   _fileDir = fileDir;

   size_t indx = fileName.find_last_of(":");
   if (std::string::npos != indx || fileName[0] == '/')
   {
      // в имени фала полный путь, игнорируем переменную fileDir
      _fileDir = "";
   }
   
   //бурем короткое имя файла
   _shortFileName = fileName;
   indx = fileName.find_last_of("\\/");
   if (std::string::npos != indx)
   {
      // в имени файла присутствует "\\/", отрезаем
      _shortFileName.erase(0, indx + 1);
      _fileDir = fileName;
      _fileDir.erase(indx + 1, _fileDir.length());
      _fileDir = fileDir + _fileDir;
   }
}
