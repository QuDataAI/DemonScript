/*!            Math - модуль математических функций используемых скриптом

Доступ к функциям модуля осуществляется через вызов Math.<имя функции>(<аргументы>)

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef FileModuleH
#define FileModuleH

#include "Module.h"
#include "ModuleFunction.h"
#include <random>

//=======================================================================================
//! Модуль математических функций
// 
class FileModule : public Module
{
   INTERFACE_MODULE(FileModule);
public:
   /*!
   Открыть файл
   \param  name имя файла
   \param  options тип открытия a - дописать в конец, w - переписать, r - на чтение 
   \return айди открытого файла для дальнейшей работы с ним
   */
   Int     open(string name, string options);
   /*!
   Вывести строку в файл
   \param  file - айди файла
   \param  data - данные для вывода в файл
   */
   void    out(int file, string data);
   /*!
   Закрыть файл
   \param  file - айди файла
   */
   void    close(int file);
   /*!
   Проверить, достигнут ли конец файла
   \param  file - айди файла
   \return true - конец достигнут
   */
   bool    eof(int file);
   /*!
   Проверить, открыт ли файл
   \param  file - айди файла
   \return true - файл открыт
   */
   bool    is_open(int file);
   /*!
   Прочитать строку
   \param  file - айди файла
   \return строка из файла
   */
   string  getline(int file);
   /*!
   Прочитать массив строк
   \param  file - айди файла
   \param  lines - (out) вектор, в который сложим строки
   */
   void    getlines(int file, vector<string> & lines);
   /*!
   Прочитать все данные из файла
   \param  file - айди файла
   \return Данные в виде строки
   */
   string  in(int file);
   /*!
   Вернуть размер файла
   \param  file - айди файла
   \return Размер файла
   */
   UInt  size(int file);
private:
   //! файлы открытые на запись
   map<int, shared_ptr<ofstream>> _filesOut;
   //! файлы открытые на чтение
   map<int, shared_ptr<ifstream>> _filesIn;
   //! размер файлов открытых на чтение
   map<int, UInt> _filesSize;
   //! свободный айди для файла
   Int                            _lastId = 0;
};

//=======================================================================================
//! Открывает файл
// 
class FileFuncOpen : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncOpen();
};
//=======================================================================================
//! Пишет данные в файл
// 
class FileFuncOut : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncOut();
};
//=======================================================================================
//! Закрывает файл
// 
class FileFuncClose : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncClose();
};
//=======================================================================================
//! Возвращает признак конца файла
// 
class FileFuncEOF : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncEOF();
};
//=======================================================================================
//! Возвращает открыть ли файл
// 
class FileFuncIsOpen : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncIsOpen();
};
//=======================================================================================
//! Возвращает размер файла
// 
class FileFuncSize : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncSize();
};
//=======================================================================================
//! Читает строку из файла
// 
class FileFuncGetLine : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncGetLine();
};
//=======================================================================================
//! Читает весь файл в массив строк
// 
class FileFuncGetLines : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncGetLines();
};
//=======================================================================================
//! Читает весь файл в одну строку
// 
class FileFuncIn : public ModuleFunction
{
   INTERFACE_MODULE_FUNCTION;
   FileFuncIn();
};

#endif
