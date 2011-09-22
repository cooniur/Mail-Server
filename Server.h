/*
 * Server.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "stringProcessor.h"


extern int getListeningPort(int portNo);
extern int listenForClient(int socket);
int readClient(int sockfd, char **msg);
int writeClient(int sockfd, const char *msg, int insertCRLR);
int setupSSLCommunication(int sockfd);
int readClientData(int sockfd, char **msg);
int shutdownSSL();

#endif /* SERVER_H_ */
