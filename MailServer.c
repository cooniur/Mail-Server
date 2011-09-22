/*
 * MailServer.c
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */


//#include "SqliteConnector.h"
#include "POP3Server.h"
#include "SMTPServer.h"
#include <stdio.h>


void process_commandline(int argc, char *argv[]);

int main(int argc, char * argv[]) {
	int POP3ServerResp = 0;
	int SMTPServerResp = 0;

	process_commandline(argc, argv);
	setupDB();

//	printf("STARTING POP3 SERVER...\n");
//	if (fork()==0) {
//		POP3ServerResp = startPOP3Server();
//		if (POP3ServerResp == -1) {
//			perror("ERROR: Cannot start the POP3 Server");
//			return(-1);
//		}
//		return(0);
//	}

	printf("STARTING SMTP SERVER...\n");
//	if (fork()==0) {
		SMTPServerResp = startSMTPServer();
		if (SMTPServerResp == -1) {
			perror("ERROR: Cannot start the SMTP Server");
			return(-1);
		}
		return(0);
//	}

//	return(0);
}

void process_commandline(int argc, char *argv[]) {
	int cnt;
	for (cnt = 1; cnt < argc; cnt++) {
		if (argv[cnt][0]=='-') {
			switch(argv[cnt++][1]) {
				case 's':
					setSMTPPortNo(atoi(argv[cnt]));
					break;
				case 'p':
					setPOP3PortNo(atoi(argv[cnt]));
					break;
			}
		}
	}
}
