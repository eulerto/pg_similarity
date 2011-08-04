/* $PostgreSQL $ */

-- Adjust this setting to control where the objects get dropped.
SET search_path = public;

DROP OPERATOR ~++ (text, text);
DROP FUNCTION block (text, text);
DROP FUNCTION block_op (text, text);

DROP OPERATOR ~## (text, text);
DROP FUNCTION cosine (text, text);
DROP FUNCTION cosine_op (text, text);

DROP OPERATOR ~-~ (text, text);
DROP FUNCTION dice (text, text);
DROP FUNCTION dice_op (text, text);

DROP OPERATOR ~!! (text, text);
DROP FUNCTION euclidean (text, text);
DROP FUNCTION euclidean_op (text, text);

DROP OPERATOR ~@~ (text, text);
DROP FUNCTION hamming_text (text, text);
DROP FUNCTION hamming_text_op (text, text);
DROP FUNCTION hamming (varbit, varbit);
DROP FUNCTION hamming_op (varbit, varbit);

DROP OPERATOR ~?? (text, text);
DROP FUNCTION jaccard (text, text);
DROP FUNCTION jaccard_op (text, text);

DROP OPERATOR ~%% (text, text);
DROP FUNCTION jaro (text, text);
DROP FUNCTION jaro_op (text, text);

DROP OPERATOR ~@@ (text, text);
DROP FUNCTION jarowinkler (text, text);
DROP FUNCTION jarowinkler_op (text, text);

DROP OPERATOR ~== (text, text);
DROP FUNCTION lev (text, text);
DROP FUNCTION lev_op (text, text);

--DROP OPERATOR ~@@ (text, text);
DROP FUNCTION levslow (text, text);
DROP FUNCTION levslow_op (text, text);

DROP OPERATOR ~^^ (text, text);
DROP FUNCTION matchingcoefficient (text, text);
DROP FUNCTION matchingcoefficient_op (text, text);

DROP OPERATOR ~|| (text, text);
DROP FUNCTION mongeelkan (text, text);
DROP FUNCTION mongeelkan_op (text, text);

DROP OPERATOR ~#~ (text, text);
DROP FUNCTION needlemanwunsch (text, text);
DROP FUNCTION needlemanwunsch_op (text, text);

DROP OPERATOR ~** (text, text);
DROP FUNCTION overlapcoefficient (text, text);
DROP FUNCTION overlapcoefficient_op (text, text);

DROP OPERATOR ~~~ (text, text);
DROP FUNCTION qgram (text, text);
DROP FUNCTION qgram_op (text, text);

DROP OPERATOR ~=~ (text, text);
DROP FUNCTION smithwaterman (text, text);
DROP FUNCTION smithwaterman_op (text, text);

DROP OPERATOR ~!~ (text, text);
DROP FUNCTION smithwatermangotoh (text, text);
DROP FUNCTION smithwatermangotoh_op (text, text);

DROP OPERATOR ~*~ (text, text);
DROP FUNCTION soundex (text, text);
DROP FUNCTION soundex_op (text, text);
