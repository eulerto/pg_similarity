# pg_similarity extension

EXTENSION = pg_similarity
MODULE_big = pg_similarity
OBJS = tokenizer.o similarity.o similarity_gin.o \
       block.o cosine.o dice.o euclidean.o hamming.o jaccard.o \
       jaro.o levenshtein.o matching.o mongeelkan.o needlemanwunsch.o \
	   overlap.o qgram.o smithwaterman.o smithwatermangotoh.o soundex.o
DATA_built = pg_similarity.sql
DATA = pg_similarity--1.0.sql pg_similarity--unpackaged--1.0.sql
REGRESS = test1 test2 test3 test4
#DOCS = README.md

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_similarity
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
