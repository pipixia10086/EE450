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

#define SERVERPORT "23195"	// the port for UDP with  serverB
#define SERVERPORTA "21195"	// the port for UDP with  serverA

#define MAXBUFLEN 4000
#define MAXATTRLEN 50

int search(){
    return 0;
}


void set_str(char* des, char* src){
    char* p = des;
    char* p1 = src;
    while (*p1 != '\0'){
        *p = *p1;
        p ++;
        p1 ++;
    }
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
    char linkid[MAXATTRLEN];
    int search_id;

    char bw1[MAXATTRLEN]; //bw sent by client
    char length1[MAXATTRLEN]; //length sent by client
    char velocity1[MAXATTRLEN]; //velocity sent by client
    char noise_power1[MAXATTRLEN]; //noise_power sent by client

    memset(linkid, ' ', MAXATTRLEN);
    memset(bw1, ' ', MAXATTRLEN);
    memset(length1, ' ', MAXATTRLEN);
    memset(velocity1, ' ', MAXATTRLEN);
    memset(noise_power1, ' ', MAXATTRLEN);



    char* function1; //function sent by client
    char* bw; //bw sent by client
    char* length; //length sent by client
    char* velocity; //velocity sent by client
    char* noise_power; //noise_power sent by client



    char word[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    char send_data[MAXBUFLEN];

//    /Desktop
    char* filepath = "backendA.txt";
    char last_line[500];
    char current_line[500] = "0";


    char returnString[MAXBUFLEN];
    char* delimit = "#";


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, SERVERPORTA, &hints, &servinfo)) != 0) {
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


    printf("The Server A is up and running using UDP on port <21195>.\n");
    FILE* file = fopen(filepath, "a+");
    if (file == NULL) {
        printf("Error: Databse %s not exist.\n",filepath);
        return 1;
    }


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

//        printf("%s %s\n", function, word);
//        fflush(stdout);


        if (strcmp(function,"write")==0) {
            function1 = strtok(word, delimit);

            bw = strtok(NULL, delimit);
            numbytes = strlen(bw);
            bw[numbytes] = '\0';
            set_str(bw1, bw);
            bw1[MAXATTRLEN-1] = '\0';

//        printf("%s ", bw);
//        printf("%d ", strlen(bw));

            length = strtok(NULL, delimit);
            numbytes = strlen(length);
            length[numbytes] = '\0';
            set_str(length1, length);
            length1[MAXATTRLEN-1] = '\0';

            velocity = strtok(NULL, delimit);
            numbytes = strlen(velocity);
            velocity[numbytes] = '\0';
            set_str(velocity1, velocity);
            velocity1[MAXATTRLEN-1] = '\0';

            noise_power = strtok(NULL, delimit);
            numbytes = strlen(noise_power);
            noise_power[numbytes] = '\0';
            set_str(noise_power1, noise_power);
            noise_power1[MAXATTRLEN-1] = '\0';

            // debug
//            printf("The Server A received input <%s> and parameters <%s><%s><%s><%s>\n",
//                   function,bw,length,velocity,noise_power);

        }


//        printf("The Server A received input <%s> and parameters <%s><%s><%s><%s>\n",
//               function,bw1,length1,velocity1,noise_power1);


        FILE* file = fopen(filepath, "a+");
        if (file == NULL) {
            printf("Error: Databse %s not exist.\n",filepath);
            return 1;
        }

        // get the last line
        int next_id;
        char next_id_str[20];
        char max_id[20];
        while(fgets (last_line, 500, file)!=NULL ){
            strcpy(current_line, last_line);
        }
//        printf("%s\n", current_line);

        if (strcmp(current_line, "0") == 0){
            // the file is empty, maxid = 1
            next_id = 1;
        }
            // the file is not empty, read the lastid and set maxid
        else{
            char* tmp = current_line;
            int num = 0;
            while(*tmp != ' '){
                max_id[num] = *tmp;
                num ++;
                tmp ++;
            }
            next_id = atof(max_id) + 1;
        }

//        printf("%s %d\n", max_id, next_id);

        sprintf(next_id_str,"%d",next_id);

        set_str(linkid, next_id_str);
        linkid[MAXATTRLEN-1] = '\0';

//        printf("%s\n", linkid);

        if (strcmp(function,"write")==0) {
            printf("The Server A received input for writing\n");
            fputs(linkid, file);
            fputs(bw1, file);
            fputs(length1, file);
            fputs(velocity1, file);
            fputs(noise_power1, file);
            fputs("\n", file);

            sprintf(send_data,"The Server A wrote link <%d> to database",next_id);

            printf("The Server A wrote link <%d> to database\n", next_id);
        }
        else if (strcmp(function,"compute")==0) {
            search_id = atoi(word);
            printf("The Server A received input <%d> for computing\n", search_id);

            if(search_id >= next_id){
                strcpy(send_data, "notfound");
                printf("Link ID not found\n");
            }
            else{
                // bug: if some line are lost.
                fseek(file, 0, SEEK_SET);
                while(search_id){
                    fgets (send_data, sizeof returnString, file);
                    search_id --;
                }
                send_data[strlen(send_data)] = '\0';
//                printf("%s\n", returnString);
                printf("The Server A finished sending the search result to AWS\n");
            }
        }

        //printf("main: final return string is <%s>\n", returnString);

        fclose(file);

        // send back to aws
//        strcpy(send_data,returnString);
        //printf(" ServerA SEND : %s\n",send_data);

        numbytes = sendto(sockfd,send_data,strlen(send_data),0,
                          (struct sockaddr *)&their_addr, addr_len);


        //printf("debug: numbytes is %d\n", numbytes);
//        free(returnString);
//        returnString=NULL;
        //printf("main: after free final return string is <%s>\n", returnString);


        fflush(stdout); //wait for next connect

        //close(sockfd);
    }

    return 0;
}
