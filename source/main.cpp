#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include "minwinbase.h"

//#pragma comment (lib,"ws2_32.lib")

using namespace std;

int main()
{
    cout << "Hello World!" << endl;
    //Intialize winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2,2);

    int wsok = WSAStartup(ver, &wsData);
    if(wsok !=0)
    {
        cerr << "Can't Initialize winsock! Quit"<<endl;
        return 1;
    }


    //Create winsock
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! quit"<<endl;
        return 1;
    }

    //Bind the IP adress and port to socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    
    //Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
    
    //wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    //if(clientSocket == INVALID_SOCKET) //
    
    char host[NI_MAXHOST];      //client's remote name
    char service[NI_MAXHOST];   // Service (i/e port) the client is connrct on

    ZeroMemory(host,NI_MAXHOST);    // same as memset(host,0,NI_MAXHOST)
    ZeroMemory(service, NI_MAXHOST);

    if (getnameinfo((sockaddr*)&client, sizeof(client), 
    host, NI_MAXHOST,service, NI_MAXSERV,0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port" <<
            ntohs(client.sin_port) << endl;
    }    
 

    //Close listening socket
    closesocket(listening);


    //while loop: accept and echo message back to client
    char buf[4096];

    int messageCount = 0;
    while (true)
    {
        ZeroMemory(buf, 4096);

        // wait for client to send data
        int byteRecived = recv(clientSocket, buf, 4096, 0);
        if(byteRecived == SOCKET_ERROR)
        {
            cerr <<" Error in recv(). Quiting" << endl;
            break;
        }

        if(byteRecived == 0)
        {
            cout << " client disconnected " << endl;
           break; 
        }

        // Echo message back to client
        if(!messageCount)
        {
            const char* str = "Init;0;;\n";
            send(clientSocket, str, sizeof(str), 0);
        }
        messageCount++;
        send(clientSocket, buf, byteRecived + 1, 0);      
        cout << "\n \n in buffer: "<< buf << endl;        
    }    

    cout << "\nTHE END"<< endl;    
    //Close the sock
    closesocket(clientSocket);

    //Clean winsok
    WSACleanup();

    
    system("pause");
    return 0;
}
