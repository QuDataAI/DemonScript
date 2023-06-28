#include "StackFrame.h"
#include "Sys/Debugger/Debugger.h"
#include "Function.h"
#include "Code.h"
#include "Command.h"

StackFrame*    StackFrame::_currentStackFrame = NULL;

StackFrame::StackFrame(Code &code):
   _code(&code),
   _type(STACK_FRAME_TYPE_CODE)
{
   _parent = _currentStackFrame;
   _currentStackFrame = this;
   if (_parent)
      _func = _parent->_func;
}

StackFrame::StackFrame(FunDemon &func) :
   _func(&func),
   _type(STACK_FRAME_TYPE_DEMON_FUNC)
{
   _parent = _currentStackFrame;
   _currentStackFrame = this;
   if (_func->_numCallsInStack > 0)
   {
      //текущая функция уже вызвана выше по стеку, поэтому 
      //поднимаемя вверх до первого блока кода, принадлежащего данной функции
      StackFrame* iterStackFrame = _parent;
      while (iterStackFrame && iterStackFrame->_func != _func)
         iterStackFrame = iterStackFrame->_parent;

      //и рекурсивно сохраняем переменные найденного блока и его родителей
      if (iterStackFrame && iterStackFrame->_code)
      {
         CodePtr codePtr = iterStackFrame->_code->SPTR_FROM_THIS;
         _savedVars = _func->pushVars(codePtr);
      }      
   }
   _func->_numCallsInStack++;
}

StackFrame::~StackFrame()
{
   _currentStackFrame = _parent;
   if (_type == STACK_FRAME_TYPE_DEMON_FUNC)
   {
      _func->_numCallsInStack--;
      if (_savedVars)
      {
         _func->popVars(_savedVars);
      }
   }      
}

bool StackFrame::step(UInt command)
{
   _command = command;
#ifdef USE_DEBUGGER
   if (Debugger::instance().debugging())
   {
      return Debugger::instance().onStep();
   }
#endif
   return true;
}

UInt StackFrame::currentLine(UInt stackFrameOffset/* = 0*/)
{
   if (!_currentStackFrame || !_currentStackFrame->_code)
      return 0;
  
   StackFrame* currentStackFrame = _currentStackFrame;

   while (stackFrameOffset != 0 && currentStackFrame->_parent)
   {
      if (currentStackFrame->_parent->_type == STACK_FRAME_TYPE_CODE)
         stackFrameOffset--;
      currentStackFrame = currentStackFrame->_parent;
   }
      

   return currentStackFrame->_code->_commands[currentStackFrame->_command]->line();
}

UInt StackFrame::currentSourceID()
{
   if (!_currentStackFrame || !_currentStackFrame->_code)
      return 0;

   return _currentStackFrame->_code->_commands[_currentStackFrame->_command]->sourceID();
}
