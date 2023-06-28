/*!                                  Парсинг сторок
Универсальные алгоритмы анализа строки, которая хранится в _st. 
Большинство функций имеют парметры положения beg, end в строке,
между которыми осуществляется парсинг.
Пример:
   Parser p;
   p.load("file.txt");
   p.remove_line_comments("//");         // убираем строчные комментарии во всей строке

                             (с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef ParserH
#define ParserH
#include "OTypes.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;
//=======================================================================================
//! Парсер сторок
//
class Parser
{
public: 
   Parser();

   //! Загрузить текстовый файл в строку _st
   Int  load(const string & fname);
   //! Задать (скопировав) строку
   void     set (const string & st) { _st = st; }
   // Получить (изменить) строку
   string & get ()  { return _st; }           
   //!< Получить длинну строки
   size_t   size()  { return _st.size(); }    

   //! Получить (на чтение) символ в позиции pos
   char operator [](size_t pos) { return _st[pos]; }

   //! Пропустить пробелы и переводы на новую строку, вернув новое начало (где пробелы кончились)
   size_t skip_spaces(size_t beg = 0, size_t end = -1);

   //! Пропустить пробелы без переводов на новую строку, вернув новое начало (где пробелы кончились)
   size_t skip_spaces_not_new_lines(size_t beg = 0, size_t end = -1);

   //! Вернуть начало следующей строки
   size_t next_line(size_t beg = 0, size_t end = -1);

   //! Извлекаем начальные позиции каждой строки скрипта
   void linesBegPosExtract();

   //! Линия, соответствующая указанной позиции в строке
   size_t line(size_t beg, size_t* col = 0);

   //! Вернуть положение символа ch  или end, если его нет
   size_t find(char ch, size_t beg = 0, size_t end = -1);

   //! Вернуть номер строки (отсчёт от 1), соответствующей позиции  pos
   UInt get_row (size_t pos, UInt *col_in_row=0);
   //! Вернуть номер колонки (отсчёт от 1) в тексте для позиции  pos
   UInt get_col (size_t pos);

   //! Пропустив пробелы, вернуть первое слово и указатель после него 
   //! Слово прерывается пробелом или символами из списка brk
   size_t get_word(string & word, const string & brk = "=().", 
                   size_t beg = 0, size_t end = -1);

   //! Пропустив пробелы, вернуть первое имя и указатель после него 
   //! Имя состоит из латинских букв, цифр и символов chrs 
   size_t get_name(string & name, const string & chrs = "_$",
                   size_t beg = 0, size_t end = -1);

   //! Пропустив пробелы, прочитать первое вещественное число val и указатель после него
   size_t get_float(Float & val,  size_t beg = 0, size_t end = -1);

   //! Пропустив пробелы, прочитать name(name1, name2,...)
   size_t get_function_head(string &name, vector<string> &args, size_t beg=0, size_t end=-1);

   //! Устранить строчные комменатрии, начинающиеся с start (не более двух символов)
   Int remove_line_comments(const string & start="//", size_t beg=0, size_t end=-1);
   //! Устранить блочные комменатрии, между start и finish (два символа!)
   Int remove_block_comments(const string & start = "/*", const string & finish = "*/",  
                             size_t beg = 0, size_t end = -1);

   //! Получить список слов, разделённых запятой; пробелы игнорируются
   //! Слово -- это либо последовательность символов без пробела: $qn1_io+ ,
   //! либо строка в двойных кавычках: "safa askf"
   Int get_list(vector<string> &lst, size_t beg = 0, size_t end = -1);

   // Получить ссылку на замыкающую скобку блока, игнорируя вложения {  { { } { } }  }
   size_t get_block(char bra='{', char ket='}', size_t beg = 0, size_t end = -1);
   
   // Получить строку в кавычках
   //size_t get_string(size_t beg = 0, size_t end = -1)

   //! Вывести ошибку парсинга
   Int error  (const string & err, size_t pos);
   //! Вывести предупреждение парсинга
   void warning(const string & err, size_t pos);

   Int num_errors()    { return _num_errors; }    //!< Число ошибок парсинга
   Int num_warnings()  { return  _num_warnings;}  //!< Число предупреждений

   Bool reservedName(const string & name);        //!< проверить, зарезервировано ли имя

protected:
   vector<size_t> _linesBegPos;     //!< смещение начала каждой строки для быстрого вычисления текщей строки
   size_t         _line;            //!< последняя текущая линия, для оптимизации поиска новой
   string         _st;              //!< cтрока для парсинга 
   Int            _num_errors;      //!< Число ошибок парсинга
   Int            _num_warnings;    //!< Число предупреждений
};
#endif
