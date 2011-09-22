/*
 * Server.c
 *
 *  Created on: Mar 11, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */


#include "Server.h"

#define MAX_CALLS 5

socklen_t addr_len = sizeof(struct sockaddr_in);
SSL * ssl = NULL;

/**
 * SETS UP THE CONNECTIONS AND SOCKETS FOR THE SERVER
 */
int getListeningPort(int portNo) {
	struct sockaddr_in server;
	int sockfd;
	int true = 1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("ERROR: Cannot open a socket");
		return sockfd;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) == -1) {
        perror("ERROR: Cannot set the socket options");
        return(-1);
    }

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(portNo);

	if (bind(sockfd, (struct sockaddr *)&server, addr_len) < 0) {
		perror("ERROR: Cannot bind to the sock");
		return(-1);
	}

	if (listen(sockfd, MAX_CALLS) < 0) {
		perror("ERROR: Cannot setup the sock");
		return(-1);
	}

	return(sockfd);
}

/**
 * LISTENS FOR ANY INCOMING CONNECTIONS ON THE SPECIFIED SOCKET
 */
int listenForClient(int socket) {
	int cfd;
	struct sockaddr_in client;

	if ((cfd = accept (socket, (struct sockaddr *)& client, &addr_len)) < 0) {
		perror("ERROR: Cannot setup the sock");
		return(-1);
	}
	return(cfd);
}

/**
 * API TO READ DATA FROM THE CLIENT CONNECTION
 * Parameters:
 * int sockfd - the socket connection to client
 * char *msg - incoming message
 */

int readClient(int sockfd, char **msg) {
	char rbuffer[4000];
	char wbuffer[4000];
	if (ssl == NULL) {
		read(sockfd,rbuffer,sizeof(rbuffer));
	}
	else {
		SSL_read(ssl, rbuffer, sizeof(rbuffer));
		ERR_print_errors_fp(stderr);
	}
	memset(wbuffer,0,1024);
	strcpy(wbuffer, cleanNewLine(rbuffer));
	*msg = (char *) malloc (sizeof(char *) * strlen(wbuffer) + 1);
	if (*msg == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	strcpy(*msg, wbuffer);
	return(strlen(*msg));
}

int readClientData(int sockfd, char **msg) {
	char rbuffer[4000];
	if (ssl == NULL) {
		read(sockfd,rbuffer,sizeof(rbuffer));
	}
	else {
		SSL_read(ssl, rbuffer, sizeof(rbuffer));
		ERR_print_errors_fp(stderr);
	}
	*msg = (char *) malloc (sizeof(char *) * strlen(rbuffer) + 1);
	if (*msg == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	strcpy(*msg, rbuffer);
	return(strlen(*msg));
}


/**
 * API TO SEND DATA BACK TO THE CLIENT CONNECTION
 * Parameters:
 * int sockfd - the socket connection to client
 * char *msg - message to be sent
 * int insertCRLR - add a new line carriage at the end of message (values: TRUE/FALSE)
 */
int writeClient(int sockfd, const char *msg, int insertCRLR) {
	char *localbuff;
	localbuff = (char *)malloc(sizeof(char *) * (strlen(msg) + strlen("\r\n")));
	if (localbuff == NULL) {
		perror("ERROR: Cannot allocate memory");
		return(-1);
	}
	strcpy(localbuff, msg);
	if (insertCRLR) {
		strcat(localbuff,"\r\n");
	}
	if (ssl == NULL)
		write(sockfd, localbuff, strlen(localbuff));
	else
		SSL_write(ssl, localbuff, strlen(localbuff));
	free(localbuff);
	return(0);
}

void LoadCerts(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
	/* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) ) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

void printCerts(SSL* ssl) {
	X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);	/* Get certificates (if available) */
    if ( cert != NULL ) {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}


int setupSSLCommunication(int client) {
	SSL_METHOD *method;
	SSL_CTX *ctx;
	int err;
	ssl = NULL;

	err = SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = SSLv23_server_method(); /* create new server-method instance */
	ctx = SSL_CTX_new(method);
	if ( ctx == NULL ) {
		ERR_print_errors_fp(stderr);
		abort();
	}

	LoadCerts(ctx, "host.pem", "host.pem");	/* load certs */

	ssl = SSL_new(ctx);         					/* get new SSL state with context */

	err = SSL_set_fd(ssl, client);
	err = SSL_accept(ssl);
	return(1);
}

int shutdownSSL() {
	if (ssl!=NULL) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}
}

