#include "TCPServerWin.h"
#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <iostream>
#include <WinSock2.h>
#include <winsock.h>

shared_ptr<TCPServer> TCPServer::create()
{
   return shared_ptr<TCPServer>(new TCPServerWin());
}

bool TCPServerWin::start()
{
   if (WSAStartup(MAKEWORD(2, 2), &_wsaData) == 0)
   {
      printf("WSA Startup succes\n");
   }
   SOCKADDR_IN addr;
   int addrl = sizeof(addr);
   addr.sin_addr.S_un.S_addr = INADDR_ANY;
   addr.sin_port = htons(_port);
   addr.sin_family = AF_INET;
   _socket = socket(AF_INET, SOCK_STREAM, NULL);
   if (_socket == SOCKET_ERROR) {
      printf("Socket not created\n");
   }

   if (bind(_socket, (struct sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR) {
      printf("Socket succed binded\n");
   }

   if (listen(_socket, SOMAXCONN) != SOCKET_ERROR) {
      printf("Start listening at port %u\n", ntohs(addr.sin_port));
   }
   handle();
   return true;
}

void TCPServerWin::disconnect(UInt clientID)
{
   if (clientID >= _clients.size() || !_clients[clientID])
      return;

   _clients[clientID] = 0;
   _clientCount--;
   if (_listener)
      _listener->onClientDisconnect((int)clientID);
}

size_t TCPServerWin::sendToClient(UInt clientID, const char * buf, size_t len)
{
   if (clientID >= _clients.size() || !_clients[clientID])
      return -1;

   TCPServerClientPtr client = _clients[clientID];

   const char *data_ptr = buf;
   int bytes_sent = 0;
   int data_size = (int)len;
   int total_bytes_sent = 0;

   while (data_size > 0)
   {
      bytes_sent = send(client->_socket, data_ptr, data_size, 0);
      if (bytes_sent == SOCKET_ERROR)
         return total_bytes_sent;

      if (bytes_sent == 0)
         return total_bytes_sent;

      data_ptr += bytes_sent;
      data_size -= bytes_sent;
      total_bytes_sent += bytes_sent;
   }

   return total_bytes_sent;
}

Int TCPServerWin::receiveFromClient(UInt clientID, char * buf, size_t len)
{
   if (clientID >= _clients.size() || !_clients[clientID])
      return -1;

   TCPServerClientPtr client = _clients[clientID];

   char *data_ptr = (char*)buf;
   int  bytes_recv = 0;
   int  data_size = (int)len;
   int  total_bytes_received = 0;
   while (data_size > 0)
   {
      bytes_recv = recv(client->_socket, data_ptr, data_size, 0);
      if (bytes_recv == SOCKET_ERROR)
         return total_bytes_received;

      if (bytes_recv == 0)
         return total_bytes_received;

      data_ptr += bytes_recv;
      data_size -= bytes_recv;
      total_bytes_received += bytes_recv;
   }

   return total_bytes_received;
}

bool TCPServerWin::connected(UInt clientID)
{
   if (clientID >= _clients.size())
      return false;

   return true;
}


TCPServerWin::TCPServerClient::TCPServerClient(SOCKET socket, SOCKADDR_IN addr) :
   _socket(socket),
   _addr(addr)
{

}

TCPServerWin::TCPServerClient::~TCPServerClient()
{
   closesocket(_socket);
}

void TCPServerWin::handle()
{
   while (true)
   {
      SOCKET acceptS;
      SOCKADDR_IN addr_c;
      int addrlen = sizeof(addr_c);
      if ((acceptS = accept(_socket, (struct sockaddr*)&addr_c, &addrlen)) != 0) {
         printf("Client connected from 0  %u.%u.%u.%u:%u\n",
            (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b1,
            (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b2,
            (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b3,
            (unsigned char)addr_c.sin_addr.S_un.S_un_b.s_b4,
            ntohs(addr_c.sin_port));

         //-------------------------
         // Set the socket I/O mode: In this case FIONBIO
         // enables or disables the blocking mode for the 
         // socket based on the numerical value of iMode.
         // If iMode = 0, blocking is enabled; 
         // If iMode != 0, non-blocking mode is enabled.
         u_long iMode = 1;
         ioctlsocket(acceptS, FIONBIO, &iMode);
         registerClient(acceptS, addr_c);
         if (_maximumClients <= _clientCount)
         {
            //все клиенты подключены, завершаем прием соединений
            break;
         }
      }
      Sleep(50);
   }
}

void TCPServerWin::registerClient(SOCKET socket, SOCKADDR_IN addr)
{
   TCPServerClientPtr newClient = SPTR_MAKE(TCPServerClient)(socket, addr);
   Int                 clientID = (Int)_clients.size();
   _clientCount++;
   _clients.push_back(newClient);
   if (_listener)
   {
      _listener->onClientConnect(clientID);
   }
}