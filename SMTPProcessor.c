/*
 * SMTPProcessor.c
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "SMTPProcessor.h"


#define FALSE 0
#define TRUE 1

struct maild *mail = NULL;
int previousState, currentState;

//STATE TABLE
static int stateGraph[9][9] = {
								{1,1,1,0,0,1,1,1,1}, //EHLO,
								{1,1,1,0,0,1,1,1,1}, //HELO
								{0,0,0,1,0,1,1,1,0}, //MAILFROM
								{0,0,0,1,1,1,1,1,0}, //MAILTO
								{1,1,1,0,0,1,1,1,0}, //DATA
								{1,1,1,0,0,1,1,1,1}, //RSET
								{1,1,1,0,0,1,1,1,1}, //NOOP
								{1,1,1,0,0,1,1,1,1}, //VRFY
								{1,1,1,0,0,1,1,1,1}  //STARTTLS

};

/**
 * PROCESS EACH OF THE CLIENTS COMMANDS
 */
int processSMTPClientRequest(int clientfd) {
	int loop = TRUE;
	char *buffer;
	previousState = iSMTP_HELO;
	currentState = iSMTP_HELO;

	sendSMTPWelcomeGreeting(clientfd);
	while(loop) {//CLIENT IS CONNECTED AND LOOP UNTIL HE QUITS
		readClient(clientfd, &buffer);


		//TEST FOR QUIT
		if (strncasecmp(buffer, "kill", strlen("kill")) == 0) {//KILL SMTP SERVER
			free(buffer);
			return(-2);
		}
		else if (strncasecmp(buffer, SMTP_QUIT, strlen(SMTP_QUIT)) == 0) {//EXIT WHILE LOOP
			if (processSMTPQuit(clientfd, buffer) == 0) {
				loop = FALSE;
				writeClient(clientfd, SMTP_RESP_221, TRUE);
			}
		}
		else
			processSMTPCommands(clientfd, buffer); //PROCESS EVERY OTHER SMTP COMMAND
		free(buffer);
	}
	return(0);
}




int sendSMTPWelcomeGreeting(int sockfd) {
	char *msg = "220 ESMTP MailPro Service, Version: 0.01";
	writeClient(sockfd, msg, TRUE);
	return(0);
}



int processSMTPCommands(int sockfd, char *msg) {
	if (strncasecmp(msg, SSL_STARTTLS, strlen(SSL_STARTTLS)) == 0) { //EHLO
		if (checkSMTPStates(iSSL_STARTTLS)) {
			write(sockfd, "220 2.0.0 Ready to start TLS\r\n", strlen("220 2.0.0 Ready to start TLS\r\n"));
			setupSSLCommunication(sockfd);
		}
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_EHLO, strlen(SMTP_EHLO)) == 0) { //EHLO
		if (checkSMTPStates(iSMTP_EHLO)) {
			writeClient(sockfd, "250-ESMTP MailPro Service, Version: 0.01", TRUE);
			writeClient(sockfd, "250-STARTTLS", TRUE);
			writeClient(sockfd, "250 DSN", TRUE);
		}
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_HELO, strlen(SMTP_HELO)) == 0) { //HELO
		if (checkSMTPStates(iSMTP_HELO))
			processHelo(sockfd, msg);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_MAILFROM, strlen(SMTP_MAILFROM)) == 0) { //MAIL FROM
		if (checkSMTPStates(iSMTP_MAILFROM))
			processMailFrom(sockfd, msg);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_MAILTO, strlen(SMTP_MAILTO)) == 0) { //MAIL TO
		if (checkSMTPStates(iSMTP_MAILTO))
			processMailTo(sockfd, msg);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_DATA, strlen(SMTP_DATA)) == 0) { //DATA
		if (checkSMTPStates(iSMTP_DATA)) {
			msg = rtrim(msg,':');
			processData(sockfd, msg);
		}
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_RSET, strlen(SMTP_RSET)) == 0) { //RSET
		if (checkSMTPStates(iSMTP_RSET))
			processRset(sockfd, msg);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_NOOP, strlen(SMTP_NOOP)) == 0) { //NOOP
		if (checkSMTPStates(iSMTP_NOOP))
			writeClient(sockfd, SMTP_RESP_250, TRUE);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else if (strncasecmp(msg, SMTP_VRFY, strlen(SMTP_VRFY)) == 0) { //VERFY
		if (checkSMTPStates(iSMTP_VRFY))
			return processVerify(sockfd, msg);
		else
			writeClient(sockfd, SMTP_RESP_503, TRUE);
	}
	else {
		writeClient(sockfd, SMTP_RESP_500, TRUE);
	}
	return(0);
}

/*
 * PROCESSES THE "RCPT TO" COMMAND
 * ADDS ADDRESS TO THE RECIPIENT LIST
 */
int processMailTo(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_MAILTO);
	if (strlen(msg) == 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	else {
		strcpy(mail->to[mail->noRcpt++],msg);
		writeClient(sockfd, SMTP_RESP_250, TRUE);
	}
	free(msg);
	return(0);
}

int checkSMTPStates(int currState) {
	int stateValid = stateGraph[currentState][currState];
	if (stateValid) {
		previousState = currentState;
		currentState = currState;
	}
	return(stateValid);

}

int processMailFrom(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_MAILFROM);
	if (strlen(msg) == 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	else {

		msg = ltrim(rtrim(msg,'>'),'<');
		mail = (struct maild *) malloc (sizeof(struct maild *));
		if (mail == NULL) {
			perror("ERROR: Cannot allocate memory");
			free(msg);
			return(-1);
		}

		mail->from = (char *) malloc (sizeof(char *) * strlen(msg) + 1);
		if (mail->from == NULL) {
			perror("ERROR: Cannot allocate memory");
			free(msg);
			return(-1);
		}
		strcpy(mail->from,msg);
		strcat(mail->from,"\0");

		mail->noRcpt = 0;

		writeClient(sockfd, SMTP_RESP_250, TRUE);
	}
	free(msg);
	return(0);
}

/**
 * PROCESS THE HELO REQUEST
 */
int processHelo(int sockfd, char *msg) {
	writeClient(sockfd, SMTP_RESP_250, TRUE);
	return(0);
}

/**
 * Processes the QUIT SMTP command
 * Parameters:
 * 		sockfd: client socket number
 * 		msg: The quit message
 * Returns:
 * 		-1: Incorrect Quit Command
 * 		0 : Successful Quit Command
 */
int processSMTPQuit(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_QUIT);
	if (strlen(msg) != 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	resetVariables();
	shutdownSSL();
	free(msg);
	return(0);
}

/**
 * Processes the DATA SMTP command
 * Calls the storeMail function for the database to store the message.
 *
 * Parameters:
 * 		sockfd: client socket number
 * 		msg: The DATA command
 * Returns:
 * 		-1: Incorrect DATA Command
 * 		0 : Successful DATA Command
 */
int processData(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_DATA);
	if (strlen(msg) != 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	else {
		int resp;
		char *buffer;
		writeClient(sockfd, SMTP_RESP_354, TRUE);

		readDataFromClient(sockfd, &buffer);

		mail->msg = (char *) malloc (sizeof(char *) * (strlen(buffer) + 1));
		if (mail->msg == NULL) {
			perror("ERROR: Cannot allocate memory");
			return(-1);
		}
		strcpy(mail->msg, buffer);
		strcpy(mail->msg,rtrim(mail->msg,'\n'));
		strcpy(mail->msg,rtrim(mail->msg,'\r'));
		free(buffer);

		mail->subject = (char *) malloc (sizeof(char *) * 11);
		if (mail->subject == NULL) {
			perror("ERROR: Cannot allocate memory");
			return(-1);
		}

		if (strlen(mail->msg) < 10)
			strcpy(mail->subject, mail->msg);
		else {

			strcpy(mail->subject, substring(mail->msg,0,10));
		}

		strcpy(mail->subject, cleanNewLine(mail->subject));

		resp = storeMail(mail->from, mail->to, mail->subject, mail->msg, mail->noRcpt);
		if (resp == -1) {
			writeClient(sockfd, SMTP_RESP_554, TRUE);
			return(-1);
		}
		writeClient(sockfd, SMTP_RESP_250, TRUE);
	}
	free(msg);
	return(0);
}

/**
 * Reads the email data being sent from the client.
 */
int readDataFromClient(int sockfd, char **msg) {
	char *data;
	char *buffer = NULL;
	while(1) {
		readClientData(sockfd, &data);

		if (buffer == NULL) {
			buffer = realloc(buffer,(sizeof(char *)*(strlen(data)+1)));
			if (buffer == NULL) {
				perror("ERROR: Cannot allocate memory");
				return(-1);
			}
			strcpy(buffer, data);
		}
		else {
			buffer = realloc(buffer,(sizeof(char *)*(strlen(data) + strlen("\r\n") +1)) + strlen(buffer));
			if (buffer == NULL) {
				perror("ERROR: Cannot allocate memory");
				return(-1);
			}
			strcat(buffer,"\r\n");
			strcat(buffer,data);
		}
		if (strstr(buffer,"\r\n.\r\n") != NULL)
			break; // BREAK IS /r/n./r/n IS ENCOUNTERED
	}
	*msg = (char *) malloc (sizeof(char *) * strlen(buffer) + 1);
	if (*msg == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	memcpy(*msg, data, strlen(data));
	return(0);
}

/**
 * RESETS THE STATE TABLE
 */
int resetVariables() {
	if (mail != NULL) {
		free(mail);
	}
	previousState = iSMTP_HELO;
	currentState = iSMTP_HELO;
	return(0);
}

/**
 * PROCESSES THE RSET COMMAND TO RESET THE TABLES
 */
int processRset(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_RSET);
	if (strlen(msg) != 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	else {
		resetVariables();
		writeClient(sockfd, SMTP_RESP_250, TRUE);
	}
	free(msg);
	return(0);
}

/**
 * PROCESS THE VRFY COMMAND
 * Looks up the user from the database
 */
int processVerify(int sockfd, char *msg) {
	msg = getParameters(msg, SMTP_VRFY);
	if (strlen(msg) == 0) {
		writeClient(sockfd, SMTP_RESP_501, TRUE);
		free(msg);
		return(-1);
	}
	else {
		struct userdata *user;
		if (fetch_user_data(msg, NULL, &user)) {//fetch user from database
			writeClient(sockfd, "250 ", FALSE);
			writeClient(sockfd, msg, TRUE);
			free(user);
		}
		else
			writeClient(sockfd, SMTP_RESP_550, TRUE);
	}
	free(msg);
	return(0);
}

