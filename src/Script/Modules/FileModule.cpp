#include "FileModule.h"
#include "ModuleManager.h"

//регистрируем модуль
IMPLEMENTATION_MODULE(FileModule,"File")

//регистрируем функции модуля
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncOpen,       "open")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncOut,        "out")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncClose,      "close")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncEOF,        "eof")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncIsOpen,     "is_open")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncGetLine,    "getline")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncGetLines,   "getlines")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncIn,         "in")
IMPLEMENTATION_MODULE_FUNCTION(FileModule, FileFuncSize,       "size")

//=======================================================================================
//! Открыть файл для чтения или записи
//
Int FileModule::open(string name, string options)
{
   auto openMode = ofstream::out;

   if (options == "w")
   {
      openMode = std::ofstream::out;
      _filesOut[_lastId] = make_shared<ofstream>(_script->workPath(name), openMode);
      _filesSize[_lastId] = 0;
   }
   else if (options == "a")
   {
      openMode = std::ofstream::in;
      auto temp = make_shared<ifstream>(_script->workPath(name), openMode);
      if (temp->is_open())
      {
         temp->seekg(0, temp->end);
         _filesSize[_lastId] = temp->tellg();
         temp->seekg(0, temp->beg);
         temp->close();
      }

      openMode = std::ofstream::app;
      _filesOut[_lastId] = make_shared<ofstream>(_script->workPath(name), openMode);
   }
   else if (options == "r")
   {
      openMode = std::ofstream::in;
      _filesIn[_lastId] = make_shared<ifstream>(_script->workPath(name), openMode);
      if (_filesIn[_lastId]->is_open())
      {
         _filesIn[_lastId]->seekg(0, _filesIn[_lastId]->end);
         _filesSize[_lastId] = _filesIn[_lastId]->tellg();
         _filesIn[_lastId]->seekg(0, _filesIn[_lastId]->beg);
      }
   }
   else
   {
      TRACE_CRITICAL << "! File.open: Wrong value of second argument (need \"w\" or \"a\")\n";
      return -1;
   }
         
   int res = _lastId;
   _lastId++;
   return res;
}

//=======================================================================================
//! Вывести строку в файл
//
void FileModule::out(int file, string data)
{
   auto itFile = _filesOut.find(file);

   if (itFile == _filesOut.end())
      return;

   if (!itFile->second->is_open())
      return;

   auto fileSize = _filesSize.find(file);

   if (fileSize != _filesSize.end())
   {
      _filesSize[file] += data.length();
   }
   else
   {
      _filesSize[file] = data.length();
   }

   (*itFile->second) << data;
}

//=======================================================================================
//! Закрыть файл
//
void FileModule::close(int file)
{
   auto fileSize = _filesSize.find(file);
   
   if (fileSize != _filesSize.end())
   {
      _filesSize.erase(fileSize);
   }

   auto outFile = _filesOut.find(file);

   if (outFile != _filesOut.end())
   {
      if (!outFile->second->is_open())
         return;

      outFile->second->close();
      _filesOut.erase(outFile);
      return;
   }
   
   auto inFile = _filesIn.find(file);

   if (inFile != _filesIn.end())
   {
      if (!inFile->second->is_open())
         return;

      inFile->second->close();
      _filesIn.erase(inFile);
      return;
   }
}

//=======================================================================================
//! Проверить, достигнут ли конец файла
//
bool FileModule::eof(int file)
{
   auto inFile = _filesIn.find(file);

   if (inFile != _filesIn.end())
   {
      return inFile->second->eof();
   }

   auto outFile = _filesOut.find(file);

   if (outFile != _filesOut.end())
   {
      return outFile->second->eof();
   }

   return true;
}
//=======================================================================================
bool FileModule::is_open(int file)
{
   auto inFile = _filesIn.find(file);

   if (inFile != _filesIn.end())
   {
      return inFile->second->is_open();
   }

   auto outFile = _filesOut.find(file);

   if (outFile != _filesOut.end())
   {
      return outFile->second->is_open();
   }

   return false;
}
//=======================================================================================
//! Прочитать строку
//
string FileModule::getline(int file)
{
   auto inFile = _filesIn.find(file);

   if (inFile == _filesIn.end())
   {
      return "";
   }

   if (!inFile->second->is_open())
      return "";

   string line = "";
   std::getline((*inFile->second), line);
   return line;
}

//=======================================================================================
//! Прочитать массив строк
//
void FileModule::getlines(int file, vector<string> & lines)
{
   lines.clear();
   auto inFile = _filesIn.find(file);

   if (inFile == _filesIn.end())
   {
      return;
   }

   if (!inFile->second->is_open())
      return;

   string line = "";
   while (!inFile->second->eof())
   {
      std::getline((*inFile->second), line);
      lines.push_back(line);
   }
}

//=======================================================================================
//! Прочитать все данные из файла
//
string FileModule::in(int file)
{
   auto inFile = _filesIn.find(file);

   if (inFile == _filesIn.end())
   {
      return "";
   }

   if (!inFile->second->is_open())
      return "";

   string line = "";
   string res = "";
   while (!inFile->second->eof())
   {
      std::getline((*inFile->second), line);
      res += line;
   }

   return res;
}
//=======================================================================================
UInt FileModule::size(int file)
{
   auto fileSize = _filesSize.find(file);

   if (fileSize == _filesSize.end())
   {
      return 0;
   }

   return (UInt)fileSize->second;
}
//=======================================================================================
//! Открывает файл
//
FileFuncOpen::FileFuncOpen()
{
   _minArgs = 2;
   _maxArgs = 2;
   _argTypes.resize(2);
   _argTypes[0].push_back(ValueBase::_STR);
   _argTypes[1].push_back(ValueBase::_STR);
}
//=======================================================================================
//! Открывает файл
//
Value FileFuncOpen::run()
{
   if (!initRun())
      return Value();

   string name = _vals[0].get_Str();
   string options = _vals[1].get_Str();

   FileModule* module = (FileModule*)_module;
   Int res = module->open(name, options);

   return ValueFile(name, res);
}
//=======================================================================================
//! Пишет данные в файл
//
FileFuncOut::FileFuncOut()
{
   _minArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Пишет данные в файл
//
Value FileFuncOut::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   for (size_t i = 1; i < _vals.size(); i++)
   {
      module->out(fileId, _vals[i].toString());
      module->out(fileId, i < _vals.size()-1 ? " " : "\n");
   }
 
   return Value();
}
//=======================================================================================
//! Закрывает файл
//
FileFuncClose::FileFuncClose()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Закрывает файл
//
Value FileFuncClose::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   module->close(fileId);

   return Value();
}
//=======================================================================================
//! Возвращает признак конца файла
//
FileFuncEOF::FileFuncEOF()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Возвращает признак конца файла
//
Value FileFuncEOF::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   return Value(Logic(module->eof(fileId)));
}
//=======================================================================================
//! Возвращает открыт ли файл
//
FileFuncIsOpen::FileFuncIsOpen()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Возвращает открыт ли файл
//
Value FileFuncIsOpen::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   return Value(Logic(module->is_open(fileId)));
}
//=======================================================================================
//! Возвращает размер файла
//
FileFuncSize::FileFuncSize()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Возвращает размер файла
//
Value FileFuncSize::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   return Value((Float)module->size(fileId));
}
//=======================================================================================
//! Читает строку из файла
//
FileFuncGetLine::FileFuncGetLine()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Читает строку из файла
//
Value FileFuncGetLine::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   return Value(module->getline(fileId));
}
//=======================================================================================
//! Читает весь файл в массив строк
//
FileFuncGetLines::FileFuncGetLines()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Читает весь файл в массив строк
//
Value FileFuncGetLines::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   vector<string> lines;
   module->getlines(fileId, lines);

   vector<shared_ptr<ValueBase>> linesValues;
   for (size_t i = 0; i < lines.size(); i++)
      linesValues.push_back(Value(lines[i]).ptr());

   return ValueArr(linesValues);
}
//=======================================================================================
//! Читает весь файл в одну строку
//
FileFuncIn::FileFuncIn()
{
   _minArgs = 1;
   _maxArgs = 1;
   _argTypes.resize(1);
   _argTypes[0].push_back(ValueBase::_FILE);
}
//=======================================================================================
//! Читает весь файл в одну строку
//
Value FileFuncIn::run()
{
   if (!initRun())
      return Value();

   int fileId = _vals[0].get_Int();

   FileModule* module = (FileModule*)_module;

   return Value(module->in(fileId));
}