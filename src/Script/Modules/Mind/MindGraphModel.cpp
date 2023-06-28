#include "MindGraphModel.h"

MindGraphModel::MindGraphModel():
   _p(1.0),
   _valKeyP(0),
   _valKeyN(0),
   _valKeyGraph(0),
   _valKeyLog(0)
{

}

MindGraphModel::MindGraphModel(const Value & gVal, Float p, UInt n):
   _graphValue(gVal),
   _p(p),
   _n(n),
   _valKeyP(0),
   _valKeyN(0),
   _valKeyGraph(0),
   _valKeyLog(0),
   _log(vector<Value>())
{   

}

Value MindGraphModel::get_Value()
{
   if (!_valMap)
   {
      _valMap      = SPTR_MAKE(ValueMap)();
      _valKeyP     = ValueMapKey(SPTR_MAKE(ValueStr)("p"));
      _valKeyN     = ValueMapKey(SPTR_MAKE(ValueStr)("n"));
      _valKeyGraph = ValueMapKey(SPTR_MAKE(ValueStr)("graph"));
      _valKeyLog   = ValueMapKey(SPTR_MAKE(ValueStr)("log"));
   }
      

   _valMap->_val[_valKeyP]      = Value(_p).ptr();
   _valMap->_val[_valKeyN]      = Value(Float(_n)).ptr();
   _valMap->_val[_valKeyGraph]  = _graphValue.ptr();
   _valMap->_val[_valKeyLog]    = _log.ptr();

   return _valMap;
}

MindGraphModelPtr MindGraphModel::copy()
{
   return SPTR_MAKE(MindGraphModel)(_graphValue.copy(),_p,_n);
}

void MindGraphModel::pushLog(const string & str)
{
   _log.push(Value(str));
}
