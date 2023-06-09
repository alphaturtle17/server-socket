#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> 
#include <cstdio>
//Bunch of include statements will probably delete some if unnecessary
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> //maybe remove, look into getnameinfo gethostbyname and getaddrinfo
#include "../include/hashset.hpp" //[hash-set] [5]
#include "../include/pack.hpp"

//#define DEF_HOST "localhost"
//#define DEF_PORT 8081

//got annoyed at looking at that
void errorCheck(int num){
    if(num == -1){
        fprintf(stderr, "Error: %s\n", strerror(errno)); //catching error in socket
        exit(errno);
    }
}

int main(int argc, char* argv[]){        
    //[this-is-a-good-place-to-start-for-the-exam] [14]
    int serverSocket, newSocket, n;
    struct sockaddr_in serverAddress, clientAddress;
    unsigned int cliLength;
    char buffer[256];                         /* hold string read from buffer */
    
    string hostname = "localhost"; //uses localhost and port default
    int port = 8081;

    //[my_which] [4]
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--hostname") == 0 && i + 1 < argc){ //checks if flag is passed --hostname
            char* address = strtok(argv[i+1], ":");
            char* portStr = strtok(NULL, ":");

            if(address != NULL){
                hostname = address;
            }
            if(portStr != NULL){
                port = atoi(portStr);
            }
        }
    } 
    //[lab8] [6] not entirely used, but used concepts from it
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); //[socket] [8] use AF_INET because ipv4 is 32 bit 
    errorCheck(serverSocket);

    //[bind] [7] binding socket
    //[INET] [9] more binding socket stuff learning to set up the server address
    //[this-is-a-good-place-to-start-for-the-exam] [14]
    //[bzero] [15]
    bzero((char*) &serverAddress, sizeof(serverAddress));
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;//inet_addr(hostname);
    serverAddress.sin_port = htons(port);

    // Bind the socket to the server address

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        fprintf(stderr, "Failed to bind socket: %s\n", strerror(errno));
        exit(errno); //if no bind we leave
    }
    // Listen for connections
    //[Listen] [10] 
    //[this-is-a-good-place-to-start-for-the-exam] [14]
    listen(serverSocket, 5);
    printf("Server listening:\n");  

    HashSet hs(1000); //creating empty hashmap with a really large size therefore no collisions (not the best for storage but it's what logic works rn)
    
    while(1){
        cliLength = sizeof(clientAddress);
        newSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &cliLength);
        if(newSocket == -1){
            fprintf(stderr, "Error on accept: %s\n", strerror(errno));
            continue;
        }
        fprintf(stdout, "Accepted: %d\n", newSocket);

        //connection is established now, buffer time
        bzero(buffer, 256);
        n = read(newSocket, buffer, 255); //no null terminator for buffington
        buffer[n] = '\0';
        errorCheck(n);

        //Write back to client
        //n = write(newSocket, "Message received", n);
        n = write(newSocket, "Message received\n", n);
        errorCheck(n);

        //decrypt
        int len = strlen(buffer);
        for(int i = 0; i < len; i++){
            buffer[i] = buffer[i] ^ 42; //decrypt
        }
        fprintf(stdout, "Decrypted message: %s\n", buffer);
        //Testing my code:
        //vec
        //��File��name�things.txt�bytes��h�a�l�l�o� �m�y� �n�a�m�e� �i�s� �t�h�o�r

        //Deserialize the message to the struct (file or request)
        string message = buffer;
        string type = message.substr(0, message.find(':')); //extracting what type it could be
        fprintf(stdout, "\t%s\t%s\n", message, type);
        if (type == "File") { 
            std::string fName = message.substr(message.find(':') + 1);
            std::string fContent = message.substr(message.find('\n') + 1);
 
            FILE* fp = fopen(fName.c_str(), "w");
            if (fp != NULL) {
                size_t contentSize = fContent.size();
                fwrite(fContent.c_str(), sizeof(char), contentSize, fp);
                fclose(fp);

                bool inserted = hs.insert(fName, fp);
                if (!inserted) {
                    fprintf(stderr, "Failed to insert file into the hash map\n");
                }
            } else {
                fprintf(stderr, "Failed to open file: %s\n", fName.c_str());
            }
        }else if(type == "Request"){
            std::string fName = message.substr(message.find(':') + 1);
            
            FILE* filePointer = hs.get(fName);
            std::string fileContent;

            if (filePointer != nullptr) {
                char buffer[256];
                size_t bytesRead;

                // Read the content of the file into a string
                while ((bytesRead = fread(buffer, sizeof(char), sizeof(buffer) - 1, filePointer)) > 0) {
                    buffer[bytesRead] = '\0';
                    fileContent += buffer;
                }

                // Close the file
                fclose(filePointer);

                // File found, serialize it as a File message, encrypt, and send it back to the client
                std::string response = "File:" + fName + "\n" + fileContent;

                // Encrypt the response using the XOR encryption scheme (key is 42)
                for (int i = 0; i < response.length(); i++) {
                    response[i] = response[i] ^ 42;
                }

                // Send the encrypted response back to the client
                n = write(newSocket, response.c_str(), response.length());
                errorCheck(n);
            }
        }else{
            fprintf(stderr, "Unknown type\n");
        }
    }
    
    return 0;
}