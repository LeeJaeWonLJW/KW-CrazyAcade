/*******************************************************
 * 2019 KW Jaewon, Kwangho, Hyojun
 * 
 * This file is part of Kwangwoon advanced C programming subject.
 *******************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
 
#define BUF_SIZE 100
#define NORMAL_SIZE 20
 
void* user_action(void* arg);
void* recv_action(void* arg);
void error_handling(char* msg);
 
void menu();
 
char name[NORMAL_SIZE]="[DEFALT]";
char msg_form[NORMAL_SIZE];
char serv_time[NORMAL_SIZE];
char msg[BUF_SIZE];
char serv_port[NORMAL_SIZE];
char clnt_ip[NORMAL_SIZE];
 
 
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;
 
    if (argc!=4)
    {
        printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }
 
    struct tm *t;   // now time
    time_t timer = time(NULL);
    t=localtime(&timer);
    sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour,
    t->tm_min);
 
    sprintf(name, "[%s]", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" conncet() error");
 
    menu(); //print menu
 
    pthread_create(&snd_thread, NULL, user_action, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_action, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* user_action(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    char myInfo[BUF_SIZE];
    char* who = NULL;
    char temp[BUF_SIZE];
 
    /** send join messge **/
    printf(" >> READY <<\n");
    sprintf(myInfo, "%s IP_%s\n",name , clnt_ip);
    write(sock, myInfo, strlen(myInfo));
 
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
 
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
 
        // send message
        sprintf(name_msg, "%s %s", name,msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
 
void* recv_action(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    int str_len;
 
    while(1)
    {
        str_len=read(sock, name_msg, NORMAL_SIZE+BUF_SIZE-1);
        if (str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        fputs(name_msg, stdout);
    }
    return NULL;
}
 
void menu()
{
    printf("=== GAME CLIENT ===\n");
    printf("port - %s \n", serv_port);
    printf("IP - %s \n", clnt_ip);
    printf("nickname - %s \n", name);
    printf("%s \n", serv_time);
}    
 
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
