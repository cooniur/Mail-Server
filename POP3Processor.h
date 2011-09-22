/*
 * POP3Processor.h
 *
 *  Created on: Mar 20, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef POP3PROCESSOR_H_
#define POP3PROCESSOR_H_

#include <stdlib.h>
#include "POP3Commands.h"
#include "SqliteConnector.h"
#include "Server.h"

int processPOPClientRequest(int clientfd);
int sendPOPWelcomeGreeting(int clientfd);
int processPOPCommands(int sockfd, char *msg);
int processPOPQuit(int sockfd, char *msg);
int processUser(char *msg);
int checkPOPStates(int currState);
int processPassword(char *msg);
int processList(int sockfd, char *msg);
char * getMessage(int error, char **msg);
int getListMessages(char *username);
int processStat(int sockfd, char * msg);
int processDelete(int sockfd, char *msg);
int cleanAll();
int processRetr(int sockfd, char *msg);

#endif /* POP3PROCESSOR_H_ */
