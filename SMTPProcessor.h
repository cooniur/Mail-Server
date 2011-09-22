/*
 * SMTPProcessor.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef SMTPPROCESSOR_H_
#define SMTPPROCESSOR_H_


#include "SMTPCommands.h"
#include "SqliteConnector.h"
#include "Server.h"

struct maild {
	char *from;
	char to[50][100];
	char *subject;
	char *msg;
	int noRcpt;
};

extern int processSMTPClientRequest(int clientfd);

int sendSMTPWelcomeGreeting(int sockfd);
int processSMTPCommands(int sockfd, char *msg);
int processVerify(int sockfd, char *msg);
int processHelo(int sockfd, char *msg);
int processMailFrom(int sockfd, char *msg);
int processMailTo(int sockfd, char *msg);
int processData(int sockfd, char *msg);
int processRset(int sockfd, char *msg);
int processSMTPQuit(int sockfd, char *msg);
char* rtrim(char* string, char junk);
char* ltrim(char *string, char junk);
int checkSMTPStates(int currState);
int resetVariables();
int readDataFromClient(int sockfd, char **msg);
#endif /* SMTPPROCESSOR_H_ */
