#include "Parser.h"
#include "Trace.h"

Parser::Parser():
   _num_errors(0),
   _num_warnings(0),
   _line(0)
{
   
}

//=======================================================================================
// Загрузить текстовый файл в строку _st
//
Int  Parser::load(const string &  fname)
{
   ifstream in(fname);
   if (!in)       
      return 0;

   _st = "";               
   getline(in, _st, '\0');                        // очищаем старое значение

   in.close();

   return 1;
}
//=======================================================================================
// Вернуть номер строки (отсчёт от 1), соответствующей позиции  pos
//
UInt Parser::get_row(size_t pos, UInt *col_in_row)
{
   if( pos >= _st.size() )
      pos = _st.size() - 1;

   UInt line = 1;                                 // номер строки
   size_t beg = 0;
   for (size_t p = 0; p <= pos; p++)
      if (_st[p] == '\n') { line++; beg = p; }
   if(col_in_row)
      *col_in_row = UInt(pos - beg);
   return line;
}
//=======================================================================================
// Вернуть номер колонки (отсчёт от 1) в тексте для позиции  pos
//
UInt Parser::get_col(size_t pos)
{
   if (pos >= _st.size())
      pos = _st.size() - 1;

   size_t p2 = pos;
   while(pos > 0 && _st[pos] != '\n' && _st[pos] != '\r') // ищем конец предыдущей линии
      pos--;
   if(pos==0)
      return UInt(p2+1);

   return UInt(p2 - pos + 1);
}
//=======================================================================================
// Пропустить пробелы, вернув новое начало (где пробелы кончились)
//
size_t Parser::skip_spaces(size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   while (beg < end && isspace((unsigned char)_st[beg]))
   {
      beg++;
   }
      
   return beg;
}
size_t Parser::skip_spaces_not_new_lines(size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   while (beg < end && (unsigned char)_st[beg]!='\n' && isspace((unsigned char)_st[beg]))
   {
      beg++;
   }

   return beg;
}
//=======================================================================================
// Вернуть начало следующей строки
//
size_t Parser::next_line(size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   while (beg < end && _st[beg] != '\n')          // ищем конец линии
      beg++;
   if(beg == end)
      return end;
   while (beg < end && _st[beg] == '\r')          // ищем начало следующей линии
      beg++;
   return beg;
}
//=======================================================================================
// Вернуть положение символа ch или end, если его нет
//
size_t Parser::find(char ch, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   while (beg < end) {
      if(_st[beg] == ch)
         return beg;
      beg++;
   }
   return beg;
}
//=======================================================================================
// Пропустив пробелы,  первое первое слово и указатель после него 
// Слово прерывается пробелом или символами из списка brk
//
size_t Parser::get_word(string & word, const string & brk, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   beg = skip_spaces(beg, end);
   size_t b = beg;                                // начало слова
   while (beg < end 
          && !(isspace((unsigned char)_st[beg]) || brk.find(_st[beg]) != string::npos))
      beg++;

   word = _st.substr(b, beg - b);
   return beg;
}
//=======================================================================================
// Пропустив пробелы, вернуть первое имя и указатель после него 
// Имя состоит из латинских букв, цифр и символов chrs 
//
size_t Parser::get_name(string & name, const string & chrs, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   beg = skip_spaces(beg, end);
   size_t b = beg;                                // начало слова
   while (beg < end
      && (isalpha((unsigned char)_st[beg]) || isdigit((unsigned char)_st[beg])
        || chrs.find(_st[beg]) != string::npos))
      beg++;

   name = _st.substr(b, beg - b);
   return beg;
}
//=======================================================================================
// Пропустив пробелы, прочитать вещественное число val и вернуть указатель после него
//
size_t Parser::get_float(Float & val, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   beg = skip_spaces(beg, end);
   if(beg >= end){
      error("! Parser::get_float> no text: ", beg);
      return end;
   }
   if ( !isdigit(_st[beg]) && _st[beg]!='-') {
      error("! Parser::get_float> no digit: ", beg);
      return end;
   }
   size_t b = beg;
   while(beg < end &&  (isdigit(_st[beg]) || (_st[beg] == '-' && b == beg))  )       // целая часть числа
      beg++;
   if (_st[beg] == '.') {                         // дробная часть числа
      beg++;                                      // пропускаем точку
      while (beg < end &&  isdigit(_st[beg]))
         beg++;
   }
   
   val = std::stod( _st.substr(b, beg-b) );
   return beg;
}
//=======================================================================================
// Пропустив пробелы, прочитать name(name1, name2,...)
//
size_t Parser::get_function_head(string &name, vector<string> &args, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   beg = skip_spaces(beg, end);
   if (beg >= end) {
      error("! Parser::get_function> no text: ", beg);
      return end;
   }

   name="";  args.clear();
   beg = get_name(name, "_$",  beg, end);         // имя функции
   beg = skip_spaces(beg, end);
   if (_st[beg] != '(') {
      error("! Parser::get_function> no open braket ( ", beg);
      return beg;
   }
   size_t pos = find(')', beg, end);
   if(pos >= end){
      error("! Parser::get_function> no close braket ) ", beg);
      return beg;
   }
   get_list(args, beg+1, pos);

   for (auto &arg : args)
   {
      if (reservedName(arg))
      {
         error("! Parser::get_function> Name \"" + arg + "\" is reserved ", beg);
         return beg;
      }
   }  

   return pos+1;                            // указатель за скобкой
}
//=======================================================================================
// Получить ссылку на замыкающую скобку блока, игнорируя вложения {  { { } { } }  }
//
size_t Parser::get_block(char bra, char ket, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   beg = skip_spaces(beg, end);
   if (beg >= end) {
      error("! Parser::get_block> no text: ", beg);
      return end;
   }
   if (_st[beg] != bra) {
      error("! Parser::get_block> no open braket: ", beg);
      return beg;
   }
   beg++;                                         // пропускаем скобку
   Int cnt = 1;                                   // число открытых скобок
   while (beg < end) {
      if      (_st[beg] == bra)                   // открывающая скобка
         cnt++;
      else if (_st[beg] == ket) {                 // закрывающая скобка
         cnt--;
         if(cnt == 0)                             // блок окончен
            return beg;
      }
      beg++;
   }
   error("! Parser::get_block> no close braket: ", beg);
   return end;   
}
//=======================================================================================
// Устраняет строчные комментарии, забивая их пробелами (не самый эффективный метод?)
// Возвращает число удалённых комментариев
//
Int Parser::remove_line_comments(const string & start, size_t beg, size_t end)
{
   if(end == -1) end = _st.size();
   UInt count = 0;
   UInt singleQuotes = false;
   UInt doubleQuotes = false;
   Bool comment_start = false;
   for (size_t pos = beg; pos < end; pos++) {
      if (_st[pos] == '\'' && !doubleQuotes)
         singleQuotes = !singleQuotes;
      if (_st[pos] == '"' && !singleQuotes)
         doubleQuotes = !doubleQuotes;
      if (_st[pos] == start[0]){                  // комментарий возможно начался
         if(start.size() > 1){
            if(pos+1 == end)
               return (Int)count;
            if(_st[pos+1] == start[1] && !singleQuotes && !doubleQuotes){           // таки начался
               count++;
               comment_start = true;
            }
         }
      }
      if (_st[pos] == '\n')                       // комментарий окончился
         comment_start = false;
      if (comment_start)                          // забиваем пробелами
         _st[pos] = ' ';
   }
   return (Int)count;
}
//=======================================================================================
//! Устранить блочные комменатрии, между start и finish (два символа!)
//
Int Parser::remove_block_comments(const string & start, const string & finish,
                                  size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   Int open = 0;
   while (beg+1 < end) {
      if(_st[beg] == start[0] && _st[beg + 1] == start[1]){
         _st[beg] = _st[beg + 1] = ' ';
         open++;
         beg +=2;
         continue;
      }
      if (_st[beg] == finish[0] && _st[beg + 1] == finish[1]) {
         _st[beg] = _st[beg + 1] = ' ';
         open--;
         beg += 2;
         continue;
      }

      if(open)
         _st[beg] = ' ';
      beg++;
   }
   return 1;
}
//=======================================================================================
// Получить список слов, разделённых запятой; пробелы игнорируются
//
Int Parser::get_list(vector<string> &lst, size_t beg, size_t end)
{
   if (end == -1) end = _st.size();
   lst.clear();                                  // очищаем старые результаты
   while (beg < end) {
      size_t p = skip_spaces(beg, end), b;
      if (_st[p] == ',')                         // пропускаем запятую
         p = skip_spaces(p+1, end);

      if (_st[p] == '\"') {                      // начало строки
         b = ++p;
         while (p < end && _st[p] != '\"') p++;
         if (_st[p] != '\"') {
            error("! Parser::get_list> no close \"", p);
            return 0;
         }
         if (p > b)
            lst.push_back(_st.substr(b, p - b));
      }
      else {
         b = p;
         while (p < end && !isspace((unsigned char)_st[p]) && _st[p] != ',') p++;
         if (p > b)
            lst.push_back(_st.substr(b, p - b));
      }
      beg = p + 1;
   }
   return (Int)lst.size();
}
//=======================================================================================
// Вывести ошибку парсинга
//
Int Parser::error(const string & err, size_t pos)
{  
   if (_num_errors == 0)
   {
      //решили показывать только первую ошибку
      size_t c = 1, r = line(pos, &c);                  // номер строки и колонки, где ошибка
      TRACE_POS(L_CRITICAL, r, c) << err << endl;
      _num_errors++;
   }
   return 0;
}
//=======================================================================================
// Вывести предупреждение парсинга
//
void Parser::warning(const string & err, size_t pos)
{
   size_t c = 1, r = line(pos, &c);                  // номер строки и колонки, где ошибка
   TRACE_POS(L_WARNING, r, c) << err << endl;
   _num_warnings++;
}

Bool Parser::reservedName(const string & name)
{
   return (name == "value" || name == "get" || name == "set");
}

void Parser::linesBegPosExtract()
{
   _linesBegPos.clear();
   size_t end = _st.length();
   _linesBegPos.push_back(0);
   for (int i = 0; i < end; i++)
      if (_st[i] == '\n')
         _linesBegPos.push_back(i);
}

size_t Parser::line(size_t beg, size_t* col/* = 0*/)
{
   while (true)
   {
      if ((_line + 1)<_linesBegPos.size() && beg >= _linesBegPos[_line + 1])
      {
         _line++;
         continue;
      }
      else if (_line <_linesBegPos.size() &&  beg < _linesBegPos[_line] && _line > 0)
      {
         _line--;
         continue;
      }
      break;
   }
   if (col && _line <_linesBegPos.size())
   {
      *col = beg - _linesBegPos[_line] + 1;
   }
   return _line + 1;
}

