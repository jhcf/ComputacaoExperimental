#include <stdio.h>
#include <stdlib.h>
#define MAXNODES 100
#define MAXEDGES 100*(100-1)/2  

/* GREEDY    Graph coloring algorithm C.C. McGeoch 2011 
 
   Usage: greedy filename

   To report counts:       #define REPORTSTATS  1
   To report the coloring: #define REPORTCOLORS 1
*/ 
#define REPORTSTATS  1
#define REPORTCOLORS 1

int numnodes=0;    /* number of nodes in the graph */
int numedges=0; 
int checkcount=0;  /* number of colors checked */
int colorcount=0;  /* to compute average colors looked at */ 
int maxcolor=0;    /* max color assigned */ 

/* The graph is represented as an adjacency list in two arrays */ 
int edgex[MAXNODES+1];      /* index to neighbor array */ 
int ecount[MAXNODES+1];     /* number of neighbors this node */ 
int neighbor[MAXEDGES+1];   /* adjacent nodes       */ 

int colorof[MAXNODES+1];    /* node colors */  

int colorcounts[MAXNODES+1]; 

FILE *fp, *fopen();

/*--------------------------------------------------------------*/ 
/* getgraph: Read input file and build graph */ 
/*  It works if only edge (5,3) of both (5,3), (3,5) are input, since
    it only checks lower-numbered neighbors of 5.   */ 
/* Input Format:  
      First line is integer number of nodes. 
      Then: 
         neighbors of v1, list ending with 0  (none < 1)
         neighbors of v2, list ending with 0  
         ... 
         neighbors of vn, list ending with 0  (could be 1..n-1) 
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
    if ( (neighbor[ex] <= thenode) && 
         (colorof [neighbor[ex++] ] == thecolor)) return 0; /* false */ 
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
void colorgraph()
{
    
  int v ;
  int k; 

  for (k = 1; k <= numnodes; k++) colorcounts[k] = 0; /* color distribuion*/ 

  for (v = 1; v <= numnodes; v++) {
    for (k = 1; k <= numnodes; k++ ) {
      colorcount++;  /* number of colors examined*/  
      if (checkcolorok(v, k)) {
	colorit(v, k);
        if (k > maxcolor) maxcolor=k; 
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
    printf("%d %d\t", v, colorof[v]); 
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
main(argc, argv)
int argc;
char *argv[];

{
     if (argc != 2) {
         printf("usage: color filename\n");
         exit(1);
       } else {
          if ((fp = fopen(*++argv, "r")) == NULL) {
                    printf("color: cant open file %s\n", *argv);
                    exit(1);
		  } else {

		    getgraph();
		    colorgraph();
#ifdef REPORTCOLORS
                    printf("vertex color pairs:\n"); 
                    printcolors();
#endif 

#ifdef REPORTSTATS  
                    printf("graph:\n"); 
                    printgraph(); 
		    double avecolor= ((double) colorcount) / (double)numnodes;
                    printf("n  m  compares maxcolor  colorcount ave: \n");
		    printf("%d %d    %d      %d       %d         %lf\n", 
			   numnodes, numedges,
			   checkcount, maxcolor, colorcount, avecolor);
		    int i;
                    printf("color counts:\n"); 
		    for (i=1; i <= numnodes; i++) {
		      printf("%d %d \t", i, colorcounts[i]);
		      if (i % 10 == 0) printf("\n");
		      if (colorcounts[i]==0) break;
		    }
		    		    
#endif
		  }  /* end else */ 
	} /* end else */ 
} /* end main */ 
