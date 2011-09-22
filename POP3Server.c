/*
 * POP3Server.c
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "POP3Server.h"
#include <unistd.h>


int gPOP3PortNo = 110;

int startPOP3Server() {
	int sockfd;
	int clientfd;
	int quitResp;

	sockfd = getListeningPort(gPOP3PortNo);
	if (sockfd == -1) {
		perror("ERROR: Cannot setup the POP3 Server");
		return(-1);
	}

	for (;;) {
		clientfd = listenForClient(sockfd);
		if (clientfd != -1) {
			quitResp = processPOPClientRequest(clientfd);
		}
		close (clientfd);
		if (quitResp == -2) break;
	}
	close(sockfd);
	return(0);
}


int setPOP3PortNo(int portNo) {
	gPOP3PortNo = portNo;
	return(0);
}


