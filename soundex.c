/*----------------------------------------------------------------------------
 *
 * soundex.c
 *
 * Copyright (c) 2008-2018, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"


static const char *stable =
	/*		 ABCDEFGHIJKLMNOPQRSTUVWXYZ */
	"01230120022455012623010202";

/*
 * soundex code is only defined to ASCII characters
 */
static char convert_soundex(char a)
{
	a = toupper((unsigned char) a);
	/* soundex code is only defined to ASCII characters */
	if (a >= 'A' && a <= 'Z')
		return stable[a - 'A'];
	else
		return a;
}

static char *_soundex(char *a)
{
	int		alen;
	int		i;
	int		len;
	char	*scode;
	int		lastcode = PGS_SOUNDEX_INV_CODE;

	alen = strlen(a);

	elog(DEBUG2, "alen: %d", alen);

	if (alen == 0)
		return NULL;

#ifdef PGS_IGNORE_CASE
	elog(DEBUG2, "case-sensitive turns off");
	for (i = 0; i < alen; i++)
		a[i] = toupper(a[i]);
#endif

	scode = palloc(PGS_SOUNDEX_LEN + 1);

	scode[PGS_SOUNDEX_LEN] = '\0';

	/* ignoring non-alpha characters */
	while (!isalpha(*a) && *a != '\0')
		a++;

	if (*a == '\0')
		elog(ERROR, "string doesn't contain non-alpha character(s)");

	/* get the first letter */
	scode[0] = *a++;
	len = 1;

	elog(DEBUG2, "The first letter is: %c", scode[0]);

	while (*a && len < PGS_SOUNDEX_LEN)
	{
		int curcode = convert_soundex(*a);

		elog(DEBUG3, "The code for '%c' is: %d", *a, curcode);

		if (isalpha(*a) && (curcode != lastcode) && curcode != '0')
		{
			scode[len] = curcode;
			elog(DEBUG2, "scode[%d] = %d", len, curcode);
			len++;
		}
		lastcode = curcode;
		a++;
	}

	/* fill with zeros (if necessary) */
	while (len < PGS_SOUNDEX_LEN)
	{
		scode[len] = '0';
		elog(DEBUG2, "scode[%d] = %d", len, scode[len]);
		len++;
	}

	return scode;
}

PG_FUNCTION_INFO_V1(soundex);

Datum
soundex(PG_FUNCTION_ARGS)
{
	char	*a, *b;
	char	*resa;
	char	*resb;
	float8	res;

	a = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(PG_GETARG_TEXT_P(0))));
	b = DatumGetPointer(DirectFunctionCall1(textout,
											PointerGetDatum(PG_GETARG_TEXT_P(1))));

	if (strlen(a) > PGS_MAX_STR_LEN || strlen(b) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("argument exceeds the maximum length of %d bytes",
						PGS_MAX_STR_LEN)));

	resa = _soundex(a);
	resb = _soundex(b);

	elog(DEBUG1, "soundex(%s) = %s", a, resa);
	elog(DEBUG1, "soundex(%s) = %s", b, resb);

	/*
	 * we don't have threshold in soundex algorithm, instead same code means strings
	 * are similar (i.e. threshold is 1.0) or it is not (i.e. threshold is 0.0).
	 */
	if (strncmp(resa, resb, PGS_SOUNDEX_LEN) == 0)
		res = 1.0;
	else
		res = 0.0;

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(soundex_op);

Datum soundex_op(PG_FUNCTION_ARGS)
{
	float8	res;

	res = DatumGetFloat8(DirectFunctionCall2(
							 soundex,
							 PG_GETARG_DATUM(0),
							 PG_GETARG_DATUM(1)));

	PG_RETURN_BOOL(res == 1.0);
}
