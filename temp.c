#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>

struct Advertising{
    char context[1024];
    int port;
};

int main(){
    printf("size = %d\n",FD_SETSIZE);
    return 0;
}


    while (1) {
        ///////////////
        memset(buffer, 0, 1024);
        recv(sock, buffer, 1024, 0);
        char nameTemp[1024];
        char contextTemp[1024];
        int portTemp;
        char conditionTemp[1024];
        char* separate_input = strtok(buffer, SEPARATOR);
        for(int i=0; i<PARS_NUM; i++){
            if(i==NAME_LOC){ strcpy(nameTemp, separate_input); }
            else if(i==CONTEXT_LOC){ strcpy(contextTemp, separate_input); }
            else if(i==PORT_LOC){ portTemp = atoi(separate_input); }
            else if(i==CONDITION_LOC){ strcpy(conditionTemp, separate_input); }
            separate_input = strtok(NULL, SEPARATOR);
        }
        int check = 0;
                    for(int i=0; i < 1024; i++){
                        if (portTemp == advertisings[i].port){
                            strcpy(advertisings[i].name, nameTemp);
                            strcpy(advertisings[i].context, contextTemp);
                            advertisings[i].port = portTemp;
                            strcpy(advertisings[i].condition, conditionTemp);
                            numOfAdvertising--;
                            check = 1;
                        }
                    }
                    if (check == 0){
                        strcpy(advertisings[numOfAdvertising].name, nameTemp);
                        strcpy(advertisings[numOfAdvertising].context, contextTemp);
                        advertisings[numOfAdvertising].port = portTemp;
                        strcpy(advertisings[numOfAdvertising].condition, conditionTemp);
                    }
                    numOfAdvertising++;
        //////////////

        printListOfAdvertisings(advertisings);


    }