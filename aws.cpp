#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#define PORTCLIENT "24195"  // the port for TCP with client
#define PORTMONITOR "25195"  // the port for TCP with monitor

#define SERVERPORT "23195"	// the port for UDP with  serverB
#define SERVERPORTA "21195"	// the port for UDP with  serverA
#define SERVERPORTB "22195"	// the port for UDP with  serverB

#define IPADDRESS "127.0.0.1" // local IP address

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXBUFLEN 4000


char* function; //function sent by client
char* link_id; //link_id sent by client
char* size; //size sent by client
char* power; //power sent by client
char* bw; //bw sent by client
char* length; //length sent by client
char* velocity; //velocity sent by client
char* noise_power; //noise_power sent by client
char notfound = 0;

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// use sigaction to clear dead process
void clear_dead_process()
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

// setup TCP at port
int setupTCP(char* port)
{
    int rv; // use for error checking of getaddrinfo()
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int yes=1;
    memset(&hints, 0, sizeof hints); //Zero the whole structure before use
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP SOCK_STREAM sockets
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    clear_dead_process();
    return sockfd;
}


// send text[] to monitor
int send_to_monitor(char text[], int new_fd_monitor)
{
    char text2[4000];
    strcpy (text2,text);
    if (send(new_fd_monitor, text, strlen(text2), 0) == -1)
    {
        perror("send");
        return -1;
    }
    return 0;
}


int setupUDP(char *function, char* port, char *word, char* received_data)
{
    int sockfd;
    int rv;

    struct addrinfo hints, *servinfo, *p;
    socklen_t addr_len;

    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;


    if ((rv = getaddrinfo(IPADDRESS, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return -2;
    }
  
    if (strcmp(port,SERVERPORTA)==0) {
	  // send function to server
	    if ((numbytes = sendto(sockfd, function, strlen(function), 0,
		                   p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	    }



	    // send word to server
	    int bytes_recv;

	    if ((numbytes = sendto(sockfd, word, strlen(word), 0,
		                   p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	    }




        if (strcmp((char *)function,"write") == 0){
//            printf("Sent <%s> and <%s> to Backend-Server A\n", function, word);
            printf("The AWS sent operation write to Backend-Server A using UDP over port <23195>\n");

            bytes_recv = recvfrom(sockfd,received_data,MAXBUFLEN,0, NULL, NULL);
            if ( bytes_recv == -1) {
                perror("recv");
                exit(1);
            }
            printf("The AWS received response from Backend-Server A for writing using UDP over port <21195>\n");
        }
        if (strcmp((char *)function,"compute") == 0){

            printf("The AWS sent operation compute to Backend-Server A using UDP over port <23195>\n");

            bytes_recv = recvfrom(sockfd,received_data,MAXBUFLEN,0, NULL, NULL);
            if ( bytes_recv == -1) {
                perror("recv");
                exit(1);
            }
            if (strcmp((char *)received_data,"notfound") == 0){
                notfound = 1;

            }
            else{
                printf("The AWS received link information from Backend-Server A using UDP over port <21195>\n");
            }

        }


    }

    if (strcmp(port,SERVERPORTB)==0 and notfound == 0) {

	  // send function to server
	    if ((numbytes = sendto(sockfd, function, strlen(function), 0,
		                   p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	    }



	    // send word to server
	    int bytes_recv;

	    if ((numbytes = sendto(sockfd, word, strlen(word), 0,
		                   p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	    }


        printf("The AWS sent link ID=<%s>, size=<%s>, power=<%s>, and "
                       "link information to Backend-Server B using UDP over port <23195>\n", link_id, size, power);

//        debug
//        printf("Sent <%s> and <%s> to Backend-Server B\n", function, word);

        bytes_recv = recvfrom(sockfd,received_data,MAXBUFLEN,0, NULL, NULL);
        if ( bytes_recv == -1) {
            perror("recv");
            exit(1);
        }
//        printf("%d\n", bytes_recv);
//        printf("Received :%s\n",received_data);
        printf("The AWS received response from Backend-Server B for computing using UDP over port <22195>\n");

    }


    freeaddrinfo(servinfo); // done with servinfo

    close(sockfd);

    return 0;
}



int search(char *function, char *word, char* port)
{
    return 0;
}



int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    int sockfd_monitor,new_fd_monitor; //listen on sock_fd_monitor, new connection on new_fd_monitor


    struct sockaddr_storage their_addr; // connector's address information

    socklen_t sin_size;

    char s[INET6_ADDRSTRLEN];


    char * delimit = "#";

    int numbytes; //using in receive or send

    char data_received[500]; //all data sent by client
    char data_received_copy[500]; //all data sent by client

    char* data_receiveda[500]; //all data sent by serverA
    char* data_receivedb[500]; //all data sent by serverA



    sockfd = setupTCP((char *)PORTCLIENT);
    sockfd_monitor = setupTCP(PORTMONITOR);


    printf("The AWS is up and running\n");


    int monitor_flag=0;
    while (monitor_flag == 0){
        // child socket connect with monitor
        sin_size = sizeof their_addr;
        new_fd_monitor = accept(sockfd_monitor, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd_monitor == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);

        if (!fork()) { // this is the child process
            close(sockfd_monitor); // child doesn't need the listener
            monitor_flag=1;
            continue;
        }
        close(new_fd_monitor);  // parent doesn't need this
    }



    while(1) {  // main accept() loop
        notfound = 0;


        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);


        int ra_len = sizeof(struct sockaddr_in);
        struct sockaddr_in ra;
        char *ip = NULL;
        int port=0;
        if(getpeername(new_fd, (struct sockaddr *)&ra, (socklen_t *)&ra_len) == 0)
        {
            ip = inet_ntoa(ra.sin_addr);
            port = ntohs(ra.sin_port);
        }
//        printf("server: got connection from %s:%d\n",ip, port);


        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener

            // receive function and word from client
            if ((numbytes = recv(new_fd, data_received, sizeof data_received, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            else{
                strcpy(data_received_copy, data_received);
                data_received[numbytes] = '\0';
//                printf("%s\n", data_received);
                function = strtok(data_received, delimit);

                printf("The AWS received function=<%s> from the client using TCP over port <%d>\n",
                       function, port);

                if(send_to_monitor(data_received_copy,new_fd_monitor)!=-1){
             //       perror("sendmonitor");
                }



                if (strcmp((char *)function,"write") == 0){
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

                    printf("The AWS sent operation <wirte> and arguments to the monitor using TCP over port <25195>\n");


                    //printf("The client sent write operation to AWS\n");
                    //printf("The client sent bw=%s, length=%s, velocity=%s, and noise_power=%s to AWS\n", bw, length, velocity,noise_power);

                    setupUDP(function, SERVERPORTA, data_received_copy, (char *)data_receiveda);
                }
                if (strcmp((char *)function,"compute") == 0){

                    link_id = strtok(NULL, delimit);
                    link_id[numbytes] = '\0';

                    size = strtok(NULL, delimit);
                    size[numbytes] = '\0';

                    power = strtok(NULL, delimit);
                    power[numbytes] = '\0';

		    printf("The AWS sent operation <compute> and arguments to the monitor using TCP over port <25195>\n");

//                    printf("The client sent ID=%s, size=%s, and power=%s to AWS\n", link_id, size, power);

                    setupUDP(function, SERVERPORTA, link_id, (char*)data_receiveda);

//                    printf("%s\n", data_receiveda);

                    strncat((char*)data_receiveda, size, 50);
                    strncat((char*)data_receiveda, "  ", 50);
                    strncat((char*)data_receiveda, power, 50);

//                    printf("%s\n", data_receiveda);

                    setupUDP(function, SERVERPORTB, (char *)data_receiveda, (char*)data_receivedb);


//                    printf("%s\n", data_receivedb);

                }

            }

            if (strcmp((char *)function,"write") == 0){
 
                    if (send(new_fd, "successwrite", 12, 0) == -1){
                        perror("send to client");
                    }
                    if(send_to_monitor("successwrite#",new_fd_monitor)!=-1){
                 //       perror("send to monitor");
                    }
                printf("The AWS sent result to client for operation <write> using TCP over port <24195>\n");
                printf("The AWS sent write response to the monitor using TCP over port<25195>\n");

            }


            if (strcmp((char *)function,"compute") == 0){

                if(notfound){
                    printf("Link ID not found\n");

                    if (send(new_fd, "notfound", 8, 0) == -1){
                        perror("send to client");
                    }
                    if(send_to_monitor("notfound#",new_fd_monitor)!=-1){
                 //       perror("send to monitor");
                    }
                }
                else {
                    

                    if (send(new_fd, data_receivedb, sizeof data_receivedb, 0) == -1){
                        perror("send to client");
                    }
                    if (send(new_fd_monitor, data_receivedb, sizeof data_receivedb, 0) == -1) {
                //        perror("send to monitor");
                    }

                    printf("The AWS sent result to client for operation <compute> using TCP over port <24195>\n");
                    printf("The AWS sent compute results to the monitor using TCP over port<25195>\n");

                }
            }


            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}



