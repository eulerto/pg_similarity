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

\copy simtst FROM 'data/similarity.data'

SELECT a, block(a, :a) FROM simtst WHERE a ~++ :a;
SELECT a, cosine(a, :a) FROM simtst WHERE a ~## :a;

CREATE INDEX simtsti ON simtst USING gin (a gin_similarity_ops);

SELECT a, block(a, :a) FROM simtst WHERE a ~++ :a;
SET enable_bitmapscan TO OFF;
SELECT a, block(a, :a) FROM simtst WHERE a ~++ :a;
SET enable_bitmapscan TO ON;
SELECT a, cosine(a, :a) FROM simtst WHERE a ~## :a;

DROP TABLE simtst;
