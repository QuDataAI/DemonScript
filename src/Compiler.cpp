#include "Compiler.h"
#include "Script/Command.h"
#include "Script/Function.h"
#include "Script/Expr.h"
#include "Modules/ModuleManager.h"
#include "Graph/GraphManager.h"
#include "Values/ValueLambda.h"
#include <limits.h>

Compiler::Compiler(ScriptPtr script):
   _beg(0),
   _is_new_local_var(false),
   _varArgType(VAR_ARG_NONE),
   _is_new_static_var(false),
   _allow_var_dupl(false),
   _sourceID(-1),
   _script(script)
{
}

//=======================================================================================
// Парсинг всего скрипта  хранящегося в _st 
//
Int  Compiler::parse(size_t sourceID)
{ 
   _sourceID   = sourceID;
   _num_errors = _num_warnings = 0;
   _beg  = 0;                                      // начинаем с начала строки _st
   _line = 0;
   linesBegPosExtract();
   remove_line_comments();                        // удаляем комментарии
   remove_block_comments("/*", "*/");             // удаляем комментарии

   Int res = 0;
   res = parse(_script->_code, _st.size());              // до конца строки

   return res;
}

//=======================================================================================
// Парсинг num_commands комманд скрипта кода code до позиции end строки _st
//
Int Compiler::parse(CodePtr code, size_t end, UInt num_commands)
{   
   if (_beg + 3 < end && (UInt8)_st[_beg] == 0xef && (UInt8)_st[_beg + 1] == 0xbb && (UInt8)_st[_beg + 2] == 0xbf)
      _beg += 3;  //пропускаем BOM если существует

   while ( _beg < end && num_commands > 0 ) {     // читаем num_commands
      _beg = skip_spaces(_beg, end);   
      if(_beg >= end)                             // файл или  блок закончились
         return 1;
      if( _st[_beg] == ';'){                      // пропускаем ;
         _beg++;
         continue;
      }

      _varArgType = VAR_ARG_NONE;
      _is_new_local_var = false;
      _is_new_static_var = false;
      _allow_var_dupl = false;
      size_t pos = get_word(_name,";=(:",_beg,end);// после него пробелы или ';', '=', '(', ':'
      if     (_name == "#include"){//----------------- вставка модуля:
         _beg += 8;
         if (!parse_include(code, end))
            return 0;
      }
      else if(_name == "var") {    //----------------- объявление переменной:
         _beg += 3;
         if(!parse_var(code, end))
            return 0;
      }
      else if (_name == "static") {    //----------------- объявление переменной:
         _beg += 6;
         if (!parse_static(code, end))
            return 0;
      }
      else if (_name == "edges") { //----------------- объявление связей:
         _beg += 5;
         if (!parse_edges(code, end))
            return 0;
      }
      else if (_name == "nodes") { //----------------- объявление узлов:
         _beg += 5;
         if (!parse_nodes(code, end))
            return 0;
      }
      else if(_name == "out") {    //----------------- вывод выражения на консоль:
         _beg += 3;
         if (!parse_out(code, end))
            return 0;
      }
      else if (_name == "fout") {    //----------------- вывод выражения на консоль:
         _beg += 4;
         if (!parse_out(code, end, 1))
            return 0;
      }
      else if (_name == "if") {    //----------------- условный оператор:
         _beg += 2;         
         if(!parse_if(code, end))
            return 0;
      }
      else if (_name == "while") { //----------------- цикл while:
         _beg += 5;
         if (!parse_while(code, end))
            return 0;
      }
      else if (_name == "for") {   //----------------- перебор объектов:
         _beg += 3;
         if (!parse_for(code, end))
            return 0;
      }
      else if (_name == "def") {   //----------------- определение функции:
         _beg += 3;
         if (!parse_def(code, end))
            return 0;
      }
      else if (_name == "return") {//----------------- возврат из блока кода:
         _beg += 6;
         if (!parse_return(code, end))
            return 0;
      }
      else if (_name == "continue") {//----------------- переход к следующему циклу блока кода:
         _beg += 8;  
         code->add_command(SPTR_MAKE(Command_continue)(_sourceID, line(_beg-8)));
         return 1;
      }
      else if (_name == "break") {//----------------- возврат из блока кода:
         _beg += 5;
         code->add_command(SPTR_MAKE(Command_break)(_sourceID, line(_beg-5)));
         return 1;
      }
      else if (_name == "global") {//----------------- объявление доступа к глобальной переменной
         _beg += 6;
         if (!parse_global(code, end))
            return 0;
      } 
      else if (_name == "get" && _st[_beg + 3] == ':') {  //----------------- дальше блок кода для функции в режиме Get
            return 0;
      }
      else if (_name == "set" && _st[_beg + 3] == ':') {  //----------------- дальше блок кода для функции в режиме Set
         return 0;
      }
      else {
         size_t commandBeg = _beg;
         ExprPtr expr = parse_EXPR(code, end);
         if (!expr) {
            error("! Compiler::parse> Can\'t read expression", _beg);
            return 0;
         }
         expr->root(true); //выряжение является корневым, его никто не включает
         code->add_command(SPTR_MAKE(Command_expr)(expr, _sourceID, line(commandBeg)) );
      }
      num_commands--;
      if (_num_errors) 
         return 0;                                // были ошибки парсинга
   }
   return 1;
}
//=======================================================================================
// Оператор включения модулей
//
Int Compiler::parse_include(CodePtr code, size_t end)
{
   size_t pos = skip_spaces(_beg, end);
   if (_st[pos] != '\"') {
      error("! Compiler::parse_include> No string after #include", _beg);
      return 0;
   }
   _beg = pos + 1;
   pos = find('\"', _beg, end);
   if (pos >= end) {
      error("! Compiler::parse_include> No close bracket for string", _beg);
      return 0;
   }

   string includeFileName = _st.substr(_beg, pos - _beg);
   string includeFileDir  = _script->srcFileDir(_sourceID);

   _script->load(includeFileName, includeFileDir, false);

   auto cm = SPTR_MAKE(Command_include)(_sourceID, line(_beg));
   _name = cm->_fname = _st.substr(_beg, pos - _beg);
   _beg = pos + 1;
   code->add_command(cm);
   return 1;
}
//=======================================================================================
// Парсинг возврата из функции
//
Int Compiler::parse_return(CodePtr code, size_t end)
{
   auto cm = SPTR_MAKE(Command_return)(_sourceID, line(_beg));
   size_t nextLine = next_line(_beg, end);
   _beg = skip_spaces(_beg, end);
   if (_st.substr(_beg, 6) == "ifdef ") {         // конструкция: return ifdef EXPR
      _beg += 6;
      cm->_isdef = 1;
   }
   if (nextLine < _beg)
   {
      // после return первод на новую строку, поэтому возвращаем Undef       
   }
   else
   {
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse> Can\'t read expression after return", _beg);
         return 0;
      }
      cm->_expr = expr;
   }

   code->add_command(cm);
   return 1;
}
Int Compiler::parse_global(CodePtr code, size_t end)
{
   size_t commandBeg = _beg;
   _beg = skip_spaces(_beg, end);
   while (_beg < end) {
      _beg = get_name(_name, "_$@#", _beg, end);
      if (!_name.size())
      {
         error("! Compiler::parse_global> name is empty", _beg);
         return 0;
      }
      //добавляем имя переменной в список разрешенных для доступа
      //из текущего кода
      code->add_permitted_global_variable(_name);
      _beg = skip_spaces(_beg, end);
      if (_beg < end && _st[_beg] != ',')
         break;
      _beg++;
   }

   auto cm = SPTR_MAKE(Command_global)(_sourceID, line(commandBeg));
   code->add_command(cm);
   return 1;
}

Int Compiler::parse_function_body(FunDemonPtr fun, bool lambda, size_t end)
{
   size_t f_end = end;
   UInt   maxNumCommands = UINT_MAX;

   if (_st[_beg] == ':')
   {
      //функция состоит из одной команды
      _beg++;
      maxNumCommands = 1;
   }
   else if (_st[_beg] == '{')
   {
      size_t f_beg = _beg + 1;
      f_end = get_block('{', '}', _beg, end);
      if (f_end >= end) {
         error("! Compiler::parse_def> No close bracket '}' after function defenition", _beg);        
         return 0;
      }

      _beg = f_beg;
   }
   else
   {
      if (lambda)
      {
         maxNumCommands = 1;
      }
      else
      {
         error("! Compiler::parse_def> No open bracket '{' after function defenition", _beg);         
         return 0;
      }
   }

   /*
      !!! однокомандные лямбда выражения типа var "a = (b,c)=>b>c" всегда возвращают результат
      выражения и эквивалентная форма будет следующая "a = (b,c)=>{return b>c}",
      поэтому return вставляем принудительно
   */
   if (lambda && maxNumCommands == 1)
   {
      parse_return(fun->_codeCommon, f_end);
      fun->_codeCommon->_shift = 0;
      return 1;
   }
   else
   {
      //парсим общий блок команд
      parse(fun->_codeCommon, f_end, maxNumCommands);
   }


   //дальше могут идти блоки get и set
   bool wasGet = false;
   bool wasSet = false;
   while (!wasGet || !wasSet)
   {
      size_t pos = get_word(_name, ";=(:", _beg, end);
      if (!wasGet && _name == "get")
      {
         wasGet = true;
         _beg = pos;
         _beg = skip_spaces(_beg, end);
         if (_beg >= end || _st[_beg] != ':') {
            error("! Compiler::parse_def> No open bracket ':' after get marker", _beg);
            return 0;
         }
         _beg++;
         parse(fun->_codeGet, f_end, maxNumCommands);
         continue;
      }
      if (!wasSet && _name == "set")
      {
         wasSet = true;
         _beg = pos;
         _beg = skip_spaces(_beg, end);
         if (_beg >= end || _st[_beg] != ':') {
            error("! Compiler::parse_def> No open bracket ':' after set marker", _beg);
            return 0;
         }
         _beg++;
         parse(fun->_codeSet, f_end, maxNumCommands);
         continue;
      }
      break;
   }

   if (maxNumCommands > 1)
      _beg = f_end + 1;                              // после }

   return 1;
}

Int Compiler::parseCodeBlock(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_st[_beg] == ':')
      _beg = skip_spaces(_beg + 1, end);                // после ':'
   if (_beg < end && _st[_beg] == '{') {                // дальше идет блок команд    
      size_t pos = get_block('{', '}', _beg, end);
      _beg++;                                           // пропускаем {
      if (pos >= end) {
         error("! Compiler::parseCodeBlock> No close bracket '}' after if", _beg);
         return 0;
      }
      if (!parse(code, pos)) {
         error("! Compiler::parseCodeBlock> Can\'t read block of commands after if :{ }", _beg);
         return 0;
      }
      _beg = pos + 1;                                   // после }
   }
   else {                                               // это однокомандный блок
      if (!parse(code, end, 1)) {                       // парсим только одну команду
         error("! Compiler::parseCodeBlock> Can\'t read command after if : ...", _beg);
         return 0;
      }
   }
   return 1;
}
//=======================================================================================
// Парсинг оператора объявления локальной переменной
//
Int Compiler::parse_var(CodePtr code, size_t end)
{
   _is_new_local_var = true;
   size_t commandBeg = _beg;
   ExprPtr expr = parse_EXPR(code, end);
   if (!expr) {
      error("! Compiler::parse_var> Can\'t read expression after var", _beg);
      return 0;
   }
   auto cm = SPTR_MAKE(Command_var)(_sourceID, line(commandBeg));
   cm->_exprs.push_back(expr);

   while (_beg < end) {
      _beg = skip_spaces(_beg, end);
      if (_beg >= end)                            // файл или  блок закончились
         break;
      if (_st[_beg] != ',')                 
         break;
      _beg++;                                     // пропускаем запятую
      _is_new_local_var = true;
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse_expr> Can\'t read expression after var and comma", _beg);
         return 0;
      }
      cm->_exprs.push_back(expr);
   }
   code->add_command(cm);
   return 1;
}
//=======================================================================================
// Парсинг оператора объявления статической переменной
//
Int Compiler::parse_static(CodePtr code, size_t end)
{
   _is_new_static_var = true;
   size_t commandBeg = _beg;
   ExprPtr expr = parse_EXPR(code, end);
   if (!expr) {
      error("! Compiler::parse_static> Can\'t read expression after var", _beg);
      return 0;
   }
   auto cm = SPTR_MAKE(Command_static)(_sourceID, line(commandBeg));
   cm->_exprs.push_back(expr);

   while (_beg < end) {
      _beg = skip_spaces(_beg, end);
      if (_beg >= end)                            // файл или  блок закончились
         break;
      if (_st[_beg] != ',')
         break;
      _beg++;                                     // пропускаем запятую
      _is_new_static_var = true;
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse_expr> Can\'t read expression after var and comma", _beg);
         return 0;
      }
      cm->_exprs.push_back(expr);
   }
   code->add_command(cm);
   return 1;

}

Int Compiler::parse_edges(CodePtr code, size_t end)
{
   auto cm = SPTR_MAKE(Command_edges)(_sourceID, line(_beg));
   Int cnt = 0;
   while (_beg < end) {
      _beg = skip_spaces(_beg, end);
      if (_beg >= end)                            // файл или  блок закончились
         break;

      if (_st[_beg] == ',')                       // пропускаем запятую
         _beg++;
      else if (cnt)
         break;

      cnt++;
      size_t pos = get_name(_name, "_$@#", _beg, end);
      if (_name.size() == 0)
         break;
      
      cm->_edges.push_back(_name);             // запоминаем константы

      // регестрируем имя ребра
      Int id = GraphManager::instance().addEdgeName(_name);      
      _beg = pos;
   }
   code->add_command(cm);
   return 1;
}

Int Compiler::parse_nodes(CodePtr code, size_t end, ExprPtr graphExpr)
{
   auto cm = SPTR_MAKE(Command_nodes)(graphExpr, _sourceID, line(_beg));
   Int cnt = 0;
   while (_beg < end) {
      _beg = skip_spaces(_beg, end);
      if (_beg >= end)                            // файл или  блок закончились
         break;

      if (_st[_beg] == ',')                       // пропускаем запятую
         _beg++;
      else if (cnt)
         break;

      cnt++;
      size_t pos = get_name(_name, "_$@#", _beg, end);
      if (_name.size() == 0)
         break;

      cm->_nodes.push_back(_name);             // запоминаем константы

                                               // регестрируем имя узла
      GraphManager::instance().addNodeName(_name);

      _beg = pos;
   }
   code->add_command(cm);
   return 1;
}

//=======================================================================================
// Парсинг условного оператора 
//
Int Compiler::parse_if(CodePtr code, size_t end)
{   
   //теперь разрешены следующие конструкции:
   // 1.
   //   if True : out a
   //   false     out b
   //   undef     out b
   //   else      out b
   // 2.
   //   if True { out a}
   //   false   { out b}
   //   undef   { out b}
   //   else    { out b}
   // 3.
   //   if True: { out a}
   //   false:   { out b}
   //   undef:   { out b}
   //   else:    { out b}

   size_t commandBeg = _beg;
   ExprPtr ex = parse_EXPR(code, end);              // выражение условия
   if (!ex) {
      error("! Compiler::parse_if> Can\'t read condition after if", _beg);
      return 0;
   }
   if (_beg >= end || (_st[_beg] != '{' && _st[_beg] != ':'))
   {
      error("! Compiler::parse_if> No ':' or '{' after if", _beg);
      return 0;
   }
   CodePtr codeIf   = SPTR_MAKE(Code)(code);
   CodePtr codeFalse = 0;
   CodePtr codeUndef = 0;
   CodePtr codeElse  = 0;
   if (!parseCodeBlock(codeIf, end))
   {
      error("! Compiler::parse_if> parseCodeBlock after if", _beg);
      return 0;
   }
   //а теперь посмотрим не идет ли дальше "false", "undef" или "else"
   while (true)
   {
      _beg = skip_spaces(_beg, end);
      if (_st[_beg] == ';')
         _beg++;
      _beg = skip_spaces(_beg, end);
      size_t pos = get_word(_name, ":;=(", _beg, end);
      if (!codeFalse && _name == "false")
      {
         _beg = pos;
         codeFalse = SPTR_MAKE(Code)(code); 
         if (!parseCodeBlock(codeFalse, end))
         {
            error("! Compiler::parse_if> parseCodeBlock after if false", _beg);
            return 0;
         }
         continue;
      } 
      else if (!codeUndef && _name == "undef")
      {
         _beg = pos;
         codeUndef = SPTR_MAKE(Code)(code);
         if (!parseCodeBlock(codeUndef, end))
         {
            error("! Compiler::parse_if> parseCodeBlock after if undef", _beg);            
            return 0;
         }
         continue;
      }
      else if (_name == "else")
      {
         _beg = pos;
         codeElse = SPTR_MAKE(Code)(code);
         if (!parseCodeBlock(codeElse, end))
         {
            error("! Compiler::parse_if> parseCodeBlock after if else", _beg);            
            return 0;
         }
         break; //else всегда в конце
      }
      break;
   }

   code->add_command(SPTR_MAKE(Command_if)(ex, codeIf, codeFalse, codeUndef, codeElse, _sourceID, line(commandBeg)));

   return 1;
}
//=======================================================================================
// Оператор цикла while
//
Int Compiler::parse_while(CodePtr code, size_t end)
{
   //теперь разрешены следующие конструкции:
   // 1.
   //   while True : break
   // 2.
   //   while True :  
   //     { break }
   // 3.
   //   while True
   //     { break }

   size_t commandBeg = _beg;
   CodePtr cd = SPTR_MAKE(Code)(code);           // код оператора while
   auto cm = SPTR_MAKE(Command_while)(cd,_sourceID,line(commandBeg));
   cm->_expr = parse_EXPR(cm->_code, end);     // условие выполнения цикла
   if (!cm->_expr) {
      error("! Compiler::parse_while> Can\'t parse while-condition", _beg);
      return 0;
   }
   _beg = skip_spaces(_beg, end);

   if (_beg >= end || (_st[_beg] != ':' && _st[_beg] != '{')) {
      error("! Compiler::parse_while> No \':\' or \'{\' after while", _beg);
      return 0;
   }

   if (_st[_beg] == ':')
      _beg = skip_spaces(_beg + 1, end);          // после ':'
   if (_st[_beg] == '{') {                        // это блочный for
      size_t pos = get_block('{', '}', _beg, end);
      if (pos >= end) {
         error("! Compiler::parse_while> No close bracket \'}\' after for", _beg);
         return 0;
      }
      _beg++;                                     // пропускаем {
      if (!parse(cd, pos)) {
         error("! Compiler::parse_while> Can\'t read block of commands after for :{ }", _beg);
         return 0;
      }
      code->add_command(cm);
      _beg = pos + 1;                             // после }
   }
   else {                                         // это строчный for (одна команда)
      if (!parse(cd, end, 1)) {                  // парсим только одну команду
         error("! Compiler::parse_while> Can\'t read command after for : ...", _beg);
         return 0;
      }
      code->add_command(cm);
   }
   return 1;
}
//=======================================================================================
// Оператор цикла for
//
Int Compiler::parse_for(CodePtr code, size_t end)
{
   //теперь разрешены следующие конструкции:
   // 1.
   //   for i in [0] : out i
   // 2.
   //   for i in [0] : 
   //     { out i }
   // 3.
   //   for i in [0]  
   //     { out i }
   // 4.
   //   for i,j in Arr : out i,j  // i - ключ, j - значение
   // 5.
   //   for X in X isa B : out X  // выводим узлы для которых истинно выражение справа от in
   //
   size_t commandBeg = _beg;
   _beg = get_name(_name, "_", _beg, end);
   if (_name.size() == 0) {
      error("! Compiler::parse_for> Can\'t read var after for", _beg);
      return 0;
   }
   CodePtr cd = SPTR_MAKE(Code)(code);          // код оператора for
   auto cm = SPTR_MAKE(Command_for)(cd, _sourceID, line(commandBeg));
   cm->_var1 = cd->add_local_variable(_name);            // добавляем переменную в локальные
   _beg = skip_spaces(_beg, end);

   if (_beg < end && _st[_beg] == ',')
   {
      // имеем схему типа for i,j in ...
      // парсим j
      _beg++;
      _beg = get_name(_name, "_", _beg, end);
      if (_name.size() == 0) {
         error("! Compiler::parse_for> Can\'t read var after for i,", _beg);
         return 0;
      }
      cm->_var2 = cd->add_local_variable(_name);            // добавляем переменную в локальные
      _beg = skip_spaces(_beg, end);
   }
   if (_beg < end && _st[_beg] == ',')
   {
      // имеем схему типа for i,j,z in ...
      // парсим j
      _beg++;
      _beg = get_name(_name, "_", _beg, end);
      if (_name.size() == 0) {
         error("! Compiler::parse_for> Can\'t read var after for i,j,", _beg);
         return 0;
      }
      cm->_var3 = cd->add_local_variable(_name);            // добавляем переменную в локальные
      _beg = skip_spaces(_beg, end);
   }
   if (_beg < end && _st[_beg] == ',')
   {
      // имеем схему типа for i,j,z,w in ...
      // парсим j
      _beg++;
      _beg = get_name(_name, "_", _beg, end);
      if (_name.size() == 0) {
         error("! Compiler::parse_for> Can\'t read var after for i,j,z,", _beg);
         return 0;
      }
      cm->_var4 = cd->add_local_variable(_name);            // добавляем переменную в локальные
      _beg = skip_spaces(_beg, end);
   }
   _beg = skip_spaces(_beg, end);
   if (_beg < end && (_st[_beg] == ':' || _st[_beg] == '{'))
   {
      //тут может быть короткая форма записи: for X: out X (эквивалентно for X in True: out X)
      cm->_expr = SPTR_MAKE(ExprConst)(Logic::True);
      cm->_iterGraphExpr = true;
   }
   else
   {
      //ждем in
      if (_beg + 2 >= end || _st[_beg] != 'i' || _st[_beg + 1] != 'n' || _st[_beg + 2] != ' ') {
         error("! Compiler::parse_for> No 'in' after variable for", _beg);
         return 0;
      }
      _beg += 3;                                      // пропускаем in

      cm->_expr = parse_EXPR(cm->_code, end);      // условие выполнения цикла
      if (!cm->_expr) {
         error("! Compiler::parse_for> No for-condition after in", _beg);
         return 0;
      }
      if (cm->_var1->used())
      {
         //если переменная слева от in используется в выражении, 
         //то мы выполняем итерацию по всем узам графа, которые удовлетворяют выражению справа от in
         cm->_iterGraphExpr = true;
      }
      else
      {
         if (cm->_expr->kind() == Expr::_CONST)
         {
            //для обратной совместимости ловим for X in True
            cm->_iterGraphExpr = SPTR_DCAST(ExprConst,cm->_expr)->_const.type() == ValueBase::_LOGIC;
         }
         else
         {
            cm->_iterGraphExpr = false;
         }         
      }
   }
   _beg = skip_spaces(_beg, end);
   if (_beg >= end || (_st[_beg] != ':' && _st[_beg] != '{')) {
      error("! Compiler::parse_for> No \':\' or \'{\' after for", _beg);
      return 0;
   }   
   if (_st[_beg] == ':')
      _beg = skip_spaces(_beg+1, end);                // после ':'

   if (_st[_beg] == '{') {                        // это блочный for
      size_t pos = get_block('{', '}', _beg, end);
      if (pos >= end) {
         error("! Compiler::parse_for> No close bracket \'}\' after for", _beg);
         return 0;
      }
      _beg++;                                     // пропускаем {
      if (!parse(cd, pos)) {
         error("! Compiler::parse_for> Can\'t read block of commands after for :{ }",_beg);
         return 0;
      }
      code->add_command(cm);
      _beg = pos + 1;                             // после }
   }
   else {                                         // это строчный for (одна команда)
      if (!parse(cd, end, 1)) {                  // парсим только одну команду
         error("! Compiler::parse_for> Can\'t read command after for : ...", _beg);
         return 0;
      }
      code->add_command(cm);
   }
   return 1;
}
//=======================================================================================
// Парсинг вывода на консоль
//
Int Compiler::parse_out(CodePtr code, size_t end, Int fout)
{
   size_t commandBeg = _beg;
   auto cm = SPTR_MAKE(Command_out)(_sourceID, line(commandBeg));
   _beg = skip_spaces_not_new_lines(_beg, end);
   cm->_fout = fout;
   if (_beg >= end || _st[_beg] == '\n')
   {
      //пустой out, заменяем на out GRAPH
      ExprPtr expr = SPTR_MAKE(ExprVar)(findVariable(code, "GRAPH"));
      cm->_exprs.push_back(expr);
   }
   else
   {
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse_out> Can\'t read expression after out", _beg);
         return 0;
      }
      cm->_exprs.push_back(expr);
   }
   while (_beg < end) {
      _beg = skip_spaces(_beg, end);
      if (_beg >= end)                            // файл или  блок закончились
         break;
      if (_st[_beg] != ',')                       // пропускаем запятую
         break;
      _beg++;                                     // пропускаем запятую
      _beg = skip_spaces_not_new_lines(_beg, end);
      if (_beg < end)
      {
         if (_st[_beg] == ',')
         {
            //за запятой сразу идет запятая, душим пробел
            _beg++;                                     // пропускаем запятую
            _beg = skip_spaces_not_new_lines(_beg, end);
            if (_beg >= end || _st[_beg] == '\n')
            {
               //за запятой сразу идет перевод коретки, устанавливаем признак того, что коретку переводить не нужно
               cm->_carRet = false;
               break;
            }
         }
         else if (_beg >= end || _st[_beg] == '\n')
         {
            ExprPtr expr = SPTR_MAKE(ExprConst)(" ");
            cm->_exprs.push_back(expr);

            //за запятой сразу идет перевод коретки, устанавливаем признак того, что коретку переводить не нужно
            cm->_carRet = false;
            break;
         }
         else
         {
            ExprPtr expr = SPTR_MAKE(ExprConst)(" ");
            cm->_exprs.push_back(expr);
         }
      }
      
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse_out> Can\'t read expression after out and comma",_beg);
         return 0;
      }
      cm->_exprs.push_back(expr);
   }
   code->add_command(cm);
   return 1;
}
//=======================================================================================
// Парсинг объявления функции
//
Int Compiler::parse_def(CodePtr code, size_t end)
{
   vector<string> args;
   _beg = get_function_head(_name, args, _beg, end);
   _beg = skip_spaces(_beg, end);
   if (_beg >= end || (_st[_beg] != '{' && _st[_beg] != ':')) {
      error("! Compiler::parse_def> No open bracket '{' or ':' after def function", _beg);
      return 0;
   }

   if (_script->get_function(_name))
   {
      stringstream str;
      str << "! Compiler::parse_def> function with name " << _name << " is already defined";
      error(str.str(), _beg);
      return 0;
   }

   FunDemonPtr fun = SPTR_MAKE(FunDemon)(_name, args, _script);
   _script->add_function(fun);

   if (!parse_function_body(fun, false, end))
   {
      return 0;
   } 
   
   return 1;
}
//=======================================================================================
// EXPR  :-  VAR = EXPR | TERM1                
//
ExprPtr Compiler::parse_EXPR(CodePtr code, size_t end)
{
   return parseTermAssign(code, end);
}

ExprPtr Compiler::parseTermAssign(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermAssign> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermMultiary(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermAssign> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg < end) &&
            (
               (_st[_beg] == '=')
            )
      )
   {
      _beg++;
      _is_new_local_var = false;
      _is_new_static_var = false;
      ExprPtr nextTermExpr = parseTermAssign(code, end);
      if (!nextTermExpr) {
         return 0;
      }
      termExpr = SPTR_MAKE(ExprAssign)(termExpr, nextTermExpr);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermMultiary(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermMultiary> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermEquality(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermMultiary> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
      (_beg <= end) &&
      (
      (_st[_beg] == '?')
      )
      )
   {
      _beg++;
      ExprPtr termSecondArity = parseTermEquality(code, end);
      if (!termSecondArity) {
         error("! Compiler::parseTermMultiary> can't parse term after ?", _beg);
         return 0;
      }
      _beg = skip_spaces(_beg, end);
      if (_st[_beg] != ':')
      {
         error("! Compiler::parseTermMultiary> can't find ':' after ?", _beg);
         return 0;
      }
      _beg++;
      ExprPtr termThirdArity = parseTermEquality(code, end);
      if (!termSecondArity) {        
         error("! Compiler::parseTermMultiary> can't parse term after ?:", _beg);
         return 0;
      }
      _beg = skip_spaces(_beg, end);
      ExprPtr termForthArity = 0;  //4-й и 5-й не являются обязательными
      ExprPtr termFifthArity = 0;
      if (_st[_beg] == ':')      //есть 4-й
      {
         _beg++;
         termForthArity = parseTermEquality(code, end);
         if (!termForthArity) {
            error("! Compiler::parseTermMultiary> can't parse term after ?::", _beg);
            return 0;
         }
         _beg = skip_spaces(_beg, end);
         if (_st[_beg] == ':')      //есть 5-й
         {
            _beg++;
            termFifthArity = parseTermEquality(code, end);
            if (!termFifthArity) {
               error("! Compiler::parseTermMultiary> can't parse term after ?::", _beg);
               return 0;
            }
         }
      }
      termExpr = SPTR_MAKE(ExprMultiary)(termExpr,
                                         termSecondArity, 
                                         termThirdArity, 
                                         termForthArity,
                                         termFifthArity);
   }
   return termExpr;
}
ExprPtr Compiler::parseTermEquality(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermImpl> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermImpl(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermImpl> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (  
            ((_beg + 2) <= end) &&
            (
               (_st[_beg] == '<' && _st[_beg + 1] == '-' && _st[_beg + 1] == '>')
            )                       
         )
   {
      if (_st[_beg] == '<' && _st[_beg + 1] == '-' && _st[_beg + 1] == '>') {          // эквивалентность
         _beg += 3;
         ExprPtr nextTermExpr = parseTermImpl(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprEquality)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}
ExprPtr Compiler::parseTermImpl(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermImpl> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermOr(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermImpl> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg + 1 <= end) &&
            (
               _st[_beg] == '-' && _st[_beg + 1] == '>'
            )      
         )
   {
      if (_st[_beg] == '-' && _st[_beg+1] == '>') {          // импликация
         _beg += 2;
         ExprPtr nextTermExpr = parseTermOr(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprImpl)(termExpr, nextTermExpr);;
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}
ExprPtr Compiler::parseTermOr(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermOr> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermAnd(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermOr> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg <= end) &&
            (
               _st[_beg] == '|'
            )
         )
   {
      if (_st[_beg] == '|') {          // логическое "И"
         _beg += 1;
         ExprPtr nextTermExpr = parseTermAnd(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprOr)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}
ExprPtr Compiler::parseTermAnd(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermAnd> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermEqNotEq(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermAnd> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg <= end) &&
            (
               _st[_beg] == '&'
            )
         )
   {
      if (_st[_beg] == '&') {          // логическое "И"
         _beg += 1;
         ExprPtr nextTermExpr = parseTermEqNotEq(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprAnd)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}
ExprPtr Compiler::parseTermEqNotEq(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermEqNotEq> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermEdge(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermEqNotEq> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg + 1 <= end) &&
            (
               (_st[_beg] == '=' && _st[_beg + 1] == '=') ||
               (_st[_beg] == '!' && _st[_beg + 1] == '=') 
            )
         )
   {
      if (_st[_beg] == '=' && _st[_beg + 1] == '=') {          // равно
         _beg += 2;
         ExprPtr nextTermExpr = parseTermEdge(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprEq)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '!' && _st[_beg + 1] == '=') {     // не равно
         _beg += 2;
         ExprPtr nextTermExpr = parseTermEdge(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprNeq)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermEdge(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermEdge> End of file", _beg);
      return 0;
   }

   size_t endLinePos = find('\n', _beg, end);
   size_t sepPos = find(';', _beg, endLinePos);

   size_t endCommand = endLinePos < sepPos ? endLinePos : sepPos;

   ExprPtr termExpr = parseTermComp(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermEdge> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   if (_beg >= endCommand ||
      (
         _beg < end &&
         (  _st[_beg] == '=' ||
            _st[_beg] == ',' ||
            _st[_beg] == '{' ||
            _st[_beg] == '}' ||
            _st[_beg] == ':' ||
            _st[_beg] == ';' ||
            _st[_beg] == ']' ||
            _st[_beg] == '&' ||
            _st[_beg] == '|' ||
            _st[_beg] == '-' ||
            _st[_beg] == '*' ||
            _st[_beg] == ')' 
          )
      )                      ||
      (
         _beg + 1 < end && 
         (
            _st[_beg] == '!' && _st[_beg + 1] == '='
         )
      )
      )
      return termExpr;

   //тут ожидаем конструкцию типа: X E Y

   Logic  val = Logic::True;
   size_t pos = _beg;

   //перед отношением может быть отрицание "!"
   if (_st[_beg] == '!')
   {
      val = Logic::False;
      _beg++;
   }

   //перед отношением может быть неопределенность "?"
   if (_st[_beg] == '?')
   {
      val = Logic::Undef;
      _beg++;
   }

   ExprPtr X_Expr = termExpr;
   ExprPtr E_Expr = parseTermComp(code, end);
   if (!E_Expr || _beg > endCommand)
   {
      error("! Compiler::parseTermEdge> Can\'t parse E in \"X E Y\"", _beg);
      return 0;
   }

   //тут может быть вилка
   // либо True == a ?in b
   // либо True == a ?in : on
   // поэтому проверяем не идет ли дальше двоеточие
   _beg = skip_spaces(_beg, end);
   if (_st[_beg] == ':')
   {
      //дальше идет мультиарное условие, поэтому возвращаемся
      _beg = pos;
      return termExpr;
   }

   ExprPtr Y_Expr = parseTermComp(code, end);
   if (!Y_Expr || _beg > end)
   {
      error("! Compiler::parseTermEdge> Can\'t parse Y in \"X E Y\"", _beg);
      return 0;
   }

   termExpr = SPTR_MAKE(ExprEdge)(X_Expr, E_Expr, Y_Expr, val);

   return termExpr;
}

ExprPtr Compiler::parseTermComp(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermComp> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermPlusMinus(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermComp> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg + 1 <= end) &&
            (
               (_st[_beg] == '>' && _st[_beg + 1] == '=') ||
               (_st[_beg] == '<' && _st[_beg + 1] == '=') ||
               (_st[_beg] == '<')                         ||
               (_st[_beg] == '>') 
            )
         )
   {
      if (_st[_beg] == '<' && _st[_beg + 1] != '=') {          // меньше
         _beg += 1;
         ExprPtr nextTermExpr = parseTermPlusMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprLt)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '>' && _st[_beg + 1] != '=') {     // больше
         _beg += 1;
         ExprPtr nextTermExpr = parseTermPlusMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprGt)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '<' && _st[_beg + 1] == '=') {     // меньше или равно
         _beg += 2;
         ExprPtr nextTermExpr = parseTermPlusMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprLtEq)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '>' && _st[_beg + 1] == '=') {     // больше  или равно
         _beg += 2;
         ExprPtr nextTermExpr = parseTermPlusMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprGtEq)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermPlusMinus(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermPlusMinus> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermDotDivMod(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermPlusMinus> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);
   while (
            (_beg + 1 <= end) &&
            (
               (_st[_beg] == '+')                         ||
               (_st[_beg] == '-' && _st[_beg + 1] != '>')
            )
         )
   {
      if (_st[_beg] == '+') {                                // сложение
         _beg++;
         ExprPtr nextTermExpr = parseTermDotDivMod(code, end);
         if (!nextTermExpr) {            
            return 0;
         }
         termExpr = SPTR_MAKE(ExprPlus)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '-' && _st[_beg + 1] != '>') {    // вычитание
         _beg++;
         ExprPtr nextTermExpr = parseTermDotDivMod(code, end);
         if (!nextTermExpr) {            
            return 0;
         }
         termExpr = SPTR_MAKE(ExprMinus)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermDotDivMod(CodePtr code, size_t end)
{
  _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermDotDivMod> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermPreMinus(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermDotDivMod> Can\'t read term", _beg);
      return 0;
   }

   _beg = skip_spaces(_beg, end);

   while (
            (_beg <= end) &&
            (
               (_st[_beg] == '*')                         ||
               (_st[_beg] == '/')                         ||
               (_st[_beg] == '\\')                        ||
               (_st[_beg] == '%')
             )
         )
   {
      if (_st[_beg] == '*') {                        // умножение
         _beg++;
         ExprPtr nextTermExpr = parseTermPreMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprMult)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '\\') {                               // целочисленное деление
         _beg++;
         ExprPtr nextTermExpr = parseTermPreMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprDivInt)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '/') {                               // вещественное деление
         _beg++;
         ExprPtr nextTermExpr = parseTermPreMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprDiv)(termExpr, nextTermExpr);
      }
      else if (_st[_beg] == '%') {                   // модуль
         _beg++;
         ExprPtr nextTermExpr = parseTermPreMinus(code, end);
         if (!nextTermExpr) {
            return 0;
         }
         termExpr = SPTR_MAKE(ExprMod)(termExpr, nextTermExpr);
      }
      _beg = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermPreMinus(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermNot> End of file", _beg);
      return 0;
   }
   _beg = skip_spaces(_beg, end);

   ExprPtr termExpr = NULL;

   while (
      (_beg + 1 <= end) &&
      (
         _st[_beg] == '-' && _st[_beg + 1] != '-'
         )
      )
   {
      _beg++;
      ExprPtr nextTermExpr = parseTermPreMinus(code, end);
      if (!nextTermExpr) {
         error("! Compiler::parseTermPreMinus> Can\'t read term", _beg);
         return 0;
      }
      termExpr = SPTR_MAKE(ExprPreMinus)(nextTermExpr);

      _beg = skip_spaces(_beg, end);
   }

   if (termExpr)
      return termExpr;

   termExpr = parseTermNot(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermPreMinus> Can\'t read term", _beg);
      return 0;
   }
   return termExpr;
}

ExprPtr Compiler::parseTermNot(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermNot> End of file", _beg);
      return 0;
   }
   _beg = skip_spaces(_beg, end);

   ExprPtr termExpr = NULL;

   while (
            (_beg + 1 <= end) &&
            (
               _st[_beg] == '!' && _st[_beg+1] != '='
            )            
         )
   {
      _beg++;
      ExprPtr nextTermExpr = parseTermNot(code, end);
      if (!nextTermExpr) {
         error("! Compiler::parseTermNot> Can\'t read term", _beg);
         return 0;
      }
      termExpr = SPTR_MAKE(ExprNot)(nextTermExpr);

      _beg = skip_spaces(_beg, end);
   }

   if (termExpr)
      return termExpr;

   termExpr = parseTermUndef(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermNot> Can\'t read term", _beg);
      return 0;
   }
   return termExpr;
}

ExprPtr Compiler::parseTermUndef(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermUndef> End of file", _beg);
      return 0;
   }
   _beg = skip_spaces(_beg, end);

   ExprPtr termExpr = NULL;

   while (
      (_beg <= end) &&
      (
         _st[_beg] == '?'
         )
      )
   {
      _beg++;
      ExprPtr nextTermExpr = parseTermUndef(code, end);
      if (!nextTermExpr) {
         error("! Compiler::parseTermUndef> Can\'t read term", _beg);
         return 0;
      }
      termExpr = SPTR_MAKE(ExprUndef)(nextTermExpr);

      _beg = skip_spaces(_beg, end);
   }

   if (termExpr)
      return termExpr;

   termExpr = parseTermPostAccessValueIncrDecr(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermNot> Can\'t read term", _beg);
      return 0;
   }
   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessValueIncrDecr(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermPostAccessValueIncrDecr> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermPostAccessPlusMinusDotDivMod(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermPostAccessValueIncrDecr> Can\'t read term", _beg);
      return 0;
   }

   size_t pos = skip_spaces_not_new_lines(_beg, end);

   while (
      (pos + 1 <= end) &&
      (
         (_st[pos] == '+' && _st[pos + 1] == '+') ||
         (_st[pos] == '-' && _st[pos + 1] == '-')
         ) &&
      termExpr->kind() == Expr::_VAR //инкремент и дикремент можно выполнять только над переменными
      )
   {
      _beg = pos;
      ExprVarPtr var = SPTR_DCAST(ExprVar,termExpr);
      if ((_beg + 1) < end && _st[_beg] == '+' && _st[_beg + 1] == '+')          // Var++
      {
         //тут инкремент объекта
         _beg += 2;
         termExpr = SPTR_MAKE(ExprVarIncr)(var->variable());
      }
      else if ((_beg + 1) < end && _st[_beg] == '-' && _st[_beg + 1] == '-')     // Var--
      {
         //тут декримент объекта
         _beg += 2;
         termExpr = SPTR_MAKE(ExprVarDecr)(var->variable());
      }
      pos = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessPlusMinusDotDivMod(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::PostAccessPlusMinusDotDivMod> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermPostAccessValue(code, end);
   if (!termExpr) {
      error("! Compiler::PostAccessPlusMinusDotDivMod> Can\'t read term", _beg);
      return 0;
   }

   size_t pos = skip_spaces_not_new_lines(_beg, end);

   while (
      (pos + 1 <= end) &&
      (
      (_st[pos] == '+'  && _st[pos + 1] == '=') ||
      (_st[pos] == '-'  && _st[pos + 1] == '=') ||
      (_st[pos] == '*'  && _st[pos + 1] == '=') ||
      (_st[pos] == '/'  && _st[pos + 1] == '=') ||
      (_st[pos] == '\\' && _st[pos + 1] == '=') ||
      (_st[pos] == '%'  && _st[pos + 1] == '=')
      ) &&
      termExpr->kind() == Expr::_VAR //данные операции можно выполнять только над переменными
      )
   {
      _beg = pos;
      ExprVarPtr var = SPTR_DCAST(ExprVar,termExpr);
      if ((_beg + 1) < end && _st[_beg] == '+' && _st[_beg + 1] == '=')          
      {
         //тут +=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after +=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarPlusAssign)(var->variable(), argExpr);
      }
      else if ((_beg + 1) < end && _st[_beg] == '-' && _st[_beg + 1] == '=')    
      {
         //тут -=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after -=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarMinusAssign)(var->variable(), argExpr);
      }
      else if ((_beg + 1) < end && _st[_beg] == '*' && _st[_beg + 1] == '=')
      {
         //тут -=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after *=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarDotAssign)(var->variable(), argExpr);
      }
      else if ((_beg + 1) < end && _st[_beg] == '/' && _st[_beg + 1] == '=')
      {
         //тут -=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after /=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarDivFloatAssign)(var->variable(), argExpr);
      }
      else if ((_beg + 1) < end && _st[_beg] == '\\' && _st[_beg + 1] == '=')
      {
         //тут -=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after \\=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarDivIntAssign)(var->variable(), argExpr);
      }
      else if ((_beg + 1) < end && _st[_beg] == '%' && _st[_beg + 1] == '=')
      {
         //тут -=
         _beg += 2;
         ExprPtr argExpr = parse_EXPR(code, end);
         if (argExpr == 0) {
            error("! Compiler::PostAccessPlusMinusDotDivMod> can't parse expr after %=", _beg);
            return 0;
         }
         termExpr = SPTR_MAKE(ExprVarModAssign)(var->variable(), argExpr);
      }
      pos = skip_spaces(_beg, end);
   }
   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessValue(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermPostAccessValueField> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = parseTermValue(code, end);
   if (!termExpr) {
      error("! Compiler::parseTermPostAccessValueField> Can\'t read term", _beg);
      return 0;
   }

   //запрещено вставлять пробел между переменной и '[' так как будет проблема
   //при парсингк a in [b,c]
   //size_t pos = skip_spaces_not_new_lines(_beg, end);
   size_t pos = _beg;
   while (
      (pos + 1 <= end) &&
      (
         (_st[pos] == '[') || (_st[pos] == '.') ||
         (_st[pos] == '=' && _st[pos+1] == '>')
      )
      )
   {
      _beg = pos;
      if (_st[_beg] == '[')
      {
         _beg++;
         ExprPtr nextTermExpr = parseTermPostAccessValueElement(code, end, termExpr);
         if (!nextTermExpr) {
            error("! Compiler::parseTermPostAccessValue> Can\'t read term after [", _beg);
            return 0;
         }
         termExpr = nextTermExpr;
         pos = skip_spaces_not_new_lines(_beg, end);
         continue;
      }
      if (_st[_beg] == '.')
      {
         _beg++;
         ExprPtr nextTermExpr = parseTermPostAccessValueMethod(code, end, termExpr);
         if (!nextTermExpr) {
            nextTermExpr = parseTermPostAccessValueField(code, end, termExpr);
            if (!nextTermExpr)
            {
               error("! Compiler::parseTermPostAccessValue> Can\'t read term after .", _beg);
               return 0;
            }
         }
         termExpr = nextTermExpr;
         pos = skip_spaces_not_new_lines(_beg, end);
         continue;
      }
      if (_st[_beg] == '=' && _st[_beg+1] == '>')
      {
         _beg+=2;
         ExprPtr nextTermExpr = parseTermPostAccessValueField(code, end, termExpr);
         if (!nextTermExpr) {
            error("! Compiler::parseTermPostAccessValue> Can\'t read term after =>", _beg);
            return 0;
         }
         termExpr = nextTermExpr;
         pos = skip_spaces_not_new_lines(_beg, end);
         continue;
      }
   }

   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessValueElement(CodePtr code, size_t end, ExprPtr leftExpr)
{
   ExprPtr indexExpr = parse_EXPR(code, end);
   if (!indexExpr) {
      error("! Compiler::parseTermPostAccessValueElement> Error in var[...]", _beg);
      return 0;
   }
   _beg = skip_spaces(_beg, end);
   if (_beg >= end || _st[_beg] != ']') {
      error("! Compiler::parseTermPostAccessValueElement> No close bracket in var[...]", _beg);
      return 0;
   }
   _beg++;
   ExprPtr termExpr = SPTR_MAKE(ExprValElement)(leftExpr, indexExpr);
   _beg = skip_spaces(_beg, end);

   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessValueMethod(CodePtr code, size_t end, ExprPtr leftExpr)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermPostAccessValueMethod> End of file", _beg);
      return 0;
   }

   ExprPtr termExpr = NULL;
   size_t pos = _beg;
   pos = get_name(_name, "_@$#", pos, end);
   pos = skip_spaces(pos, end);
   if (pos < end && _st[pos] == '(' && _name.size()>0) {  // это метод объекта типа X.size()
      _beg = pos;
      ExprFunPtr fun = parse_FUN(code, end);
      if (!fun) {
         error("! Compiler::parseTermPostAccessValueMethod> Can\'t parse function X" + _name + "(...)", _beg);
         return 0;
      }
      fun->_obj = leftExpr;
      termExpr = fun;
   }
   else if (_name == "nodes")
   {
      //оператор добавления узлов в граф, который слева от точки
      _beg = pos;
      parse_nodes(code, end, leftExpr);
      termExpr = leftExpr;
   }

   return termExpr;
}

ExprPtr Compiler::parseTermPostAccessValueField(CodePtr code, size_t end, ExprPtr leftExpr)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermPostAccessValueField> End of file", _beg);
      return 0;
   }

   ExprPtr expr = parseTermValue(code, end);
   if (!expr)
   {
      error("! parseTermPostAccessValueField> Can't parse term after =>", _beg);
      return 0;
   }

   ExprPtr termExpr = SPTR_MAKE(ExprValueField)(leftExpr, expr);
   _beg = skip_spaces(_beg, end);

   return termExpr;
}

ExprPtr Compiler::parseTermValue(CodePtr code, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_beg >= end) {
      error("! Compiler::parseTermValue> End of file", _beg);
      return 0;
   }

   size_t pos = get_name(_name, "_$@#", _beg, end);
   ExprPtr val = NULL;

   if ((val = parseTermValueFunc(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueModuleFunc(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueLambdaExpression(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueExprInBrackets(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueConst(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueVariable(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueEdgeNameOrNodeName(code, pos, end)) || _num_errors)
      return val;
   else if ((val = parseTermValueNewVariable(code, pos, end)) || _num_errors)
      return val;

   return val;   
}

ExprPtr Compiler::parseTermValueFunc(CodePtr code, size_t pos, size_t end)
{
   if (_beg >= end)
      return NULL;

   if (_name.size()) {                              // есть имя 
      pos = skip_spaces(pos, end);
      if (_st[pos] == '(') {                        // и после него идёт скобка (
         _beg = pos;
         return parse_FUN(code, end);               // это функция
      }
   }
   return NULL;
}

ExprPtr Compiler::parseTermValueModuleFunc(CodePtr code, size_t pos, size_t end)
{
   Module* module = ModuleManager::instance()->getModule(_name);
   if (!module)
      return 0;   //это не модуль

   _beg = pos;
   if (_beg >= end || _st[_beg] != '.')   // после модуля должна всегда быть точка
   {
      error("! Compiler::parse_value> No '.' after module name ", _beg);
      return 0;
   }
   //теперь берем имя функции
   _beg = get_name(_name, "_$", _beg + 1, end);
   
   _beg = skip_spaces(_beg, end);
   if (_st[_beg] != '(')
   {
      //возможно это константа модуля
      Value moduleConstant;
      if (!module->getConstant(_name, moduleConstant))
      {
         error("! parseTermValueModuleFunc> constant with name " + _name +" is not registered", _beg);
         return 0;
      }
      auto constExpr = SPTR_MAKE(ExprConst)(moduleConstant);
      return constExpr;
   }

   //создаем объект функции если последняя присутствует в модуле
   ModuleFunctionPtr fun = module->getFunction(_name);
   if (!fun)
   {
      error("! Compiler::parse_value> Function " + _name + " is not registered", _beg);
      return 0;
   }
   auto funExpr = SPTR_MAKE(ExprFun)();
   funExpr->_fun = fun;
   VarArgType varArgType = VAR_ARG_NONE;
   if (fun->allowNewVarsInArgs())
   {
      varArgType = VAR_ARG_ALL;
      _allow_var_dupl = true;
   }
   // парсим аргументы функции
   if (!parse_FUN_ARGS(code, varArgType, funExpr->_args, _beg, end))
   {
      return 0;
   }
   if ((funExpr->_args.size() < fun->_minArgs) || (funExpr->_args.size() > fun->_maxArgs)) {
      stringstream str;
      str << "! parseTermValueModuleFunc> Incorrect number of arguments in function: " << fun->_name << " (expected ";
      if (fun->_minArgs == fun->_maxArgs)
      {
         str << fun->_minArgs;
      }
      else
      {
         str << "from " << fun->_minArgs << " to " << fun->_maxArgs;
      }
      str << ")";
      error(str.str(), _beg);
      return 0;
   }

   return funExpr;
}

ExprPtr Compiler::parseTermValueConst(CodePtr code, size_t pos, size_t end)
{
   ExprPtr valConst = NULL;
  
   if (valConst = parseTermValueConstArray(code, end))
      return valConst;
   else if (valConst = parseTermValueConstMap(code, end))
      return valConst;
   else if (valConst = parseTermValueConstFloat(code, end))
      return valConst;
   else if (valConst = parseTermValueConstString(code, end))
      return valConst;

   if (!_name.size()) {
      error("! Compiler::parseTermValueConst> No name of variable, node or edge", _beg);
      return 0;
   }
   if (_name[0] == '$') {                         //------------ это объект сознания:
      _beg = pos;
      valConst = SPTR_MAKE(ExprNodeConst)(_name, _script);
   }
   else if (_name[0] == '@') {                    //------------- это отношение:
      _beg = pos;
      Int id = GraphManager::instance().edge(_name);
      valConst = SPTR_MAKE(ExprConst)(Value(id));
   }
   else if (_name == "False") {
      _beg = pos;
      valConst = SPTR_MAKE(ExprConst)(Logic(1, 0));
   }
   else if (_name == "True") {
      _beg = pos;
      valConst = SPTR_MAKE(ExprConst)(Logic(0, 1));
   }
   else if (_name == "Undef") {
      _beg = pos;
      valConst = SPTR_MAKE(ExprConst)(Logic(0, 0));
   }
   else if (_name == "None") {
      _beg = pos;
      valConst = SPTR_MAKE(ExprConst)(ValueNone::ptr());
   }

   return valConst;
}

ExprPtr Compiler::parseTermValueLambdaExpression(CodePtr code, size_t pos, size_t end)
{
   if (_st[_beg] != '(')
      return 0;
   // дальше может идти обычное либо лямбда выражение (a>b) или (a,b)=>a>b
   pos = get_block('(', ')', _beg, end);
   if (pos >= end) {
      error("! Compiler::parseTermValueLambdaExpression> No close ')' in block ()", _beg);
      return 0;
   }
   pos++;
   pos = skip_spaces(pos, end);
   if (pos + 1 >= end || _st[pos] != '=' || _st[pos+1] != '>') {
      //если за скобками не идут символы '=>' то это не лямбда выражение
      return 0;
   }
   vector<string> args;
   _beg = get_function_head(_name, args, _beg, end);
   _beg = skip_spaces(_beg, end);
   _beg += 2; //пропускаем "=>"
   _beg = skip_spaces(_beg, end);

   auto lambdaVal = SPTR_MAKE(ValueLambda)(args, _script);

   if (!parse_function_body(lambdaVal->get_Lambda(), true, end))
   {
      return 0;
   }

   return SPTR_MAKE(ExprConst)(Value(lambdaVal));
}

ExprPtr Compiler::parseTermValueExprInBrackets(CodePtr code, size_t pos, size_t end)
{
   if (_st[_beg] == '(') {     //--------------------- скобки (EXPR):
      _beg++;                                     // пропускаем открывающую скобку
      ExprPtr ex = parse_EXPR(code, end);
      if (!ex) {
         error("! Compiler::parseTermValueExprInBrackets> Can\'t read term in brackets ()", _beg);
         return 0;
      }
      if (_st[_beg] == ',')
      {
         //дальше идет логическое значение типа (EXPR,EXPR)
         _beg++;
         ExprPtr exprP0 = ex;
         ExprPtr exprP1 = parse_EXPR(code, end);
         if (!exprP1) 
         {
            error("! Compiler::parseTermValueExprInBrackets> Can\'t parse second logic argument", _beg);
            return 0;
         }
         if (_st[_beg] != ')') 
         {
            ExprPtr exprP2 = NULL;
            ExprPtr exprP3 = NULL;
            if (_beg < end && _st[_beg] == ',')
            {
               _beg++;
               exprP2 = parse_EXPR(code, end);
               if (_st[_beg] == ',')
               {
                  _beg++;
                  exprP3 = parse_EXPR(code, end);
               }
            }
            if (_beg >= end || _st[_beg] != ')' || (!exprP2 && !exprP3))
            {
               error("! Compiler::parseTermValueExprInBrackets> No close bracket ')' after logic or fus exppression", _beg);
               return 0;
            }
            _beg++;
            if (!exprP3)
            {
               exprP3 = exprP2;
               exprP2 = exprP1;
            }
            ExprPtr ex = SPTR_MAKE(ExprFus)(exprP0, exprP1, exprP2, exprP3);
            return ex;
         }
         _beg++;
         ExprPtr ex = SPTR_MAKE(ExprLogic)(exprP0, exprP1);
         return ex;
      }

      if (_st[_beg] != ')') {
         error("! Compiler::parseTermValueExprInBrackets> No close bracket ')' after exppression", _beg);
         return 0;
      }
      _beg++;                                    // пропускаем закрывающую скобку
      return ex;
   }
   else return 0;
}

ExprPtr Compiler::parseTermValueVariable(CodePtr code, size_t pos, size_t end)
{
   if (_is_new_local_var && reservedName(_name))
   {
      error("! Compiler::parseTermValueVariable> Name \"" + _name + "\" is reserved ", _beg);
      return 0;
   }
   VariablePtr var = findVariable(code, _name, _is_new_local_var ? 1 : 0);
   if (!var) {
     return 0;
   }

   if ((_is_new_local_var || _is_new_static_var) && !_allow_var_dupl)
   {
      warning("Compiler::parseTermValueVariable Duplicate declaration of variable \"" + _name + "\"", _beg);
   }

   _beg = pos;
   return SPTR_MAKE(ExprVar)(var);
}

ExprPtr Compiler::parseTermValueEdgeNameOrNodeName(CodePtr code, size_t pos, size_t end)
{
   if (GraphManager::instance().isEdgeName(_name))
   {
      Int id = GraphManager::instance().edge(_name);
      _beg = pos;
      return SPTR_MAKE(ExprConst)(Value(id));
   }
   else if (GraphManager::instance().isNodeName(_name))
   {
      _beg = pos;
      return SPTR_MAKE(ExprNodeConst)(_name, _script);
   }
   return 0;
}

ExprPtr Compiler::parseTermValueNewVariable(CodePtr code, size_t pos, size_t end)
{
   if (_is_new_local_var) {
      VariablePtr var = code->add_local_variable(_name);
      _beg = pos;
      return SPTR_MAKE(ExprVar)(var);
   }
   else if (_is_new_static_var)
   {
      VariablePtr var = code->add_static_variable(_name);
      _beg = pos;
      return SPTR_MAKE(ExprVar)(var);
   }
   else
   {
      //упс. что-то пошло не так
      error("! Compiler::parseTermValueNewVariable> Name " + _name + " is not variable or edge or node!", _beg);
      return 0;
   }
}

ExprPtr Compiler::parseTermValueConstArray(CodePtr code, size_t end)
{
   if (_st[_beg] != '[')
      return 0;

   size_t pos = get_block('[', ']', _beg, end);
   if (pos >= end) {
      error("! Compiler::parseTermValueConstArray> No close bracket in array [ ... ]", _beg);
      return 0;
   }
   _beg++;                                        // пропускаем [
   ExprArrayPtr arr = SPTR_MAKE(ExprArray)();
   Float v;
   while (_beg < pos) {
      _beg = skip_spaces(_beg, pos);
      if (_beg >= pos || _st[_beg] == ']') {
         _beg++;                                  // пропускаем ]
         break;
      }
      if (isdigit(_st[_beg])) {
         _beg = get_float(v, _beg, pos);
         arr->_exprs.push_back(SPTR_MAKE(ExprConst)(v));
      }
      else {
         ExprPtr ex = parse_EXPR(code, pos);
         if (!ex) {
            error("! Compiler::parseTermValueConstArray> Can\'t parse expression in array", _beg);
            return 0;
         }
         arr->_exprs.push_back(ex);
      }
      _beg = skip_spaces(_beg, pos);
      if (_st[_beg] == ',')                        // пропускаем запятую
         _beg++;
   }
   if (_st[_beg] == ']')
      _beg++;                                     // пропускаем ]

   Int cnst = 1;
   for (UInt i = 0; i < arr->_exprs.size(); i++)
      if (arr->_exprs[i]->kind() != Expr::_CONST) {
         cnst = 0; break;
      }
   if (cnst) {                                   // все элементы массива константы
      vector<Value> lst; lst.reserve(arr->_exprs.size());
      for (UInt i = 0; i < arr->_exprs.size(); i++)
         lst.push_back((SPTR_DCAST(ExprConst,arr->_exprs[i]))->_const);
      return SPTR_MAKE(ExprConst)(lst);
   }
   return arr;
}

ExprPtr Compiler::parseTermValueConstMap(CodePtr code, size_t end)
{
   if (_st[_beg] != '{')
      return 0;

   size_t pos = get_block('{', '}', _beg, end);
   if (pos >= end) {
      error("! Compiler::parseTermValueConstMap> No close bracket in map { ... }", _beg);
      return 0;
   }
   _beg++;                                        // пропускаем {
   ExprMapPtr mapExpr = SPTR_MAKE(ExprMap)();
   while (_beg < pos) {
      _beg = skip_spaces(_beg, pos);
      if (_beg >= pos || _st[_beg] == '}') {
         _beg++;                                  // пропускаем }
         break;
      }
      ExprPtr keyExpr = NULL;
      keyExpr = parse_EXPR(code, pos);
      if (!keyExpr) {
         error("! Compiler::parseTermValueConstMap> Can\'t parse key expression in map", _beg);
         return 0;
      }
      _beg = skip_spaces(_beg, pos);
      //тут ждем ':' как разделитель между ключем и значением
      if (_beg >= end || _st[_beg] != ':') {
         error("! Compiler::parseTermValueConstMap> Can\'t find ':' between key and value", _beg);
         return 0;
      }
      _beg++;
      ExprPtr valExpr = NULL;
      valExpr = parse_EXPR(code, pos);
      if (!valExpr) {
         error("! Compiler::parseTermValueConstMap> Can\'t parse val expression in map", _beg);
         return 0;
      }
      mapExpr->_exprs.push_back(ExprMap::ExprMapPir(keyExpr,valExpr));
      _beg = skip_spaces(_beg, pos);
      if (_st[_beg] == ',')                        // пропускаем запятую
         _beg++;
   }
   if (_st[_beg] == '}')
      _beg++;                                     // пропускаем }

   return mapExpr;
}

ExprPtr Compiler::parseTermValueConstFloat(CodePtr code, size_t end)
{
   if (isdigit(_st[_beg]) || _st[_beg] == '-') {   //--------------------- вещественное число:
      Float v = 0;
      _beg = get_float(v, _beg, end);
      return SPTR_MAKE(ExprConst)(v);
   }
   else return 0;
}

ExprPtr Compiler::parseTermValueConstString(CodePtr code, size_t end)
{
   bool quotes       = _st[_beg] == '\'';
   bool doubleQuotes = _st[_beg] == '\"';
   if (!quotes && !doubleQuotes)
      return 0;

   _beg++;
   size_t pos = _beg;
   while (true)
   {
      pos = find(quotes?'\'':'\"', pos, end);
      if (pos >= end) {
         error("! Compiler::parseTermValueConstString> No close bracket for string", _beg);
         return 0;
      }
      if (_st[pos - 1] == '\\')
      {
         //ковычки экранированы, пропускаем
         pos++;
         continue;
      }
      break;
   }
   size_t strLen = pos - _beg;
   _name = _st.substr(_beg, pos - _beg);
   string newStr;
   //разэкранируем спец. символы '\\', '\"', '\n', '\t'
   size_t p = 0;
   while (p<strLen)
   {
      if (_name[p] != '\\')
      {
         p++;
         continue;
      }

      if (p + 1 < strLen)
      {
         if (_name[p + 1] == '\\')
         {
            //экранирование слеша
            _name.replace(p, 2, "\\");
            p++;
            strLen--;
         }
         else if (_name[p + 1] == '"')
         {
            //экранирование двойных ковычек
            _name.replace(p, 2, "\"");
            p++;
            strLen--;
         }
         else if (_name[p + 1] == '\'')
         {
            //экранирование одинарных ковычек
            _name.replace(p, 2, "\'");
            p++;
            strLen--;
         }
         else if (_name[p + 1] == 'n')
         {
            //экранирование перевода на новую строку
            _name.replace(p, 2, "\n");
            p++;
            strLen--;
         }
         else if (_name[p + 1] == 't')
         {
            //экранирование табуляции
            _name.replace(p, 2, "\t");
            p++;
            strLen--;
         }
         else p++;
      }
   }
   _beg = pos + 1;
   return SPTR_MAKE(ExprConst)(_name);
}


//=======================================================================================
// Парсим аргументы функции
//
bool Compiler::parse_FUN_ARGS(CodePtr code, VarArgType varArgType, vector<ExprPtr> &args, size_t &beg, size_t end)
{
   _beg = skip_spaces(_beg, end);
   if (_st[_beg] != '(')
   {
      error("! parse_FUN_ARGS> No open bracket ( after function call", _beg);
      return false;
   }
   _beg++;                                  // пропускаем (
   while (_beg < end) {                     // читаем аргументы
      _beg = skip_spaces(_beg, end);
      if (_st[_beg] == ')')                 // конец списка аргументов
         break;
      VarArgType saveVarArgType = _varArgType;
      _varArgType = _varArgType == VAR_ARG_ALL ? _varArgType : varArgType; // у ALL приоритет над вложенными вызовами
      _is_new_local_var = (_varArgType == VAR_ARG_ALL) || (_varArgType == VAR_ARG_FIRST_ONLY && args.size() == 0);
      ExprPtr expr = parse_EXPR(code, end);
      if (!expr) {
         error("! Compiler::parse_FUN_ARGS> Can\'t function arg", _beg);
         return false;
      }
      _varArgType = saveVarArgType;
      args.push_back(expr);
      _beg = skip_spaces(_beg, end);
      if (_beg < end && _st[_beg] == ',')
         _beg++;
   }
   if (_beg >= end || _st[_beg] != ')') {
      error("! parse_FUN_ARGS> No close bracket ) after function call", _beg);
      return false;
   }
   _beg++;
   return true;
}

VariablePtr Compiler::findVariable(CodePtr code, const string & name, int local /*== 0*/)
{
   VariablePtr var = code->find_variable(name, local);  // ищем в списке переменных блока
   if (!var &&
      (
         code->is_global_variable_permitted(name) ||
         _script->_code->is_global_variable_permitted(name)
         )
      )    // ищем в списке разрешенных глобальных переменных (из команд global)
   {
      var = _script->_code->get_variable(name);
   }
   return var;
}

Int Compiler::error(const string & err, size_t pos)
{
   if (_num_errors == 0)
   {
      //решили показывать только первую ошибку
      size_t c = 1, r = line(pos, &c);                  // номер строки и колонки, где ошибка
      TRACE_FILE_POS(L_CRITICAL, (int)_sourceID, r, c) << err << endl;
      _num_errors++;
   }
   return 0;
}

void Compiler::warning(const string & err, size_t pos)
{   
   size_t c = 1, r = line(pos, &c);                  // номер строки и колонки, где ошибка
   TRACE_FILE_POS(L_WARNING, (int)_sourceID, r, c) << err << endl;
   _num_warnings++;
}

//=======================================================================================
// Парсим функцию
//
ExprFunPtr Compiler::parse_FUN(CodePtr code, size_t end)
{
   /*
      тут может быть три сценария вызова функции
      1. name()    - вызов функции/демона с именем name
      2. varName() - вызов лямбда функции 
   */
   ExprFunPtr funcExpr   = SPTR_MAKE(ExprFun)();
   bool       funcFound  = false;

   //ищем напрямую функцию по имени
   funcExpr->_fun = _script->get_function(_name);
   if (funcExpr->_fun) {
      funcFound = true;
   }

   if (!funcFound)
   {
      //парсим имя как переменую
      VariablePtr var = findVariable(code, _name);
      if (var)
      {
         funcExpr->_obj = SPTR_MAKE(ExprVar)(var);
         funcFound = true;
      }
   }
   if (!funcFound) {
      error("! parse_FUN> Unknown function name: " + _name, _beg);
      return 0;
   }
   VarArgType varArgType = ((_name == "exists") || (_name == "forall")) ? VAR_ARG_FIRST_ONLY : VAR_ARG_NONE;

   if (!parse_FUN_ARGS(code, varArgType, funcExpr->_args, _beg, end))
   {
      return 0;
   }

   if (funcExpr->_fun && (
         (funcExpr->_args.size() < funcExpr->_fun->_minArgs) || (funcExpr->_args.size() > funcExpr->_fun->_maxArgs)
      )
      ) {
      stringstream str;
      str << "! parse_FUN> Incorrect number of arguments in function: " << funcExpr->_fun->_name << " (expected ";
      if (funcExpr->_fun->_minArgs == funcExpr->_fun->_maxArgs)
      {
         str << funcExpr->_fun->_minArgs;
      }
      else
      {
         str << "from " << funcExpr->_fun->_minArgs << " to " << funcExpr->_fun->_maxArgs;
      }
      str << ")";
      error(str.str(), _beg);
      return 0;
   }
   return funcExpr;
}

//=======================================================================================
