/*----------------------------------------------------------------------------
 *
 * jaro.c
 *
 * Jaro Distance [1] is a similarity measure
 *
 *      1      m      1      m      1     m - t
 * s = --- * ----- + --- * ----- + --- * -------
 *      3     |a|     3     |b|     3       m
 *
 * where m is the number of matching characters [2], t is the number of
 * transpositions [3], |a| is the length of string a and |b| is the length of
 * string b.
 *
 * [2] two characters from a and b are considered matching iif they're not
 * farther than floor(max(|a|, |b|) / 2) - 1.
 *
 * [3] number of transpositions is the number of matchings that are in a
 * different sequence order divided by 2.
 *
 * Jaro-Winkler [4] Distance is a similarity measure
 *
 * It's an improvement over Jaro's original work. It gives more weight if the
 * initial characters are the same. So,
 *
 * w = s + (l * p * (1 - s))
 *
 * where l is the length of common prefix up to 4 characters, p is a scaling
 * factor (Winkler's suggestion is 0.1), and s is the Jaro Distance.
 *
 * For example:
 *
 * x: euler
 * y: heuser
 *
 *      1     4     1     4     1     4 - 0     4      2     1
 * s = --- * --- + --- * --- + --- * ------- = ---- + --- + --- = 0.822...
 *      3     5     3     6     3       4       15     9     3
 *
 *
 * w = 0.822 + (0 * 0.1 * (1 - 0.822)) = 0.822...
 *
 *
 * [1] Jaro, M. A. (1989). "Advances in record linking methodology as applied
 * to the 1985 census of Tampa Florida". Journal of the American Statistical
 * Society 84 (406): 414–20.
 *
 * [4] Winkler, W. E. (2006). "Overview of Record Linkage and Current Research
 * Directions". Research Report Series, RRS.
 * http://www.census.gov/srd/papers/pdf/rrs2006-02.pdf.
 *
 *
 * Copyright (c) 2008-2012, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"

#include <math.h>

/* GUC variables */
double	pgs_jaro_threshold = 0.7f;
bool	pgs_jaro_is_normalized = true;
double	pgs_jarowinkler_threshold = 0.7f;
bool	pgs_jarowinkler_is_normalized = true;


static double _jaro(char *a, char *b)
{
	int		alen, blen;
	int		i, j, k;

	int		cd;		/* common window distance */
	int		cc = 0;		/* number of common characters */
	int		tr = 0;		/* number of transpositions */
	double	res;
	int		*amatch;	/* matchs in string a; match = 1; unmatch = 0  !! USED? !!*/
	int		*bmatch;	/* matchs in string b; match = 1; unmatch = 0 */
	int		*posa;		/* positions of matched characters in a */
	int		*posb;		/* positions of matched characters in b */

	alen = strlen(a);
	blen = strlen(b);

	elog(DEBUG1, "alen: %d; blen: %d", alen, blen);

	if (alen > PGS_MAX_STR_LEN || blen > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("argument exceeds the maximum length of %d bytes",
					PGS_MAX_STR_LEN)));

	/* if one string has zero length then return zero */
	if (alen == 0 || blen == 0)
		return 0.0;

	/*
	 * allocate 2 vectors of integers. each position will be 0 or 1 depending
	 * on the character in that position is found between common window distance.
	 */
	amatch = palloc(sizeof(int) * alen);
	bmatch = palloc(sizeof(int) * blen);

	for (i = 0; i < alen; i++)
		amatch[i] = 0;
	for (j = 0; j < blen; j++)
		bmatch[j] = 0;

	/* common window distance is floor(max(alen, blen) / 2) - 1 */
	cd = (int) floor((alen > blen ? alen : blen) / 2) - 1;
	/* catch case when alen = blen = 1 */
	if (cd < 0)
		cd = 0;

	elog(DEBUG1, "common window distance: %d", cd);

#ifdef PGS_IGNORE_CASE
	elog(DEBUG2, "case-sensitive turns off");
	for (i = 0; i < alen; i++)
		a[i] = tolower(a[i]);
	for (j = 0; j < blen; j++)
		b[j] = tolower(b[j]);
#endif

	for (i = 0; i < alen; i++)
	{
		/*
		 * calculate window test limits. limit inf to 0 and sup to blen 
		 */
		int inf = max2(i - cd, 0);
		int sup = i + cd + 1;
		sup = min2(sup, blen);

		/*
		 * no more common characters 'cause we don't have characters in b
		 * to test with characters in a
		 */
		if (inf >= sup)
			break;

		for (j = inf; j < sup; j++)
		{
			/*
			 * if found some match and it's not matched yet:
			 * (i) flag match characters in a and b
			 * (ii) increment cc
			 */
			if (bmatch[j] != 1 && a[i] == b[j])
			{
				amatch[i] = 1;
				bmatch[j] = 1;
				cc++;

				break;
			}
		}
	}

	elog(DEBUG1, "common characters: %d", cc);

	/* no common characters then return 0 */
	if (cc == 0)
		return 0.0;

	/* allocate vector of positions */
	posa = palloc(sizeof(int) * cc);
	posb = palloc(sizeof(int) * cc);
	
	k = 0;
	for (i = 0; i < alen; i++)
	{
		if (amatch[i] == 1)
		{
			posa[k] = i;
			k++;
		}
	}

	k = 0;
	for (j = 0; j < blen; j++)
	{
		if (bmatch[j] == 1)
		{
			posb[k] = j;
			k++;
		}
	}

	pfree(amatch);
	pfree(bmatch);

	/* counting half-transpositions */
	for (i = 0; i < cc; i++)
		if (a[posa[i]] != b[posb[i]])
			tr++;

	pfree(posa);
	pfree(posb);

	elog(DEBUG1, "half transpositions: %d", tr);

	/* real number of transpositions */
	tr /= 2;

	elog(DEBUG1, "real transpositions: %d", tr);

	res = PGS_JARO_W1 * cc / alen + PGS_JARO_W2 * cc / blen + PGS_JARO_WT * (cc - tr) / cc;

	elog(DEBUG1, "jaro(%s, %s) = %f * %d / %d + %f * %d / %d + %f * (%d - %d) / %d = %f",
			a, b, PGS_JARO_W1, cc, alen, PGS_JARO_W2, cc, blen, PGS_JARO_WT, cc, tr, cc, res);

	return res;
}

PG_FUNCTION_INFO_V1(jaro);

Datum
jaro(PG_FUNCTION_ARGS)
{
	char	*a, *b;
	float8	res;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	res = _jaro(a, b);

	elog(DEBUG1, "is normalized: %d", pgs_jaro_is_normalized);
	elog(DEBUG1, "jaro(%s, %s) = %f", a, b, res);

	/* normalized and unnormalized version are the same */
	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(jaro_op);

Datum jaro_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_jaro_is_normalized;
	pgs_jaro_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					jaro,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_jaro_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_jaro_threshold);
}

PG_FUNCTION_INFO_V1(jarowinkler);

Datum
jarowinkler(PG_FUNCTION_ARGS)
{
	char	*a, *b;
	float8	resj, res;
	int	i;
	int	plen = 0;

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(1))));

	resj = _jaro(a, b);

	res = resj;

	elog(DEBUG1, "jaro(%s, %s) = %f", a, b, resj);

	if (resj > PGS_JARO_BOOST_THRESHOLD)
	{
		for (i = 0; i < strlen(a) && i < strlen(b) && i < PGS_JARO_PREFIX_SIZE; i++)
		{
			if (a[i] == b[i])
				plen++;
			else
				break;
		}

		elog(DEBUG1, "prefix length: %d", plen);

		res += PGS_JARO_SCALING_FACTOR * plen * (1.0 - resj);
	}

	elog(DEBUG1, "is normalized: %d", pgs_jarowinkler_is_normalized);
	elog(DEBUG1, "jarowinkler(%s, %s) = %f + %d * %f * (1.0 - %f) = %f",
			a, b, resj, plen, PGS_JARO_SCALING_FACTOR, resj, res);

	/* normalized and unnormalized version are the same */
	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(jarowinkler_op);

Datum jarowinkler_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_jarowinkler_is_normalized;
	pgs_jarowinkler_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
					jarowinkler,
					PG_GETARG_DATUM(0),
					PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_jarowinkler_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_jarowinkler_threshold);
}
