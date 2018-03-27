/*----------------------------------------------------------------------------
 *
 * smithwatermangotoh.c
 *
 * Copyright (c) 2008-2018, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"

double	pgs_swg_threshold = 0.7f;
bool	pgs_swg_is_normalized = true;

/*
 * TODO move this function to similarity.c
 */
static double _smithwatermangotoh(char *a, char *b)
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

PG_FUNCTION_INFO_V1(smithwatermangotoh);

Datum
smithwatermangotoh(PG_FUNCTION_ARGS)
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

	res = _smithwatermangotoh(a, b);

	elog(DEBUG1, "is normalized: %d", pgs_swg_is_normalized);
	elog(DEBUG1, "maximum length: %.3f", maxvalue);
	elog(DEBUG1, "swgdistance(%s, %s) = %.3f", a, b, res);

	if (maxvalue == 0)
	{
		res = 1.0;
	}
	if (pgs_swg_is_normalized)
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

	elog(DEBUG1, "swg(%s, %s) = %.3f", a, b, res);

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(smithwatermangotoh_op);

Datum smithwatermangotoh_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_swg_is_normalized;
	pgs_swg_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					smithwatermangotoh,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_swg_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_swg_threshold);
}
