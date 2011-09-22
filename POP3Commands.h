/*
 * POP3Commands.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef POP3COMMANDS_H_
#define POP3COMMANDS_H_

static const char *POP_USER = "USER";
static const char *POP_PASS = "PASS";
static const char *POP_LIST = "LIST";
static const char *POP_RETR = "RETR";
static const char *POP_DELE = "DELE";
static const char *POP_QUIT = "QUIT";
static const char *POP_STAT = "STAT";
static const char *POP_NOOP = "NOOP";
static const char *POP_RSET = "RSET";
static const char *POP_SSL_STARTTLS = "STARTTLS";

/* POP RESPONSE */
static const char *POP_OK = "+OK";
static const char *POP_ERR = "-ERR";

/* POP RESPONSE MESSAGES */
static const char *POP_SERV_RDY = " POP3 server ready";
static const char *POP_CONNECTION_CLOSE = "Connection closed by foreign host";

static const int iPOP_USER = 0;
static const int iPOP_PASS = 1;
static const int iPOP_LIST = 2;
static const int iPOP_RETR = 3;
static const int iPOP_DELE = 4;
static const int iPOP_STAT = 5;
static const int iPOP_NOOP = 6;
static const int iPOP_RSET = 7;

#endif /* POP3COMMANDS_H_ */
