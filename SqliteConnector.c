/*
 * SqliteConnector.c
 *
 *  Created on: Mar 15, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "SqliteConnector.h"

//DDL STATEMENTS
static const char *ddl_create_table_users = "CREATE TABLE IF NOT EXISTS users (uname TEXT PRIMARY KEY, pass TEXT NOT NULL)";
static const char *ddl_create_table_mails = "CREATE TABLE IF NOT EXISTS mails (msgid NUMBER PRIMARY KEY, size NUMBER, subject TEXT NOT NULL, body TEXT)";
static const char *ddl_create_table_mailboxes = "CREATE TABLE IF NOT EXISTS mailboxes (msgid NUMBER, sender TEXT NOT NULL, receipt TEXT NOT NULL, deleteflag NUMBER)";

//DML STATEMENTS
static const char *dml_insert_user_uone = "INSERT INTO users VALUES('uone@fit.com','welcome')";
static const char *dml_insert_user_utwo = "INSERT INTO users VALUES('utwo@fit.com','welcome')";
static const char *dml_insert_user_admin = "INSERT INTO users VALUES('admin@fit.com','admin')";
static const char *dml_insert_mails = "INSERT INTO MAILS(MSGID, SIZE, SUBJECT, BODY) VALUES(";
static const char *dml_insert_mailboxes = "INSERT INTO MAILBOXES(MSGID, SENDER, RECEIPT, DELETEFLAG) VALUES(";
static const char *dml_del_msg = "update mailboxes set deleteflag = 1 where ";

//QUERY STATEMENTS
static const char *sel_max_mailid = "SELECT MAX(msgid) from mails";
static const char *sel_list_all = "select mb.msgid, m.size from mails m inner join mailboxes mb on mb.msgid = m.msgid where mb.deleteflag = 0 and receipt =";
static const char *sel_mesg = "select m.msgid, m.size, m.subject, m.body, mb.sender, mb.receipt from mails m inner join mailboxes mb on mb.msgid = m.msgid where ";


int setupDB() {
	int retval;
	sqlite3 *handle;

	printf("SETTING UP DATABASE\n");
	getConnection(DBNAME, &handle);

	retval = sqlite3_exec(handle, ddl_create_table_users, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot setup the database");
		freeConnection(handle);
		return(-1);
	}

	retval = sqlite3_exec(handle, ddl_create_table_mails, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot setup the database");
		freeConnection(handle);
		return(-1);
	}

	retval = sqlite3_exec(handle, ddl_create_table_mailboxes, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot setup the database");
		freeConnection(handle);
		return(-1);
	}

	printf("SETTING UP DATABASE...DONE\n");

	add_user(handle);

	freeConnection(handle);
	return (0);
}


int add_user(sqlite3 *handle) {
	int retval;

	printf("ADDING USER: ADMIN");
	retval = sqlite3_exec(handle, dml_insert_user_admin, 0, 0, 0);
	if (retval == SQLITE_CONSTRAINT) {
		printf("...ALREADY EXISTS\n");
	}
	else if (retval == SQLITE_OK) {
		printf("...DONE\n");
	}

	printf("ADDING USER: UONE");
	retval = sqlite3_exec(handle, dml_insert_user_uone, 0, 0, 0);
	if (retval == SQLITE_CONSTRAINT) {
		printf("...ALREADY EXISTS\n");
	}
	else if (retval == SQLITE_OK) {
		printf("...DONE\n");
	}

	printf("ADDING USER: UTWO");
	retval = sqlite3_exec(handle, dml_insert_user_utwo, 0, 0, 0);
	if (retval == SQLITE_CONSTRAINT) {
		printf("...ALREADY EXISTS\n");
	}
	else if (retval == SQLITE_OK) {
		printf("...DONE\n");
	}
	return(0);
}

int fetch_user_data(char *username, char *password, struct userdata **user) {
	char *query;
	struct userdata *result;
	int col_username = 0;
	int col_password = 1;
	sqlite3_stmt *stmt;
	int retval;
	sqlite3 *handle;
	int rowCount = 0;

	getConnection(DBNAME, &handle);

	generateUserQuery(username, password, &query);
	rowCount = getQueryCount(handle, query);
	if (rowCount > 0) {
		int rCounter = 0;
		retval = sqlite3_prepare_v2(handle, query, -1, &stmt, 0);
		if (retval) {
			printf("Selecting data from DB Failed\n");
			freeConnection(handle);
			return -1;
		}

		result = (struct userdata *) malloc (sizeof (struct userdata *) * rowCount);
		if (result == NULL) {
			perror("ERROR: Cannot allocate memory");
			freeConnection(handle);
			return(-1);
		}

		// Read the number of rows fetched
		// int cols = sqlite3_column_count(stmt);
		while(1)
		{
			// fetch a row's status
			retval = sqlite3_step(stmt);

			if(retval == SQLITE_ROW)
			{
				// SQLITE_ROW means fetched a row
				strcpy(result[rCounter].username,(const char*)sqlite3_column_text(stmt,col_username));
				strcpy(result[rCounter].password,(const char*)sqlite3_column_text(stmt,col_password));
				rCounter++;
			}
			else if(retval == SQLITE_DONE)
			{
				break;
			}
			else
			{
				// Some error encountered
				printf("Some error encountered\n");
				freeConnection(handle);
				return -1;
			}
		}
		sqlite3_finalize(stmt);
		*user = result;
	}

	free(query);
	freeConnection(handle);

	return(rowCount);
}

int freeConnection(sqlite3 *handle) {
	return sqlite3_close(handle);
}

int getConnection(char *dbname, sqlite3 **handle) {
	return sqlite3_open(dbname, handle);
}

int generateUserQuery(char *username, char *password, char **qry) {
	char *query;
	query = (char *) malloc (sizeof(char *) * 1024);
	if (query == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	memset(query,'0',1024);
	strcpy(query,"select * from users");
	if ((username != NULL) || (password != NULL)) {
		strcat(query, " where ");
		if (username != NULL) {
			strcat(query, " uname = '");
			strcat(query,username);
			strcat(query, "'");
		}
		if ((username != NULL) && (password != NULL)) {
			strcat(query, " and ");
		}
		if (password != NULL) {
			strcat(query, " pass = '");
			strcat(query,password);
			strcat(query, "'");
		}
	}
	strcat(query,"\0");
	*qry = query;

	return(0);
}

int deleteMsg(char *username, int mesgid) {
	char *query;
	int retval;
	sqlite3 *handle;
	char sMesgId[10];

	sprintf(sMesgId,"%d",mesgid);

	generateDeleteQuery(username,sMesgId,&query);

	getConnection(DBNAME, &handle);

	retval = sqlite3_exec(handle, ddl_create_table_users, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot setup the database");
		freeConnection(handle);
		return(-1);
	}

	retval = sqlite3_exec(handle, ddl_create_table_users, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot setup the database");
		freeConnection(handle);
		return(-1);
	}

	free(query);
	freeConnection(handle);
	return(retval);
}

int generateDeleteQuery(char *username, char *mesgid, char **qry) {
	//select mb.msgid, m.size from mails m inner join mailboxes mb where receipt = username
	char *query;
	query = (char *) malloc (sizeof(char *) * 1024);
	if (query == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}

	memset(query,'0',1024);
	strcpy(query, dml_del_msg);
	strcat(query, " msgid = ");
	strcat(query, mesgid);
	strcat(query, " and receipt = '");
	strcat(query, username);
	strcat(query,"'");
	strcat(query,"\0");
	*qry = query;

	return(0);
}


int generateListQuery(char *username, char **qry) {
	//select mb.msgid, m.size from mails m inner join mailboxes mb where receipt = username
	char *query;
	query = (char *) malloc (sizeof(char *) * 1024);
	if (query == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	memset(query,'0',1024);
	strcpy(query,sel_list_all);
	strcat(query, "'");
	strcat(query,username);
	strcat(query, "'");
	strcat(query,"\0");
	*qry = query;

	return(0);
}


int getQueryCount(sqlite3 *handle, const char *query) {
	char countQuery[1024];
	sqlite3_stmt *stmt;
	int retval;

	memset(countQuery,'0',1024);
	strcpy(countQuery,"Select count(*) as Count from (");
	strcat(countQuery,query);
	strcat(countQuery,")\0");

	retval = sqlite3_prepare_v2(handle, countQuery, -1, &stmt, 0);
	if (retval) {
		printf("Selecting data from DB Failed\n");
		return -1;
	}

	// Read the number of rows fetched
	// int cols = sqlite3_column_count(stmt);
	retval = sqlite3_step(stmt);

	if(retval == SQLITE_ROW)
		retval = sqlite3_column_int(stmt,0);
	else
		retval = 0;
	sqlite3_finalize(stmt);
	return retval;
}


int storeMail(char *from, char to[50][100], char *subject, char *msg, int noRcpt) {
	char query[4000];
	char msgId[1000];
	char msgSize[1000];
	sqlite3 *handle;
	int iCount;
	int retval;

	sprintf( msgId, "%d", getNextMailId());
	sprintf( msgSize, "%d", (int)strlen(msg));

	memset(query,'0',4000);
	//INSERT MAIL
	strcpy(query,dml_insert_mails);
	strcat(query,msgId);
	strcat(query,", ");
	strcat(query,msgSize);
	strcat(query,", '");
	strcat(query,subject);
	strcat(query,"', '");
	strcat(query,msg);
	strcat(query,"')");

	getConnection(DBNAME, &handle);
	retval = sqlite3_exec(handle, query, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot insert data in the table");
		freeConnection(handle);
		return(-1);
	}

	//INSERT MAILBOX

	memset(query,'0',4000);
	for (iCount = 0; iCount < noRcpt; iCount++) {
		strcpy(query,dml_insert_mailboxes);
		strcat(query,msgId);
		strcat(query,", '");
		strcat(query,from);
		strcat(query,"', '");
		strcat(query,to[iCount]);
		strcat(query,"', 0)");

		retval = sqlite3_exec(handle, query, 0, 0, 0);
		if (retval == -1) {
			perror("ERROR: Cannot insert data in the table");
			freeConnection(handle);
			return(-1);
		}
	}

	memset(query,'0',4000);
	strcpy(query,"commit");

	retval = sqlite3_exec(handle, query, 0, 0, 0);
	if (retval == -1) {
		perror("ERROR: Cannot insert data in the table");
		freeConnection(handle);
		return(-1);
	}

	freeConnection(handle);
	return(0);
}


int getNextMailId() {
	sqlite3_stmt *stmt;
	int retval;
	sqlite3 *handle;

	getConnection(DBNAME, &handle);

	retval = sqlite3_prepare_v2(handle, sel_max_mailid, -1, &stmt, 0);
	if (retval) {
		printf("Selecting data from DB Failed\n");
		freeConnection(handle);
		return -1;
	}

	// Read the number of rows fetched
	// int cols = sqlite3_column_count(stmt);
	retval = sqlite3_step(stmt);

	if(retval == SQLITE_ROW)
		retval = sqlite3_column_int(stmt,0) + 1;
	else
		retval = 1;

	sqlite3_finalize(stmt);
	freeConnection(handle);
	return(retval);
}

int replaceBind(char **stmt, char *orig, char *replacement) {
	return replace_str(stmt, orig, replacement);
}


int fetch_list_data(char *username, struct listdata **list) {
	char *query;
	struct listdata *result;
	int col_mesgid = 0;
	int col_size = 1;
	sqlite3_stmt *stmt;
	int retval;
	sqlite3 *handle;
	int rowCount = 0;
	int stat_details = 0;
	int stat_sizesum = 0;

	generateListQuery(username, &query);
	getConnection(DBNAME, &handle);
	rowCount = getQueryCount(handle, query);

	result = (struct listdata *) malloc (sizeof (struct listdata *) * (rowCount + 1));
	if (result == NULL) {
		perror("ERROR: Cannot allocate memory");
		freeConnection(handle);
		return(-1);
	}

	if (rowCount > 0) {
		int rCounter = 0;
		retval = sqlite3_prepare_v2(handle, query, -1, &stmt, 0);
		if (retval) {
			printf("Selecting data from DB Failed\n");
			freeConnection(handle);
			return -1;
		}

		rCounter++;

		// Read the number of rows fetched
		// int cols = sqlite3_column_count(stmt);
		while(1)
		{
			// fetch a row's status
			retval = sqlite3_step(stmt);

			if(retval == SQLITE_ROW)
			{
				// SQLITE_ROW means fetched a row
				result[rCounter].count = rCounter;
				result[rCounter].mesgid = sqlite3_column_int(stmt,col_mesgid);
				result[rCounter].mesgsize = sqlite3_column_int(stmt,col_size);
				stat_sizesum += result[rCounter].mesgsize;
				rCounter++;
			}
			else if(retval == SQLITE_DONE)
			{
				break;
			}
			else
			{
				// Some error encountered
				printf("Some error encountered\n");
				freeConnection(handle);
				return -1;
			}
		}
		sqlite3_finalize(stmt);
	}

	//SET STAT DETAILS
	result[stat_details].count = stat_details;
	result[stat_details].mesgid = rowCount;
	result[stat_details].mesgsize = stat_sizesum;


	*list = result;

	free(query);
	freeConnection(handle);

	return(rowCount);
}

int generateMesgQuery(char *username, char *mesgid, char **qry) {
	char *query;
	query = (char *) malloc (sizeof(char *) * 1024);
	if (query == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	memset(query,'0',1024);
	strcpy(query,sel_mesg);
	strcat(query, " m.msgid = ");
	strcat(query, mesgid);
	strcat(query, " and mb.receipt = '");
	strcat(query, username);
	strcat(query,"'");
	strcat(query,"\0");
	*qry = query;

	return(0);
}

int fetch_mesg_data(char *username, int mesgId, struct emaildata **mail) {
	char *query;
	struct emaildata *result;
	int col_mesgId = 0;
	int col_mesgSize = 1;
	int col_mesgSubject = 2;
	int col_mesgBody = 3;
	int col_mesgFrom = 4;
	int col_mesgTo = 5;
	char sMesgId[10];
	sqlite3_stmt *stmt;
	int retval;
	sqlite3 *handle;
	int rowCount = 0;

	getConnection(DBNAME, &handle);

	sprintf(sMesgId, "%d", mesgId);

	generateMesgQuery(username, sMesgId, &query);
	rowCount = getQueryCount(handle, query);
	if (rowCount > 0) {
		int rCounter = 0;
		retval = sqlite3_prepare_v2(handle, query, -1, &stmt, 0);
		if (retval) {
			printf("Selecting data from DB Failed\n");
			freeConnection(handle);
			return -1;
		}

		result = (struct emaildata *) malloc (sizeof (struct emaildata *) * rowCount);
		if (result == NULL) {
			perror("ERROR: Cannot allocate memory");
			freeConnection(handle);
			return(-1);
		}

		// Read the number of rows fetched
		// int cols = sqlite3_column_count(stmt);
		while(1)
		{
			// fetch a row's status
			retval = sqlite3_step(stmt);

			if(retval == SQLITE_ROW)
			{
				// SQLITE_ROW means fetched a row
				result[rCounter].mesgid = sqlite3_column_int(stmt,col_mesgId);
				result[rCounter].mesgsize = sqlite3_column_int(stmt,col_mesgSize);
				strcpy(result[rCounter].subject,(const char*)sqlite3_column_text(stmt,col_mesgSubject));
				result[rCounter].body = (char *)malloc(sizeof(char *)*result[rCounter].mesgsize);
				if (result[rCounter].body == NULL) {
					perror("ERROR: Cannot allocate memory");
					sqlite3_finalize(stmt);
					free(query);
					freeConnection(handle);
					return(-1);
				}
				strcpy(result[rCounter].body,(const char*)sqlite3_column_text(stmt,col_mesgBody));
				//From
				result[rCounter].from = (char *)malloc(sizeof(char *)*50);
				if (result[rCounter].from == NULL) {
					perror("ERROR: Cannot allocate memory");
					sqlite3_finalize(stmt);
					free(query);
					freeConnection(handle);
					return(-1);
				}
				strcpy(result[rCounter].from,(const char*)sqlite3_column_text(stmt,col_mesgFrom));
				//TO
				result[rCounter].to = (char *)malloc(sizeof(char *)*50);
				if (result[rCounter].to == NULL) {
					perror("ERROR: Cannot allocate memory");
					sqlite3_finalize(stmt);
					free(query);
					freeConnection(handle);
					return(-1);
				}
				strcpy(result[rCounter].to,(const char*)sqlite3_column_text(stmt,col_mesgTo));
				rCounter++;
			}
			else if(retval == SQLITE_DONE)
			{
				break;
			}
			else
			{
				// Some error encountered
				printf("Some error encountered\n");
				freeConnection(handle);
				return -1;
			}
		}
		sqlite3_finalize(stmt);
		*mail = result;
	}

	free(query);
	freeConnection(handle);

	return(rowCount);
}
