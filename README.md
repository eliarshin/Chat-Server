# Chat-Server
Implemented in C

=== Description ===

Description: 
The following program will create a chat server by using the function select, 
everyone can communicate through TELNET and connect to local host server, he will recive a guest number.

Program files:
chatserver.c - the main program written there and it include our chat server.

Functions:
    
chatserver.c : 
int checkDigit(char*) - recieve the port and check if it valid.
void singalHandler(int) - recive signal number and free our mallocs
int main(int argc, char *argv[]) - 
recieve from user the port and create a local server, 
after we run the local server everyone can connect to the server with telnet and enter to the chat.
The main build is using array of struct msg , it include the message, the sender and the reciver.
by that we control and know who send the message and how to stream it.

