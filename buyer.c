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

#define SEPARATOR " "
#define PARS_NUM 4
#define NAME_LOC 0
#define CONTEXT_LOC 1
#define PORT_LOC 2
#define CONDITION_LOC 3

struct Advertising{
    char name[1024];
    char context[1024];
    int port;
    char condition[1024];
    int sock_fd;
};

struct Ad{
    char name[1024];
    char context[1024];
    int port;
    char condition[1024];
    int client_fd;
    int server_fd;
};

char* convert_int_to_string(int number){
    int m = number;
    int digit = 0;
    while (m) {
        digit++;
        m /= 10;
    }
    char* arr;
    char arr1[digit];
    arr = (char*)malloc(digit);
    int index = 0;
    while (number) {
        arr1[++index] = number % 10 + '0';
        number /= 10;
    }
    int i;
    for (i = 0; i < index; i++) {
        arr[i] = arr1[index - i];
    }
    arr[i] = '\0';
    return (char*)arr;
}

void printListOfAdvertisings(struct Advertising advertisings[1024]) {
    for(int i=0;i<1024;i++) {
        if (strcmp(advertisings[i].name, "empty") != 0) {
            printf("%s     %s     %d     %s\n", advertisings[i].name, advertisings[i].context,advertisings[i].port ,advertisings[i].condition);
        }
    }
}

void printMenuOfCommands(){
    printf("---\nPlease choose your command\n");
    printf("---list_of_ad\n");
    printf("---parchase_ad\n");
}
void alarm_handler(int sig) {
    printf("Seller didn't respond.\n");
}
int connectServer(int port) {
    int fd;
    struct sockaddr_in server_address;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }
    return fd;
}

int main(int argc, char const *argv[]) {
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    char name[1024] = {0};
    char command[1024] = {0};
    struct sockaddr_in bc_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(atoi(argv[1])); 
    bc_address.sin_addr.s_addr = inet_addr("192.168.1.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    printf("\n*************Wellcome*************\n");
    printf("---Please enter your name\n");
    char nameTemp[1024] = {0};
    char contextTemp[1024] = {0};
    char conditionTemp[1024] = {0};
    int temp1[10] = {0};
    int temp2[10] = {0};
    int temp3[10] = {0};
    for(int i = 0; i < 1024; i++) {
        temp1[i] = temp2[i];
        temp2[i] = temp3[i];
        temp3[i] = temp1[i];
    }
    struct Ad ads[1024];
    int portTemp = 0;
    int client_fdTemp = 0;
    int server_fdTemp = 0;
    for(int i = 0; i < 1024; i++) {
        strcpy(ads[i].name, nameTemp);
        strcpy(ads[i].context, contextTemp);
        strcpy(ads[i].condition, conditionTemp);
        ads[i].port = portTemp;
        ads[i].client_fd = client_fdTemp;
        ads[i].server_fd = server_fdTemp;
    }
    memset(name, 0, 1024);
    read(0, name, 1024);
    name[strcspn(name, "\n")] = 0;
    fd_set master_set;
    struct Advertising advertisings[1024];
    for(int k=0;k<1024;k++){
        strcpy(advertisings[k].name, "empty");
    }
    int numOfAdvertising = 0;
    printMenuOfCommands();
    while (1) {
        FD_ZERO(&master_set);
        FD_SET(sock, &master_set);
        FD_SET(0, &master_set);
        memset(buffer, 0, 1024);
        select(FD_SETSIZE, &master_set, NULL, NULL, NULL);

        for(int i=0;i<FD_SETSIZE;i++){
            if (FD_ISSET(i, &master_set)) {
                if (i == sock){  
                    printf("\n---new list of advertisings\n");
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    char nameTemp[1024];
                    char contextTemp[1024];
                    int portTemp;
                    char conditionTemp[1024];
                    int check = 0;
                    char* separate_input = strtok(buffer, SEPARATOR);
                    for(int j=0; j<PARS_NUM; j++){
                        if(j==NAME_LOC){ strcpy(nameTemp, separate_input); }
                        else if(j==CONTEXT_LOC){ strcpy(contextTemp, separate_input); }
                        else if(j==PORT_LOC){ portTemp = atoi(separate_input); }
                        else if(j==CONDITION_LOC){ strcpy(conditionTemp, separate_input); }
                        separate_input = strtok(NULL, SEPARATOR);
                    }
                    for(int j=0; j < 1024; j++){
                        if (portTemp == advertisings[j].port){
                            strcpy(advertisings[j].name, nameTemp);
                            strcpy(advertisings[j].context, contextTemp);
                            advertisings[j].port = portTemp;
                            strcpy(advertisings[j].condition, conditionTemp);
                            advertisings[j].sock_fd = -1;
                            numOfAdvertising--;
                            check = 1;
                        }
                    }
                    if (check == 0){
                        strcpy(advertisings[numOfAdvertising].name, nameTemp);
                        strcpy(advertisings[numOfAdvertising].context, contextTemp);
                        advertisings[numOfAdvertising].port = portTemp;
                        strcpy(advertisings[numOfAdvertising].condition, conditionTemp);
                        advertisings[numOfAdvertising].sock_fd = -1;
                    }
                    numOfAdvertising++;
                    printListOfAdvertisings(advertisings);
                    memset(buffer, 0, 1024);
                }
                else if(i==0){
                    read(0, buffer, 1024);
                    buffer[strcspn(buffer, "\n")] = 0;
                    char *req = strtok(buffer, SEPARATOR);
                    if(strcmp(req,"list_of_ad")==0){
                        printListOfAdvertisings(advertisings);
                    }
                    else if(strcmp(req,"parchase_ad")==0){
                        printf("\n---give the context of the ad and the price of the offer\n");
                        read(0, command, 1024);
                        char* contextTemp = strtok(command, SEPARATOR);
                        int price = atoll(strtok(NULL,SEPARATOR));
                        char *strPrice = convert_int_to_string(price);
                        for(int j=0; j < 1024; j++){
                            if(strcmp(advertisings[j].context, contextTemp) == 0){
                                char response[1024]={0};
                                int temp1[10] = {0};
                                int temp2[10] = {0};
                                int temp3[10] = {0};
                                for(int i = 0; i < 1024; i++) {
                                    temp1[i] = temp2[i];
                                    temp2[i] = temp3[i];
                                    temp3[i] = temp1[i];
                                }
                                int fd;
                                if(advertisings[j].sock_fd==-1)
                                    fd=connectServer(advertisings[j].port);
                                else
                                   fd=advertisings[j].sock_fd;
                                advertisings[j].sock_fd=fd;
                                strcat(contextTemp, SEPARATOR);
                                strcat(contextTemp, strPrice);
                                send(fd, contextTemp, strlen(contextTemp), 0);
                                // signal(SIGALRM, alarm_handler);
                                // siginterrupt(SIGALRM, 1);
                                // alarm(60);
                                recv(fd, response, 1024, 0);
                                // alarm(0);
                                if (strcmp(response,"accept")==0)
                                    printf("The seller accepted the offer\n");
                                else
                                    printf("The seller rejected the offer\n");
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
