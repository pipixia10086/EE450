EE450 Session#1
Full Name: Ziyao Yang
Student ID: 9490544195

What you have done in the assignment
	I have completed all requirements.     
	Phase 1
		All three server programs, AWS, Back-end Server A and B, boot up in this phase, listening to specific port as well as displaying boot up message. 
		After that, the client and monitor establish TCP connections with AWS and then the client sends the function and parameters to AWS server.

	Phase 2 
		AWS server send selected input values to the back-server A and B, depending on their functionalities over UDP

	Phase 2A
		AWS receive write command and parameters from clients and forward the request to back-end server A. The server A add the data to the database and generat a new LINK_ID and then tell AWS  			server that the command was performed successfully.

	Phase 2B
		When AWS receives compute command and parameters from clients, it firstly request link information from ServerA. ServerA will check the database and if link information is matahed, it will 			return the information otherwise tell the AWS server that link information are not found.
		Secondly，the AWS server forwards the information to ServerB and ServerB computes three delays and then return these results to AWS server. 

	Phase 3
		When the AWS receives the acknowledgement, it needs to forward the acknowledgment to the client and monitor using TCP.
		When the AWS receives the computation results, it needs to forward the result “end-to-end delay” to the client and all results (all three delays) to the monitor using TCP.


What your code files are and what each one of them does.
	aws.c  
		Receive function and input parameters from client over TCP, send the function and input to two back-end servers separately over UDP. send the result to the monitor and client.
	client.c  
		Send function and input parameters to aws over TCP. Receive feedback from aws and show on screen. 
	monitor.c  
		Display result from aws server.  
	serverA.c 
		Receive function and parameters from aws over UDP and add the item into the database or research based on the funciton 
	serverB.c  
		Receive function and parameters from aws over UDP, compute and send back result to aws.  

The format of all the messages exchanged

	AWS on screen messages
		“The AWS is up and running.”
		“The AWS received operation <FUNCTION> from the client using TCP over port <port number>”
		“The AWS sent operation <FUNCTION> and arguments to the monitor using TCP over port <port number>”
		“The AWS sent operation <FUNCTION> to Backend-Server A using UDP over port <port number>”
		“The AWS received link information from Backend-Server A using UDP over
		port <port number>” OR “Link ID not found”
		“The AWS sent link ID=<LINK_ID>, size=<SIZE>, power=<POWER>, and link information to Backend-Server B using UDP over port <port number>”
		“The AWS received outputs from Backend-Server B using UDP over port <port number>”
		“The AWS sent result to client for operation <FUNCTION> using TCP over port <port number>”
		“The AWS sent write response to the monitor using TCP over port <port number>”
		“The AWS sent compute results to the monitor using TCP over port <port number>”

	Client on screen messages
		“The client is up and running”
		“The client sent write operation to AWS”
		"The client sent ID=<LINK_ID>, size=<SIZE>, and power=<POWER> to AWS”
		“The write operation has been completed successfully”

	Monitor on screen messages
		“The monitor is up and running.”
		“The monitor received BW = <BANDWIDTH>, L = <LENGTH>, V = <VELOCITY> and P = <NOISE POWER> from the AWS”
		“The monitor received link ID=<LINK_ID>, size=<SIZE>, and power=<POWER> from the AWS”
		“The write operation has been completed successfully”
		“The result for link <LINK_ID>: Tt = <Transmission Time>ms, Tp = <Propagation Time>ms, Delay = <Delay>ms”

	Backend-Server A on screen messages
		“The Server A is up and running using UDP on port <port number>.”
		“The Server A received input for writing”
		“The Server A wrote link <LINK_ID> to database”
		“The Server A received input <LINK_ID> for computing”
		“The Server A finished sending the search result to AWS” OR “Link ID not found”

	Backend-Server B on screen messages
		“The Server B is up and running using UDP on port <port number>.”
		“The Server B received link information: link <LINK_ID>, file size <SIZE>, and signal power <POWER>”
		“The Server B finished the calculation for link <LINK_ID>”
		“The Server B finished sending the output to AWS”


Any idiosyncrasy of your project.
	I check the input and give the right input format and set a parameter to limit the size of buffer. So that I can deal with overstack and some other problems easily. What's more, my database is based on unique ID, which is a relational database, so that I could find the required information quickly. 


Reused Code: 
	I use code from Beej's book for my project to set up TCP and UDP connection. 
