#include <stdio.h>
#include <stdlib.h>
#define MAXNODES 100
#define MAXEDGES 100*(100-1)/2  


/* RANDOM Random Graph coloring algorithm C.C. McGeoch 2011 
   Runs I iterations of greedy with random vertex orders, reports best
   coloring found.  

   Usage: random filename I s
   filename: input file containing G, in format below. 
   i: number of iterations
   s: optional random number seed 

   Input format:
     n            // first line: number of vertices, named [1..n] 
     x y z  0     // neighbors of v1, list ending in 0
     a b c d e 0  // neighbors of v2, list ending in 0
     ...  
     x y 0        // neighbors of vn, list ending in 0
 
     Note: Both edges (v,w) and (w,v) must be present in input
     Lines after end are ignored as comments

   Outputs: 
     To report performance indicators:       #define REPORTSTATS  1
     To report the coloring and counts:      #define REPORTCOLORS 1
     To see some intermediate values #define DEBUG 1

*/ 
#define REPORTSTATS  1
#define REPORTCOLORS 1
#define DEBUG 1

int numnodes=0;    /* number of nodes in the graph */
int numedges=0; 
int iterations=1;   /* default */  

/* algorithm statistics*/ 
int checkcount=0;  /* number of colors checked */
int colorcount=0;  /* to compute average colors looked at */ 
int maxcolor=0;    /* max color assigned */ 

/* The graph is represented as an adjacency list in two arrays */ 
int edgex[MAXNODES+1];      /* index to neighbor array */ 
int ecount[MAXNODES+1];     /* number of neighbors this node */ 
int neighbor[MAXEDGES+1];   /* adjacent nodes       */ 

int colorof[MAXNODES+1];    /* node colors */  
int colorcounts[MAXNODES+1]; /* color distributions */ 

int bestcoloring[MAXNODES+1]; /* save best coloring */ 
int colordist[MAXNODES+1];    /* save best color distribution */ 

FILE *fp, *fopen();

/*--------------------------------------------------------------*/ 
/* getgraph: Read input file and build graph */ 
/* Input Format:  
      First line is integer number of nodes. 
      Then: 
         neighbors of v1, list ending with 0  
         neighbors of v2, list ending with 0  
         ... 
         neighbors of vn, list ending with 0  
      Lines after n+1 inputs are ignored as comments.

*/
void getgraph()
{
  int i;   
  int edgeindex = 1; 
  int edgecount; 
  int nodeval; 

    fscanf(fp,"%d", &numnodes);  /* number of nodes in graph */

    if (numnodes >MAXNODES) {
      printf("too many nodes: recompile and try again\n");
      exit(1); 
    }
    
    for (i=1; i <= numnodes; i++) { 
      edgex[i] = edgeindex; /* where it starts in edge array */ 
      
      fscanf(fp, "%d", &nodeval); 
      edgecount=0; 

      while (nodeval != 0) {  
	numedges++;         /* total for accounting */
        edgecount++;        /* total neighbors this node */ 

        neighbor[edgeindex++] = nodeval; 

	fscanf(fp, "%d", &nodeval);        

      } /* for each adjacent node */
      ecount[i] = edgecount; 

    } /* for each node in graph */ 

} /* end getgraph */         


/*--------------------------------------------------------------*/
/* check if this color is valid 0 no, 1 yes                     */ 

int checkcolorok(int thenode, int thecolor)
{
  int ex = edgex[thenode];  /* index to neighbor list */ 
  int edges = ecount[thenode]; /* number of neighbors */ 

  int e=1;     

  while (e++ <= edges){
    checkcount++; 
    if (colorof [neighbor[ex++] ] == thecolor) return 0; /* false */ 
  }
 
  return  1; 
}

/*--------------------------------------------------------------*/ 
void colorit (int thenode, int thecolor) 
{
  colorof[thenode] = thecolor;
  colorcounts[thecolor]++; 
}   

/*--------------------------------------------------------------*/ 
int colorgraph()
{
  int v ;
  int k; 

  for (k = 1; k <= numnodes; k++) colorcounts[k] = 0; /* color distribuion*/ 

  for (v = 1; v <= numnodes; v++) {
    for (k = 1; k <= numnodes; k++ ) {
      colorcount++;  /* number of colors examined*/  

      if (checkcolorok(v, k)) {
	colorit(v, k);
        if (k > maxcolor) maxcolor=k;  /* overall  */ 
	break;
    }
    }//for k
  }//for v
}
/*-----------------------------------------------------------*/
void printcolors() 
{
  int v; 
  for (v =1; v<= numnodes; v++) {
    printf("%d %d\t", v, bestcoloring[v]); 
    if (v % 10 == 0) printf("\n"); 
  }
}

/*-----------------------------------------------------------*/ 
void printgraph() 
{
  int src;
  int ex; 
  int i; 
 
  for (src = 1; src <= numnodes; src++) {
    ex = edgex[src];
    printf("\n v %d: ", src); 
    for (i=1; i <= ecount[src]; i++) {
      printf("%d \t", neighbor[ex++]);
    }
  }
  printf("\n"); 
}

/*-----------------------------------------------------------*/
void randomcolor() {

  int it, i, k,v, r, x; 
  int bestcolor=numnodes+1; 
  int tmp; 
  int pv; 

  int perm[numnodes+1]; /*initialize node permutation*/ 
  for (i = 1; i <= numnodes;i++) perm[i] = i; 

  for (it = 0; it < iterations; it++)   { 
    /* random permutation */ 

    for (i = numnodes; i >=2; i--) {
      r =  (int) (drand48() * i )+1;
      
      //printf("\n swap  %d  %d \n", i, r); 

      tmp= perm[i];
      perm[i] = perm[r];
      perm[r]=tmp;
    }

#ifdef DEBUG
    printf("\npermutation: ");
    for (x=1; x <= numnodes; x++) printf("%d\t", perm[x]); 
    printf("\n");
#endif

    maxcolor=0; 
    for (i = 1; i <= numnodes; i++) colorof[i] = 0;     /* reinit coloring */ 
    for (k = 1; k <= numnodes; k++) colorcounts[k] = 0; /* color distribution*/ 

    /* color with random permutation */ 
    for (v = 1; v <= numnodes; v++) {
      for (k = 1; k <= numnodes; k++ ) {
	colorcount++;                /* total number of colors examined*/  

	pv = perm[v]; 
	if (checkcolorok(pv, k)) {
	  colorit(pv, k);
#ifdef DEBUG
	  printf("(%d %d)\t", pv, k);
#endif
	  if (k > maxcolor) maxcolor=k;  
	  break;
	}//if 
      }//for k
    }//for v

#ifdef REPORTSTATS
    printf("\n iteration %d  maxcolor %d \n", it, maxcolor); 
    printf("vertex color pairs:\n");
    for (x = 1; x <=numnodes; x++) printf("%d %d \t", x, colorof[x]); 
#endif

    if (bestcolor > maxcolor) {/* save best */ 
      bestcolor=maxcolor; 
      for (i = 1; i <=numnodes; i++) bestcoloring[i] = colorof[i]; 
      for (i = 1; i <=numnodes; i++) colordist[i] = colorcounts[i];
    }
  }// for it
}//randomcolor 

/*-----------------------------------------------------------*/ 
main(argc, argv)
int argc;
char *argv[];

{

  if ((argc != 3) && (argc != 4))  {
    printf("usage: random filename i s  (or)\n");
    printf("       random filename i \n"); 
    exit(1);
  }

  //  if ((fp = fopen(*++argv, "r")) == NULL) {

  if ((fp = fopen(argv[1], "r")) == NULL) {
    printf("color: cant open file %s\n", argv[1]);
    exit(1);
  } 
        
  iterations = atoi(argv[2]);

  if (argc != 4) 
    srand48( (long) time(0) );
  else 
    srand48((long) atoi(argv[3])); 

  getgraph(); 
  randomcolor(); 
      
#ifdef REPORTCOLORS
  printf("\nvertex color pairs:\n"); 
  printcolors();
#endif 
      
#ifdef REPORTSTATS  
  printf("\ngraph:\n"); 
  printgraph(); 
  //double avecolor= ((double) colorcount) / (double)numnodes;
  printf("n  m  it  compares maxcolor  colorcount  \n");
  printf("%d %d  %d  %d      %d       %d          \n", 
	 numnodes, numedges, iterations,
	 checkcount, maxcolor, colorcount);
  int i;
  printf("color counts:\n"); /* of best coloring only*/ 
  for (i=1; i <= numnodes; i++) {
    printf("%d %d \t", i, colordist[i]);
    if (i % 10 == 0) printf("\n");
    if (colordist[i]==0) break;
  }
      
#endif

} /* end main */ 
