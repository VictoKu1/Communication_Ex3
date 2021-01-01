#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 1000

int main(){

	//Socket Configuration
	struct sockaddr_in serverAddr, clientAddr;
	int sockfd, nsockfd;
	char buff[BUFFSIZE] ;
	int PORT = 5060;
	char* IP = "127.0.0.1";

	FILE *filePointer; 

	//Create Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("%s\n", "Error creating Client socket.");
		return 0;
	}

	//create connection with server
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);
	memset(&(serverAddr.sin_zero), '\0', 0);

	if(connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) < 0){
		printf("%s\n", "Error creating connection.");
		return 0;
	}

	printf("%s\n","Connected with Server." );


	//Change the CC algorithm
	int len = sizeof(buff);

    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("Current CC Algorithm: %s\n", buff);

	for(int k =0; k<2; k++){
		int byteswritten = 0;
		for(int i=0 ; i < 5; i++){
			int lll = 0;
			//sending file
			filePointer = fopen("test.txt", "rb");
			if(filePointer == NULL){
				printf("%s\n", "File not found.");
			}else{
				printf("%s%d\n", "sending file ", i+1);
				while(fread ( buff, 1, sizeof(buff), filePointer )) { 
		           // Print the dataToBeRead  

					byteswritten = send (sockfd, &buff, BUFFSIZE, 0);
					//lll += byteswritten;
					//for(int i=0; i<strlen(buff); i++){
						//byteswritten = send (sockfd, &buff[i], 1, 0);
					//}
		        } 
			}

			//printf("%s%d\n", "Bytes: ", lll);
			char buf[BUFFSIZE] = "$";
			byteswritten = send (sockfd, buf, BUFFSIZE, 0);
			fclose(filePointer);
		}

		//Change CC algorithm
	    strcpy(buff, "reno");
	    len = strlen(buff);
	    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, len) != 0)
	    {
	        perror("setsockopt");
	        return -1;
	    }

	    len = sizeof(buff);
	    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0)
	    {
	        perror("getsockopt");
	        return -1;
	    }

	    if(k == 0){
	    	printf("CC Algorithm Changed to: %s\n", buff);
	    }
	}
	
	close(sockfd);
	return 0;
}
