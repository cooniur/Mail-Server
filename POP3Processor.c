/*
 * POP3Processor.c
 *
 *  Created on: Mar 20, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "POP3Processor.h"

#define FALSE 0
#define TRUE 1

char* username = NULL;
char* password = NULL;
struct listdata *list;

int processPOPClientRequest(int clientfd) {
	int loop = TRUE;
	char *buffer;

	sendPOPWelcomeGreeting(clientfd);
	while(loop) {
		readClient(clientfd, &buffer);

		//TEST FOR QUIT
		if (strncasecmp(buffer, "kill", strlen("kill")) == 0) {//EXIT WHILE LOOP
			cleanAll();
			free(buffer);
			return(-2);
		}
		else if (strncasecmp(buffer, POP_QUIT, strlen(POP_QUIT)) == 0) {//EXIT WHILE LOOP
			if (processPOPQuit(clientfd, buffer) == 0) {
				loop = FALSE;
				cleanAll();
				strcpy(buffer," Goodbye");
				writeClient(clientfd, getMessage(FALSE, (char**) &buffer), TRUE);
			}
		}
		else
			processPOPCommands(clientfd, buffer);
		free(buffer);
	}
	return(0);
}

int cleanAll() {
	if (list != NULL) free(list);
	if (username != NULL) free(username);
	if (password != NULL) free(password);
	shutdownSSL();
	return(0);
}

int sendPOPWelcomeGreeting(int sockfd) {
	char *msg = " Welcome to MailPro Service, Version: 0.01\r\n";
	writeClient(sockfd, POP_OK, FALSE);
	writeClient(sockfd, msg, TRUE);
	return(0);
}

int processPOPQuit(int sockfd, char *msg) {
	msg = getParameters(msg, POP_QUIT);
	if (strlen(msg) != 0) {
		writeClient(sockfd, POP_ERR, FALSE);
		return(-1);
	}
	free(msg);
	return(0);
}

char * getMessage(int error, char **msg) {
	char buffer[1020];
	if (error == FALSE)
		strcpy(buffer, POP_OK);
	else
		strcpy(buffer, POP_ERR);
	strcat(buffer, " ");
	strcat(buffer, *msg);
	strcpy(*msg, buffer);
	return(*msg);
}

int checkPOPStates(int currState) {
	return(1);
}

int getListMessages(char *username) {
	if (list != NULL)
		free(list);
	fetch_list_data(username, (struct listdata **)&list);
	return(0);
}
/**
 * Processes all the POP3 commands such as
 * USER, PASS, LIST, RETR, DELE, NOOP, RSET, STAT
 */
int processPOPCommands(int sockfd, char *msg) {
	char *clientMsg = NULL;
	clientMsg = (char *)malloc(sizeof(char *) * 1024);
	if (clientMsg == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	if (strncasecmp(msg, POP_SSL_STARTTLS, strlen(POP_SSL_STARTTLS)) == 0) { //EHLO
		if (setupSSLCommunication(sockfd)) {
			writeClient(sockfd, "OK Begin TLS negotiation now", TRUE);
		}
	}
	else if (strncasecmp(msg, POP_USER, strlen(POP_USER)) == 0) { //USER
		if (checkPOPStates(iPOP_USER)) {
			if (processUser(msg)== -1) {
				strcpy(clientMsg,"User name required");
				writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
			}
			else
				writeClient(sockfd, POP_OK, TRUE);
		}
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else if (strncasecmp(msg, POP_PASS, strlen(POP_PASS)) == 0) { //PASS
		if (checkPOPStates(iPOP_PASS)) {
			if (processPassword(msg) == -1) {
				strcpy(clientMsg,"Password required");
				writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
			}
			else {
				struct userdata *user;
				if (fetch_user_data(username, password, &user)) { //VALID USER

					//GET MESSAGE LISTING FOR THE USER
					getListMessages(username);

					writeClient(sockfd, POP_OK, TRUE);
					free(user);
				}
				else {
					strcpy(clientMsg,"Invalid User");
					writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
				}
			}
		}
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else if (strncasecmp(msg, POP_LIST, strlen(POP_LIST)) == 0) { //LIST
		if (checkPOPStates(iPOP_LIST))
			processList(sockfd, msg);
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else if (strncasecmp(msg, POP_RETR, strlen(POP_RETR)) == 0) { //RETR
		if (checkPOPStates(iPOP_RETR)) {
			if (processRetr(sockfd, msg) == -1) {
				strcpy(clientMsg,"Invalid Syntax");
				writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
			}
		}
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else if (strncasecmp(msg, POP_DELE, strlen(POP_DELE)) == 0) { //DELE
		if (checkPOPStates(iPOP_DELE)) {
			if (processDelete(sockfd, msg)== -1) {
				strcpy(clientMsg,"Invalid Syntax");
				writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
			}
		}
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else if (strncasecmp(msg, POP_RSET, strlen(POP_RSET)) == 0) { //RSET
//		if (checkStates(iPOP_RSET))
//			processRset(sockfd, msg);
//		else
//			writeClient(sockfd, POP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, POP_NOOP, strlen(POP_NOOP)) == 0) { //NOOP
		if (checkPOPStates(iPOP_NOOP))
			writeClient(sockfd, POP_OK, TRUE);
		else
			writeClient(sockfd, POP_ERR, TRUE);
	}
	else if (strncasecmp(msg, POP_STAT, strlen(POP_STAT)) == 0) { //STAT
		if (checkPOPStates(iPOP_STAT)) {
			processStat(sockfd, msg);
		}
		else {
			strcpy(clientMsg,"Invalid State");
			writeClient(sockfd, getMessage(TRUE, &clientMsg), TRUE);
		}
	}
	else {
		strcpy(clientMsg,"Invalid State");
		writeClient(sockfd,getMessage(TRUE, &clientMsg), TRUE);
	}
	free(clientMsg);
	return(0);
}


int processRetr(int sockfd, char *msg) {
	msg = getParameters(msg, POP_RETR);
	if (strlen(msg) == 0) {
		free(msg);
		return(-1);
	}
	else {
		int stat_details = 0;
		struct emaildata *mail;
		int msgCount = atoi(msg);
		if (msgCount > list[stat_details].mesgid || msgCount < 1) {
			writeClient(sockfd, POP_ERR, TRUE);
			return(0);
		}
		fetch_mesg_data(username, list[msgCount].mesgid, &mail);
		writeClient(sockfd,"From: ",FALSE);
		writeClient(sockfd,mail->from, TRUE);
		writeClient(sockfd,"To: ",FALSE);
		writeClient(sockfd,mail->to, TRUE);
		writeClient(sockfd,"Subject: ",FALSE);
		writeClient(sockfd,mail->subject, TRUE);
		writeClient(sockfd,mail->body, TRUE);
		writeClient(sockfd, POP_OK, TRUE);
		free(mail->body);
		free(mail->from);
		free(mail->to);
		free(mail);
	}
	free(msg);
	return(0);
}

int processDelete(int sockfd, char *msg) {
	msg = getParameters(msg, POP_DELE);
	if (strlen(msg) == 0) {
		return(-1);
	}
	else {
		int stat_details = 0;
		int msgCount = atoi(msg);
		if (msgCount > list[stat_details].mesgid || msgCount < 1) {
			writeClient(sockfd, POP_ERR, TRUE);
			return(0);
		}
		deleteMsg(username, list[msgCount].mesgid);
		getListMessages(username);
		writeClient(sockfd, POP_OK, TRUE);
	}
	return(0);
}

/**
 * Returns the total stats of the mailbox
 */
int processStat(int sockfd, char * msg) {
	char buffer[4000];
	char sTotalMsg[100];
	char sMesgCount[10];
	int stat_details = 0;

	sprintf(sMesgCount,"%03d",list[stat_details].mesgid);
	sprintf(sTotalMsg,"%03d",list[stat_details].mesgsize);

	memset(buffer,'\0',4000);
	strcpy(buffer,POP_OK);
	strcat(buffer," ");
	strcat(buffer,sMesgCount);
	strcat(buffer," ");
	strcat(buffer,sTotalMsg);
	writeClient(sockfd,buffer,TRUE);
	return(0);
}

/**
 * Process the User command
 */
int processUser(char *msg) {
	msg = getParameters(msg, POP_USER);
	if (strlen(msg) == 0) {
		return(-1);
	}
	else {
		if (username != NULL)
			free(username);
		username = (char *) malloc(sizeof(char *)*(strlen(msg) + 1));
		if (username == NULL) {
			perror("ERROR: Cannot allocate memory");
			return(-1);
		}
		strcpy(username, msg);
	}
	free(msg);
	return(0);
}

/**
 * Processes the pass command
 */
int processPassword(char *msg) {
	msg = getParameters(msg, POP_PASS);
	if (strlen(msg) == 0) {
		return(-1);
	}
	else {
		if (password != NULL)
			free(password);
		password = (char *) malloc(sizeof(char *)*(strlen(msg) + 1));
		if (password == NULL) {
			perror("ERROR: Cannot allocate memory");
			return(-1);
		}
		strcpy(password, msg);
	}
	free(msg);
	return(0);
}

/**
 * Generates the string containing messages and its size
 */
int getListDetails(int msgid, char **msg) {
	char *buffer;
	char sTotalMsg[100];
	char sMesgCount[10];
	char sMesgSize[10];
	int iCounter;
	int startMesgCount = msgid, endMesgCount = msgid;

	int stat_details = 0;
	int totalMsg = list[stat_details].mesgid;

	sprintf(sTotalMsg,"%d",totalMsg);

	buffer = (char *)malloc(sizeof(char *) * 4000);
	if (buffer == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	if (msgid > totalMsg && msgid != -1) {
		strcpy(buffer, POP_ERR);
		strcat(buffer, " no such message, only ");
		strcat(buffer, sTotalMsg);
		strcat(buffer, "  messages in maildrop");
		*msg = buffer;
		return(-1);
	}
	if (msgid == -1) {
		startMesgCount = 1;
		endMesgCount = totalMsg;
	}
	memset(buffer,'\0',4000);
	for (iCounter = startMesgCount; iCounter <= endMesgCount; iCounter++) {
		sprintf(sMesgCount,"%03d",list[iCounter].count);
		sprintf(sMesgSize,"%03d",list[iCounter].mesgsize);
		strcat(buffer,sMesgCount);
		strcat(buffer," ");
		strcat(buffer,sMesgSize);
		strcat(buffer,"\r\n");
	}
	strcat(buffer,POP_OK);
	*msg = buffer;
	return(0);
}

/**
 * Processes the List command
 * Parameter to the list command is optional.
 */
int processList(int sockfd, char *msg) {
	char *buffer;
	int getMesg = -1;
	msg = getParameters(msg, POP_LIST);
	if (strlen(msg) != 0) {
		getMesg = atoi(msg);
	}
	getMesg = getListDetails(getMesg, (char **)&buffer);
	writeClient(sockfd,buffer,TRUE);
	free(buffer);
	free(msg);
	return(getMesg);
}
