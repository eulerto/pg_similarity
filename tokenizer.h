/*----------------------------------------------------------------------------
 *
 * tokenizer.h
 *
 * Copyright (c) 2008-2020, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "postgres.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define	PGS_MAX_TOKEN_LEN	1024

#define	PGS_GRAM_LEN		3
#define	PGS_BLANK_CHAR		' '

#define	PGS_FULL_NGRAM

typedef struct Token
{
	char		*data;	/* token data */
	int		freq;	/* frequency */
	struct Token	*next;	/* next token */
} Token;

typedef struct TokenList
{
	int	isset;	/* is a set? */
	int	size;	/* list size */
	Token	*head;	/* first token */
	Token	*tail;	/* last token */
} TokenList;

TokenList *initTokenList(int isset);
void destroyTokenList(TokenList *t);
int addToken(TokenList *t, char *s);
int removeToken(TokenList *t);
Token *searchToken(TokenList *t, char *s);
void printToken(TokenList *t);

void tokenizeByNonAlnum(TokenList *t, char *s);
void tokenizeBySpace(TokenList *t, char *s);
void tokenizeByGram(TokenList *t, char *s);
void tokenizeByCamelCase(TokenList *t, char *s);
