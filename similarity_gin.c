/*----------------------------------------------------------------------------
 *
 * similarity_gin.c
 *
 * GIN support routines
 *
 * Copyright (c) 2008-2012, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "postgres.h"

#include "access/gin.h"
#include "access/skey.h"
/*
#include "access/reloptions.h"
#include "utils/guc.h"
#include "utils/syscache.h"
*/

#include "similarity.h"
#include "tokenizer.h"

/* choose one of them */
/*
#define PGS_BY_WORD			1
*/
#define PGS_BY_ALNUM		1
/*
#define	PGS_BY_GRAM			1
#define PGS_BY_CAMELCASE	1
*/

PG_FUNCTION_INFO_V1(gin_extract_value_token);
Datum gin_extract_value_token(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(gin_extract_query_token);
Datum gin_extract_query_token(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(gin_token_consistent);
Datum gin_token_consistent(PG_FUNCTION_ARGS);

Datum
gin_extract_value_token(PG_FUNCTION_ARGS)
{
	text	*value = (text *) PG_GETARG_TEXT_P(0);
	int32	*ntokens = (int32 *) PG_GETARG_POINTER(1);

	Datum	*tokens = NULL;
	char	*buf;


	elog(DEBUG3, "gin_extract_value_token() called");

	buf = text_to_cstring(value);
	*ntokens = 0;

	if (buf != NULL)
	{
		TokenList	*tlist;
		Token		*t;

		tlist = initTokenList(1);

		/*
		 * TODO we want to index according to out GUCs
		 * TODO we need to store the tokenized-by information
		 * TODO so we can decide to use or not the index for a
		 * TODO given query
		 */
#ifdef PGS_BY_WORD
		tokenizeBySpace(tlist, buf);
#elif PGS_BY_ALNUM
		tokenizeByNonAlnum(tlist, buf);
#elif PGS_BY_GRAM
		tokenizeByGram(tlist, buf);
#elif PGS_BY_CAMELCASE
		tokenizeByCamelCase(tlist, buf);
#else
		elog(ERROR, "choose a supported tokenizer");
#endif

		*ntokens = tlist->size;

		if (tlist->size > 0)
		{
			int		i;

			tokens = (Datum *) palloc(sizeof(Datum) * tlist->size);

			t = tlist->head;

			for (i = 0; i < tlist->size; i++)
			{
				text	*td;
	
				td = cstring_to_text_with_len(t->data, strlen(t->data));
				tokens[i] = PointerGetDatum(td);
	
				t = t->next;
			}
		}

		destroyTokenList(tlist);
	}

	PG_FREE_IF_COPY(value, 0);

	PG_RETURN_POINTER(tokens);
}

Datum
gin_extract_query_token(PG_FUNCTION_ARGS)
{
	text			*value = (text *) PG_GETARG_TEXT_P(0);
	int32			*ntokens = (int32 *) PG_GETARG_POINTER(1);

/*
	StrategyNumber	strategy = PG_GETARG_UINT16(2);
	bool			**pmatch = (bool **) PG_GETARG_POINTER(3);
	Pointer			**extra_data = (Pointer *) PG_GETARG_POINTER(4);
*/

#if	PG_VERSION_NUM >= 90100
/*
	bool			**null_flags = (bool **) PG_GETARG_POINTER(5);
*/
	int32			*search_mode = (int32 *) PG_GETARG_POINTER(6);
#endif

	Datum			*tokens = NULL;
	char			*buf;


	elog(DEBUG3, "gin_extract_query_token() called");

	buf = text_to_cstring(value);
	*ntokens = 0;

	if (buf != NULL)
	{
		TokenList	*tlist;
		Token		*t;

		tlist = initTokenList(1);

		/*
		 * TODO we want to index according to out GUCs
		 * TODO we need to store the tokenized-by information
		 * TODO so we can decide to use or not the index for a
		 * TODO given query
		 */
#ifdef PGS_BY_WORD
		tokenizeBySpace(tlist, buf);
#elif PGS_BY_ALNUM
		tokenizeByNonAlnum(tlist, buf);
#elif PGS_BY_GRAM
		tokenizeByGram(tlist, buf);
#elif PGS_BY_CAMELCASE
		tokenizeByCamelCase(tlist, buf);
#else
		elog(ERROR, "choose a supported tokenizer");
#endif

		*ntokens = tlist->size;

		if (tlist->size > 0)
		{
			int		i;

			tokens = (Datum *) palloc(sizeof(Datum) * tlist->size);

			t = tlist->head;

			for (i = 0; i < tlist->size; i++)
			{
				text	*td;
	
				td = cstring_to_text_with_len(t->data, strlen(t->data));
				tokens[i] = PointerGetDatum(td);
	
				t = t->next;
			}
		}

		destroyTokenList(tlist);
	}

#if	PG_VERSION_NUM >= 90100
	if (*ntokens == 0)
		*search_mode = GIN_SEARCH_MODE_ALL;
#endif

	PG_FREE_IF_COPY(value, 0);

	PG_RETURN_POINTER(tokens);
}

Datum
gin_token_consistent(PG_FUNCTION_ARGS)
{
/*
	bool			*check = (bool *) PG_GETARG_POINTER(0);
	StrategyNumber	strategy = PG_GETARG_UINT16(1);
	text			*query = PG_GETARG_TEXT_P(2);
	int32			ntokens = PG_GETARG_INT32(3);
	Pointer			*extra_data = (Pointer *) PG_GETARG_POINTER(4);
*/

	bool			*recheck = (bool *) PG_GETARG_POINTER(5);

/*
#if PG_VERSION_NUM >= 90100
	Datum			**query_tokens = PG_GETARG_POINTER(6);
	bool			**null_flags = (bool **) PG_GETARG_POINTER(7);
#endif
*/

	elog(DEBUG3, "gin_token_consistent() called");

	/*
	 * Heap tuple might match the query. Evaluating the query operator directly
	 * against the originally indexed item.
	 */
	*recheck = true;

	PG_RETURN_BOOL(true);
}
