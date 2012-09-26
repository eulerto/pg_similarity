-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_similarity" to load this file. \quit

-- Block
ALTER EXTENSION pg_similarity ADD function block(text, text);
ALTER EXTENSION pg_similarity ADD function block_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~++(text, text);

-- Cosine
ALTER EXTENSION pg_similarity ADD function cosine(text, text);
ALTER EXTENSION pg_similarity ADD function cosine_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~##(text, text);

-- Dice
ALTER EXTENSION pg_similarity ADD function dice(text, text);
ALTER EXTENSION pg_similarity ADD function dice_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~-~(text, text);

-- Euclidean
ALTER EXTENSION pg_similarity ADD function euclidean(text, text);
ALTER EXTENSION pg_similarity ADD function euclidean_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~!!(text, text);

-- Hamming
ALTER EXTENSION pg_similarity ADD function hamming(varbit, varbit);
ALTER EXTENSION pg_similarity ADD function hamming_op(varbit, varbit);
ALTER EXTENSION pg_similarity ADD function hamming_text(text, text);
ALTER EXTENSION pg_similarity ADD function hamming_text_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~@~(text, text);

-- Jaccard
ALTER EXTENSION pg_similarity ADD function jaccard(text, text);
ALTER EXTENSION pg_similarity ADD function jaccard_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~??(text, text);

-- Jaro
ALTER EXTENSION pg_similarity ADD function jaro(text, text);
ALTER EXTENSION pg_similarity ADD function jaro_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~%%(text, text);

-- Jaro-Winkler
ALTER EXTENSION pg_similarity ADD function jarowinkler(text, text);
ALTER EXTENSION pg_similarity ADD function jarowinkler_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~@@(text, text);

-- Levenshtein
ALTER EXTENSION pg_similarity ADD function lev(text, text);
ALTER EXTENSION pg_similarity ADD function lev_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~==(text, text);
-- just for academic purpose
--ALTER EXTENSION pg_similarity ADD function levslow(text, text);
--ALTER EXTENSION pg_similarity ADD function levslow_op(text, text);
--ALTER EXTENSION pg_similarity ADD operator ~=^(text, text);
-- those functions were created in earlier versions but are no longer included
DROP FUNCTION levslow(text, text);
DROP FUNCTION levslow_op(text, text);

-- Matching Coefficient
ALTER EXTENSION pg_similarity ADD function matchingcoefficient(text, text);
ALTER EXTENSION pg_similarity ADD function matchingcoefficient_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~^^(text, text);

-- Monge-Elkan
ALTER EXTENSION pg_similarity ADD function mongeelkan(text, text);
ALTER EXTENSION pg_similarity ADD function mongeelkan_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~||(text, text);

-- Needleman-Wunsch
ALTER EXTENSION pg_similarity ADD function needlemanwunsch(text, text);
ALTER EXTENSION pg_similarity ADD function needlemanwunsch_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~#~(text, text);

-- Overlap
ALTER EXTENSION pg_similarity ADD function overlapcoefficient(text, text);
ALTER EXTENSION pg_similarity ADD function overlapcoefficient_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~**(text, text);

-- Q-Gram
ALTER EXTENSION pg_similarity ADD function qgram(text, text);
ALTER EXTENSION pg_similarity ADD function qgram_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~~~(text, text);

-- Smith-Waterman
ALTER EXTENSION pg_similarity ADD function smithwaterman(text, text);
ALTER EXTENSION pg_similarity ADD function smithwaterman_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~=~(text, text);

-- Smith-Waterman-Gotoh
ALTER EXTENSION pg_similarity ADD function smithwatermangotoh(text, text);
ALTER EXTENSION pg_similarity ADD function smithwatermangotoh_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~!~(text, text);

-- Soundex
ALTER EXTENSION pg_similarity ADD function soundex(text, text);
ALTER EXTENSION pg_similarity ADD function soundex_op(text, text);
ALTER EXTENSION pg_similarity ADD operator ~*~(text, text);

-- GIN support
ALTER EXTENSION pg_similarity ADD function gin_extract_value_token(internal, internal, internal);
ALTER EXTENSION pg_similarity ADD function gin_extract_query_token(internal, internal, int2, internal, internal, internal, internal);
ALTER EXTENSION pg_similarity ADD function gin_token_consistent(internal, int2, internal, int4, internal, internal, internal, internal);
ALTER EXTENSION pg_similarity ADD operator class gin_similarity_ops using gin;
