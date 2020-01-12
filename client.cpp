/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "24195" // the port client will be connecting to

#define MAXDATASIZE 5000 // max number of bytes we can get at once

#define ONCEDATASIZE 20 // max number of bytes we can get at once

#define IPADDRESS "127.0.0.1"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int sendall(int s, char *buf, int *len)
{
    int total = 0; // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    *len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}


int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char sendbuf[MAXDATASIZE];

    char * delimit = "#";
    int num=0;

    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];


    if (!((argc == 6 and strcmp((char *)argv[1],"write") == 0)|(argc == 5 and strcmp((char *)argv[1],"compute") == 0))){
        fprintf(stderr,"usage: ./client write <BW> <LENGTH> <VELOCITY> <NOISEPOWER>\n");
        fprintf(stderr,"usage: ./client compute <LINK_ID> <SIZE> <SIGNALPOWER>\n");
        exit(1);
    }

    printf("The client is up and running.”\n");

    //  construct content to be sent
    if(strcmp((char *)argv[1],"write") == 0){
        num = 6;
        printf("The client sent write opreation to AWS.”\n");
    }
    else{
        num = 5;
        printf("The client sent ID=<%s>, size=<%s>, and power=<%s> to AWS”\n", argv[2], argv[3], argv[4]);
    }


    for (int i = 1; i < num; ++i) {
        strcat(sendbuf, argv[i]);
        strcat(sendbuf, delimit);
    }

//    debug
//    printf("%s\n", sendbuf);


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(IPADDRESS, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }


    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("untitled1: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("untitled1: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "untitled1: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);



    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = send(sockfd, sendbuf, strlen(sendbuf), 0)) == -1) {
        perror("send");
        exit(1);
    }

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';


    if(strcmp((char *)argv[1],"write") == 0){
        if (strcmp(buf,"successwrite") == 0){
            printf("The write operation has been completed successfully\n");
        }
    }

    if(strcmp((char *)argv[1],"compute") == 0){
        if (strcmp(buf,"notfound") == 0){
            printf("Link ID not found\n");
        }
        else{
//            printf("%s\n", buf);

            char * delimit = "#";
            char* flag;
            char* link_id;
            char * transmission_delay;
            char * propagation_delay;
            char * end_to_end_delay;

            flag = strtok(buf, delimit);

            link_id = strtok(NULL, delimit);
            numbytes = strlen(link_id);
            link_id[numbytes] = '\0';

            transmission_delay = strtok(NULL, delimit);
            numbytes = strlen(transmission_delay);
            transmission_delay[numbytes] = '\0';

            propagation_delay = strtok(NULL, delimit);
            numbytes = strlen(propagation_delay);
            propagation_delay[numbytes] = '\0';

            end_to_end_delay = strtok(NULL, delimit);
            numbytes = strlen(end_to_end_delay);
            end_to_end_delay[numbytes] = '\0';

            printf("The delay for link<%s> is <%s>ms\n", argv[2], end_to_end_delay);

        }
    }


    close(sockfd);

    return 0;
}

