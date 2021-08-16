#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>

#define MAX 4096
typedef struct msg
{
    char msg[MAX];
    int sender;
    int reciver;
}msg;

 int mainSocket; // main socket 
msg *arr = NULL; // arr of our struct
 int arrSize = 0; // save arr size
int checkDigit(char*); // check valid digit
void sigHandler(int signalNumber); // signal gender

int main(int argc, char *argv[])
{
    //init
    struct sockaddr_in server;
    int port;
    char buffer[MAX];
    signal(SIGINT,sigHandler);

    //check for valid argv
    if(argc < 2) // check if port given
    {
        printf("No port given\n");
        return EXIT_FAILURE;
    }

    if(checkDigit(argv[1]) < 0) // check for valid port
    {
        printf("Wrong port, please use only numbers\n");
        return EXIT_FAILURE;
    }

    port = atoi(argv[1]); // set up the port

    // init the server
    mainSocket = socket(AF_INET,SOCK_STREAM,0);
    if(mainSocket < 0)
    {
        perror("Opening socket went wrong\n");
        return EXIT_FAILURE;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(mainSocket,(struct sockaddr *)&server,sizeof(server)) < 0)
    {
        perror("Failure on binding\n");
        return EXIT_FAILURE;
    }
    listen(mainSocket,5);
    //finish initialize server

   //init for chat
    int newMsg = 0; // save the space that deleted from arr
    //msg arr[1000];// = NULL; // arr of messages
    fd_set rfd; //for the select funct
    fd_set copyw; // copy write
    fd_set copyr; // copy read
    int currSocket; // current socket to serve
    char whoWrite[15];

    //set fds
    FD_ZERO(&rfd);
    FD_ZERO(&copyr);
    FD_ZERO(&copyw);

    //printf("main socker is %d",mainSocket);
    FD_SET(mainSocket,&rfd);


 
    while(1)
    {
        copyr = rfd;
         if(newMsg==0)
             FD_ZERO(&copyw);
         else
             copyw = rfd;

        //printf("there are %d msg\n",newMsg);

        select(getdtablesize(),&copyr,&copyw,NULL,NULL);
        if(FD_ISSET(mainSocket,&copyr)) // check if som1 want to join to chat
        {
            printf("server is read to ready from socket <%d>\n",mainSocket);
            currSocket = accept(mainSocket,NULL,NULL);
            if(currSocket > 0)
            {
                FD_SET(currSocket,&rfd);
                //printf("Socket recivee\n");
            }
        }  

        for(int i = mainSocket+1; i<getdtablesize() ; i++)// run on sockets.
        {
            if(FD_ISSET(i,&copyr))
            {   //printf("in if because %d\n",i);  
                printf("server is ready to read from socket <%d>\n",i);
                bzero(buffer,MAX);
                int tmp;
                tmp = read(i,buffer,MAX);
                // printf("BUFFER = %s TMP = %d\n",buffer , tmp);
                
                if(tmp == 0)// CASE OF DC
                {
                    // printf("Guest left\n");
                    close(i);
                    FD_CLR(i,&rfd);
                    
                }
                else
                {

                    for(int j=mainSocket+1;j<getdtablesize();j++)
                    {
                        if(FD_ISSET(j,&rfd))
                        {   
                            if(i!=j)
                            {
                                // printf("sender is %d \n",i);
                                // printf("reciver is %d \n",j);
                            int index=arrSize;
                           // printf("Size of arr = %ld size of message = %ld\n",sizeof(arr),sizeof(msg));
                            arr=(msg*)realloc(arr,sizeof(msg)*(arrSize+1));
                             //printf("AFTER REALLOC Size of arr = %ld size of message = %ld\n",sizeof(arr),sizeof(msg));
                             arrSize++;
                             //printf("@@@@@@@@@@@@@@ ARR SIZE = %d\n",arrSize);
                             arr[index].sender=i;
                             memcpy( arr[index].msg, buffer, MAX);
                             arr[index].reciver=j;
                             newMsg++;
                            }
                        }
                    }
                }      
            }
        }

        for(int i = mainSocket+1; i<getdtablesize() ; i++)// run on sockets.
        {
            if( FD_ISSET(i,&copyw) )
            {
                for(int j=0;j<arrSize;j++)
                {
                    if(arr[j].reciver==i)
                    {   //printf("j is %d i is %d\n",j,i);
                        bzero(whoWrite,15);
                        printf("server is ready to write to socket <%d>\n",i);
                        sprintf(whoWrite,"guest%d: ",arr[j].sender);
                        write (i,whoWrite,sizeof(whoWrite));
                        write(i,arr[j].msg,MAX);
                        
                        
                        arr[j].reciver=-1;
                        newMsg--;
                    }    
                }
            } 
        }        
    }   
    
}

void sigHandler(int signalNumber)
{
    if(arr != NULL)
    {
       free(arr);
    }
    close(mainSocket);
    exit(0);
}

int checkDigit(char* port)
{
    int i;
	for (i = 0; i < strlen(port); i++)
		if (port[i] < '0' || port[i] > '9')  
			return -1;
	return 0;
}