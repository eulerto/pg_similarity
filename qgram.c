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
 * Copyright (c) 2008-2026, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"
#include "tokenizer.h"


/* GUC variables */
int		pgs_qgram_tokenizer = PGS_UNIT_GRAM;
double	pgs_qgram_threshold = 0.7;
bool	pgs_qgram_is_normalized = true;

PG_FUNCTION_INFO_V1(qgram);

Datum
qgram(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(pgs_block_internal(PG_GETARG_TEXT_P(0),
										PG_GETARG_TEXT_P(1),
										pgs_qgram_tokenizer,
										pgs_qgram_is_normalized));
}

PG_FUNCTION_INFO_V1(qgram_op);

Datum
qgram_op(PG_FUNCTION_ARGS)
{
	/* the threshold is normalized, so compare a normalized result */
	float8		res = pgs_block_internal(PG_GETARG_TEXT_P(0),
										 PG_GETARG_TEXT_P(1),
										 pgs_qgram_tokenizer,
										 true);

	PG_RETURN_BOOL(res >= pgs_qgram_threshold);
}
