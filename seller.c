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
#include <time.h>
#include <math.h>

#define SEPARATOR " "

struct Advertising{
    char name[1024];
    char context[1024];
    int port;
    char condition[1024];
    int client_fd;
    int server_fd;
};

struct Info{
    char context[1024];
    char price[1024];
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

int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 4);
    return server_fd;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
    return client_fd;
}

void printMenuOfCommands(){
    printf("\n---Please choose your command\n");
    printf("---publish_ad\n");
    printf("---answer_req\n");
}

int main(int argc, char const *argv[]) {
    //////////////////////////////////////////
    int sock, broadcast = 1, opt = 1;
    char name[1024];
    char command[1024] = {0};
    struct sockaddr_in bc_address;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(atoi(argv[1])); 
    bc_address.sin_addr.s_addr = inet_addr("192.168.1.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    srand(time(0));
    printf("\n*************Wellcome*************\n");
    printf("---Please enter your name\n");
    memset(name, 0, 1024);
    read(0, name, 1024);
    char nameTemp[1024] = {0};
    char contextTemp[1024] = {0};
    char conditionTemp[1024] = {0};
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
    name[strcspn(name, "\n")] = 0;
    struct Info informations[1024];
    fd_set master_set,working_set;
    FD_ZERO(&master_set);
    FD_SET(0, &master_set);
    int numOfAdvertising = 0;
    int numOfOffer = 0;
    struct Advertising advertisings[1024];
    printMenuOfCommands();

    while (1){
        working_set=master_set;
        memset(command, 0, 1024);
        select(1024, &working_set, NULL, NULL, NULL);

        for(int i = 0; i < 1024; i++) {
            int check = 0;
            for(int j = 0; j < 1024; j++) {
                if(advertisings[j].server_fd == i)
                    check = 1;
            }
            if (FD_ISSET(i, &working_set)) {
                if(i==0){
                    read(0, command, 1024);
                    command[strcspn(command, "\n")] = 0;
                    char *req = strtok(command, " ");
                    if(strcmp(req,"publish_ad") == 0){
                        printf("\n---Please enter your context of advertising\n");
                        char nameTemp[1024] = {0};
                        char contextTemp[1024] = {0};
                        char conditionTemp[1024] = {0};
                        memset(contextTemp, 0, 1024);
                        read(0, contextTemp, 1024);
                        contextTemp[strcspn(contextTemp, "\n")] = 0;
                        int randomNum = (rand() % (9090)) + 1;
                        int portTemp = randomNum;
                        int sock_fd = setupServer(portTemp);
                        strcpy(nameTemp, name);
                        strcpy(conditionTemp, "pending_purchase");
                        char *strPort = convert_int_to_string(portTemp);
                        FD_SET(sock_fd, &master_set);
                        strcpy(advertisings[numOfAdvertising].name, nameTemp);
                        strcpy(advertisings[numOfAdvertising].context, contextTemp);
                        strcpy(advertisings[numOfAdvertising].condition, conditionTemp);
                        advertisings[numOfAdvertising].port = portTemp;
                        advertisings[numOfAdvertising].server_fd = sock_fd;
                        numOfAdvertising++;
                        strcat(nameTemp, SEPARATOR);
                        strcat(nameTemp, contextTemp);
                        strcat(nameTemp, SEPARATOR);
                        strcat(nameTemp, strPort);
                        strcat(nameTemp, SEPARATOR);
                        strcat(nameTemp, conditionTemp);
                        printf("---the advertising was published successfully\n\n");
                        sendto(sock, nameTemp, strlen(nameTemp), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                    }
                    else if(strcmp(req,"answer_req") == 0){
                        printf("\n---Select the desired ad and enter the negotiation result\n");
                        read(0, command, 1024);
                        command[strcspn(command, "\n")] = 0;
                        char* contextTemp = strtok(command, " ");
                        char* response = strtok(NULL, " ");
                        struct Advertising ad;
                        for(int j = 0; j < 1024; j++) {
                            if (strcmp(advertisings[j].context, contextTemp) == 0){
                                send(advertisings[j].client_fd, response, strlen(response), 0);
                                char *strPort = convert_int_to_string(advertisings[j].port);
                                if (strcmp(response,"accept") == 0){
                                    strcpy(advertisings[j].condition, "expired");  
                                    char information[1024];
                                    int file_fd;
                                    file_fd = open("file.txt", O_APPEND | O_RDWR);
                                    strcpy(information, advertisings[j].context);
                                    strcat(information, SEPARATOR);
                                    char priceTemp[1024];
                                    for(int k = 0; k < 1024; k++) {
                                        if(strcmp(informations[k].context, advertisings[j].context) == 0){
                                            strcpy(priceTemp, informations[k].price);
                                            strcpy(informations[k].context, " ");
                                        }
                                    }
                                    strcat(information, priceTemp);
                                    write(file_fd, information, strlen(information));
                                    close(file_fd);
                                }
                                else
                                    strcpy(advertisings[j].condition, "pending_purchase");
                                char nameTemp[1024] = {0};
                                strcpy(nameTemp, advertisings[j].name);
                                strcat(nameTemp, SEPARATOR);
                                strcat(nameTemp, advertisings[j].context);
                                strcat(nameTemp, SEPARATOR);
                                strcat(nameTemp, strPort);
                                strcat(nameTemp, SEPARATOR);
                                strcat(nameTemp, advertisings[j].condition);
                                sendto(sock, nameTemp, strlen(nameTemp), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                            }
                        }
                    }
                }
                else if(check == 1){
                    int new_socket = acceptClient(i);
                    if(new_socket!=-1)
                        FD_SET(new_socket, &master_set);
                    for(int j = 0; j < 1024; j++) {
                        if(advertisings[j].server_fd == i) {
                            advertisings[j].client_fd = new_socket;
                            char *strPort = convert_int_to_string(advertisings[j].port);
                            strcpy(advertisings[j].condition, "underـnegotiation");
                            char nameTemp[1024] = {0};
                            strcpy(nameTemp, advertisings[j].name);
                            strcat(nameTemp, SEPARATOR);
                            strcat(nameTemp, advertisings[j].context);
                            strcat(nameTemp, SEPARATOR);
                            strcat(nameTemp, strPort);
                            strcat(nameTemp, SEPARATOR);
                            strcat(nameTemp, advertisings[j].condition);
                            sendto(sock, nameTemp, strlen(nameTemp), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
                        }
                    }
                    printf("\n---client %d connected to the server\n", new_socket);
                }
                else {
                    int bytes_received;
                    bytes_received = recv(i , command, 1024, 0);//////////////////

                    command[strcspn(command, "\n")] = 0;
                    char* contextTemp = strtok(command, " ");
                    char* priceTemp = strtok(NULL, " ");
                    strcpy(informations[numOfOffer].price, priceTemp);
                    strcpy(informations[numOfOffer].context, contextTemp);
                    numOfOffer++;
                    int temp1[10] = {0};
                    int temp2[10] = {0};
                    int temp3[10] = {0};
                    for(int i = 0; i < 1024; i++) {
                        temp1[i] = temp2[i];
                        temp2[i] = temp3[i];
                        temp3[i] = temp1[i];
                    }
                    if (bytes_received == 0) {
                        printf("client %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    printf("---The offer is for %s\n", command);
                }
            }
        }
    }
    return 0;
}