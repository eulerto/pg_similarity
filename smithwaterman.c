/*----------------------------------------------------------------------------
 *
 * smithwaterman.c
 *
 * Smith-Waterman is an algorithm that performs a global alignment on two 
 * sequences.
 *
 * It is a dynamic programming algorithm that is used to biological sequence
 * comparison. The operation costs (scores) are specified by similarity 
 * matrix. It also uses a linear gap penalty (like Levenshtein).
 *
 * For example:
 *
 * similarity matrix
 *
 * +-------------------+
 * |   | A | C | A | C |
 * +-------------------+
 * | A | 2 | 1 | 2 | 1 |
 * +-------------------+
 * | G | 1 | 1 | 1 | 1 |
 * +-------------------+
 * | C | 0 | 3 | 2 | 3 |
 * +-------------------+
 * | A | 2 | 2 | 5 | 4 |
 * +-------------------+
 *
 * x: ACACACTA
 * y: AGCACACA
 * match cost: 2
 * mismatch cost: -1
 * insertion cost: -1
 * deletion cost: -1
 *
 * +---------------------------------------+
 * |   |   | A | C | A | C | A  | C  | T  | A  |
 * +-------------------------------------------+
 * |   | 0 | 0 | 0 | 0 | 0 |  0 |  0 |  0 |  0 |
 * +-------------------------------------------+
 * | A | 0 | 2 | 1 | 2 | 1 |  2 |  1 |  0 |  2 |
 * +-------------------------------------------+
 * | G | 0 | 1 | 1 | 1 | 1 |  1 |  1 |  0 |  1 |
 * +-------------------------------------------+
 * | C | 0 | 0 | 3 | 2 | 3 |  2 |  3 |  2 |  1 |
 * +-------------------------------------------+
 * | A | 0 | 2 | 2 | 5 | 4 |  5 |  4 |  3 |  4 |
 * +-------------------------------------------+
 * | C | 0 | 1 | 4 | 4 | 7 |  6 |  7 |  6 |  5 |
 * +-------------------------------------------+
 * | A | 0 | 2 | 3 | 6 | 6 |  9 |  8 |  7 |  8 |
 * +-------------------------------------------+
 * | C | 0 | 1 | 4 | 5 | 8 |  8 | 11 | 10 |  9 |
 * +-------------------------------------------+
 * | A | 0 | 2 | 3 | 6 | 7 | 10 | 10 | 10 | 12 |
 * +-------------------------------------------+
 *
 * http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm
 *
 *
 * Copyright (c) 2008-2010, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"


/* GUC variables */
double	pgs_sw_threshold = 0.7f;
bool	pgs_sw_is_normalized = true;

/*
 * TODO move this function to similarity.c
 */
static double _smithwaterman(char *a, char *b)
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
		if (matrix == NULL)
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
/*
		XXX why simmetrics does this way?
		XXX original algorithm initializes first column with zeros

		float c = swcost(a, b, i, 0);

		if (i == 0)
			matrix[0][0] = max3(0.0, -1 * PGS_SW_GAP_COST, c);
		else
			matrix[i][0] = max3(0.0, matrix[i-1][0] - PGS_SW_GAP_COST, c);

		if (matrix[i][0] > maxvalue)
			maxvalue = matrix[i][0];
*/
		matrix[i][0] = 0.0;
	}
	for (j = 0; j <= blen; j++)
	{
/*
		XXX why simmetrics does this way?
		XXX original algorithm initializes first row with zeros

		float c = swcost(a, b, 0, j);

		if (j == 0)
			matrix[0][0] = max3(0.0, -1 * PGS_SW_GAP_COST, c);
		else
			matrix[0][j] = max3(0.0, matrix[0][j-1] - PGS_SW_GAP_COST, c);

		if (matrix[0][j] > maxvalue)
			maxvalue = matrix[0][j];
*/
		matrix[0][j] = 0.0;
	}

	for (i = 1; i <= alen; i++)
	{
		for (j = 1; j <= blen; j++)
		{
			/* get operation cost */
			float c = swcost(a, b, i - 1, j - 1);

			matrix[i][j] = max4(0.0,
						matrix[i-1][j] + PGS_SW_GAP_COST,
						matrix[i][j-1] + PGS_SW_GAP_COST,
						matrix[i-1][j-1] + c);
			elog(DEBUG2, "(i, j) = (%d, %d); cost(%c, %c): %.3f; max(zero, top, left, diag) = (0.0, %.3f, %.3f, %.3f) = %.3f -- %.3f (%d, %d)",
						i, j, a[i-1], b[j-1], c,
						matrix[i-1][j] + PGS_SW_GAP_COST,
						matrix[i][j-1] + PGS_SW_GAP_COST,
						matrix[i-1][j-1] + c, matrix[i][j], matrix[i][j-1], i, j-1);

			if (matrix[i][j] > maxvalue)
				maxvalue = matrix[i][j];
		}
	}

	for (i = 0; i <= alen; i++)
		for (j = 0; j <= blen; j++)
			elog(DEBUG1, "(%d, %d) = %.3f", i, j, matrix[i][j]);

	for (i = 0; i <= alen; i++)
		free(matrix[i]);
	free(matrix);

	return maxvalue;
}

PG_FUNCTION_INFO_V1(smithwaterman);

Datum
smithwaterman(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	double		maxvalue;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	maxvalue = (float8) min2(strlen(a), strlen(b));

	res = _smithwaterman(a, b);

	elog(DEBUG1, "is normalized: %d", pgs_sw_is_normalized);
	elog(DEBUG1, "maximum length: %.3f", maxvalue);
	elog(DEBUG1, "swdistance(%s, %s) = %.3f", a, b, res);

	if (maxvalue == 0.0)
	{
		res = 1.0;
	}
	if (pgs_sw_is_normalized)
	{
		if (PGS_SW_MAX_COST > (-1 * PGS_SW_GAP_COST))
			maxvalue *= PGS_SW_MAX_COST;
		else
			maxvalue *= -1 * PGS_SW_GAP_COST;

		/* paranoia ? */
		if (maxvalue == 0.0)
			res = 1.0;
		else
			res = (res / maxvalue);
	}

	elog(DEBUG1, "sw(%s, %s) = %.3f", a, b, res);

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(smithwaterman_op);

Datum smithwaterman_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_sw_is_normalized;
	pgs_sw_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					smithwaterman,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_sw_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_sw_threshold);
}
