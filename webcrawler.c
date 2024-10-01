#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

void fetch_html (const char *hostname)
{
	//socket, address, and buffer var declaration
	int sock;
	struct sockaddr_in server_addr;
	char buffer[BUFFER_SIZE];
	struct hostent *he;
	
	//create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket creation failed!");
		return;
	}
	
	//server address config
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
		
	if((he = gethostbyname(hostname)) == NULL)
	{
		perror("Couldnt resolve hostname");
		close(sock);
		return;
	}
	
	//memory copy
	memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
	
	//connect server 
	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0 )
	{
		perror("Connection failed!");
		close(sock);
		return;
	}
	
	printf("Connected to %s \n", hostname);
	
	const char *request_template = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
	char request[BUFFER_SIZE];
	snprintf(request, BUFFER_SIZE, request_template, hostname);
	
	//send request to server
	send(sock, request, strlen(request), 0);
	
	int bytes_received = read(sock, buffer, BUFFER_SIZE -1);
	if (bytes_received < 0)
	{
		perror("Read error");
		close(sock);
		return;
	}
	
	buffer[bytes_received] = '\0';		//null terminate the buffer
	
	//print received html in console
	printf("\nReceived HTML: \n%s\n ", buffer);
	
	//close socket to clean up
	close(sock);
}

int main()
{
	char hostname[256];  

	printf("Enter the hostname (e.g., localhost or example.com): ");
	scanf("%255s", hostname);

	printf("\nFetching HTML for: %s\n", hostname);
	fetch_html(hostname);

	return 0;
}
