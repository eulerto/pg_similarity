[![Coverity Scan Build Status](https://scan.coverity.com/projects/4830/badge.svg)](https://scan.coverity.com/projects/pg_similarity)

Introduction
============

**pg\_similarity** is an extension to support similarity queries on [PostgreSQL](http://www.postgresql.org/). The implementation is tightly integrated in the RDBMS in the sense that it defines operators so instead of the traditional operators (= and <>) you can use ~~~ and ~!~ (any of these operators represents a similarity function).

**pg\_similarity** has three main components:

 - **Functions**: a set of functions that implements similarity algorithms available in the literature. These functions can be used as UDFs and, will be the base for implementing the similarity operators;
 - **Operators**: a set of operators defined at the top of similarity functions. They use similarity functions to obtain the similarity threshold and, compare its value to a user-defined threshold to decide if it is a match or not;
 - **Session Variables**: a set of variables that store similarity function parameters. Theses variables can be defined at run time.

Installation
============

**pg\_similarity** is supported on [those platforms](http://www.postgresql.org/docs/current/static/supported-platforms.html) that PostgreSQL is. The installation steps depend on your operating system.

You can also keep up with the latest fixes and features cloning the Git repository.

```
$ git clone https://github.com/eulerto/pg_similarity.git
```

UNIX based Operating Systems
----------------------------

Before you are able to use your extension, you should build it and load it at the desirable database.

```
$ tar -zxf pg_similarity-1.0.tgz
$ cd pg_similarity-1.0
$ $EDITOR Makefile # edit PG_CONFIG iif necessary
$ USE_PGXS=1 make
$ USE_PGXS=1 make install
$ psql mydb
psql (10.3)
Type "help" for help.

mydb=# CREATE EXTENSION pg_similarity;
CREATE EXTENSION
```

The typical usage is to copy a sample file at tarball (`pg_similarity.conf.sample`) to PGDATA (renaming it as `pg_similarity.conf`) and include the following line in *postgresql.conf*:

```
include = 'pg_similarity.conf'
```

Windows
-------

Sorry, never tried^H^H^H^H^H Actually I tried that but it is not that easy as on UNIX. :( There are two ways to build PostgreSQL on Windows: (i) MingW and (ii) MSVC. The former is supported but it is not widely used and the latter is popular because Windows binaries (officially distributed) are built using MSVC. If you choose to use Mingw, just follow the UNIX instructions above to build pg_similarity. Otherwise, the MSVC steps are below:

- Edit `pg_similarity.vcxproj` replacing `c:\postgres\pg103` with PostgreSQL prefix directory;
- Open this project file in MS Visual Studio and build it;
- Copy `pg_similarity.dll` to `pg_config --pkglibdir`;
- Copy `pg_similarity.control` and `pg_similarity--*.sql` to `SHAREDIR/extension` (SHAREDIR is `pg_config --sharedir`).

Functions and Operators
=======================

This extension supports a set of similarity algorithms. The most known algorithms are covered by this extension. You must be aware that each algorithm is suited for a specific domain. The following algorithms are provided.

 - L1 Distance (as known as City Block or Manhattan Distance);
 - Cosine Distance;
 - Dice Coefficient;
 - Euclidean Distance;
 - Hamming Distance;
 - Jaccard Coefficient;
 - Jaro Distance;
 - Jaro-Winkler Distance;
 - Levenshtein Distance;
 - Matching Coefficient;
 - Monge-Elkan Coefficient;
 - Needleman-Wunsch Coefficient;
 - Overlap Coefficient;
 - Q-Gram Distance;
 - Smith-Waterman Coefficient;
 - Smith-Waterman-Gotoh Coefficient;
 - Soundex Distance.

<table>
  <tr>
    <th>Algorithm</th>
    <th>Function</th>
    <th>Operator</th>
	<th>Use Index?</th>
    <th>Parameters</th>
  </tr>
  <tr>
    <td>L1 Distance</td>
    <td>block(text, text) returns float8</td>
    <td>~++</td>
	<td>yes</td>
    <td>
        pg_similarity.block_tokenizer (enum)<br/>
        pg_similarity.block_threshold (float8)<br/>
        pg_similarity.block_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Cosine Distance</td>
    <td>cosine(text, text) returns float8</td>
    <td>~##</td>
	<td>yes</td>
    <td>
      pg_similarity.cosine_tokenizer (enum)<br/>
      pg_similarity.cosine_threshold (float8)<br/>
      pg_similarity.cosine_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Dice Coefficient</td>
    <td>dice(text, text) returns float8</td>
    <td>~-~</td>
	<td>yes</td>
    <td>
      pg_similarity.dice_tokenizer (enum)<br/>
      pg_similarity.dice_threshold (float8)<br/>
      pg_similarity.dice_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Euclidean Distance</td>
    <td>euclidean(text, text) returns float8</td>
    <td>~!!</td>
	<td>yes</td>
    <td>
      pg_similarity.euclidean_tokenizer (enum)<br/>
      pg_similarity.euclidean_threshold (float8)<br/>
      pg_similarity.euclidean_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Hamming Distance</td>
    <td>hamming(bit varying, bit varying) returns float8<br/>
    hamming_text(text, text) returns float8</td>
    <td>~@~</td>
	<td>no</td>
    <td>
      pg_similarity.hamming_threshold (float8)<br/>
      pg_similarity.hamming_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Jaccard Coefficient</td>
    <td>jaccard(text, text) returns float8</td>
    <td>~??</td>
	<td>yes</td>
    <td>
      pg_similarity.jaccard_tokenizer (enum)<br/>
      pg_similarity.jaccard_threshold (float8)<br/>
      pg_similarity.jaccard_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Jaro Distance</td>
    <td>jaro(text, text) returns float8</td>
    <td>~%%</td>
	<td>no</td>
    <td>
      pg_similarity.jaro_threshold (float8)<br/>
      pg_similarity.jaro_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Jaro-Winkler Distance</td>
    <td>jarowinkler(text, text) returns float8</td>
    <td>~@@</td>
	<td>no</td>
    <td>
      pg_similarity.jarowinkler_threshold (float8)<br/>
      pg_similarity.jarowinkler_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Levenshtein Distance</td>
    <td>lev(text, text) returns float8</td>
    <td>~==</td>
	<td>no</td>
    <td>
      pg_similarity.levenshtein_threshold (float8)<br/>
      pg_similarity.levenshtein_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Matching Coefficient</td>
    <td>matchingcoefficient(text, text) returns float8</td>
    <td>~^^</td>
	<td>yes</td>
    <td>
      pg_similarity.matching_tokenizer (enum)<br/>
      pg_similarity.matching_threshold (float8)<br/>
      pg_similarity.matching_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Monge-Elkan Coefficient</td>
    <td>mongeelkan(text, text) returns float8</td>
    <td>~||</td>
	<td>no</td>
    <td>
      pg_similarity.mongeelkan_tokenizer (enum)<br/>
      pg_similarity.mongeelkan_threshold (float8)<br/>
      pg_similarity.mongeelkan_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Needleman-Wunsch Coefficient</td>
    <td>needlemanwunsch(text, text) returns float8</td>
    <td>~#~</td>
	<td>no</td>
    <td>
      pg_similarity.nw_threshold (float8)<br/>
      pg_similarity.nw_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Overlap Coefficient</td>
    <td>overlapcoefficient(text, text) returns float8</td>
    <td>~**</td>
	<td>yes</td>
    <td>
      pg_similarity.overlap_tokenizer (enum)<br/>
      pg_similarity.overlap_threshold (float8)<br/>
      pg_similarity.overlap_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Q-Gram Distance</td>
    <td>qgram(text, text) returns float8</td>
    <td>~~~</td>
	<td>yes</td>
    <td>
      pg_similarity.qgram_threshold (float8)<br/>
      pg_similarity.qgram_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Smith-Waterman Coefficient</td>
    <td>smithwaterman(text, text) returns float8</td>
    <td>~=~</td>
	<td>no</td>
    <td>
      pg_similarity.sw_threshold (float8)<br/>
      pg_similarity.sw_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Smith-Waterman-Gotoh Coefficient</td>
    <td>smithwatermangotoh(text, text) returns float8</td>
    <td>~!~</td>
	<td>no</td>
    <td>
      pg_similarity.swg_threshold (float8)<br/>
      pg_similarity.swg_is_normalized (bool)
    </td>
  </tr>
  <tr>
    <td>Soundex Distance</td>
    <td>soundex(text, text) returns float8</td>
    <td>~*~</td>
	<td>no</td>
    <td>
    </td>
  </tr>
</table>

The several parameters control the behavior of the pg\_similarity functions and operators. I don't explain in detail each parameter because they can be classified in three classes: **tokenizer**, **threshold**, and **normalized**.

 - **tokenizer**: controls how the strings are tokenized. The valid values are **alnum**, **gram**, **word**, and **camelcase**. All tokens are lowercase (this option can be set at compile time; see PGS\_IGNORE\_CASE at source code). Default is **alnum**;
   - **alnum**: delimiters are any non-alphanumeric characters. That means that only alphabetic characters in the standard C locale and digits (0-9) are accepted in tokens. For example, the string "Euler\_Taveira\_de\_Oliveira 22/02/2011" is tokenized as "Euler", "Taveira", "de", "Oliveira", "22", "02", "2011";
   - **gram**: an n-gram is a subsequence of length n. Extracting n-grams from a string can be done by using the sliding-by-one technique, that is, sliding a window of length n through out the string by one character. For example, the string "euler taveira" (using n = 3) is tokenized as "eul", "ule", "ler", "er ", "r t", " ta", "tav", "ave", "vei", "eir", and "ira". There are some authors that consider n-grams adding "  e", " eu", "ra ", and "a  " to the set of tokens, that is called full n-grams (this option can be set at compile time; see PGS\_FULL\_NGRAM at source code);
   - **word**: delimiters are white space characters (space, form-feed, newline, carriage return, horizontal tab, and vertical tab). For example, the string "Euler Taveira de Oliveira 22/02/2011" is tokenized as "Euler", "Taveira", "de", "Oliveira", and "22/02/2011";
   - **camelcase**: delimiters are capitalized characters but they are also included as first token characters. For example, the string "EulerTaveira de Oliveira" is tokenized as "Euler", "Taveira de ", and "Oliveira".
 - **threshold**: controls how flexible will be the result set. These values are used by operators to match strings. For each pair of strings, if the calculated value (using the corresponding similarity function) is greater or equal the threshold value, there is a match. The values range from **0.0** to **1.0**. Default is **0.7**;
 - **normalized**: controls whether the similarity coefficient/distance is normalized (between 0.0 and 1.0) or not. Normalized values are used automatically by operators to match strings, that is, this parameter only makes sense if you are using similarity functions. Default is **true**.

Examples
========

Set parameters at run time.

```
mydb=# show pg_similarity.levenshtein_threshold;
 pg_similarity.levenshtein_threshold
-------------------------------------
 0.7
(1 row)

mydb=# set pg_similarity.levenshtein_threshold to 0.5;
SET
mydb=# show pg_similarity.levenshtein_threshold;
 pg_similarity.levenshtein_threshold
-------------------------------------
 0.5
(1 row)

mydb=# set pg_similarity.cosine_tokenizer to camelcase;
SET
mydb=# set pg_similarity.euclidean_is_normalized to false;
SET
```

Simple tables for examples.

```
mydb=# create table foo (a text);
CREATE TABLE
mydb=# insert into foo values('Euler'),('Oiler'),('Euler Taveira de Oliveira'),('Maria Taveira dos Santos'),('Carlos Santos Silva');
INSERT 0 5
mydb=# create table bar (b text);
CREATE TABLE
mydb=# insert into bar values('Euler T. de Oliveira'),('Euller'),('Oliveira, Euler Taveira'),('Sr. Oliveira');
INSERT 0 4
```

*Example 1*: Using similarity functions **cosine**, **jaro**, and **euclidean**.

```
mydb=# select a, b, cosine(a,b), jaro(a, b), euclidean(a, b) from foo, bar;
             a             |            b            |  cosine  |   jaro   | euclidean
---------------------------+-------------------------+----------+----------+-----------
 Euler                     | Euler T. de Oliveira    |      0.5 |     0.75 |  0.579916
 Euler                     | Euller                  |        0 | 0.944444 |         0
 Euler                     | Oliveira, Euler Taveira |  0.57735 | 0.605797 |  0.552786
 Euler                     | Sr. Oliveira            |        0 | 0.505556 |  0.225403
 Oiler                     | Euler T. de Oliveira    |        0 | 0.472222 |  0.457674
 Oiler                     | Euller                  |        0 |      0.7 |         0
 Oiler                     | Oliveira, Euler Taveira |        0 | 0.672464 |  0.367544
 Oiler                     | Sr. Oliveira            |        0 | 0.672222 |  0.225403
 Euler Taveira de Oliveira | Euler T. de Oliveira    |     0.75 |  0.79807 |      0.75
 Euler Taveira de Oliveira | Euller                  |        0 | 0.677778 |  0.457674
 Euler Taveira de Oliveira | Oliveira, Euler Taveira | 0.866025 | 0.773188 |       0.8
 Euler Taveira de Oliveira | Sr. Oliveira            | 0.353553 | 0.592222 |  0.552786
 Maria Taveira dos Santos  | Euler T. de Oliveira    |        0 |  0.60235 |       0.5
 Maria Taveira dos Santos  | Euller                  |        0 | 0.305556 |  0.457674
 Maria Taveira dos Santos  | Oliveira, Euler Taveira | 0.288675 | 0.535024 |  0.552786
 Maria Taveira dos Santos  | Sr. Oliveira            |        0 | 0.634259 |  0.452277
 Carlos Santos Silva       | Euler T. de Oliveira    |        0 | 0.542105 |   0.47085
 Carlos Santos Silva       | Euller                  |        0 | 0.312865 |  0.367544
 Carlos Santos Silva       | Oliveira, Euler Taveira |        0 | 0.606662 |   0.42265
 Carlos Santos Silva       | Sr. Oliveira            |        0 | 0.507728 |  0.379826
(20 rows)
```

*Example 2*: Using operator **levenshtein** (~==) and changing its threshold at run time.

```
mydb=# show pg_similarity.levenshtein_threshold;
 pg_similarity.levenshtein_threshold
-------------------------------------
 0.7
(1 row)

mydb=# select a, b, lev(a,b) from foo, bar where a ~== b;
             a             |          b           |   lev
---------------------------+----------------------+----------
 Euler                     | Euller               | 0.833333
 Euler Taveira de Oliveira | Euler T. de Oliveira |     0.76
(2 rows)

mydb=# set pg_similarity.levenshtein_threshold to 0.5;
SET
mydb=# select a, b, lev(a,b) from foo, bar where a ~== b;
             a             |          b           |   lev
---------------------------+----------------------+----------
 Euler                     | Euller               | 0.833333
 Oiler                     | Euller               |      0.5
 Euler Taveira de Oliveira | Euler T. de Oliveira |     0.76
(3 rows)
```

*Example 3*: Using operator **qgram** (~~~) and changing its threshold at run time.

```
mydb=# set pg_similarity.qgram_threshold to 0.7;
SET
mydb=# show pg_similarity.qgram_threshold;
 pg_similarity.qgram_threshold
-------------------------------
 0.7
(1 row)

mydb=# select a, b,qgram(a, b) from foo, bar where a ~~~ b;
             a             |            b            |  qgram
---------------------------+-------------------------+----------
 Euler                     | Euller                  |      0.8
 Euler Taveira de Oliveira | Euler T. de Oliveira    |  0.77551
 Euler Taveira de Oliveira | Oliveira, Euler Taveira | 0.807692
(3 rows)

mydb=# set pg_similarity.qgram_threshold to 0.35;
SET
mydb=# select a, b,qgram(a, b) from foo, bar where a ~~~ b;
             a             |            b            |  qgram
---------------------------+-------------------------+----------
 Euler                     | Euler T. de Oliveira    | 0.413793
 Euler                     | Euller                  |      0.8
 Oiler                     | Euller                  |      0.4
 Euler Taveira de Oliveira | Euler T. de Oliveira    |  0.77551
 Euler Taveira de Oliveira | Oliveira, Euler Taveira | 0.807692
 Euler Taveira de Oliveira | Sr. Oliveira            | 0.439024
(6 rows)

```

*Example 4*: Using a set of operators using the same threshold (0.7) to ilustrate that some similarity functions are appropriated to certain data domains.

```
mydb=# select * from bar where b ~@@ 'euler'; -- jaro-winkler operator
          b
----------------------
 Euler T. de Oliveira
 Euller
(2 rows)

mydb=# select * from bar where b ~~~ 'euler'; -- qgram operator
 b
---
(0 rows)

mydb=# select * from bar where b ~== 'euler'; -- levenshtein operator
   b
--------
 Euller
(1 row)

mydb=# select * from bar where b ~## 'euler'; -- cosine operator
 b
---
(0 rows)
```

License
=======

> Copyright © 2008-2012 Euler Taveira de Oliveira
> All rights reserved.

> Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

> Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer;
> Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution;
> Neither the name of the Euler Taveira de Oliveira nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

> THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

