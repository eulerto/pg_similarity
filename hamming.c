/*----------------------------------------------------------------------------
 *
 * hamming.c
 *
 * Hamming Distance is a similarity metric
 *
 * Hamming distance between two strings of equal length is the number of
 * positions for which the correspondings symbols are different, i.e, the
 * number of substitutions required to change one into the other.
 *
 * X is a bit string
 * Y is a bit string
 *
 * For each position i we compare X[i] with Y[i]; if it doesn't match we
 * accumulate those mismatches.
 *
 * For example:
 *
 * x: 1010101010
 * y: 1010111000
 *         ^  ^
 *      mismatches
 *
 * s = 2
 *
 *
 * Copyright (c) 2008-2010, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "utils/varbit.h"


/* GUC variables */
double	pgs_hamming_threshold = 0.7f;
bool	pgs_hamming_is_normalized = true;

PG_FUNCTION_INFO_V1(hamming);

Datum
hamming(PG_FUNCTION_ARGS)
{
	VarBit		*a, *b;
	int		alen, blen;
	bits8		*pa, *pb;
	int		maxlen;
	float4		res = 0.0;
	int		i;
	int		n;

	a = PG_GETARG_VARBIT_P(0);
	b = PG_GETARG_VARBIT_P(1);

	alen = VARBITLEN(a);
	blen = VARBITLEN(b);

	elog(DEBUG1, "alen: %d; blen: %d", alen, blen);

	if (alen != blen)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("bit strings must have the same length")));

	/* alen and blen have the same length */
	maxlen = alen;

	pa = VARBITS(a);
	pb = VARBITS(b);

	for (i = 0; i < VARBITBYTES(a); i++)
	{
		n = *pa++ ^ *pb++;
		while (n)
		{
			res += n & 1;
			n >>= 1;
		}
	}

	elog(DEBUG1, "is normalized: %d", pgs_hamming_is_normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);
	/*
	 * FIXME print string of bits
	elog(DEBUG1, "hammingdistance(%s, %s) = %.3f", DatumGetCString(varbit_out(VarBitPGetDatum(a))), DatumGetCString(varbit_out(VarBitPGetDatum(b))), res);
	*/

	/* if one string has zero length then return one */
	if (maxlen == 0)
	{
		PG_RETURN_FLOAT4(1.0);
	}
	else if (pgs_hamming_is_normalized)
	{
		res = 1.0 - (res / maxlen);
		/*
		 * FIXME print string of bits
		elog(DEBUG1, "hamming(%s, %s) = %.3f", DatumGetCString(varbit_out(VarBitPGetDatum(a))), DatumGetCString(varbit_out(VarBitPGetDatum(b))), res);
		*/
		PG_RETURN_FLOAT4(res);
	}
	else
	{
		PG_RETURN_FLOAT4(res);
	}
}

PG_FUNCTION_INFO_V1(hamming_op);

Datum hamming_op(PG_FUNCTION_ARGS)
{
	float4	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_hamming_is_normalized;
	pgs_hamming_is_normalized = true;

	res = DatumGetFloat4(DirectFunctionCall2(
					hamming,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_hamming_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_hamming_threshold);
}

PG_FUNCTION_INFO_V1(hamming_text);

Datum
hamming_text(PG_FUNCTION_ARGS)
{
	char		*a, *b;
	int			alen, blen;
	char		*pa, *pb;
	int			maxlen;
	float4		res = 0.0;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	alen = strlen(a);
	blen = strlen(b);

	if (alen > PGS_MAX_STR_LEN || blen > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	elog(DEBUG1, "alen: %d; blen: %d", alen, blen);

	if (alen != blen)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("bit strings must have the same length")));

	elog(DEBUG1, "a: %s ; b: %s", a, b);

	/* alen and blen have the same length */
	maxlen = alen;

	pa = a;
	pb = b;
	while (*pa != '\0')
	{
		elog(DEBUG4, "a: %c ; b: %c", *pa, *pb);

		if (*pa++ != *pb++)
			res++;
	}

	elog(DEBUG1, "is normalized: %d", pgs_hamming_is_normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);

	elog(DEBUG1, "hammingdistance(%s, %s) = %.3f", DatumGetCString(a), DatumGetCString(b), res);

	/* if one string has zero length then return one */
	if (maxlen == 0)
	{
		PG_RETURN_FLOAT4(1.0);
	}
	else if (pgs_hamming_is_normalized)
	{
		res = 1.0 - (res / maxlen);
		elog(DEBUG1, "hamming(%s, %s) = %.3f", DatumGetCString(a), DatumGetCString(b), res);
		PG_RETURN_FLOAT4(res);
	}
	else
	{
		PG_RETURN_FLOAT4(res);
	}
}

PG_FUNCTION_INFO_V1(hamming_text_op);

Datum hamming_text_op(PG_FUNCTION_ARGS)
{
	float4	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_hamming_is_normalized;
	pgs_hamming_is_normalized = true;

	res = DatumGetFloat4(DirectFunctionCall2(
					hamming_text,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_hamming_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_hamming_threshold);
}
