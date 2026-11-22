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
 * Copyright (c) 2008-2026, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "port/pg_bitutils.h"
#include "utils/varbit.h"

/* bytes XORed per pg_popcount() call */
#define	PGS_HAMMING_CHUNK	8192


/* GUC variables */
double	pgs_hamming_threshold = 0.7;
bool	pgs_hamming_is_normalized = true;

static float8
hamming_internal(VarBit *a, VarBit *b, bool normalized)
{
	int		alen, blen;
	uint8		*pa, *pb;
	int		maxlen;
	float8		res;
	int		nbytes;
	uint64		dist = 0;
	char		buf[PGS_HAMMING_CHUNK];

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

	/*
	 * The distance is the number of bits set in a XOR b. XOR a chunk at a time
	 * into a local buffer and let pg_popcount() count it. The pad bits of the
	 * last byte are always zero in both strings, so they don't count.
	 */
	nbytes = VARBITBYTES(a);
	for (int off = 0; off < nbytes; off += PGS_HAMMING_CHUNK)
	{
		int		len = Min(PGS_HAMMING_CHUNK, nbytes - off);

		CHECK_FOR_INTERRUPTS();

		for (int i = 0; i < len; i++)
			buf[i] = pa[off + i] ^ pb[off + i];
		dist += pg_popcount(buf, len);
	}
	res = (float8) dist;

	elog(DEBUG1, "is normalized: %d", normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);
	/*
	 * FIXME print string of bits
	elog(DEBUG1, "hammingdistance(%s, %s) = %.3f", DatumGetCString(varbit_out(VarBitPGetDatum(a))), DatumGetCString(varbit_out(VarBitPGetDatum(b))), res);
	*/

	/* if one string has zero length then return one */
	if (maxlen == 0)
		return 1.0;
	else if (normalized)
	{
		res = 1.0 - (res / maxlen);
		/*
		 * FIXME print string of bits
		elog(DEBUG1, "hamming(%s, %s) = %.3f", DatumGetCString(varbit_out(VarBitPGetDatum(a))), DatumGetCString(varbit_out(VarBitPGetDatum(b))), res);
		*/
		return res;
	}
	else
		return res;
}

PG_FUNCTION_INFO_V1(hamming);

Datum
hamming(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(hamming_internal(PG_GETARG_VARBIT_P(0),
									  PG_GETARG_VARBIT_P(1),
									  pgs_hamming_is_normalized));
}

PG_FUNCTION_INFO_V1(hamming_op);

Datum
hamming_op(PG_FUNCTION_ARGS)
{
	/* the threshold is normalized, so compare a normalized result */
	float8		res = hamming_internal(PG_GETARG_VARBIT_P(0),
									   PG_GETARG_VARBIT_P(1),
									   true);

	PG_RETURN_BOOL(res >= pgs_hamming_threshold);
}

static float8
hamming_text_internal(text *ta, text *tb, bool normalized)
{
	char		*a, *b;
	int			alen, blen;
	char		*pa, *pb;
	int			maxlen;
	float8		res = 0.0;

	a = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(ta)));
	b = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(tb)));

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
				 errmsg("text strings must have the same length")));

	elog(DEBUG1, "a: %s ; b: %s", a, b);

	/* alen and blen have the same length */
	maxlen = alen;

	pa = a;
	pb = b;
	while (*pa != '\0')
	{
		elog(DEBUG4, "a: %c ; b: %c", *pa, *pb);

		if (*pa++ != *pb++)
			res += 1.0;
	}

	elog(DEBUG1, "is normalized: %d", normalized);
	elog(DEBUG1, "maximum length: %d", maxlen);

	elog(DEBUG1, "hammingdistance(%s, %s) = %.3f", a, b, res);

	/* if one string has zero length then return one */
	if (maxlen == 0)
		return 1.0;
	else if (normalized)
	{
		res = 1.0 - (res / maxlen);
		elog(DEBUG1, "hamming(%s, %s) = %.3f", a, b, res);
		return res;
	}
	else
		return res;
}

PG_FUNCTION_INFO_V1(hamming_text);

Datum
hamming_text(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(hamming_text_internal(PG_GETARG_TEXT_P(0),
										   PG_GETARG_TEXT_P(1),
										   pgs_hamming_is_normalized));
}

PG_FUNCTION_INFO_V1(hamming_text_op);

Datum
hamming_text_op(PG_FUNCTION_ARGS)
{
	/* the threshold is normalized, so compare a normalized result */
	float8		res = hamming_text_internal(PG_GETARG_TEXT_P(0),
											PG_GETARG_TEXT_P(1),
											true);

	PG_RETURN_BOOL(res >= pgs_hamming_threshold);
}
