/*!            StackFrame - кадр стека, хранящий указатель на текущую команду, 
а также значение переменных если кадр является не текущим

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
****************************************************************************************/

#ifndef StackFrameH
#define StackFrameH

#include "OTypes.h"          // Определение базовых типов
#include "Value.h"
#include "Function.h"
#include <stack>

using namespace std;

class FunDemon;
class Script;

SPTR_DEF(Code);

class StackFrame
{
public:
   enum StackFrameType {
      STACK_FRAME_TYPE_CODE,        //!< кадр содержит код из набора команд
      STACK_FRAME_TYPE_DEMON_FUNC   //!< кадр содержит точку входа в функцию
   };
   StackFrame(Code&     code);      //!< добавляем тип кадра STACK_FRAME_TYPE_CODE
   StackFrame(FunDemon& func);      //!< добавляем тип кадра STACK_FRAME_TYPE_DEMON_FUNC
   ~StackFrame();
   /*!
   Изменяется указатель текущей команды
   \param command порядковый номер команды в текущем коде
   \return разрешено ли выполнять текущую команду (для прерывания кода отладчиком)
   */
   bool step(UInt command);
   /*!
   Указатель на текущий кадр стека кода   
   */
   static StackFrame*   currentStackFrame() { return _currentStackFrame; }
   /*!
   Текущий номер строки
   \param stackFrameOffset смещение относительно текущего кадра стека
   */
   static UInt   currentLine(UInt stackFrameOffset = 0);
   /*!
   Текущий идентификатор исходников
   */
   static UInt   currentSourceID();
private:
   static StackFrame*         _currentStackFrame;        //!< текущий кадр стека
   StackFrameType             _type;                     //!< тип кадра
   StackFrame*                _parent     = NULL;        //!< родительский кадр стека
   Code*                      _code       = NULL;        //!< текущий код
   FunDemon *                 _func       = NULL;        //!< текущая функция
   FunDemon::VarsSavePtr      _savedVars  = NULL;        //!< сохраненные переменные 
   UInt                       _command    = 0;           //!< порядковый номер команды в текущем коде
   friend class Debugger;
   friend class FunDemon;
   friend class FunEval;
};

//=======================================================================================
#endif
