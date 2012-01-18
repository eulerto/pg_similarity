/*----------------------------------------------------------------------------
 *
 * block.c
 *
 * L1 distance or City Block distance or Manhattan distance
 *
 * Aims to measure the distance between two strings
 *
 * X is a list of n-grams
 * Y is a list of n-grams
 * T is a set of n-grams of X and/or Y
 *
 * For each n-gram in T we count occorrences in X and Y; we sum the
 * absolute difference between nx and ny.
 *
 * For example:
 *
 * x: euler = {eu, ul, le, er}
 * y: heuser = {he, eu, us, se, er}
 * t: {eu, ul, le, er, he, us, se}
 *
 *        eu        ul        le        er        he        us        se
 * s = |1 - 1| + |1 - 0| + |1 - 0| + |1 - 1| + |0 - 1| + |0 - 1| + |0 - 1| = 5
 *
 * PS> we call n-grams: (i) n-sequence of letters (ii) n-sequence of words
 *
 * http://en.wikipedia.org/wiki/Block_distance
 *
 *
 * Copyright (c) 2008-2012, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_block_tokenizer = PGS_UNIT_ALNUM;
double	pgs_block_threshold = 0.7f;
bool	pgs_block_is_normalized = true;

PG_FUNCTION_INFO_V1(block);

Datum
block(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	TokenList	*s, *t, *u;
	Token		*p, *q, *r;
	int			totpossible;
	int			totdistance;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	/* lists */
	s = initTokenList(0);
	t = initTokenList(0);
	/* set list */
	u = initTokenList(1);

	switch (pgs_block_tokenizer)
	{
		case PGS_UNIT_WORD:
			tokenizeBySpace(s, a);
			tokenizeBySpace(t, b);
			/* all tokens in a set */
			tokenizeBySpace(u, a);
			tokenizeBySpace(u, b);
			break;
		case PGS_UNIT_GRAM:
			tokenizeByGram(s, a);
			tokenizeByGram(t, b);
			/* all tokens in a set */
			tokenizeByGram(u, a);
			tokenizeByGram(u, b);
			break;
		case PGS_UNIT_CAMELCASE:
			tokenizeByCamelCase(s, a);
			tokenizeByCamelCase(t, b);
			/* all tokens in a set */
			tokenizeByCamelCase(u, a);
			tokenizeByCamelCase(u, b);
			break;
		case PGS_UNIT_ALNUM:	/* default */
		default:
			tokenizeByNonAlnum(s, a);
			tokenizeByNonAlnum(t, b);
			/* all tokens in a set */
			tokenizeByNonAlnum(u, a);
			tokenizeByNonAlnum(u, b);
			break;
	}

	elog(DEBUG3, "Token List A");
	printToken(s);
	elog(DEBUG3, "Token List B");
	printToken(t);
	elog(DEBUG3, "All Token List");
	printToken(u);

	totpossible = s->size + t->size;

	totdistance = 0.0;

	p = u->head;
	while (p != NULL)
	{
		int		acnt = 0;
		int		bcnt = 0;

		q = s->head;
		while (q != NULL)
		{
			elog(DEBUG4, "p: %s; q: %s", p->data, q->data);
			if (strcmp(p->data, q->data) == 0)
				acnt++;
			q = q->next;
		}

		r = t->head;
		while (r != NULL)
		{
			elog(DEBUG4, "p: %s; r: %s", p->data, r->data);
			if (strcmp(p->data, r->data) == 0)
				bcnt++;
			r = r->next;
		}

		if (acnt > bcnt)
			totdistance += (acnt - bcnt);
		else
			totdistance += (bcnt - acnt);

		elog(DEBUG2,
			"\"%s\" => acnt(%d); bcnt(%d); totdistance(%d)",
					p->data, acnt, bcnt, totdistance);

		p = p->next;
	}

	elog(DEBUG1, "is normalized: %d", pgs_block_is_normalized);
	elog(DEBUG1, "total possible: %d", totpossible);
	elog(DEBUG1, "total distance: %d", totdistance);

	destroyTokenList(s);
	destroyTokenList(t);
	destroyTokenList(u);

	if (pgs_block_is_normalized)
		res = (float8) (totpossible - totdistance) / totpossible;
	else
		res = totdistance;

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(block_op);

Datum block_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_block_is_normalized;
	pgs_block_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					block,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_block_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_block_threshold);
}
