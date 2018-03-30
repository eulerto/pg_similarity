-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_similarity" to load this file. \quit

-- Block
CREATE FUNCTION block (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'block'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION block_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'block_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~++ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = block_op,
	COMMUTATOR = '~++',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Cosine
CREATE FUNCTION cosine (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'cosine'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION cosine_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'cosine_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~## (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = cosine_op,
	COMMUTATOR = '~##',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Dice
CREATE FUNCTION dice (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'dice'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION dice_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'dice_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~-~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = dice_op,
	COMMUTATOR = '~-~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Euclidean
CREATE FUNCTION euclidean (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'euclidean'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION euclidean_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'euclidean_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~!! (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = euclidean_op,
	COMMUTATOR = '~!!',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Hamming
CREATE FUNCTION hamming (varbit, varbit) RETURNS float8
AS 'MODULE_PATHNAME','hamming'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION hamming_op (varbit, varbit) RETURNS bool
AS 'MODULE_PATHNAME', 'hamming_op'
LANGUAGE C STABLE STRICT;

CREATE FUNCTION hamming_text (text, text) RETURNS float8
AS 'MODULE_PATHNAME','hamming_text'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION hamming_text_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'hamming_text_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~@~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = hamming_text_op,
	COMMUTATOR = '~@~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Jaccard
CREATE FUNCTION jaccard (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'jaccard'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION jaccard_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'jaccard_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~?? (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = jaccard_op,
	COMMUTATOR = '~??',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Jaro
CREATE FUNCTION jaro (text, text) RETURNS float8
AS 'MODULE_PATHNAME','jaro'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION jaro_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'jaro_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~%% (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = jaro_op,
	COMMUTATOR = '~%%',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Jaro-Winkler
CREATE FUNCTION jarowinkler (text, text) RETURNS float8
AS 'MODULE_PATHNAME','jarowinkler'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION jarowinkler_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'jarowinkler_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~@@ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = jarowinkler_op,
	COMMUTATOR = '~@@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Levenshtein
CREATE FUNCTION lev (text, text) RETURNS float8
AS 'MODULE_PATHNAME','lev'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION lev_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'lev_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~== (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = lev_op,
	COMMUTATOR = '~==',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Those functions are here just for academic purposes
--CREATE FUNCTION levslow (text, text) RETURNS float8
--AS 'MODULE_PATHNAME','levslow'
--LANGUAGE C IMMUTABLE STRICT;

--CREATE FUNCTION levslow_op (text, text) RETURNS bool
--AS 'MODULE_PATHNAME', 'levslow_op'
--LANGUAGE C STABLE STRICT;

--CREATE OPERATOR ~@@ (
--	LEFTARG = text,
--	RIGHTARG = text,
--	PROCEDURE = levslow_op,
--	COMMUTATOR = '~@@',
--	RESTRICT = contsel,
--	JOIN = contjoinsel
--);

-- Matching Coefficient
CREATE FUNCTION matchingcoefficient (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'matchingcoefficient'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION matchingcoefficient_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'matchingcoefficient_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~^^ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = matchingcoefficient_op,
	COMMUTATOR = '~^^',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Monge-Elkan
CREATE FUNCTION mongeelkan (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'mongeelkan'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mongeelkan_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'mongeelkan_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~|| (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = mongeelkan_op,
	COMMUTATOR = '~||',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Needleman-Wunsch
CREATE FUNCTION needlemanwunsch (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'needlemanwunsch'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION needlemanwunsch_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'needlemanwunsch_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~#~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = needlemanwunsch_op,
	COMMUTATOR = '~#~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Overlap Coefficient
CREATE FUNCTION overlapcoefficient (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'overlapcoefficient'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION overlapcoefficient_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'overlapcoefficient_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~** (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = overlapcoefficient_op,
	COMMUTATOR = '~**',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Q-Gram
CREATE FUNCTION qgram (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'qgram'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION qgram_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'qgram_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~~~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = qgram_op,
	COMMUTATOR = '~~~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Smith-Waterman
CREATE FUNCTION smithwaterman (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'smithwaterman'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION smithwaterman_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'smithwaterman_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~=~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = smithwaterman_op,
	COMMUTATOR = '~=~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Smith-Waterman-Gotoh
CREATE FUNCTION smithwatermangotoh (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'smithwatermangotoh'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION smithwatermangotoh_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'smithwatermangotoh_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~!~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = smithwatermangotoh_op,
	COMMUTATOR = '~!~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

-- Soundex
CREATE FUNCTION soundex (text, text) RETURNS float8
AS 'MODULE_PATHNAME', 'soundex'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION soundex_op (text, text) RETURNS bool
AS 'MODULE_PATHNAME', 'soundex_op'
LANGUAGE C STABLE STRICT;

CREATE OPERATOR ~*~ (
	LEFTARG = text,
	RIGHTARG = text,
	PROCEDURE = soundex_op,
	COMMUTATOR = '~*~',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

--
-- GIN support
--

CREATE FUNCTION gin_extract_value_token(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gin_extract_query_token(internal, internal, int2, internal, internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gin_token_consistent(internal, int2, internal, int4, internal, internal, internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR CLASS gin_similarity_ops
FOR TYPE text USING gin
AS
    OPERATOR    1   ~++,		-- block
    OPERATOR    2   ~##,		-- cosine
    OPERATOR    3   ~-~,		-- dice
    OPERATOR    4   ~!!,		-- euclidean
    OPERATOR    5   ~??,		-- jaccard
--    OPERATOR    6   ~%%,		-- jaro
--    OPERATOR    7   ~@@,		-- jarowinkler
--    OPERATOR    8   ~==,		-- lev
    OPERATOR    9   ~^^,		-- matchingcoefficient
--    OPERATOR    10  ~||,		-- mongeelkan
--    OPERATOR    11  ~#~,		-- needlemanwunsch
    OPERATOR    12  ~**,		-- overlapcoefficient
    OPERATOR    13  ~~~,		-- qgram
--    OPERATOR    14  ~=~,		-- smithwaterman
--    OPERATOR    15  ~!~,		-- smithwatermangotoh
--    OPERATOR    16  ~*~,		-- soundex
    FUNCTION    1   bttextcmp(text, text),
    FUNCTION    2   gin_extract_value_token(internal, internal, internal),
    FUNCTION    3   gin_extract_query_token(internal, internal, int2, internal, internal, internal, internal),
    FUNCTION    4   gin_token_consistent(internal, int2, internal, int4, internal, internal, internal, internal),
    STORAGE text;
