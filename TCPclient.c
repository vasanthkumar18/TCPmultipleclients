#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <arpa/inet.h>


#define PORT 8080 



void *receive(void *newsockfd)
{
	int sockfd=*((int *)newsockfd);
	int msglen;
	while(1)
	{
		char msg[2048];
		int read=recv(sockfd,msg,2048,0);
		if(read==0)
		{
			printf("SERVER's CONNECTION IS CLOSED\n");
			exit(1);
		}
		msg[read]='\0';
		puts(msg);
	}
}
 

int main(int argc, char *argv[]) 
{ 
	int sockfd; 
	struct sockaddr_in servaddr; // For Internet address
	
	
	
	
	char clientname[128];
	strcpy(clientname,argv[1]);
	

		// socket create and verification 
			sockfd = socket(AF_INET, SOCK_STREAM, 0); 
			if (sockfd == -1) { 
				printf("CREATION OF SOCKET'S FAILED\n"); 
				exit(0); 
			} 
			else
				printf("SOCKET IS CREATED\n"); 
			
			bzero(&servaddr, sizeof(servaddr)); 



		// assign IP, PORT 
			servaddr.sin_family = AF_INET; 
			servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
			servaddr.sin_port = htons(PORT); 

	
	
		
		// connect the client socket to server socket 
			if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
			{ 
				printf("CONNECTION WITH SERVER IS FAILED\n"); 
				exit(0); 
			} 
			else
				printf("CONNECTED TO SERVER\n"); 
				
		// To broadcast that you came to the chat system
		send(sockfd,clientname,128,0);
		
		
		// creating a client thread : which will always wait for a message
			pthread_t thread;
			pthread_create(&thread,NULL,receive,(void *)&sockfd);
		
		
		// sending messages to SERVER from this client
			printf("1.To display other users: DISP\n2.To chat with other users: CHAT\n3.To exit from the system: tata\n");
			while(1)
			{
				char buff[1024];
				scanf("%s",buff);
				
				if(strcmp(buff,"tata")==0)
				{
					send(sockfd,buff,1024,0);
					printf("(YOUR) %s's CONNECTION IS CLOSED\n",clientname);
					break;
				}
				if(strcmp(buff,"DISP")==0)
				{
					send(sockfd,buff,1024,0);
				}
				if(strcmp(buff,"CHAT")==0)
				{
					send(sockfd,buff,1024,0);

					scanf("%[^\n]s",buff);
					send(sockfd,buff,1024,0);
				}
					
				
			}
			
		// close our sockfd
		
		close(sockfd);
		exit(1);
		return 0;
		
} 

