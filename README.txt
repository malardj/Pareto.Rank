The program in fitness.c  computes the Pareto ranks of row feature vectors
stored in its input file and write those same vectors along with their 
respective Pareto ranks in the output file. The ranking algorithm is by
Goldberg except for some modifications that are not used by the main program.
The main program assumes that bigger is better.


Compile this code as any C source code:

gcc -o prank fitness.c

Run the code as:

./prank infile outfile , or ./prank.exe infile outfile

as appropriate.


The input file format:

The 1st line has 3 integers: number of rows, of features and a zero typically.
The remaining lines all start with a string id followed by a list of features.

Here is an example:

5 4 0
1 1572864 16324751 81.09 2069.04
2 705024 10510000 93.17 830.18
3 786432 8162376 81.09 2069.04
4 147456 2897000 90.96 846.42
5 186368 2566000 54.58 635.15

The output file has a 1 at the end of the first line and it looks like this, with computed Pareto ranks appended after the colon.


5 4 1
1	1.57286e+06	1.63248e+07	81.09	2069.04	:	0
2	705024	1.051e+07	93.17	830.18	:	0
3	786432	8.16238e+06	81.09	2069.04	:	1
4	147456	2.897e+06	90.96	846.42	:	0
5	186368	2.566e+06	54.58	635.15	:	2


This is pretty primitive except maybe that output files can be input too.

This source code is released under the GNU Lesser General Public License v3.

Enjoy,

Joel Malard
malardj@acm.org


