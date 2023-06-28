/*!               Компилятор текста скрипта процедурных знаний

 приоритет операций позаимствован из с++ https://ru.cppreference.com/w/cpp/language/operator_precedence
 и для скрипта имеет следующий вид:
   
   Assign:                          = 	           прямое присваивание
   Multiary:                        ?:::          мультиарное условие
   Equality:                        <->           эквивалентность
   Impl:                            ->            импликация
   Or:                              | 	           логическое ИЛИ
   And:                             & 	           логическое И
   EqNotEq:                         ==  !=        операции сравнения = и ≠
   Edge                             X E Y         ребро графа
   Comp:                            <   <=        операции сравнения < и ≤
                                    >   >=        операции сравнения > и ≥
   PlusMinus:                       +             сложение
                                    -             вычитание
   DotDivMod:                       *             умножение
                                    /             вещественное деление
                                    \             целочисленное деление
                                    % 	           остаток
   Undef:                           ?             проверка на неопределенность аргумента справа
   Not:                             !             логическое НЕ 
   PreMinus:                        -             преотрицание
   PostAccessIncrDecr               a++           постинкремент
                                    a--           постдекремент
   PostAccessPlusMinusDotDivMod     a+=           сложение и присваивание
                                    a-=           вычитание и присваивание
                                    a*=           умножение и присваивание
                                    a/=           вещественное деление и присваивание
                                    a\=           целочисленное деление и присваивание
                                    a%=           остаток и присваивание
   PostAccessValueElement a[]           обращение к элементу объекта по индексу
   PostAccessValueMethod  a.name()      обращение к методу объекта
   PostAccessValueField   a.            обращение к полю объекта
   Value:
           Func:          name() 	    вызов локальной функции                   
           ModuleFunc:    Module.name() вызов функции из модуля
           Const:         $a, @a        константа объект сознания, ребро графа
             Array:       []            константа содержащая массив
             Map:         {}            константа содержащая объект
             Float:       0.0           константа содержащая число
             String:      ""            константа содержащая строку
           ExprInBrackets:
                          (EXPR)        выражение в скобках
                          (EXPR,EXPR)   логическое значение (p0,p1)
           Variable:      a             переменная
                               
Соответственно при парсинге используются функции:
   parseTerm<имя группы><имя подгруппы и т.д ...>()

Простейшей командой скрипта является выражение EXPR, описывающееся следующей грамматикой:
   EXPR      :- Assign                                               
   Assign    :- Multiary = Multiary                       | Multiary   // прямое присваивание
   Multiary  :- Equality?Equality:Equality:Equality       | Equality   // мультиарное условие
   Equality  :- Impl <-> Impl                             | Impl       // эквивалентность
   Impl      :- Or   ->  Or                               | Or         // импликация
   Or        :- And      |   And                          | And        // логическое ИЛИ
   And       :- EqNotEq  &   EqNotEq                      | EqNotEq    // логическое И
   EqNotEq   :- Comp ==,!= Comp                           | Comp       // операции сравнения = и ≠
   Comp      :- PlusMinus <,<=,>,>=  PlusMinus            | PlusMinus  // операции сравнения <,<=,>,>=
   PlusMinus :- DotDivMod +,-  DotDivMod                  | DotDivMod  // сложение и вычитание
   DotDivMod :- Not *,/,\,% Not                           | Not        // умножение, деления и остаток
   Not       :- !PostFunc                                 | PostFunc   // отрицание
   PostAccess:- Value[] | Value. | Value++ | Value-- |    
                PostAccess.name                           | Value      // обращение к элементу объекта по индексу, к полю объекта, постинкремент, постдекремент
   Value     :- name()  | Module.name()                   | ValueConst // вызов локальной функции или вызов функции из модуля
   ValueConst: $a | @a | [] | 0.0 | "" | (EXPR)           | Variable   // константа объект сознания, ребро графа, массив число или строка
   Variable  :- a                                                      // доступ к переменной (создание в случае var)                                         

Программа являеся кодом CODE, который состоит из списка комманд:
   var    LST                                        // объявление переменных
   out    LST                                        // вывод значения выражений
   return EXPR                                       // остановка текущей функции
   return isdef EXPR                                 // остановка текущей функции
   if     EXPR : EXPR                                // условный оператор
   if     EXPR : {CODE}                              // условный оператор
   for    VAR in EXPR : EXPR                         // перебор списка
   for    VAR in EXPR : { CODE }                     // перебор списка
   def    NAME(LST_VAR) { CODE }                     // определение функции (демона)
   EXPR                                              // одиночное выражение

                             (с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/
#ifndef CompilerH
#define CompilerH
#include "OTypes.h"          // Определение базовых типов
#include "Parser.h"          // Функции парсинга строки
#include "Script/Script.h"   // Исполнитель скрипта
#include "Script/Expr.h"     // Выражения скрипта

#include <vector>
#include <string>
#include <map>
#include <iostream>
using namespace std;

SPTR_DEF(FunDemon);
//=======================================================================================
//! Компилятор текста скрипта
/*
Является наследником класса Parser, который занимается различными универсальными
действиями со стокой (пропуск пробелов, разбиение на слова и т.п.), см. Parser.h
*/
class Compiler: public Parser
{
public:
   /// Тип объявления команды "var" перед аргументами функции
   enum VarArgType {
      VAR_ARG_NONE,        //!< отсутствует команда "var"
      VAR_ARG_FIRST_ONLY,  //!< команда "var" присутствует только перед первым аргументом функции
      VAR_ARG_ALL          //!< команда "var" присутствует перед всеми аргументами функции
   };

   Compiler(ScriptPtr script);
   /*
   Парсинг всего скрипта  хранящегося в _st (см. класс Parser)
   \param sourceID идентификатор текущих исходников
   \return результат парсинга
   */
   Int  parse(size_t sourceID);

private:
   size_t           _sourceID;         //!< идентификатор текущих исходников
   size_t           _beg;              //!< текущая позиция в строке
   Bool             _is_new_local_var; //!< флаг активности команды объявления локальной переменной
   VarArgType       _varArgType;       //!< тип объявления команды "var" перед аргументами функции
   Bool             _is_new_static_var;//!< флаг активности команды объявления статической переменной
   Bool             _allow_var_dupl;   //!< разрешено ли дублирование объявления переменных
   string           _name;             //!< чтобы всё время память не выделять
   //! Парсинг num_commands комманд скрипта кода code до позиции end строки _st
   Int  parse(CodePtr code, size_t end, UInt num_commands=-1);

   Int parse_include(CodePtr code, size_t end);                        //!< оператор включения модулей
   Int parse_var    (CodePtr code, size_t end);                        //!< оператор объявления локальной переменной
   Int parse_static (CodePtr code, size_t end);                        //!< оператор объявления статической переменной
   Int parse_edges  (CodePtr code, size_t end);                        //!< оператор объявления связей
   Int parse_nodes  (CodePtr code, size_t end, ExprPtr graphExpr = 0);   //!< оператор объявления узлов
   Int parse_if     (CodePtr code, size_t end);                        //!< условный оператор 
   Int parse_while  (CodePtr code, size_t end);                        //!< оператор цикла while
   Int parse_for    (CodePtr code, size_t end);                        //!< оператор цикла for
   Int parse_out    (CodePtr code, size_t end, Int fout = 0);          //!< парсинг вывода на консоль
   Int parse_def    (CodePtr code, size_t end);                        //!< парсинг объявления функции
   Int parse_return (CodePtr code, size_t end);                        //!< парсинг возврата из функции
   Int parse_global (CodePtr code, size_t end);                        //!< парсинг добавления глобальной переменной в локальную область видимости
   Int parse_function_body(FunDemonPtr fun, bool lambda, size_t end);  //!< парсим тело функции
   /*!
      Парсим блок кода состоящий либо из одной команды после ':', 
      либо из нескольких команд между '{' и '}'
      \param code ссылка на код, в который будут добавлены команды
      \param end конечная позиция парсинга
      \return результат операции
   */
   Int parseCodeBlock(CodePtr code, size_t end); 

   /*!
   Тип функции парсинга термов
   */
   typedef ExprPtr (Compiler::*ParseTermFunction)(CodePtr code, size_t end);

   /*!
   Парсим выражение
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parse_EXPR (CodePtr code, size_t end);                     
   /*!
   Парсим прямое присваивание ("=")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermAssign(CodePtr code, size_t end);
   /*!
   Парсим мультиарное условие ("?:::")
   В тернарном      виде E?:A:B     - если E истина, то A, иначе B
   В кватернарном   виде E?:A:B:C   - если E истина, то A, если E ложь, то B, иначе С
   В квантиринарном виде E?:A:B:C:D - если E истина, то A, если E ложь, то B, если E не определено, то С, иначе D 
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermMultiary(CodePtr code, size_t end);
   /*!
   Парсим эквивалентность ("<->")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermEquality(CodePtr code, size_t end);
   /*!
   Парсим импликацию ("->")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermImpl(CodePtr code, size_t end);
   /*!
   Парсим логическое "ИЛИ" ("|")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermOr(CodePtr code, size_t end);
   /*!
   Парсим логическое "И" ("&")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermAnd(CodePtr code, size_t end);
   /*!
   Парсим операции cравнения "=" и "!=" 
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermEqNotEq(CodePtr code, size_t end);
   /*!
   Парсим обращение к ребру графа по схеме "X E Y"
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermEdge(CodePtr code, size_t end);
   /*!
   Парсим операции cравнения "<", "<=", ">", ">="
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermComp(CodePtr code, size_t end);
   /*!
   Парсим операции сложения и вычитания
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermPlusMinus(CodePtr code, size_t end);
   /*!
   Парсим операции умножения, деления и остатка
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermDotDivMod(CodePtr code, size_t end);
   /*!
   Парсим унарное отрицание ("-")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermPreMinus(CodePtr code, size_t end);
   /*!
   Парсим логическое нет ("!")
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermNot(CodePtr code, size_t end);
   /*!
   Парсим логическую неопределенность ("?") ?a <-> a == Undef
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermUndef(CodePtr code, size_t end);
   /*!
   Парсим постинкремент и постдекремент;
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermPostAccessValueIncrDecr(CodePtr code, size_t end);
   /*!
   Парсим "+=","-=","*=","/=","\=","%=";
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermPostAccessPlusMinusDotDivMod(CodePtr code, size_t end);
   /*!
   Парсим обращение к объекту;
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermPostAccessValue(CodePtr code, size_t end);
   /*!
   Парсим обращение к элементу объекта по индексу;
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \param leftExpr выражение вычисляющее значение слева от постоперации
   \return выражение
   */
   ExprPtr parseTermPostAccessValueElement(CodePtr code, size_t end, ExprPtr leftExpr);
   /*!
   Парсим обращение к методу объекта;
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \param leftExpr выражение вычисляющее значение слева от постоперации
   \return выражение
   */
   ExprPtr parseTermPostAccessValueMethod(CodePtr code, size_t end, ExprPtr leftExpr);
   /*!
   Парсим обращение к полю объекта;
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \param leftExpr выражение вычисляющее значение слева от постоперации
   \return выражение
   */
   ExprPtr parseTermPostAccessValueField(CodePtr code, size_t end, ExprPtr leftExpr);
   /*!
   Парсим значения
   [] 	 константа содержащая массив
   0.0 	 константа содержащая число
   ""     константа содержащая строку
   name() вызов функции
   var    переменная
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValue(CodePtr code, size_t end);
   /*!
   Парсим вызов функции
   \param code указатель на текущий код
   \param pos  позиция конца имени 
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueFunc(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим вызов функции из модуля (Math.round())
   \param code указатель на текущий код
   \param pos  позиция конца имени 
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueModuleFunc(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим константы:
   [] 	 константа содержащая массив
   0.0 	 константа содержащая число
   ""     константа содержащая строку
   \param code указатель на текущий код
   \param pos  позиция конца имени 
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueConst(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим лямбда выражение (arg1,arg2,...) => arg1 + arg2
   \param code указатель на текущий код
   \param pos  позиция конца имени
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueLambdaExpression(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим выражение в скобках (EXPR)
   \param code указатель на текущий код
   \param pos  позиция конца имени 
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueExprInBrackets(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим переменную
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueVariable(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим имя отношения либо узла
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueEdgeNameOrNodeName(CodePtr code, size_t pos, size_t end);
   /*!
   Добавляем переменную
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueNewVariable(CodePtr code, size_t pos, size_t end);
   /*!
   Парсим константу содержащую массив
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueConstArray(CodePtr code, size_t end);
   /*!
   Парсим константу содержащую мэп
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueConstMap(CodePtr code, size_t end);
   /*!
   Парсим константу содержащую вещественное число
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueConstFloat(CodePtr code, size_t end);
   /*!
   Парсим константу содержащую строку
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprPtr parseTermValueConstString(CodePtr code, size_t end);
   /*!
   Парсим функцию
   \param code указатель на текущий код
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   ExprFunPtr parse_FUN(CodePtr code, size_t end);
   /*!
   Парсим аргументы функции
   \param code указатель на текущий код
   \param varArgType тип объявления переменной аргумента
   \param args аргументы функции
   \param beg  начало аргументов функции
   \param end  позиция окончания буфера скрипта
   \return выражение
   */
   bool parse_FUN_ARGS(CodePtr code, VarArgType varArgType, vector<ExprPtr> &args, size_t &beg, size_t end);
   /*!
   Найти переменную по имени
   \param code указатель на текущий код
   \param name имя переменной
   \param local ищем переменную только в локальном блоке
   \return ссылка на переменную, если найдена
   */
   VariablePtr findVariable(CodePtr code, const string &name, int local = 0);
   //! Вывести ошибку компиляции
   Int error(const string & err, size_t pos);
   //! Вывести предупреждение компиляции
   void warning(const string & err, size_t pos);

   ScriptPtr _script;                           //!< исполнитель скрипта в который помещаются команды после компиляции
   friend class FunEval;
};
#endif
