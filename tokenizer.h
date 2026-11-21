/*----------------------------------------------------------------------------
 *
 * tokenizer.h
 *
 * Copyright (c) 2008-2026, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "postgres.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define	PGS_MAX_TOKEN_LEN	1024

#define	PGS_GRAM_LEN		3
#define	PGS_BLANK_CHAR		' '

#define	PGS_FULL_NGRAM

/*
 * The tokenizer API is shared by several files but is not SQL-callable, so
 * keep it out of the shared library's dynamic symbol table. PostgreSQL 16
 * and later already build extensions with -fvisibility=hidden; older
 * versions don't.
 *
 * XXX Remove PGS_INTERNAL once PostgreSQL < 16 is no longer supported.
 */
#if (defined(__GNUC__) || defined(__clang__)) && !defined(WIN32)
#define	PGS_INTERNAL	__attribute__((visibility("hidden")))
#else
#define	PGS_INTERNAL
#endif

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

extern PGS_INTERNAL TokenList *initTokenList(int isset);
extern PGS_INTERNAL void destroyTokenList(TokenList *t);
extern PGS_INTERNAL void printToken(TokenList *t);

extern PGS_INTERNAL void tokenizeByNonAlnum(TokenList *t, char *s);
extern PGS_INTERNAL void tokenizeBySpace(TokenList *t, char *s);
extern PGS_INTERNAL void tokenizeByGram(TokenList *t, char *s);
extern PGS_INTERNAL void tokenizeByCamelCase(TokenList *t, char *s);

#endif							/* TOKENIZER_H */
