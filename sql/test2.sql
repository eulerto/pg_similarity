--
-- errors
--
SHOW pg_similarity.foo_tokenizer;
SHOW pg_similarity.foo_is_normalized;
SET pg_similarity.cosine_threshold to 1.1;
SET pg_similarity.qgram_tokenizer to 'alnum';
SHOW pg_similarity.jaro_tokenizer;

--
-- valid values
--
SET pg_similarity.block_is_normalized to true;
SET pg_similarity.cosine_threshold = 0.72;
SET pg_similarity.dice_tokenizer to 'alnum';
SET pg_similarity.euclidean_is_normalized to false;
SET pg_similarity.jaro_winkler_is_normalized to false;
SET pg_similarity.qgram_tokenizer to 'gram';
