#include "TCPServer.h"


using namespace std;

TCPServer::TCPServer():
   _port(80),
   _maximumClients(0),
   _listener(0),
   _clientCount(0)
{
}

TCPServer::~TCPServer()
{
}


bool TCPServer::init(UInt16 port, int maximumClients, TCPServerListener * listener)
{
   _port           = port;
   _maximumClients = maximumClients;
   _listener       = listener;
   return true;
}



