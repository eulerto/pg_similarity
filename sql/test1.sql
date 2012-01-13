--
-- pg_similarity
-- testing similarity functions and operators
--

--
-- Turn off echoing so that expected file does not depend on contents of
-- this file
--
SET client_min_messages to warning;
\set ECHO none
\i pg_similarity.sql
RESET client_min_messages;
\set ECHO all

\set a '\'Euler Taveira de Oliveira\''
\set b '\'Euler T Oliveira\''
\set c '\'Oiler Taviera do Oliviera\''

select block(:a, :b), block_op(:a, :b), :a ~++ :b as operator;
select cosine(:a, :b), cosine_op(:a, :b), :a ~## :b as operator;
select dice(:a, :b), dice_op(:a, :b), :a ~-~ :b as operator;
select euclidean(:a, :b), euclidean_op(:a, :b), :a ~!! :b as operator;
select hamming_text(:a, :c), hamming_text_op(:a, :c), :a ~@~ :c as operator;
select jaccard(:a, :b), jaccard_op(:a, :b), :a ~?? :b as operator;
select jaro(:a, :b), jaro_op(:a, :b), :a ~%% :b as operator;
select jarowinkler(:a, :b), jarowinkler_op(:a, :b), :a ~@@ :b as operator;
select lev(:a, :b), lev_op(:a, :b), :a ~== :b as operator;
select levslow(:a, :b), levslow_op(:a, :b);
select matchingcoefficient(:a, :b), matchingcoefficient_op(:a, :b), :a ~^^ :b as operator;
--select mongeelkan(:a, :b), mongeelkan_op(:a, :b), :a ~|| :b as operator;
--select needlemanwunsch(:a, :b), needlemanwunsch_op(:a, :b), :a ~#~ :b as operator;
select overlapcoefficient(:a, :b), overlapcoefficient_op(:a, :b), :a ~** :b as operator;
select qgram(:a, :b), qgram_op(:a, :b), :a ~~~ :b as operator;
--select smithwaterman(:a, :b), smithwaterman_op(:a, :b), :a ~=~ :b as operator;
--select smithwatermangotoh(:a, :b), smithwatermangotoh_op(:a, :b), :a ~!~ :b as operator;
select soundex(:a, :b), soundex_op(:a, :b), :a ~*~ :b as operator;
