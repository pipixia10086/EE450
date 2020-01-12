#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<math.h>
#include <ctype.h>


#define SERVERPORT "23195"	// the port for UDP with  serverB
#define SERVERPORTB "22195"	// the port for UDP with  serverA

#define MAXBUFLEN 4000
#define MAXATTRLEN 50



char *ltrim(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return str;
    }

    int len = 0;
    char *p = str;
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(str, p, strlen(str) - len + 1);

    return str;
}


int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];

    char function[MAXATTRLEN];

    char* function1; //function sent by client
    char* id;
    char* size; //size sent by client
    char* power; //power sent by client

    char* bw; //bw sent by client
    char* length; //length sent by client
    char* velocity; //velocity sent by client
    char* noise_power; //noise_power sent by client

    float size1; //size sent by client
    float power1; //power sent by client

    float bw1; //bw sent by client
    float length1; //length sent by client
    float velocity1; //velocity sent by client
    float noise_power1; //noise_power sent by client

    //input
    float capacity;
    float transmission_delay1;
    float propagation_delay1;
    float end_to_end_delay1;
    char transmission_delay[MAXATTRLEN];
    char propagation_delay[MAXATTRLEN];
    char end_to_end_delay[MAXATTRLEN];

    char word[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    char send_data[MAXBUFLEN];
    char* delimit = " ";


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, SERVERPORTB, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    addr_len = sizeof their_addr;

    printf("The Server B is up and running using UDP on port <%s>.\n", SERVERPORTB);


    while(1) {

        numbytes = recvfrom(sockfd, function, MAXBUFLEN-1 , 0,
                            (struct sockaddr *)&their_addr, &addr_len);

        function[numbytes] = '\0';

        numbytes = recvfrom(sockfd, word, MAXBUFLEN-1 , 0,
                            (struct sockaddr *)&their_addr, &addr_len);

        word[numbytes] = '\0';


        if (numbytes==-1) {
            perror("recv");
            exit(1);
        }

//        printf("%s\n", word);
//        fflush(stdout);

        id = strtok(word, delimit);

        bw = strtok(NULL, delimit);
        numbytes = strlen(bw);
        bw[numbytes] = '\0';
        bw1 = atof(bw);

        length = strtok(NULL, delimit);
        numbytes = strlen(length);
        length[numbytes] = '\0';
        length1 = atof(length);

        velocity = strtok(NULL, delimit);
        numbytes = strlen(velocity);
        velocity[numbytes] = '\0';
        velocity1 = atof(velocity);

        noise_power = strtok(NULL, delimit);
        numbytes = strlen(noise_power);
        noise_power[numbytes] = '\0';
        noise_power1 = atof(noise_power);

        size = strtok(NULL, delimit);
        numbytes = strlen(size);
        size[numbytes] = '\0';
        size1 = atof(size);

        power = strtok(NULL, delimit);
        numbytes = strlen(power);
        power[numbytes] = '\0';
        power1 = atof(power);


        size = ltrim(size);
//        printf("%s\n", size);

        printf("The Server B received link information: link <%s>, file size <%s>, and signal power <%s>\n",
               id,size,power);

//        printf("The Server B received input <%s> and parameters <%s><%s><%s><%s>\n",
//               function,bw,length,velocity,noise_power);

        power1 = pow(10, (power1-30)/10);
        noise_power1 = pow(10, (noise_power1-30)/10);
        capacity = bw1 * log2(1 + power1/noise_power1);

//	printf("<%f><%f><%f>\n", power1, noise_power1, capacity);

        transmission_delay1 = size1/capacity/1000;
        sprintf(transmission_delay,"%.2f", transmission_delay1+0.0049);

        propagation_delay1 = length1 / velocity1 * 1000;
        sprintf(propagation_delay,"%.2f", propagation_delay1+0.0049);

        end_to_end_delay1 = transmission_delay1 + propagation_delay1;
        sprintf(end_to_end_delay,"%.2f", end_to_end_delay1+0.0049);

        printf("The Server B finished the calculation for link <%s>\n", id);


//        printf("<%s><%s><%s>\n", transmission_delay, propagation_delay, end_to_end_delay);


        sprintf(send_data,"successcompute#%s#%s#%s#%s",id, transmission_delay, propagation_delay, end_to_end_delay);
//        printf("%s\n", send_data);


        // send back to aws
        numbytes = sendto(sockfd,send_data,strlen(send_data),0,
                          (struct sockaddr *)&their_addr, addr_len);

//        printf("%d", numbytes);

        printf("The Server B finished sending the output to AWS\n");

        //printf("debug: numbytes is %d\n", numbytes);

//        free(send_data);

        //printf("main: after free final return string is <%s>\n", returnString);

        fflush(stdout); //wait for next connect

        //close(sockfd);
    }

    return 0;
}
