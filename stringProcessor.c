/*
 * stringProcessor.c
 *
 *  Created on: Mar 17, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#include "stringProcessor.h"

/**
 * RTRIM A STRING BASED ON THE CHARACTER SPECIFIED
 */
char* rtrim(char* string, char junk) {
	int slength = strlen(string);
	if (slength) {
		char* original = string + slength;
		while(*--original == junk);
		*(original + 1) = '\0';
	}
    return string;
}


/**
 * LTRIM A STRING BASED ON THE CHARACTER SPECIFIED
 */
char* ltrim(char *string, char junk) {
    char* original = string;
    char *p = original;
    int trimmed = 0;
    do
    {
        if (*original != junk || trimmed)
        {
            trimmed = 1;
            *p++ = *original;
        }
    }
    while (*original++ != '\0');
    return string;
}

/**
 * FINDS A SUBSTRING OF A STRING
 */
char* substring(const char* str, size_t begin, size_t len)
{
	if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
		return 0;
	return strndup(str + begin, len);
}

/**
 * USED TO REMOVE THE NEW LINE CHARACTERS (\r\n)
 */
char * cleanNewLine(char *input) {
	int i;
	for ( i = 0; i < strlen(input); i++ ) {
		if ( input[i] == '\r') {
			input[i] = '\0';
			break;
		}
		else if ( input[i] == '\n' ) {
	        input[i] = '\0';
	        break;
	    }
	}
	return(input);
}

/**
 * REPLACES A SUBSTRING FROM A STRING.
 */
int replace_str(char **str, char *orig, char *rep)
{
	char buffer[4096];
	char *p;

	if(!(p = strstr(*str, orig)))  // Is 'orig' even in 'str'?
		return 0;

	strncpy(buffer, *str, p-*str); // Copy characters from 'str' start to 'orig' st$
	buffer[p-*str] = '\0';

	sprintf(buffer+(p-*str), "%s%s", rep, p+strlen(orig));

	*str = buffer;
	return(0);
}

/**
 * EXTRACTS THE PARAMETERS PASSED TO ANY SERVER COMMANDS
 */
char * getParameters(char *msg, const char *command) {
	return rtrim(ltrim(substring(msg, strlen(command), strlen(msg) - strlen(command)),' '),' ');
}
