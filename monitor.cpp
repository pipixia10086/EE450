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

#define PORT "25195" // the TCP port of aws that monitor connect to

#define MAXDATASIZE 4000 // max number of bytes we can get at once
#define IPADDRESS "127.0.0.1"


char* flag; //function sent by client
char* link_id; //link_id sent by client
char* size; //size sent by client
char* power; //power sent by client
char* bw; //bw sent by client
char* length; //length sent by client
char* velocity; //velocity sent by client
char* noise_power; //noise_power sent by client
char * delimit = "#";

char * transmission_delay;
char * propagation_delay;
char * end_to_end_delay;





// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

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
            perror("monitor : socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("monitor : connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "monitor : failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("The monitor is up and running.\n");

    freeaddrinfo(servinfo); // all done with this structure

    while (1){
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';

        flag = strtok(buf, delimit);

//        printf("%s\n", flag);

        if (strcmp((char *)flag,"write") == 0){
            bw = strtok(NULL, delimit);
            numbytes = strlen(bw);
            bw[numbytes] = '\0';

            length = strtok(NULL, delimit);
            numbytes = strlen(length);
            length[numbytes] = '\0';

            velocity = strtok(NULL, delimit);
            numbytes = strlen(velocity);
            velocity[numbytes] = '\0';

            noise_power = strtok(NULL, delimit);
            numbytes = strlen(noise_power);
            noise_power[numbytes] = '\0';

            printf("The monitor received BW=<%s>, L=<%s>, V=<%s>, and P=<%s> from the AWS\n",
                   bw, length, velocity, noise_power);
        }

        if (strcmp((char *)flag,"notfound") == 0) {
            printf("Link ID not found\n");
        }
        if (strcmp((char *)flag,"successwrite") == 0) {
            printf("The write operation has been completed successfully\n");
        }


        if (strcmp((char *)flag,"compute") == 0) {

            link_id = strtok(NULL, delimit);
            link_id[numbytes] = '\0';

            size = strtok(NULL, delimit);
            size[numbytes] = '\0';

            power = strtok(NULL, delimit);
            power[numbytes] = '\0';

            printf("The monitor received link ID=<%s>, size=<%s>,and power=<%s> from the AWS\n",
                   link_id, size, power);
        }
        if (strcmp((char *)flag,"successcompute") == 0) {
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

            printf("The result for link<%s>:\n"
                           "Tt=<%s>ms\n"
                           "Tp=<%s>ms\n"
                           "Delay=<%s>ms\n", link_id, transmission_delay, propagation_delay, end_to_end_delay);
        }

            //exit(0);
    }
    close(sockfd);

    return 0;
}

