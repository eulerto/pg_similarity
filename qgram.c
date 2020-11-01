/*----------------------------------------------------------------------------
 *
 * qgram.c
 *
 * Q-Gram distance
 *
 * This function is the same as block (L1 distance); the only difference is that
 * it uses only tokenizeByGram.
 *
 * For example:
 *
 * x: euler = {eu, ul, le, er}
 * y: heuser = {he, eu, us, se, er}
 * t: {eu, ul, le, er, he, us, se}
 *
 *        eu        ul        le        er        he        us        se
 * s = |1 - 1| + |1 - 0| + |1 - 0| + |1 - 1| + |0 - 1| + |0 - 1| + |0 - 1| = 5
 *
 * PS> we call n-grams: (i) n-sequence of letters (ii) n-sequence of words
 *
 * http://en.wikipedia.org/wiki/Block_distance
 *
 *
 * Copyright (c) 2008-2020, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_qgram_tokenizer = PGS_UNIT_GRAM;
double	pgs_qgram_threshold = 0.7f;
bool	pgs_qgram_is_normalized = true;

PG_FUNCTION_INFO_V1(qgram);

Datum
qgram(PG_FUNCTION_ARGS)
{
	float8		res;
	bool		tmp;
	int			tmp2;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	tmp = pgs_block_is_normalized;
	pgs_block_is_normalized = pgs_qgram_is_normalized;

	/*
	 * store *_tokenizer value temporarily 'cause
	 * we're using block function
	 */
	tmp2 = pgs_block_tokenizer;
	pgs_block_tokenizer = pgs_qgram_tokenizer;

	res = DatumGetFloat8(DirectFunctionCall2(
							 block,
							 PG_GETARG_DATUM(0),
							 PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_block_is_normalized = tmp;
	pgs_block_tokenizer = tmp2;

	PG_RETURN_FLOAT8(res);
}

PG_FUNCTION_INFO_V1(qgram_op);

Datum qgram_op(PG_FUNCTION_ARGS)
{
	float8	res;

	/*
	 * store *_is_normalized value temporarily 'cause
	 * threshold (we're comparing against) is normalized
	 */
	bool	tmp = pgs_qgram_is_normalized;
	pgs_qgram_is_normalized = true;

	res = DatumGetFloat8(DirectFunctionCall2(
							 qgram,
							 PG_GETARG_DATUM(0),
							 PG_GETARG_DATUM(1)));

	/* we're done; back to the previous value */
	pgs_qgram_is_normalized = tmp;

	PG_RETURN_BOOL(res >= pgs_qgram_threshold);
}
