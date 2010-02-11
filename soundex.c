/*----------------------------------------------------------------------------
 *
 * soundex.c
 *
 * Copyright (c) 2008-2010, Euler Taveira de Oliveira
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

static char *_soundex(const char *a)
{
	int		alen;
	int		i;
	int		len;
	char	scode[PGS_SOUNDEX_LEN + 1];
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

	scode[PGS_SOUNDEX_LEN] = '\0';

	/* ignoring non-alpha characters */
	while (!isalpha(*a) && a)
		a++;

	if (!a)
		elog(ERROR, "string doesn't contain non-alpha character(s)");

	/* get the first letter */
	scode[0] = *a++;
	len = 1;

	elog(DEBUG2, "The first letter is: %c", scode[0]);

	while (*a && len < PGS_SOUNDEX_LEN)
	{
		int curcode = convert_soundex(*a);

		elog(DEBUG3, "The code for '%c' is: %d", *a, c);

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
	char	*a;
	char	res[PGS_SOUNDEX_LEN];

	res = palloc(PGS_SOUNDEX_LEN + 1);

	a = DatumGetPointer(DirectFunctionCall1(textout, PointerGetDatum(PG_GETARG_TEXT_P(0))));

	if (strlen(a) > PGS_MAX_STR_LEN)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("argument exceeds the maximum length of %d bytes",
					 PGS_MAX_STR_LEN)));

	res = _soundex(a);

	elog(DEBUG1, "soundex(%s) = %s", a, res);

	PG_RETURN_TEXT_P(cstring_to_text(res));
}
