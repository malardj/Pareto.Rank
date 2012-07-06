/* Pareto ranking software.

   Written by Joel Malard and provided free of charge and as is under the
   GNU Lesser General Public License Version 3.

   Send questions and enquiries to malardj@acm.org .
   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** \brief compares two string for pareto dominance.
 \param[in] f number of features
 \param[in] p array of f doubles
 \param[in] q array of f doubles
 \param[in] fiz how much slack to cut on each component
 \param[in] maxisbest flag
 \return returns +1 when sample  p dominates sample q,
  returns -1 if q dominates p and return 0 otherwise.
 \details This codes assumes that all objective functions are either
          maximized or minized depending on the value of maxisbest.

 \remark A fuzziness factor was added to account for known uncertainty
         bounds on the experimental errors. This changes the semantic
         of Pareto dominance.

*/  
int ParetoCompare(int f, double *p, double *q, double *fiz, int maxisbest)
{
    int i, result, partial;
    int sign = (maxisbest==0) ? -1 : 1;

    result = 0;
    if ( fiz==0 )
    {
        for (i=0; i<f; i++) {
            if (p[i] == q[i]) continue;
	       partial = (p[i] > q[i]) ? sign : -sign;
	       if (result == 0) {
                result = partial;
            } else if ( result!=partial ) {
                return 0;
            }
        }
    }
    else
    {
        for (i=0; i<f; i++) {
            double w = p[i]-q[i];
            if ( w<0.0 ) w = -w;
            if ( w<=fiz[i] ) {
                return 0;
            }
            partial = (p[i] > q[i]) ? sign : -sign;
            if (result == 0) {
                result = partial;
            } else if ( result!=partial ) {
                return 0;
            }
        }
    }

    return result;
}

/** \brief Sets the fitness of a population using pareto ranking.
 \param[in] n population size.
 \param[in] f number of features in every sample.
 \param[in] keys[n] n identification keys.
 \param[in] data[n][f] feature values for each sample.
 \param[in] fiz[f] fuziness per feature when base address is non null.
 \param[in] maxisbest applicable across every feature and sample.
 \param[out] rank[] array of ranks for each sample.

*/
void ParetoScale(int n, int f, char **keys, double **data,
                    double *fiz, int maxisbest, int *rank)
{
	int k, i, j, bF, MaxRank, iset;
	int **doms, *ndom;
	double MaxVal;
	int ParetoPeel = 0;
	int ParetoCard = 1;
	int ParetoMetric = ParetoPeel;

	/* Set fitness according to cardinality of pareto dominating set */
	if ( ParetoMetric == ParetoCard )
	{
		/* for each string, count the number of strings
		 * that dominate it. Count duplicates.
		 */ 
		for ( i=0; i<n; i++ )
		{
			for ( j=i+1; j<n; j++ )
			{
				bF = ParetoCompare ( f, data[i], data[j], fiz, maxisbest);
				if ( bF<0 )
				{
					rank[i]++;
				}
				else if ( bF>0 )
				{
					rank[j]++;
				}
			}
		}
	}
	else /* Set fitness according to distance to pareto front. */
	{
		/* allocate some more memory to hold the dominance graph. */
		ndom = (int *) calloc((size_t)n, sizeof(int));
		if ( ndom==NULL ) {
			fprintf(stderr, "ParetoScale: no space left for ndom");
			return;
		}
	
		doms = (int **) calloc((size_t)n,sizeof(int *));
		if ( doms==NULL ) {
			fprintf(stderr, "ParetoScale: no space left for doms");
			return;
		}

		for ( i=0; i<n; i++ ) doms[i] = NULL;
	
		/* compute for each string the list of strings
		 * it dominates and the number of strings that
		 * dominate it.
		 */

		for ( i=0; i<n; i++ )
		{
			for ( j=i+1; j<n; j++ )
			{
				bF = ParetoCompare ( f, data[i], data[j], fiz, maxisbest);
				if ( bF<0 )
				{		/* j dominates i */
					rank[i]++;
					k = ndom[j];
					doms[j] = realloc(doms[j], (k+1)*sizeof(int));
					if ( doms[j]==NULL )
					{
						fprintf(stderr, "ParetoScale: no space");
						return;
					}
					doms[j][k] = i;
					ndom[j]++;
				}
				else if ( bF>0 )
				{	/* i dominates j */
					rank[j]++;
					k = ndom[i];
					doms[i] = realloc(doms[i], (k+1)*sizeof(int));
					if ( doms[i]==NULL )
					{
						fprintf(stderr, "ParetoScale: no space");
						return;
					}
					doms[i][k] = j;
					ndom[i]++;
				}
			}
		}
	
		/* string with a null rank[] are non-dominated. */
		MaxRank = -1;

		for ( i=0; i<n; i++ )
		{
			rank[i] = -1;
		}
		do
		{
			/* prime this iteration */
			iset = 0;
	
			/* lower the ranks of nodes dominated by newly
			 * stacked strings.
			 */
			for ( i=0; i<n; i++ )
			{
				if ( rank[i] <= -1 )
				{
					for ( j=0; j<ndom[i]; j++ )
					{
						k = doms[i][j];
						rank[k]--;
					}
				}
			}

			/* set the next level of Pareto ranks */
			MaxRank++;
			for ( i=0; i<n; i++ )
			{
				if ( rank[i] >= 0 ) continue;
				if ( rank[i] == -1 ) rank[i] = MaxRank;
				else {
					rank[i] = -1;
					iset = 1;
				}
			}
		}
		while ( iset>0 );
	
		/* get rid of additional work space */
		for ( i=0; i<n; i++ )
			if ( doms[i]!=NULL ) free(doms[i]);
		free(doms);
		free(ndom);
	}
	
	return;
	
}

int main(int argc, char **argv)
{
	int i, j, n, f, g;
	char **keys=0;
	double **data = 0;
	double *fiz = 0;
	int maxisbest = 1;
	int *rank=0;
	char *finame =0;
	FILE *fip = 0;
	char *foname = 0;
	FILE *fop = 0;
	char buffer[2000];
#ifdef VERBOSE
	int verbose = 1;
#else
	int verbose = 0;
#endif

	if ( argc == 0 ) {
		fprintf(stderr,"USAGE: %s input [output]\n");
		return 1;
	}

	if (argc>=2 ) finame = argv[1];
	if ( finame != 0 ) fip = fopen(finame,"r");
	else fip = stdin ;

	if (argc>=3 ) foname = argv[2];
	if ( foname != 0 ) fop = fopen(foname,"w");
	else fop = stdout ;

	fscanf(fip,"%d %d %d",&n, &f, &g);
	fprintf(fop,"%d %d 1\n",n, f);
	if(verbose)fprintf(stderr,"n=%d\tf=%d\tg=%d\n",n,f,g);

	keys = (char **)calloc(n,sizeof(char *));
	data = (double **)calloc(n,sizeof(double *));
	rank = (int*)calloc(n,sizeof(int));

	for ( i=0; i<n; i++ ) {
		fscanf(fip,"%s",buffer);
		keys[i] = strdup(buffer);
		if(verbose)fprintf(stderr,"i\t%s",keys[i]);
		data[i] = (double *)calloc(f,sizeof(double));
		for ( j=0; j<f; j++ ) {
			fscanf(fip,"%lg",&data[i][j]);
			if(verbose)fprintf(stderr,"\t%lg",data[i][j]);
		}
		if ( g != 0 ) {
			char sep[5];
			int t;
			fscanf(fip,"%s%d",sep,&t);
			if(verbose)fprintf(stderr,"\t:\t%d",t);
		}
		if(verbose)fprintf(stderr,"\n");
	}
	if(verbose)fprintf(stderr,"******************************\n");
	ParetoScale(n, f, keys, data, fiz, maxisbest, rank);
	for ( i=0; i<n; i++ ) {
		if(verbose)fprintf(stderr,"o\t%s",keys[i]);
		fprintf(fop,"%s",keys[i]);
		for ( j=0; j<f; j++ ) {
			if(verbose)fprintf(stderr,"\t%lg",data[i][j]);
			fprintf(fop,"\t%lg",data[i][j]);
		}
		if(verbose)fprintf(stderr,"\t:\t%d\n",rank[i]);
		fprintf(fop,"\t:\t%d\n",rank[i]);
	}
	if(verbose)fprintf(stderr,"******************************\n");
	return 0;
}
