/*----------------------------------------------------------------------------
 *
 * similarity.c
 *
 * Copyright (c) 2008-2026, Euler Taveira de Oliveira
 *
 *----------------------------------------------------------------------------
 */

#include "similarity.h"

#include <float.h>

#ifdef PG_MODULE_MAGIC_EXT
PG_MODULE_MAGIC_EXT(
	.name = "pg_similarity",
	.version = PGS_VERSION
);
#else
PG_MODULE_MAGIC;
#endif

/*
 * Module load callback
 *
 * Holds GUC variables that cause some behavior changes in similarity functions
 *
 */
void
_PG_init(void)
{
	static const struct config_enum_entry pgs_tokenizer_options[] =
	{
		{"alnum", PGS_UNIT_ALNUM, false},
		{"gram", PGS_UNIT_GRAM, false},
		{"word", PGS_UNIT_WORD, false},
		{"camelcase", PGS_UNIT_CAMELCASE, false},
		{NULL, 0, false}
	};
	static const struct config_enum_entry pgs_gram_options[] =
	{
		{"gram", PGS_UNIT_GRAM, false},
		{NULL, 0, false}
	};

	/* Block */
	DefineCustomEnumVariable("pg_similarity.block_tokenizer",
							 "Sets the tokenizer for Block similarity function.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_block_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.block_threshold",
							 "Sets the threshold used by the Block similarity function.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_block_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.block_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_block_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Cosine */
	DefineCustomEnumVariable("pg_similarity.cosine_tokenizer",
							 "Sets the tokenizer for Cosine similarity function.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_cosine_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.cosine_threshold",
							 "Sets the threshold used by the Cosine similarity function.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_cosine_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.cosine_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_cosine_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Dice */
	DefineCustomEnumVariable("pg_similarity.dice_tokenizer",
							 "Sets the tokenizer for Dice similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_dice_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.dice_threshold",
							 "Sets the threshold used by the Dice similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_dice_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.dice_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_dice_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Euclidean */
	DefineCustomEnumVariable("pg_similarity.euclidean_tokenizer",
							 "Sets the tokenizer for Euclidean similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_euclidean_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.euclidean_threshold",
							 "Sets the threshold used by the Euclidean similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_euclidean_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.euclidean_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_euclidean_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Hamming */
	DefineCustomRealVariable("pg_similarity.hamming_threshold",
							 "Sets the threshold used by the Block similarity metric.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_hamming_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.hamming_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_hamming_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Jaccard */
	DefineCustomEnumVariable("pg_similarity.jaccard_tokenizer",
							 "Sets the tokenizer for Jaccard similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_jaccard_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.jaccard_threshold",
							 "Sets the threshold used by the Jaccard similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_jaccard_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.jaccard_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_jaccard_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Jaro */
	DefineCustomRealVariable("pg_similarity.jaro_threshold",
							 "Sets the threshold used by the Jaro similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_jaro_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.jaro_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_jaro_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Jaro-Winkler */
	DefineCustomRealVariable("pg_similarity.jarowinkler_threshold",
							 "Sets the threshold used by the Jaro-Winkler similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_jarowinkler_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.jarowinkler_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_jarowinkler_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Levenshtein */
	DefineCustomRealVariable("pg_similarity.levenshtein_threshold",
							 "Sets the threshold used by the Levenshtein similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_levenshtein_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.levenshtein_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_levenshtein_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Matching Coefficient */
	DefineCustomEnumVariable("pg_similarity.matching_tokenizer",
							 "Sets the tokenizer for Matching Coefficient similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_matching_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.matching_threshold",
							 "Sets the threshold used by the Matching Coefficient similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_matching_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.matching_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_matching_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Monge-Elkan */
	DefineCustomEnumVariable("pg_similarity.mongeelkan_tokenizer",
							 "Sets the tokenizer for Monge-Elkan similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_mongeelkan_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.mongeelkan_threshold",
							 "Sets the threshold used by the Monge-Elkan similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_mongeelkan_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.mongeelkan_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_mongeelkan_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Needleman-Wunsch */
	DefineCustomRealVariable("pg_similarity.nw_threshold",
							 "Sets the threshold used by the Needleman-Wunsch similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_nw_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.nw_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_nw_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.nw_gap_penalty",
							 "Sets the gap penalty used by the Needleman-Wunsch similarity measure.",
							 NULL,
							 &pgs_nw_gap_penalty,
							 -5.0,
							 -DBL_MAX,
							 DBL_MAX,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Overlap Coefficient */
	DefineCustomEnumVariable("pg_similarity.overlap_tokenizer",
							 "Sets the tokenizer for Overlap Coefficient similarity measure.",
							 "Valid values are \"alnum\", \"gram\", \"word\", or \"camelcase\".",
							 &pgs_overlap_tokenizer,
							 PGS_UNIT_ALNUM,
							 pgs_tokenizer_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.overlap_threshold",
							 "Sets the threshold used by the Overlap Coefficient similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_overlap_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.overlap_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_overlap_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Q-Gram */
	DefineCustomEnumVariable("pg_similarity.qgram_tokenizer",
							 "Sets the tokenizer for Q-Gram similarity function.",
							 "Valid value is \"gram\".",
							 &pgs_qgram_tokenizer,
							 PGS_UNIT_GRAM,
							 pgs_gram_options,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomRealVariable("pg_similarity.qgram_threshold",
							 "Sets the threshold used by the Q-Gram similarity function.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_qgram_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.qgram_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_qgram_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Smith-Waterman */
	DefineCustomRealVariable("pg_similarity.sw_threshold",
							 "Sets the threshold used by the Smith-Waterman similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_sw_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.sw_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_sw_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	/* Smith-Waterman-Gotoh */
	DefineCustomRealVariable("pg_similarity.swg_threshold",
							 "Sets the threshold used by the Smith-Waterman-Gotoh similarity measure.",
							 "Valid range is 0.0 .. 1.0.",
							 &pgs_swg_threshold,
							 0.7,
							 0.0,
							 1.0,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);
	DefineCustomBoolVariable("pg_similarity.swg_is_normalized",
							 "Sets if the result value is normalized or not.",
							 NULL,
							 &pgs_swg_is_normalized,
							 true,
							 PGC_USERSET,
							 0,
							 NULL,
							 NULL,
							 NULL);

	EmitWarningsOnPlaceholders("pg_similarity");
}
