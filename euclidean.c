/*----------------------------------------------------------------------------
 *
 * euclidean.c
 *
 * Euclidean distance
 *
 * aims to measure the distance between two strings
 *
 * X is a list of n-grams
 * Y is a list of n-grams
 * T is a set of n-grams of X and/or Y
 *
 * For each n-gram in T we count occorrences in X and Y; we sum the
 * quadratic difference between nx and ny. At the end, we get the
 * square root of the sum.
 *
 * For example:
 *
 * x: euler = {eu, ul, le, er}
 * y: heuser = {he, eu, us, se, er}
 * t: {eu, ul, le, er, he, us, se}
 *
 *             eu          ul          le          er          he          us          se
 * s = sqrt((1 - 1)^2 + (1 - 0)^2 + (1 - 0)^2 + (1 - 1)^2 + (0 - 1)^2 + (0 - 1)^2 + (0 - 1)^2) =
 * s = sqrt(5) = 2.236067977...
 *
 * PS> we call n-grams: (i) n-sequence of letters (ii) n-sequence of words
 *
 *
 * Copyright (c) 2008-2026, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"

#include <math.h>


/* GUC variables */
int		pgs_euclidean_tokenizer = PGS_UNIT_ALNUM;
double	pgs_euclidean_threshold = 0.7;
bool	pgs_euclidean_is_normalized = true;

static float8
euclidean_internal(text *ta, text *tb, bool normalized)
{
	char		*a, *b;
	TokenList	*s, *t, *u;
	Token		*p, *q, *r;
	double		totdistance;
	double		totpossible;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(ta)));
	b = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(tb)));

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

	switch (pgs_euclidean_tokenizer)
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

	totpossible = sqrt(s->size * s->size + t->size * t->size);

	totdistance = 0.0;

	p = u->head;
	while (p != NULL)
	{
		int		acnt = 0;
		int		bcnt = 0;

		CHECK_FOR_INTERRUPTS();

		q = s->head;
		while (q != NULL)
		{
			elog(DEBUG4, "p: %s; q: %s", p->data, q->data);
			if (strcmp(p->data, q->data) == 0)
			{
				acnt++;
				break;
			}
			q = q->next;
		}

		r = t->head;
		while (r != NULL)
		{
			elog(DEBUG4, "p: %s; r: %s", p->data, r->data);
			if (strcmp(p->data, r->data) == 0)
			{
				bcnt++;
				break;
			}
			r = r->next;
		}

		totdistance += (acnt - bcnt) * (acnt - bcnt);

		elog(DEBUG2,
			 "\"%s\" => acnt(%d); bcnt(%d); totdistance(%.2f)",
			 p->data, acnt, bcnt, totdistance);

		p = p->next;
	}

	totdistance = sqrt(totdistance);

	elog(DEBUG1, "is normalized: %d", normalized);
	elog(DEBUG1, "total possible: %.2f", totpossible);
	elog(DEBUG1, "total distance: %.2f", totdistance);

	destroyTokenList(s);
	destroyTokenList(t);
	destroyTokenList(u);

	if (normalized)
		res = (totpossible - totdistance) / totpossible;
	else
		res = totdistance;

	return res;
}

PG_FUNCTION_INFO_V1(euclidean);

Datum
euclidean(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(euclidean_internal(PG_GETARG_TEXT_P(0),
										PG_GETARG_TEXT_P(1),
										pgs_euclidean_is_normalized));
}

PG_FUNCTION_INFO_V1(euclidean_op);

Datum
euclidean_op(PG_FUNCTION_ARGS)
{
	/* the threshold is normalized, so compare a normalized result */
	float8		res = euclidean_internal(PG_GETARG_TEXT_P(0),
										 PG_GETARG_TEXT_P(1),
										 true);

	PG_RETURN_BOOL(res >= pgs_euclidean_threshold);
}
