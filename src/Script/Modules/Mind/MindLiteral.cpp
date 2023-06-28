#include "MindLiteral.h"

MindLiteral::MindLiteral(MindAtomWPtr atom, bool not_):
   _atom(atom),
   _not(not_)
{
}

string MindLiteral::to_str(Graph &g, bool not_, bool ignoreVal)
{
   return _atom.lock()->to_str(g, not_ ? (!_not) : _not, ignoreVal);
}
