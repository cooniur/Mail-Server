/*
 * stringProcessor.h
 *
 *  Created on: Mar 17, 2010
 *      Author: Antall Fernandes and Sheetal Ramesh Tatiya
 */

#ifndef STRINGPROCESSOR_H_
#define STRINGPROCESSOR_H_

#include <string.h>
#include <stdio.h>

char * rtrim(char* string, char junk);
char * ltrim(char *string, char junk);
char * substring(const char* str, size_t begin, size_t len);
char * cleanNewLine(char *input);
int replace_str(char **str, char *orig, char *rep);
char * getParameters(char *msg, const char *command);

#endif /* STRINGPROCESSOR_H_ */
