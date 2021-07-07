#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <arpa/inet.h>



#define PORT 8080 

pthread_mutex_t lock;
pthread_t close_t;



struct client{

	int index,sockfd,len,alive;
	struct sockaddr_in clientaddr;
	char client_name[1024];
};

struct client clients[1024];
pthread_t thread[1024];
int client_count=0;


// for ERRORs
void error (const char *buff)
{
    perror(buff);
    exit(1);
}


// close the server
void *endserver(void* sockfd)
{
  	char buff[1024];
 	while(1)
 	{
		scanf("%s",buff);
		if(strcmp(buff,"tata") == 0)
	  	break;
  	}
	int newsockfd = *((int *) sockfd);
	close(newsockfd);
	printf("SERVER IS DISCONNECTED\n");
	exit(1);
}


// THE CHAT SYSTEM
void *chat( void *aboutcli)
{
	// Implementing details about client
		struct client* aboutclient = (struct client*) aboutcli; 
		int index = aboutclient->index;
		int client_socket = aboutclient->sockfd;
		aboutclient->alive = 1;
		char name[128];
	
	recv(client_socket,name,128, 0);
	strcpy(aboutclient->client_name,name);


	// Broadcast that Client joined the Chat
		char joinmsg[2048];
		snprintf(joinmsg, 2048, "%s IS CONNECTED TO US.\n", name);
		printf("%s", joinmsg);		
		for (int i = 0; i < client_count; i++)
		{
			if (i != index && clients[i].alive != 0)
				send(clients[i].sockfd, joinmsg, 2048, 0);
		}

	
	// ACTUAL CHAT SYSTEM
	while(1)
	{

		char buff[1024];
		char msg[2048];
		int read = recv(client_socket,buff,1024,0);
		buff[read] = '\0';
		char output[1024];

		if(strcmp(buff,"DISP") == 0)
		{
			pthread_mutex_lock(&lock); //lock the current thread
			int l = 0;

			for(int i = 0;i<client_count;i++)
			{
				if(clients[i].alive == 0)	continue;
			 
			 	else if(i!=index)
					l += snprintf(output + l,1024,"%s is on socket %d.\n",clients[i].client_name,clients[i].sockfd);

				else
					l += snprintf(output + l,1024,"(You)%s are on socket %d.\n",name,client_socket);

			}

			send(client_socket,output,1024,0);
			pthread_mutex_unlock(&lock); //unlock the thread
			continue;

		}
		if(strcmp(buff,"CHAT") == 0)
		{
			
			pthread_mutex_lock(&lock); // lock the current thread
			bzero(buff, 1024); // clear the buffer

			read = recv(client_socket,buff,1024,0);
			buff[read] = '\0';
			
			snprintf(msg, 2048, "%s: %s\n", name, buff);

			for (int i = 0; i < client_count; i++)
			{
					if (i != index && clients[i].alive != 0)
							send(clients[i].sockfd, msg, 2048, 0);
			}

			pthread_mutex_unlock(&lock); //unlock the thread
		}
		
		if(strcmp(buff, "tata") == 0 || read == 0) // Broadcast that client left
		{
			pthread_mutex_lock(&lock); //lock the thread
			
			clients[index].alive = 0;
			printf("%s HAS LEFT THE CHAT.\n", name);
			
			snprintf(buff, 1024, "%s has left the room.\n", name);		
			for (int i = 0; i < client_count; i++)
			{
					if (i != index && clients[i].alive != 0)
							send(clients[i].sockfd, buff, 1024, 0);
			}
			pthread_mutex_unlock(&lock); // unlock the thread
			
			bzero(buff, 1024);
			int *temp;
			
			pthread_exit(temp); //terminate the corresponding thread
		}
	}
	
	fprintf(stderr,"Here somehow");
	return NULL;

}




// Driver function
int main() 
{ 
	int sockfd; 
	struct sockaddr_in servaddr; //Internet address
	

		// socket create and verification 
			sockfd = socket(AF_INET, SOCK_STREAM, 0); 
			if (sockfd == -1) 
			{ 
				printf("SOCKET CREATION FAILED \n"); 
				exit(0); 
			} 
			else
				printf("SOCKET IS CREATED\n"); 
	
			bzero(&servaddr, sizeof(servaddr)); 

		
		
		// assign IP, PORT 
			servaddr.sin_family = AF_INET; 
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
			servaddr.sin_port = htons(PORT); 

		
		
		
		// Binding newly created socket to given IP and verification 
			if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) 
			{ 
				printf("SOCKET BINDING IS FAILED\n"); 
				exit(0); 
			} 
			else
			printf("SOCKET IS BINDED: SERVER STARTED\n"); 
	
		
	
		
		// Now server is ready to listen and verify 
			if ((listen(sockfd, 1024)) != 0) 
			{ 
				printf("LISTENING IS FAILED\n"); 
			exit(0); 
			} 
			else
			printf("SERVER IS LISTENING\n"); 
			


		
		// Checking if the server is to be closed 
			pthread_create(&close_t,NULL,endserver,(void *) &sockfd);
		
		
		// Accept the data packet from client and verify 
		while(1)
		{
			//accept
			clients[client_count].sockfd=accept(sockfd,(struct sockaddr*) &clients[client_count].clientaddr,&clients[client_count].len);
			clients[client_count].index=client_count;
			
			
			// create a thread for a connection
			pthread_create(&thread[client_count],NULL,chat,(void *) &clients[client_count]);
			client_count=client_count+1;
			
		}
		
		return 0;
			
}
			

		

