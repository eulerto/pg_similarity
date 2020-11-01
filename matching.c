/*----------------------------------------------------------------------------
 *
 * matching.c
 *
 * The Matching Coefficient is a simple vector based approach
 *
 *          nt
 * s = -------------
 *      max(nx, ny)
 *
 * where nt is the number of common n-grams found in both strings, nx is the
 * number of n-grams in x and, ny is the number of n-grams in y.
 *
 * For example:
 *
 * x: euler = {e, u, l, e, r}
 * y: heuser = {h, e, u, s, e, r}
 *
 *      4
 * s = --- = 0.666...
 *      6
 *
 * PS> we call n-grams: (i) n-sequence of letters (ii) n-sequence of words
 *
 *
 * Copyright (c) 2008-2020, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_matching_tokenizer = PGS_UNIT_ALNUM;
double	pgs_matching_threshold = 0.7f;
bool	pgs_matching_is_normalized = true;

PG_FUNCTION_INFO_V1(matchingcoefficient);

Datum
matchingcoefficient(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	TokenList	*s, *t;
	Token		*p, *q;
	int		atok, btok, comtok, maxtok;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("argument exceeds the maximum length of %d bytes",
						PGS_MAX_STR_LEN)));

	/* lists */
	s = initTokenList(0);
	t = initTokenList(0);

	switch (pgs_matching_tokenizer)
	{
		case PGS_UNIT_WORD:
			tokenizeBySpace(s, a);
			tokenizeBySpace(t, b);
			break;
		case PGS_UNIT_GRAM:
			tokenizeByGram(s, a);
			tokenizeByGram(t, b);
			break;
		case PGS_UNIT_CAMELCASE:
			tokenizeByCamelCase(s, a);
			tokenizeByCamelCase(t, b);
			break;
		case PGS_UNIT_ALNUM:
		default:
			tokenizeByNonAlnum(s, a);
			tokenizeByNonAlnum(t, b);
			break;
	}

	elog(DEBUG3, "Token List A");
	printToken(s);
	elog(DEBUG3, "Token List B");
	printToken(t);

	atok = s->size;
	btok = t->size;
	maxtok = max2(atok, btok);

	comtok = 0;

	/*
	 * XXX consider sorting s and t when we're dealing with large lists?
	 */
	p = s->head;
	while (p != NULL)
	{
		int found = 0;

		q = t->head;
		while (q != NULL)
		{
			elog(DEBUG3, "p: %s; q: %s", p->data, q->data);
			if (strcmp(p->data, q->data) == 0)
			{
				found = 1;
				break;
			}
			q = q->next;
		}

		if (found)
		{
			comtok++;
			elog(DEBUG2, "\"%s\" found; comtok = %d", p->data, comtok);
		}

		p = p->next;
	}

	destroyTokenList(s);
	destroyTokenList(t);

	elog(DEBUG1, "is normalized: %d", pgs_matching_is_normalized);
	elog(DEBUG1, "common tokens size: %d", comtok);
	elog(DEBUG1, "maximum token size: %d", maxtok);

	if (pgs_matching_is_normalized)
		res = (float8) comtok / maxtok;
	else
		res = comtok;

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(matchingcoefficient_op);

Datum matchingcoefficient_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_matching_is_normalized;
	pgs_matching_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
							 matchingcoefficient,
							 PG_GETARG_DATUM(0),
							 PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_matching_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_matching_threshold);
}
