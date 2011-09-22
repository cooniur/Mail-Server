/*
 * SMTPServer.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef SMTPSERVER_H_
#define SMTPSERVER_H_

#include <stdio.h>
#include <unistd.h>
#include "SMTPProcessor.h"
#include "Server.h"

extern int startSMTPServer();
extern int setSMTPPortNo(int portNo);
int processSMTPClientRequest(int clientfd);

#endif /* SMTPSERVER_H_ */
