/*----------------------------------------------------------------------------
 *
 * tokenizer.c
 *
 * Tokenization support functions
 *
 * Tokens are stored in a linked list to make manipulation easy.
 * We have support for three types of tokenization:
 * (i)   space: is treated as delimiter;
 * (ii)  non-alphanumeric: is treated as delimiter;
 * (iii) n-gram: token is a n-character "window".
 *
 *
 * Copyright (c) 2008-2018, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "tokenizer.h"


TokenList *initTokenList(int a)
{
	TokenList	*t;

	t = (TokenList *) malloc(sizeof(TokenList));

	t->isset = a;
	t->size = 0;
	t->head = NULL;
	t->tail = NULL;

	elog(DEBUG4, "t->isset: %d", t->isset);

	return t;
}

void destroyTokenList(TokenList *t)
{
	char	*n;
	int		i;
	int		len;

	while (t->size > 0)
	{
		len = strlen(t->head->data);
		n = (char *) malloc(sizeof(char) * (len + 1));
		strcpy(n, t->head->data);

		i = removeToken(t);
		if (i == 0)
			elog(DEBUG3, "token \"%s\" removed; actual token list size: %d", n, t->size);
		else
			elog(DEBUG3, "failed to remove token: \"%s\"", n);

		free(n);
	}

	free(t);
}

int addToken(TokenList *t, char *s)
{
	Token	*n;

	if (t->isset)
	{
		Token *x = searchToken(t, s);
		if (x != NULL)
		{
			x->freq++;

			elog(DEBUG3, "token \"%s\" is already in the list; frequency: %d", s, x->freq);

			/* Different error code to allow memory to be freed by calling function */
			return -2;
		}
	}

	n = (Token *) malloc(sizeof(Token));
	if (n == NULL)
		return -1;

	/*
	 * memory is allocated by tokenizeByXXX()
	 */
	n->data = s;
	n->freq = 1;	/* first token */

	if (t->size == 0)
		t->tail = n;

	n->next = t->head;
	t->head = n;

	t->size++;

	return 0;
}

/*
 * free up the head node and its content
 */
int removeToken(TokenList *t)
{
	Token	*n;

	if (t->size == 0)
	{
		elog(DEBUG3, "list is empty");

		return -1;
	}

	n = t->head;
	t->head = n->next;

	if (t->size == 1)
		t->tail = NULL;

	free(n->data);
	free(n);

	t->size--;

	return 0;
}

Token *searchToken(TokenList *t, char *s)
{
	Token	*n;

	n = t->head;
	while (n != NULL)
	{
#ifdef PGS_IGNORE_CASE
		/*
		 * For portability reason, use pg_strcasecmp instead of strcasecmp
		 * (Windows doesn't provide this function).
		 */
		if (pg_strcasecmp(n->data, s) == 0)
#else
		if (strcmp(n->data, s) == 0)
#endif
		{
			elog(DEBUG4, "\"%s\" found", n->data);

			return n;
		}
		n = n->next;
	}

	return NULL;
}

void printToken(TokenList *t)
{
	Token	*n;

	elog(DEBUG3, "===================================================");

	if (t->size == 0)
		elog(DEBUG3, "word list is empty");

	n = t->head;
	while (n != NULL)
	{
		elog(DEBUG3, "addr: %p; next: %p; word: %s; freq: %d", n, n->next, n->data,
			 n->freq);

		n = n->next;
	}

	if (t->head != NULL)
		elog(DEBUG3, "head: %s", t->head->data);
	if (t->tail != NULL)
		elog(DEBUG3, "tail: %s", t->tail->data);
	elog(DEBUG3, "===================================================");
}

/*
 * XXX non alnum characters are ignored in this function
 * XXX because they are treated as delimiter characters
 */
void tokenizeByNonAlnum(TokenList *t, char *s)
{
	const char		*cptr,	/* current pointer */
			  *sptr;	/* start token pointer */
	int			c = 0;	/* number of bytes */

	elog(DEBUG3, "sentence: \"%s\"", s);

	if (t->size == 0)
		elog(DEBUG3, "token list is empty");
	else
		elog(DEBUG3, "token list contains %d tokens", t->size);

	if (t->head == NULL)
		elog(DEBUG3, "there is no head token yet");
	else
		elog(DEBUG3, "head token is \"%s\"", t->head->data);

	if (t->tail == NULL)
		elog(DEBUG3, "there is no tail token yet");
	else
		elog(DEBUG3, "tail token is \"%s\"", t->tail->data);

	cptr = sptr = s;

	while (*cptr)
	{
		while (!isalnum(*cptr) && *cptr != '\0')
		{
			elog(DEBUG4, "\"%c\" is non alnum", *cptr);

			cptr++;
		}

		if (*cptr == '\0')
			elog(DEBUG4, "end of sentence");

#ifdef PGS_IGNORE_CASE
		*sptr = tolower(*sptr);
#endif

		sptr = cptr;

		elog(DEBUG4, "token's first char: \"%c\"", *sptr);

		while (isalnum(*cptr) && *cptr != '\0')
		{
			c++;

#ifdef PGS_IGNORE_CASE
			*cptr = tolower(*cptr);
#endif

			elog(DEBUG4, "char: \"%c\"; actual token size: %d", *cptr, c);

			cptr++;
		}


		if (*cptr == '\0')
			elog(DEBUG4, "end of setence (2)");

		if (c > 0)
		{
			int ret;
			char *tok = malloc(sizeof(char) * (c + 1));
			strncpy(tok, sptr, c);
			tok[c] = '\0';

			elog(DEBUG3, "token: \"%s\"; size: %lu", tok, sizeof(char) * c);

			ret = addToken(t, tok);

			elog(DEBUG4, "actual token list size: %d", t->size);

			Assert(strlen(tok) <= PGS_MAX_TOKEN_LEN);

			/* Only free the token if it was not added to the list, otherwise it
			 * will be freed when the list is destroyed */
			if (ret == -2)
				free(tok);

			c = 0;
		}
	}
}

void tokenizeBySpace(TokenList *t, char *s)
{
	const char		*cptr,	/* current pointer */
			  *sptr;	/* start token pointer */
	int			c = 0;	/* number of bytes */

	elog(DEBUG3, "sentence: \"%s\"", s);

	if (t->size == 0)
		elog(DEBUG3, "token list is empty");
	else
		elog(DEBUG3, "token list contains %d tokens", t->size);

	if (t->head == NULL)
		elog(DEBUG3, "there is no head token yet");
	else
		elog(DEBUG3, "head token is \"%s\"", t->head->data);

	if (t->tail == NULL)
		elog(DEBUG3, "there is no tail token yet");
	else
		elog(DEBUG3, "tail token is \"%s\"", t->tail->data);

	cptr = sptr = s;

	while (*cptr)
	{
		while (isspace(*cptr) && *cptr != '\0')
		{
			elog(DEBUG4, "\"%c\" is a space", *cptr);

			cptr++;
		}

		if (*cptr == '\0')
			elog(DEBUG4, "end of sentence");

#ifdef PGS_IGNORE_CASE
		*sptr = tolower(*sptr);
#endif

		sptr = cptr;

		elog(DEBUG4, "token's first char: \"%c\"", *sptr);

		while (!isspace(*cptr) && *cptr != '\0')
		{
			c++;

#ifdef PGS_IGNORE_CASE
			*cptr = tolower(*cptr);
#endif

			elog(DEBUG4, "char: \"%c\"; actual token size: %d", *cptr, c);

			cptr++;
		}

		if (*cptr == '\0')
			elog(DEBUG4, "end of setence (2)");

		if (c > 0)
		{
			int ret;
			char *tok = malloc(sizeof(char) * (c + 1));
			strncpy(tok, sptr, c);
			tok[c] = '\0';

			elog(DEBUG3, "token: \"%s\"; size: %lu", tok, sizeof(char) * c);

			ret = addToken(t, tok);

			elog(DEBUG4, "actual token list size: %d", t->size);
			elog(DEBUG4, "tok: \"%s\"; size: %u", tok, (unsigned int) strlen(tok));

			Assert(strlen(tok) <= PGS_MAX_TOKEN_LEN);

			/* Only free the token if it was not added to the list, otherwise it
			 * will be freed when the list is destroyed */
			if (ret == -2)
				free(tok);

			c = 0;
		}
	}
}

/*
 * our n-grams are letter level and we have:
 * (i) full n-gram: euler = {" e", eu, ul, le, er, "r "}
 * (ii) normal n-gram: euler = {eu, ul, le, er}
 */
void tokenizeByGram(TokenList *t, char *s)
{
	char	*p;
	int		slen;
	int		i;

	slen = strlen(s);

	p = s;

	/*
	 * n-grams with starting character
	 */
#ifdef PGS_FULL_NGRAM
	for (i = (PGS_GRAM_LEN - 1); i > 0; i--)
	{
		int 	ret;
		char	*buf;
		buf = (char *) malloc((PGS_GRAM_LEN + 1) * sizeof(char));
		memset(buf, PGS_BLANK_CHAR, i);
		strncpy((buf + i), s, PGS_GRAM_LEN - i);
		buf[PGS_GRAM_LEN] = '\0';

		ret = addToken(t, buf);

		elog(DEBUG1, "qgram (b): \"%s\"", buf);

		/* Only free the token if it was not added to the list, otherwise it
		 * will be freed when the list is destroyed */
		if (ret == -2)
			free(buf);
	}
#else
	{
		int 	ret;
		char	*buf;
		buf = (char *) malloc((PGS_GRAM_LEN + 1) * sizeof(char));
		memset(buf, PGS_BLANK_CHAR, 1);
		strncpy((buf + 1), s, PGS_GRAM_LEN - 1);
		buf[PGS_GRAM_LEN] = '\0';

		ret = addToken(t, buf);

		elog(DEBUG1, "qgram (b): \"%s\"", buf);

		/* Only free the token if it was not added to the list, otherwise it
		 * will be freed when the list is destroyed */
		if (ret == -2)
			free(buf);
	}
#endif

	for (i = 0; i <= (slen - PGS_GRAM_LEN); i++)
	{
		int 	ret;
		char	*buf;
		buf = (char *) malloc((PGS_GRAM_LEN + 1) * sizeof(char));
		strncpy(buf, p, PGS_GRAM_LEN);
		buf[PGS_GRAM_LEN] = '\0';

		ret = addToken(t, buf);

		p++;

		elog(DEBUG1, "qgram (m): \"%s\"", buf);

		/* Only free the token if it was not added to the list, otherwise it
		 * will be freed when the list is destroyed */
		if (ret == -2)
			free(buf);
	}

	/*
	 * n-grams with ending character
	 */
#ifdef PGS_FULL_NGRAM
	for (i = 1; i < PGS_GRAM_LEN; i++)
	{
		int 	ret;
		char	*buf;
		buf = (char *) malloc((PGS_GRAM_LEN + 1) * sizeof(char));
		strncpy(buf, p, PGS_GRAM_LEN - i);
		memset((buf + (PGS_GRAM_LEN - i)), PGS_BLANK_CHAR, i);
		buf[PGS_GRAM_LEN] = '\0';

		ret = addToken(t, buf);

		p++;

		elog(DEBUG1, "qgram (a): \"%s\"", buf);

		/* Only free the token if it was not added to the list, otherwise it
		 * will be freed when the list is destroyed */
		if (ret == -2)
			free(buf);
	}
#else
	{
		int 	ret;
		char	*buf;
		buf = (char *) malloc((PGS_GRAM_LEN + 1) * sizeof(char));
		strncpy(buf, p, PGS_GRAM_LEN - 1);
		memset((buf + (PGS_GRAM_LEN - 1)), PGS_BLANK_CHAR, 1);
		buf[PGS_GRAM_LEN] = '\0';

		ret = addToken(t, buf);

		elog(DEBUG1, "qgram (a): \"%s\"", buf);

		/* Only free the token if it was not added to the list, otherwise it
		 * will be freed when the list is destroyed */
		if (ret == -2)
			free(buf);
	}
#endif
}

void tokenizeByCamelCase(TokenList *t, char *s)
{
	const char		*cptr,	/* current pointer */
			  *sptr;	/* start token pointer */
	int			c = 0;	/* number of bytes */

	elog(DEBUG3, "sentence: \"%s\"", s);

	if (t->size == 0)
		elog(DEBUG3, "token list is empty");
	else
		elog(DEBUG3, "token list contains %d tokens", t->size);

	if (t->head == NULL)
		elog(DEBUG3, "there is no head token yet");
	else
		elog(DEBUG3, "head token is \"%s\"", t->head->data);

	if (t->tail == NULL)
		elog(DEBUG3, "there is no tail token yet");
	else
		elog(DEBUG3, "tail token is \"%s\"", t->tail->data);

	cptr = sptr = s;

	while (*cptr)
	{
		while (isspace(*cptr) && *cptr != '\0')
		{
			elog(DEBUG4, "\"%c\" is a space", *cptr);

			cptr++;
		}

		if (*cptr == '\0')
			elog(DEBUG4, "end of sentence");

#ifdef PGS_IGNORE_CASE
		*sptr = tolower(*sptr);
#endif

		sptr = cptr;

		elog(DEBUG4, "token's first char: \"%c\"", *sptr);

		if (isupper(*cptr))
			elog(DEBUG4, "\"%c\" is uppercase", *cptr);
		else
			elog(DEBUG4, "\"%c\" is not uppercase", *cptr);

		/*
		 * if the first caracter is uppercase enter the loop because sometimes
		 * the first char in a camel-case notation is uppercase
		 */
		while (c == 0 || (!isupper(*cptr) && *cptr != '\0'))
		{
			c++;

#ifdef PGS_IGNORE_CASE
			*cptr = tolower(*cptr);
#endif

			elog(DEBUG4, "char: \"%c\"; actual token size: %d", *cptr, c);

			cptr++;
		}

		if (*cptr == '\0')
			elog(DEBUG4, "end of setence (2)");

		if (c > 0)
		{
			int ret;
			char *tok = malloc(sizeof(char) * (c + 1));
			strncpy(tok, sptr, c);
			tok[c] = '\0';

			elog(DEBUG3, "token: \"%s\"; size: %lu", tok, sizeof(char) * c);

			ret = addToken(t, tok);

			elog(DEBUG4, "actual token list size: %d", t->size);
			elog(DEBUG4, "tok: \"%s\"; size: %u", tok, (unsigned int) strlen(tok));

			Assert(strlen(tok) <= PGS_MAX_TOKEN_LEN);

			/* Only free the token if it was not added to the list, otherwise it
			 * will be freed when the list is destroyed */
			if (ret == -2)
				free(tok);

			c = 0;
		}
	}
}
