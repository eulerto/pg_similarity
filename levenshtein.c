/*----------------------------------------------------------------------------
 *
 * levenshtein.c
 *
 * Levenshtein Distance is one of the most famous similarity measures
 *
 * It aims to obtain the minimum number of operations (insert, delete, or
 * substitution) needed to transform one string into the other.
 *
 * For example:
 *
 * x: euler
 * y: heuser
 * all operation costs are 1.
 *
 * +---------------------------+
 * |   |   | e | u | l | e | r |
 * +---------------------------+
 * |   | 0 | 1 | 2 | 3 | 4 | 5 |
 * +---------------------------+
 * | h | 1 | 1 | 2 | 3 | 4 | 5 |
 * +---------------------------+
 * | e | 2 | 1 | 2 | 3 | 3 | 4 |
 * +---------------------------+
 * | u | 3 | 2 | 1 | 2 | 3 | 4 |
 * +---------------------------+
 * | s | 4 | 3 | 2 | 2 | 3 | 4 |
 * +---------------------------+
 * | e | 5 | 4 | 3 | 3 | 2 | 3 |
 * +---------------------------+
 * | r | 6 | 5 | 4 | 4 | 3 | 2 | <==
 * +---------------------------+
 *
 * http://en.wikipedia.org/wiki/Levenshtein_distance
 *
 *
 * Copyright (c) 2008-2012, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"


/* GUC variables */
double	pgs_levenshtein_threshold = 0.7f;
bool	pgs_levenshtein_is_normalized = true;


int _lev(char *a, char *b, int icost, int dcost)
{
	int			*arow, *brow, *trow; /* above, below, and temp row */
	int			alen, blen;
	int			i, j;
	int			res;

	alen = strlen(a);
	blen = strlen(b);

	elog(DEBUG2, "alen: %d; blen: %d", alen, blen);

	if (alen == 0)
		return blen;
	if (blen == 0)
		return alen;

	arow = (int *) malloc((blen + 1) * sizeof(int));
	brow = (int *) malloc((blen + 1) * sizeof(int));

	if (arow == NULL)
		elog(ERROR, "memory exaushted for array size %d", (alen + 1));
	if (brow == NULL)
		elog(ERROR, "memory exaushted for array size %d", (blen + 1));

#ifdef PGS_IGNORE_CASE
	elog(DEBUG2, "case-sensitive turns off");
	for (i = 0; i < alen; i++)
		a[i] = tolower(a[i]);
	for (j = 0; j < blen; j++)
		b[j] = tolower(b[j]);
#endif

	/* initial values */
	for (i = 0; i <= blen; i++)
		arow[i] = i;

	for (i = 1; i <= alen; i++)
	{
		/* first value is 'i' */
		brow[0] = i;

		for (j = 1; j <= blen; j++)
		{
			/* TODO change it to a callback function */
			/* get operation cost */
			int scost = levcost(a[i-1], b[j-1]);

			brow[j] = min3(brow[j-1] + icost,
							arow[j] + dcost,
							arow[j-1] + scost);
			elog(DEBUG2, "(i, j) = (%d, %d); cost(%c, %c): %d; min(top, left, diag) = (%d, %d, %d) = %d",
							i, j, a[i-1], b[j-1], scost,
							brow[j-1] + icost,
							arow[j] + dcost,
							arow[j-1] + scost,
							brow[j]);
		}

		/*
		 * below row becomes above row
		 * above row is reused as below row
		 */
		trow = arow;
		arow = brow;
		brow = trow;

		elog(DEBUG2, "row: ");
		for (j = 1; j <= alen; j++)
			elog(DEBUG2, "%d ", arow[j]);
	}

	res = arow[blen];

	free(arow);
	free(brow);

	return res;
}

/*
 * wastes more memory and execution time
 * XXX the purpose of this function is merely academic
 */
int _lev_slow(char *a, char *b, int icost, int dcost)
{
	int			**matrix;		/* dynamic programming matrix */
	int			alen, blen;
	int			i, j;
	int			res;

	alen = strlen(a);
	blen = strlen(b);

	elog(DEBUG2, "alen: %d; blen: %d", alen, blen);

	if (alen == 0)
		return blen;
	if (blen == 0)
		return alen;

	matrix = (int **) malloc((alen + 1) * sizeof(int *));

	if (matrix == NULL)
		elog(ERROR, "memory exaushted for array size %d", (alen + 1));

	for (i = 0; i <= alen; i++)
	{
		matrix[i] = (int *) malloc((blen + 1) * sizeof(int));
		if (matrix == NULL)
			elog(ERROR, "memory exaushted for array size %d", (blen + 1));
	}

#ifdef PGS_IGNORE_CASE
	elog(DEBUG2, "case-sensitive turns off");
	for (i = 0; i < alen; i++)
		a[i] = tolower(a[i]);
	for (j = 0; j < blen; j++)
		b[j] = tolower(b[j]);
#endif

	/* initial values */
	for (i = 0; i <= alen; i++)
		matrix[i][0] = i;
	for (j = 0; j <= blen; j++)
		matrix[0][j] = j;

	for (i = 1; i <= alen; i++)
	{
		for (j = 1; j <= blen; j++)
		{
			/* get operation cost */
			int scost = levcost(a[i-1], b[j-1]);

			matrix[i][j] = min3(matrix[i-1][j] + dcost,
								matrix[i][j-1] + icost,
								matrix[i-1][j-1] + scost);
			elog(DEBUG2, "(i, j) = (%d, %d); cost(%c, %c): %d; min(top, left, diag) = (%d, %d, %d) = %d",
							i, j, a[i-1], b[j-1], scost,
							matrix[i-1][j] + dcost,
							matrix[i][j-1] + icost,
							matrix[i-1][j-1] + scost,
							matrix[i][j]);
		}
	}

	res = matrix[alen][blen];

	for (i = 0; i <= alen; i++)
		free(matrix[i]);
	free(matrix);

	return res;
}

PG_FUNCTION_INFO_V1(lev);

Datum
lev(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	int		maxlen;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	maxlen = max2(strlen(a), strlen(b));

	res = (float8) _lev(a, b, PGS_LEV_MAX_COST, PGS_LEV_MAX_COST);

	elog(DEBUG1, "is normalized: %d", pgs_levenshtein_is_normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);
	elog(DEBUG1, "levdistance(%s, %s) = %.3f", a, b, res);

	if (maxlen == 0)
	{
		PG_RETURN_FLOAT8(1.0);
	}
	else if (pgs_levenshtein_is_normalized)
	{
		res = 1.0 - (res / maxlen);
		elog(DEBUG1, "lev(%s, %s) = %.3f", a, b, res);
		PG_RETURN_FLOAT8(res);
	}
	else
	{
		PG_RETURN_FLOAT8(res);
	}
}

PG_FUNCTION_INFO_V1(lev_op);

Datum lev_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_levenshtein_is_normalized;
	pgs_levenshtein_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					lev,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_levenshtein_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_levenshtein_threshold);
}

PG_FUNCTION_INFO_V1(levslow);

Datum
levslow(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	int		maxlen;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	maxlen = max2(strlen(a), strlen(b));

	res = (float8) _lev_slow(a, b, PGS_LEV_MAX_COST, PGS_LEV_MAX_COST);

	elog(DEBUG1, "is normalized: %d", pgs_levenshtein_is_normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);
	elog(DEBUG1, "levdistance(%s, %s) = %.3f", a, b, res);

	if (maxlen == 0)
	{
		PG_RETURN_FLOAT8(1.0);
	}
	else if (pgs_levenshtein_is_normalized)
	{
		res = 1.0 - (res / maxlen);
		elog(DEBUG1, "lev(%s, %s) = %.3f", a, b, res);
		PG_RETURN_FLOAT8(res);
	}
	else
	{
		PG_RETURN_FLOAT8(res);
	}
}

PG_FUNCTION_INFO_V1(levslow_op);

Datum levslow_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_levenshtein_is_normalized;
	pgs_levenshtein_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					levslow,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_levenshtein_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_levenshtein_threshold);
}
