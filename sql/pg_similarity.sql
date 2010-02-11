--
-- pg_similarity
-- testing similarity functions and operators
--

--
-- Turn off echoing so that expected file does not depend on contents of
-- pg_similarity.sql
--
SET client_min_messages to warning;
\set ECHO none
\i pg_similarity.sql
RESET client_min_messages;
\set ECHO all

\set a '\'Euler Taveira de Oliveira\''
\set b '\'Euler T Oliveira\''

select block(:a, :b), block_op(:a, :b), :a ~++ :b as operator;
select cosine(:a, :b), cosine_op(:a, :b), :a ~## :b as operator;
select dice(:a, :b), dice_op(:a, :b), :a ~-~ :b as operator;
select euclidean(:a, :b), euclidean_op(:a, :b), :a ~!! :b as operator;
--select hamming(:a, :b), hamming_op(:a, :b);
select jaccard(:a, :b), jaccard_op(:a, :b), :a ~?? :b as operator;
select jaro(:a, :b), jaro_op(:a, :b), :a ~%% :b as operator;
select jarowinkler(:a, :b), jarowinkler_op(:a, :b), :a ~@@ :b as operator;
select lev(:a, :b), lev_op(:a, :b), :a ~== :b as operator;
--select levslow(:a, :b), levslow_op(:a, :b);
select matchingcoefficient(:a, :b), matchingcoefficient_op(:a, :b), :a ~^^ :b as operator;
select mongeelkan(:a, :b), mongeelkan_op(:a, :b), :a ~|| :b as operator;
--select needlemanwunsch(:a, :b), needlemanwunsch_op(:a, :b), :a ~#~ :b as operator;
select overlapcoefficient(:a, :b), overlapcoefficient_op(:a, :b), :a ~** :b as operator;
select qgram(:a, :b), qgram_op(:a, :b), :a ~~~ :b as operator;
select smithwaterman(:a, :b), smithwaterman_op(:a, :b), :a ~=~ :b as operator;
select smithwatermangotoh(:a, :b), smithwatermangotoh_op(:a, :b), :a ~!~ :b as operator;

--
-- errors
--
SET pg_similarity.foo_tokenizer to 'alnum';
SET pg_similarity.foo_is_normalized to false;
SET pg_similarity.cosine_threshold to 1.1;
SET pg_similarity.qgram_tokenizer to 'alnum';
SET pg_similarity.jaro_tokenizer to 'word';

CREATE TABLE simtst (a text);

INSERT INTO simtst (a) VALUES
('Euler Taveira de Oliveira'),
('EULER TAVEIRA DE OLIVEIRA'),
('Euler T. de Oliveira'),
('Oliveira, Euler T.'),
('Euler Oliveira'),
('Euler Taveira'),
('EULER TAVEIRA OLIVEIRA'),
('Oliveira, Euler'),
('Oliveira, E. T.'),
('ETO');

-- Levenshtein
SHOW pg_similarity.levenshtein_threshold;
SELECT a FROM simtst WHERE a ~== :a;
SET pg_similarity.levenshtein_threshold to 0.4;
SHOW pg_similarity.levenshtein_threshold;
SELECT a FROM simtst WHERE a ~== :a;

-- Cosine
SHOW pg_similarity.cosine_threshold;
SELECT a FROM simtst WHERE a ~## :a;
SET pg_similarity.cosine_threshold to 0.9;
SHOW pg_similarity.cosine_threshold;
SELECT a FROM simtst WHERE a ~## :a;

-- Overlap Coefficient
SHOW pg_similarity.overlap_tokenizer;
SELECT a FROM simtst WHERE a ~** :a;
SHOW pg_similarity.overlap_tokenizer to 'gram';
SELECT a FROM simtst WHERE a ~** :a;

DROP TABLE simtst;
