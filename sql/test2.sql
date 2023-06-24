--
-- errors
--
SET pg_similarity.cosine_threshold to 1.1;
SET pg_similarity.qgram_tokenizer to 'alnum';

--
-- valid values
--
SET pg_similarity.block_is_normalized to true;
SET pg_similarity.cosine_threshold = 0.72;
SET pg_similarity.dice_tokenizer to 'alnum';
SET pg_similarity.euclidean_is_normalized to false;
SET pg_similarity.jarowinkler_is_normalized to false;
SET pg_similarity.qgram_tokenizer to 'gram';
