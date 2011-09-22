/*
 * POP3Server.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef POP3SERVER_H_
#define POP3SERVER_H_

#include <unistd.h>
#include "POP3Processor.h"
#include "Server.h"

extern int startPOP3Server();
extern int setPOP3PortNo(int portNo);
int processPOPClientRequest(int clientfd);

#endif /* POP3SERVER_H_ */
