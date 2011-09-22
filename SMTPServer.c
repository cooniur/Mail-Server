/*
 * SMTPServer.c
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "SMTPServer.h"

int gSMTPPortNo = 25;

int startSMTPServer() {
	int sockfd;
	int clientfd;
	int quitResp;

	sockfd = getListeningPort(gSMTPPortNo);
	if (sockfd == -1) {
		perror("ERROR: Cannot setup the POP3 Server");
		return(-1);
	}

	for (;;) {
		clientfd = listenForClient(sockfd);
		if (clientfd != -1) {
			quitResp = processSMTPClientRequest(clientfd);
		}
		close (clientfd);
		if (quitResp == -2) break;
	}
	close(sockfd);
	return(0);
}

int setSMTPPortNo(int portNo) {
	gSMTPPortNo = portNo;
	return(0);
}
