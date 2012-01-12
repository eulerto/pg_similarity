/*----------------------------------------------------------------------------
 *
 * jaccard.c
 *
 * Jaccard similarity coefficient is a similarity measure
 *
 * It measures similarity between sets, and is defined as the size of the
 * intersection divided by the size of the union of the sets.
 *
 *      |A intersection B|
 * s = --------------------
 *         |A union B|
 *
 * For example:
 *
 * x: euler = {eu, ul, le, er}
 * y: heuser = {he, eu, us, se, er}
 *
 *      2
 * s = --- = 0.285714286
 *      7
 *
 * PS> we call n-grams: (i) n-sequence of letters (ii) n-sequence of words
 *
 *
 * Copyright (c) 2008-2010, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_jaccard_tokenizer = PGS_UNIT_ALNUM;
double	pgs_jaccard_threshold = 0.7f;
bool	pgs_jaccard_is_normalized = true;

PG_FUNCTION_INFO_V1(jaccard);

Datum
jaccard(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	TokenList	*s, *t;
	int		atok, btok, comtok, alltok;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	/* sets */
	s = initTokenList(1);
	t = initTokenList(1);

	switch (pgs_jaccard_tokenizer)
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
		case PGS_UNIT_ALNUM:	/* default */
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

	/* combine the sets */
	switch (pgs_jaccard_tokenizer)
	{
		case PGS_UNIT_WORD:
			tokenizeBySpace(s, b);
			break;
		case PGS_UNIT_GRAM:
			tokenizeByGram(s, b);
			break;
		case PGS_UNIT_CAMELCASE:
			tokenizeByCamelCase(s, b);
			break;
		case PGS_UNIT_ALNUM:	/* default */
		default:
			tokenizeByNonAlnum(s, b);
			break;
	}

	elog(DEBUG3, "All Token List");
	printToken(s);

	alltok = s->size;

	destroyTokenList(s);
	destroyTokenList(t);

	comtok = atok + btok - alltok;

	elog(DEBUG1, "is normalized: %d", pgs_jaccard_is_normalized);
	elog(DEBUG1, "token list A size: %d", atok);
	elog(DEBUG1, "token list B size: %d", btok);
	elog(DEBUG1, "all tokens size: %d", alltok);
	elog(DEBUG1, "common tokens size: %d", comtok);

	/* normalized and unnormalized version are the same */
	res = (float8) comtok / alltok;

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(jaccard_op);

Datum jaccard_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_jaccard_is_normalized;
	pgs_jaccard_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					jaccard,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_jaccard_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_jaccard_threshold);
}
