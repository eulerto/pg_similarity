LOAD 'pg_similarity';

-- reduce noise
SET extra_float_digits TO 0;

\set a '\'Euler Taveira de Oliveira\''

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
SET pg_similarity.overlap_tokenizer to 'gram';
SET pg_similarity.overlap_threshold to 0.8;
SELECT a FROM simtst WHERE a ~** :a;

DROP TABLE simtst;
