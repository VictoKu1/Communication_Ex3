#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFSIZE 1000

int main(){
	time_t startTime, endTime;
	

	//Socket Configuration
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t addr_size;
	int sockfd, nsockfd;
	char buff[BUFFSIZE];
	int PORT = 5060;
	char* IP = "127.0.0.1";

	//Create Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("%s\n", "Error creating Server socket.");
		return 0;
	}


	//Bind Socket with port
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(serverAddr.sin_zero), '\0', 0);

	if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) < 0){
		printf("%s\n", "Error Binding socket");
		return 0;
	}

	//Listen for incoming connections

	if(listen(sockfd, 5) < 0){
		printf("%s\n", "Listen Failed.");
		return 0;
	}

	printf("%s\n", "Waiting for connection...");

	addr_size = sizeof(struct sockaddr);
	//Accept incoming connections on new socket
	nsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addr_size);

	// Read from the client socket
	int bytesRead = 0;
	
	int timesBeforeCC[5] = {0};
	int len = 0;

	for(int k =0; k<2; k++){
		startTime = time(NULL);
		int i =0;
		do{
			memset(buff, '\0', sizeof(buff));
			bytesRead = recv(nsockfd, buff, 1, 0);

			if(strcmp(buff, "$") == 0){
				printf("%d%s\n\n", i+1, " Files Recieved..");
				endTime = time(NULL);
				timesBeforeCC[i] = (int)endTime - (int)startTime;
				startTime = time(NULL);
				i++;
			}
			//printf("%s", buff);
		}while(bytesRead > 0);


		int sum =0;
		for(int j = 0; j<5; j++){
			sum += timesBeforeCC[j];
			//printf("%s%d%d\n", "Time ", j+1, timesBeforeCC[j]);
		}

		if(k==0){
			len = sizeof(buff);
		    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0)
		    {
		        perror("getsockopt");
		        return -1;
		    }

		    printf("For CC Algorithm: %s\n", buff);
		    printf("%s%f%s\n\n","Average File sending time: ", (float)sum/5, " seconds" );

		    //Change the CC algoritm
		    strcpy(buff, "reno");
		    len = strlen(buff);
		    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, len) != 0)
		    {
		        perror("setsockopt");
		        return -1;
		    }
		}
		else{
			len = sizeof(buff);
		    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0)
		    {
		        perror("getsockopt");
		        return -1;
		    }

		    printf("For CC Algorithm: %s\n", buff);
		    printf("%s%f%s\n\n","Average File sending time: ", (float)sum/5, " seconds" );
		}
	}
	

	close(sockfd);
	close(nsockfd);
	return 0;
}
