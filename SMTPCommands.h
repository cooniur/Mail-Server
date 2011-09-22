/*
 * SMTPCommands.h
 *
 *  Created on: Mar 12, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef SMTPCOMMANDS_H_
#define SMTPCOMMANDS_H_

/*SMTP COMMANDS */
static const char *SMTP_EHLO = "EHLO";
static const char *SMTP_HELO = "HELO";
static const char *SMTP_MAILFROM = "MAIL FROM:";
static const char *SMTP_MAILTO = "RCPT TO:";
static const char *SMTP_DATA = "DATA";
static const char *SMTP_RSET = "RSET";
static const char *SMTP_NOOP = "NOOP";
static const char *SMTP_QUIT = "QUIT";
static const char *SMTP_VRFY = "VRFY";

static const char *SSL_STARTTLS = "STARTTLS";

/* SMTP RESPONSES */
static const char *SMTP_RESP_200 = "200 (nonstandard success response, see rfc876)";
static const char *SMTP_RESP_211 = "211 System status, or system help reply";
static const char *SMTP_RESP_214 = "214 Help message";
static const char *SMTP_RESP_220 = "220 Service ready";
static const char *SMTP_RESP_221 = "221 Service closing transmission channel";
static const char *SMTP_RESP_250 = "250 Requested mail action okay, completed";
static const char *SMTP_RESP_251 = "251 User not local; will forward to <forward-path>";
static const char *SMTP_RESP_354 = "354 Start mail input; end with <CRLF>.<CRLF>";
static const char *SMTP_RESP_421 = "421 <domain> Service not available, closing transmission channel";
static const char *SMTP_RESP_450 = "450 Requested mail action not taken: mailbox unavailable";
static const char *SMTP_RESP_451 = "451 Requested action aborted: local error in processing";
static const char *SMTP_RESP_452 = "452 Requested action not taken: insufficient system storage";
static const char *SMTP_RESP_500 = "500 Syntax error, command unrecognised";
static const char *SMTP_RESP_501 = "501 Syntax error in parameters or arguments";
static const char *SMTP_RESP_502 = "502 Command not implemented";
static const char *SMTP_RESP_503 = "503 Bad sequence of commands";
static const char *SMTP_RESP_504 = "504 Command parameter not implemented";
static const char *SMTP_RESP_521 = "521 <domain> does not accept mail (see rfc1846)";
static const char *SMTP_RESP_530 = "530 Access denied (???a Sendmailism)";
static const char *SMTP_RESP_550 = "550 Requested action not taken: mailbox unavailable";
static const char *SMTP_RESP_551 = "551 User not local; please try <forward-path>";
static const char *SMTP_RESP_552 = "552 Requested mail action aborted: exceeded storage allocation";
static const char *SMTP_RESP_553 = "553 Requested action not taken: mailbox name not allowed";
static const char *SMTP_RESP_554 = "554 Transaction failed";

static const int iSMTP_EHLO = 0;
static const int iSMTP_HELO = 1;
static const int iSMTP_MAILFROM = 2;
static const int iSMTP_MAILTO = 3;
static const int iSMTP_DATA = 4;
static const int iSMTP_RSET = 5;
static const int iSMTP_NOOP = 6;
static const int iSMTP_VRFY = 7;

static const int iSSL_STARTTLS = 8;


#endif /* SMTPCOMMANDS_H_ */
