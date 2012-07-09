Greetings,

I am posting this source code in response to the proposal by Gary Johnson for a systematic list of the several top 500 supercomputer lists, see his post Erich Strohmaier and the TOP500: A List that’s taken on a life of its own . 

Linear ranking is a powerful tool for pushing progress but it hasn't been all that consistently good at directing progress. With that in mind and because lists of lists seemed too complex for a classification problem, I computed the Pareto ranks of about 300 of the June 12 top 500 entries based on Total Cores, Rmax, Efficiency and Mflops/Watts. I only kept those entries that have all the relevant data.

A null Pareto rank means that there is no other entry in the list that is better than this one in every aspect. An entry with a rank of 1 is only beaten on all grounds by entries of rank 0, etc. I counted 20 ranks, from 0 to 19.

There are 20 computer systems with a null rank that basically have some bragging rights over the others, they are 1, 2, 4, 7, 8, 13, 18, 20, 84, 87, 99, 100, 101, 102, 103, 14, 121, 252, 253, 358.

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


