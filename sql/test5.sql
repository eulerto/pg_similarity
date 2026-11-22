--
-- Token-based functions when a side has no tokens: 1.0 when neither side
-- has tokens, 0.0 when only one has (and never NaN)
--
LOAD 'pg_similarity';

-- reduce noise
SET extra_float_digits TO 0;

-- '!!' and '??' have no tokens for the default (alnum) tokenizer, but qgram
-- always tokenizes by n-gram, so for qgram they are two different strings
CREATE TABLE emptytst (a text, b text);

INSERT INTO emptytst (a, b) VALUES
('', ''),
('!!', '??'),
('', 'euler'),
('euler', ''),
('!!', 'euler'),
('euler', '??');

SELECT a, b,
	block(a, b), cosine(a, b), dice(a, b), euclidean(a, b),
	jaccard(a, b), matchingcoefficient(a, b), mongeelkan(a, b),
	overlapcoefficient(a, b), qgram(a, b)
FROM emptytst;

SELECT a, b,
	a ~++ b AS block, a ~## b AS cosine, a ~-~ b AS dice,
	a ~!! b AS euclidean, a ~?? b AS jaccard, a ~^^ b AS matching,
	a ~|| b AS mongeelkan, a ~** b AS overlap, a ~~~ b AS qgram
FROM emptytst;

-- unnormalized distances and counts
SET pg_similarity.block_is_normalized TO off;
SET pg_similarity.euclidean_is_normalized TO off;
SET pg_similarity.matching_is_normalized TO off;
SET pg_similarity.qgram_is_normalized TO off;

SELECT a, b,
	block(a, b), euclidean(a, b), matchingcoefficient(a, b), qgram(a, b)
FROM emptytst;

DROP TABLE emptytst;
