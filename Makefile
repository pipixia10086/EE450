all: 
	gcc -o awsoutput aws.cpp
	gcc -o client client.cpp
	gcc -o serverAoutput serverA.cpp
	gcc -o serverBoutput serverB.cpp -lm
	gcc -o monitoroutput monitor.cpp

.PHONY: aws
aws:
	./awsoutput

.PHONY:serverA
serverA:
	./serverAoutput

.PHONY:serverB
serverB:
	./serverBoutput

.PHONY: monitor
monitor:
	./monitoroutput
