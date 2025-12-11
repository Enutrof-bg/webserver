#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netinet/in.h>

int main()
{
	printf("test");
	//create socker
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	//specifying socket
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//binding socket
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	//listen to socket
	listen(serverSocket, 5);

	//accpeting connection request
	int clientSocket = accept(serverSocket, NULL, NULL);

	//receive data
	char buffer[1024] = {0};
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message from client: " << buffer << std::endl;

	//close socket
	close(serverSocket);
}