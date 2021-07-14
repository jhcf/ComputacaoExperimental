/* packsimplest.c: Simplest experiment on FFD bin packing
    Usage: pack
	Each input line:  n lb ub
	Each output line: n lb ub bins_used empty_space
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int wcomp(float *a, float *b)
{	if (*a < *b)
		return -1;
	else if (*a > *b)
		return 1;
	else
		return 0;
}

#define NMAX 100000
float	x[NMAX], b[NMAX];

int main()
{	float	lb, ub, es;
	int		i, j, n;	
	double	s = 1.0 / (RAND_MAX + 1.0);

	while (scanf("%d %f %f", &n, &lb, &ub) != EOF) {
		/* generate and sort inputs */
		for (i = 0; i < n; i++)
			x[i] = (float) (lb + (ub-lb) * (rand()*s + rand()*s*s));
		//ffd: qsort((char *) x, n, sizeof(float), wcomp);
		/* initialize bins to empty */
		for (i = 0; i < n; i++)
			b[i] = 0.0;
		/* perform the packing */
		for (i = n-1; i >= 0; i--) {
			for (j = 0; b[j]+x[i] > 1.0; j++)
				;
			b[j] += x[i];
		}
		/* gather and report statistics */
		es = 0.0;
		for (i = 0; b[i] > 0.0; i++)
			es += (float) (1.0 - b[i]);;
		printf("%d\t%g\t%g\t%d\t%g\n", n, lb, ub, i, es);
	}
	return 0;
}
