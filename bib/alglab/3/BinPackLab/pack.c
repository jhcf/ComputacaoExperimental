/* pack.c: Workhorse experiment on First Fit Decreasing bin packing
    Usage: pack
	Each input line specifies an experiment in three fields
		n		Number of weights in experiment
		lb		Lower bound on size of uniform weights (0 <= lb <= ub <= 1)
		ub		Upper bound on size of weight
	Results of each experiment produced on one output line
		n, lb, ub	Repeat of inputs
		used	Number of bins used for packing
		big		Number of weights > .5
		es		Total empty space in all used bins
		tes		Empty space in tops of all bins except the last
	History
		Jon Bentley, 22 April 2005
			Brushed off and reassembled old bin packing code
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* MISCELLANEOUS SUPPORT ROUTINES */

void error(char *s)	/* report error and die */
{	fprintf(stderr, "pack error: %s\n", s);
	exit(1);
}

double frand()
{	double s = 1.0 / (RAND_MAX + 1.0);
	return rand()*s + rand()*s*s + rand()*s*s*s + rand()*s*s*s*s;
}

/* BINS AND HEAP OVER THEM
    b[1..bl-1] is heap
	b[bl..bu-1] is the "real" bin area
 */

typedef double WType;	/* Data type of weights */
#define NMAX 2097152
#define BMAX (NMAX*2)
WType	x[NMAX], b[BMAX+1];
int		bl, bu;

void initbins(WType u, int n)
{	int i, guess;

	guess = n;  /* replace with better guess? */
	bl = 1;
	while (bl < guess)
		bl *= 2;
	bu = 2*bl;
	if (bu > BMAX)
		error("too many estimated bins");
	for (i = 1; i <= bu; i++)
		b[i] = 0.0;
}

void insert(WType tw)
{	int	i, j;
	WType	ht;

	ht = (WType) (1.0 - tw);
	if (b[1] > ht)
		error("ran out of space in bins");
	/* proceed down heap to bottom level */
	i = 1;
	while (i < bl)
		if (b[i *= 2] > ht)
			i++;
	/* add weight to the correct bin */
	b[i] += tw;
	if (0) /* change to 1 to draw the packing */
		printf("\t%d\t%lg\t%lg\n", i-bl, b[i]-tw, tw);
	/* modify heap on the way back up */
	for (i /= 2; i >= 1; i /= 2) {
		j = 2*i;
		if (b[j+1] < b[j])
			j++;
		if (b[i] == b[j])
			break;
		b[i] = b[j];
	}
}

/* COMPARE FUNCTION FOR QSORT */

int wcomp(WType *a, WType *b)
{	if (*a < *b)
		return -1;
	else if (*a > *b)
		return 1;
	else
		return 0;
}

/* MAIN */

int main()
{	WType	lb, ub, es;
	int		i, big, n, seed;	

	seed = (int) time((long *) 0);
	/* seed = 1; uncomment for debugging */
	printf("pack v01 seed=%d\n", seed);
	srand(seed);
	while (scanf("%d %lf %lf", &n, &lb, &ub) != EOF) {
		/* generate and sort inputs */
		big = 0;
		for (i = 0; i < n; i++) {
			x[i] = (WType) (lb + (ub - lb) * frand());
			if (x[i] > 0.5)
				big++;
		}
		//for ffd:   qsort((char *) x, n, sizeof(WType), wcomp);
		/* initialize bins and perform the packing*/
		initbins(ub, n);
		for (i = n-1; i >= 0; i--)
			insert(x[i]);
		/* gather and report statistics */
		es = 0.0;
		for (i = bl; b[i] > 0.0; i++)
			es += (WType) (1.0 - b[i]);
		printf("%d\t%lg\t%lg\t%d\t%d\t%lg\t%lg\n",
			n, lb, ub, i-bl, big, es, es - (1.0 - b[i-1]));
	}
	return 0;
}
