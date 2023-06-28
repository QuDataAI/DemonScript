#include "CPPJSBridge.h"

#ifdef VS_COMPILE_EMSCRIPTEN_PROJ
#else
#include "emscripten.h"
#endif

int EMSCRIPTEN_KEEPALIVE main(int argc, char **argv)
{
#ifdef VS_COMPILE_EMSCRIPTEN_PROJ
   CPPJSBridge::instance().onMessage("{\"kind\":\"start\", \"file\":\"test.ds\"}");
   CPPJSBridge::instance().onMessage("{\"kind\":\"start\", \"file\":\"test.ds\"}");
   CPPJSBridge::instance().onMessage("{\"kind\":\"start\", \"file\":\"test.ds\"}");
#endif

   return 0;
}