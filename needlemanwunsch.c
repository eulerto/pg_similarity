/*----------------------------------------------------------------------------
 *
 * needlemanwunsch.c
 *
 * Needleman-Wunsch is an algorithm that performs a global alignment on two
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
 * +-----------------------+
 * |   | A  | G  | C  | T  |
 * +-----------------------+
 * | A | 10 | -1 | -3 | -4 |
 * +-----------------------+
 * | G | -1 |  7 | -5 | -3 |
 * +-----------------------+
 * | C | -3 | -5 |  9 |  0 |
 * +-----------------------+
 * | T | -4 | -3 |  0 |  8 |
 * +-----------------------+
 *
 * x: GACTAG
 * y: ACCTGAA
 * gap penalty: -5
 *
 * +---------------------------------------------+
 * |   |     |  G  |  A  |  C  |  T  |  A  |  G  |
 * +---------------------------------------------+
 * |   |   0 |  -5 | -10 | -15 | -20 | -25 | -30 |
 * +---------------------------------------------+
 * | A |  -5 |  -1 |   5 |   0 |  -5 | -10 | -15 |
 * +---------------------------------------------+
 * | C | -10 |  -6 |   0 |  14 |   9 |   4 |  -1 |
 * +---------------------------------------------+
 * | C | -15 | -11 |  -5 |   9 |  14 |   9 |   4 |
 * +---------------------------------------------+
 * | T | -20 | -16 | -10 |   4 |  17 |  12 |   7 |
 * +---------------------------------------------+
 * | G | -25 | -13 | -15 |  -1 |  12 |  16 |  19 |
 * +---------------------------------------------+
 * | A | -30 | -18 |  -3 |  -6 |   7 |  22 |  17 |
 * +---------------------------------------------+
 * | A | -35 | -23 |  -8 |  -6 |   2 |  17 |  21 |
 * +---------------------------------------------+
 *
 * http://en.wikipedia.org/wiki/Needleman-Wunsch_algorithm
 *
 *
 * Copyright (c) 2008-2010, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"


/* GUC variables */
double	pgs_nw_threshold = 0.7f;
bool	pgs_nw_is_normalized = true;
double	pgs_nw_gap_penalty = -5.0f;


static int _nwunsch(char *a, char *b, int gap)
{
	int	*arow, *brow, *trow;
	int	alen, blen;
	int	i, j;
	int	res;

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
		arow[i] = gap * i;

	for (i = 1; i <= blen; i++)
	{
		/* first value is 'i' */
		brow[0] = gap * i;

		for (j = 1; j <= alen; j++)
		{
			/* TODO change it to a callback function */
			/* get operation cost */
			int scost = nwcost(a[j-1], b[i-1]);

			brow[j] = max3(brow[j-1] + gap,
							arow[j] + gap,
							arow[j-1] + scost);
			elog(DEBUG2, "(i, j) = (%d, %d); cost(%c, %c): %d; max(top, left, diag) = (%d, %d, %d) = %d",
							i, j, a[j-1], b[i-1], scost,
							brow[j-1] + gap,
							arow[j] + gap,
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
	}

	res = arow[alen];

	free(arow);
	free(brow);

	return res;
}

PG_FUNCTION_INFO_V1(needlemanwunsch);

Datum
needlemanwunsch(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	double		minvalue, maxvalue;
	float8		res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	maxvalue = (float8) max2(strlen(a), strlen(b));

	res = (float8) _nwunsch(a, b, pgs_nw_gap_penalty);

	elog(DEBUG1, "is normalized: %d", pgs_nw_is_normalized);
	elog(DEBUG1, "maximum length: %.3f", maxvalue);
	elog(DEBUG1, "nwdistance(%s, %s) = %.3f", a, b, res);

	if (maxvalue == 0.0)
	{
		PG_RETURN_FLOAT8(1.0);
	}
	else if (pgs_nw_is_normalized)
	{
		/* FIXME normalize nw result */
		minvalue = maxvalue;
		if (PGS_LEV_MAX_COST > pgs_nw_gap_penalty)
			maxvalue *= PGS_LEV_MAX_COST;
		else
			maxvalue *= pgs_nw_gap_penalty;

		if (PGS_LEV_MIN_COST < pgs_nw_gap_penalty)
			minvalue *= PGS_LEV_MIN_COST;
		else
			minvalue *= pgs_nw_gap_penalty;

		if (minvalue < 0.0)
		{
			maxvalue -= minvalue;
			res -= minvalue;
		}

		/* paranoia ? */
		if (maxvalue == 0.0)
		{
			PG_RETURN_FLOAT8(0.0);
		}
		else
		{
			res = 1.0 - (res / maxvalue);
			elog(DEBUG1, "nw(%s, %s) = %.3f", a, b, res);
			PG_RETURN_FLOAT8(res);
		}
	}
	else
	{
		PG_RETURN_FLOAT8(res);
	}
}

PG_FUNCTION_INFO_V1(needlemanwunsch_op);

Datum needlemanwunsch_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_nw_is_normalized;
	pgs_nw_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					needlemanwunsch,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_nw_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_nw_threshold);
}
