/*
 * SqliteConnector.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef SQLITECONNECTOR_H_
#define SQLITECONNECTOR_H_


#include <stdio.h>
#include "sqlite3.h"
#include <stdlib.h>
#include <string.h>
#include "stringProcessor.h"


#define DBNAME "mailPro.db"

struct listdata {
	int count;
	int mesgid;
	int mesgsize;
};

struct userdata {
	char username[50];
	char password[50];
};

struct maildata {
	int mesgid;
	int mesgsize;
	char subject[50];
	char *body;
};

struct emaildata {
	int mesgid;
	int mesgsize;
	char subject[50];
	char *body;
	char *from;
	char *to;
};


int add_user(sqlite3 *handle);
int fetch_user_data(char *username, char *password, struct userdata **user);
int freeConnection(sqlite3 *handle);
int getConnection(char *dbname, sqlite3 **handle);
int setupDB();
int getQueryCount(sqlite3 *handle, const char *query);
int generateUserQuery(char *username, char *password, char **qry);
int storeMail(char *from, char to[50][100], char *subject, char *msg, int noRcpt);
int getNextMailId();
int replaceBind(char **stmt, char *orig, char *replacement);
int fetch_list_data(char *username, struct listdata **list);
int generateListQuery(char *username, char **qry);
int deleteMsg(char *username, int mesgid);
int generateDeleteQuery(char *username, char *mesgid, char **qry);
int fetch_mesg_data(char *, int mesgId, struct emaildata **mail);

#endif /* SQLITECONNECTOR_H_ */
