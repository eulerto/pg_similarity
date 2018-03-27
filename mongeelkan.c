/*----------------------------------------------------------------------------
 *
 * mongeelkan.c
 *
 * Copyright (c) 2008-2018, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_mongeelkan_tokenizer = PGS_UNIT_ALNUM;
double	pgs_mongeelkan_threshold = 0.7f;
bool	pgs_mongeelkan_is_normalized = true;

/*
 * TODO move this function to similarity.c
 * TODO this function is a smithwatermangotoh() clone
 */
static double _mongeelkan(char *a, char *b)
{
	float		**matrix;		/* dynamic programming matrix */
	int		alen, blen;
	int		i, j;
	double		maxvalue;

	alen = strlen(a);
	blen = strlen(b);

	elog(DEBUG2, "alen: %d; blen: %d", alen, blen);

	if (alen == 0)
		return blen;
	if (blen == 0)
		return alen;

	matrix = (float **) malloc((alen + 1) * sizeof(float *));

	if (matrix == NULL)
		elog(ERROR, "memory exaushted for array size %d", alen);

	for (i = 0; i <= alen; i++)
	{
		matrix[i] = (float *) malloc((blen + 1) * sizeof(float));
		if (matrix[i] == NULL)
			elog(ERROR, "memory exaushted for array size %d", blen);
	}

#ifdef PGS_IGNORE_CASE
	elog(DEBUG2, "case-sensitive turns off");
	for (i = 0; i < alen; i++)
		a[i] = tolower(a[i]);
	for (j = 0; j < blen; j++)
		b[j] = tolower(b[j]);
#endif

	maxvalue = 0.0;

	/* initial values */
	for (i = 0; i <= alen; i++)
	{
		float c = megapcost(a, b, i, 0);

		if (i == 0)
		{
			matrix[0][0] = max2(0.0, c);
		}
		else
		{
			float	maxgapcost = 0.0;
			int		wstart = i - PGS_SWG_WINDOW_SIZE;
			int		k;

			if (wstart < 1)
				wstart = 1;

			for (k = wstart; k < i; k++)
				maxgapcost = max2(maxgapcost, matrix[i - k][0] - swggapcost(i - k, i));
			matrix[i][0] = max3(0.0, maxgapcost, c);
		}

		if (matrix[i][0] > maxvalue)
			maxvalue = matrix[i][0];
	}
	for (j = 0; j <= blen; j++)
	{
		float c = megapcost(a, b, 0, j);

		if (j == 0)
		{
			matrix[0][0] = max2(0.0, c);
		}
		else
		{
			float	maxgapcost = 0.0;
			int		wstart = j - PGS_SWG_WINDOW_SIZE;
			int		k;

			if (wstart < 1)
				wstart = 1;

			for (k = wstart; k < j; k++)
				maxgapcost = max2(maxgapcost, matrix[0][j - k] - swggapcost(j - k, j));
			matrix[0][j] = max3(0.0, maxgapcost, c);
		}

		if (matrix[0][j] > maxvalue)
			maxvalue = matrix[0][j];
	}

	for (i = 1; i <= alen; i++)
	{
		for (j = 1; j <= blen; j++)
		{
			int		wstart;
			int		k;
			float	maxgapcost1 = 0.0,
					maxgapcost2 = 0.0;

			/* get operation cost */
			float c = megapcost(a, b, i, j);

			wstart = i - PGS_SWG_WINDOW_SIZE;
			if (wstart < 1)
				wstart = 1;
			for (k = wstart; k < i; k++)
				maxgapcost1 = max2(maxgapcost1, matrix[i - k][0] - swggapcost(i - k, i));

			wstart = j - PGS_SWG_WINDOW_SIZE;
			if (wstart < 1)
				wstart = 1;
			for (k = wstart; k < j; k++)
				maxgapcost2 = max2(maxgapcost2, matrix[0][j - k] - swggapcost(j - k, j));

			matrix[i][j] = max4(0.0,
								maxgapcost1,
								maxgapcost2,
								matrix[i-1][j-1] + c);
			elog(DEBUG2, "(i, j) = (%d, %d); cost(%c, %c): %.3f; max(zero, top, left, diag) = (0.0, %.3f, %.3f, %.3f) = %.3f",
							i, j, a[i-1], b[j-1], c,
							maxgapcost1,
							maxgapcost2,
							matrix[i-1][j-1] + c, matrix[i][j]);

			if (matrix[i][j] > maxvalue)
				maxvalue = matrix[i][j];
		}
	}

	for (i = 0; i <= alen; i++)
		free(matrix[i]);
	free(matrix);

	return maxvalue;
}

PG_FUNCTION_INFO_V1(mongeelkan);

Datum
mongeelkan(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	TokenList	*s, *t;
	Token		*p, *q;
	double		summatches;
	double		maxvalue;
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

	switch (pgs_mongeelkan_tokenizer)
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

	summatches = 0.0;

	p = s->head;
	while (p != NULL)
	{
		maxvalue = 0.0;

		q = t->head;
		while (q != NULL)
		{
			double val = _mongeelkan(p->data, q->data);
			elog(DEBUG3, "p: %s; q: %s", p->data, q->data);
			if (val > maxvalue)
				maxvalue = val;
			q = q->next;
		}

		summatches += maxvalue;

		p = p->next;
	}

	/* normalized and unnormalized version are the same */
	res = summatches / s->size;

	elog(DEBUG1, "is normalized: %d", pgs_mongeelkan_is_normalized);
	elog(DEBUG1, "sum matches: %.3f", summatches);
	elog(DEBUG1, "s size: %d", s->size);
	elog(DEBUG1, "medistance(%s, %s) = %.3f", a, b, res);

	destroyTokenList(s);
	destroyTokenList(t);

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(mongeelkan_op);

Datum mongeelkan_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_mongeelkan_is_normalized;
	pgs_mongeelkan_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					mongeelkan,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_mongeelkan_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_mongeelkan_threshold);
}
