#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>

/* Code to generate the essential subgraph H of a random uniform graph G */
/* By C. McGeoch, modified 5/10 for AlgLab                               */
/* Version 2: memoization with a distance matrix, plus filtering  */  
/* (formerly version2a) */ 

/* Inputs (one per line): */
/*   nodes n              */
/*   trials t             */
/*   seed s     (optional)*/

/* Output: (all on one line)       */
/* Trial number, G.nodes, G.edges, */
/* Edges in H                      */
/* Cost of last edge in H          */
/* Rank (in G) of last edge in H   */
/* Rank (in G) of largest MST edge */
/* MST Diameter                    */ 

#define Assert( cond , msg ) if ( ! (cond) ) {printf("msg \n"); exit(1);} ; 

#define MAXNODES 1601
#define MAXEDGES 1279201 
#define TRUE 1
#define FALSE 0 
#define INFINITY -9999999.0

typedef char string[50]; 

double dmatrix[MAXNODES][MAXNODES]; 

/*-------------------------------------------------------------------------*/
/* Definitions for G and E graphs                                          */
/*-------------------------------------------------------------------------*/
typedef struct edgestr {       /*  Edges of essential subgraph (adj list) */
  int dest;
  double cst; 
  struct edgestr *next; 
} edgetype ; 


typedef struct nodestr {        /* Nodes of essential subgraph */
  edgetype *first;
  edgetype *last; 
} nodetype;


typedef struct {       /* Edges in random generated graph  */ 
   int vv;
   int ww;
   double cost; 
 } gedgetype; 

int edgecount;                    /* Number edges generated */ 
int edgeindex;                    /* index in edge array of G*/ 
double curmax;
double mydrand(); 
double drand48(); 

gedgetype edgeset[MAXEDGES];      /*  Edges of G */ 
nodetype nodelist[MAXNODES];      /*  Nodes of E*/ 

/*-----------------------------------------------------*/
/* Global parameters and param table                   */
/*-----------------------------------------------------*/
int nodes, trials, seed;  

int totedges; 
int rand_seed;       /*boolean for random or supplied seed*/ 
string cmdtable[10]; 
int cmdtable_size; 


/*-------------------------------------------------------*/ 
/*  Input Handlers and Misc.                             */ 
/*-------------------------------------------------------*/
/*--------------tabinit-------------------------*/
void tabinit()
{
  cmdtable_size = 3; 

  strcpy(cmdtable[0], "sentinel.spot"); 
  strcpy(cmdtable[1], "nodes");
  strcpy(cmdtable[2], "trials");
  strcpy(cmdtable[3], "seed"); 
  nodes=MAXNODES;
  trials=1;
  rand_seed = TRUE;  
}
/*------------lookup command in table------------ */ 
int lookup (cmd) 
string cmd;  
{
  /* lookup commands in command table */ 
  int i; 
  int stop;  
  strcpy(cmdtable[0], cmd); 
  stop = 1; 
  for (i=cmdtable_size; stop != 0 ; i--) stop = strcmp(cmdtable[i], cmd); 
  return ( i+1 );
}/*lookup*/ 

/*------------translate input command---------------*/ 
void transinput() { 
  char cmd[50], buf[50];
  int index;  
  while (scanf("%s", cmd) != EOF ) { 
    fgets(buf, sizeof(buf), stdin);

    index = lookup (cmd);
    switch (index ) {
    case 0 : { printf("%s: Unknown command\n", cmd); 
	       break;
	     }
    case 1 : { sscanf( buf, "%d", &nodes);
	       Assert(1<=nodes && nodes<=MAXNODES , Nodes out of range. ); 
	       break;
	     }
    case 2: { sscanf( buf, "%d", &trials); 
	      break;
	    }
    case 3: { sscanf( buf, "%d", &seed); 
 	      rand_seed = FALSE; 
	      break; 
	    }
    }/*switch*/
  }/*while scanf */

  if (rand_seed == TRUE) srand48((int) time(0));
  else srand48(seed);

}/*transinput*/



/*--------------------------------------------------------------------------*/
/*  The Heap used for Dijkstra searches                                     */
/* -------------------------------------------------------------------------*/
typedef struct {    /* Heap elements */ 
  int hid;          /* node name  */ 
  double hval;      /* distance from src */ 
} heaptype;

int      heapsize;       /* heap is in locations 1..heapsize */ 
heaptype heap[MAXNODES];
int      hloc[MAXNODES]; /* location backpointers in heap   */ 


/* --heapinit -------initialize empty with one node,  dist = 0 */ 
void heapinit(int src) { 
  heap[1].hid = src;
  heap[1].hval = 0.0; 
  hloc[heap[1].hid] = 1; 
  heapsize = 1; 

} 
/*-------heapextract -----------------------------------------*/ 
heaptype heapextract() { 

  heaptype heaptop  = heap[1]; 

  heap[1] = heap[heapsize--]; 
  hloc[ heap[1].hid ]  = 1; 

  heaptype  tmp; 
  int hix = 1; 
  int lchild, rchild, which; 
  double min ; 


#ifdef HEAP 
  int i; 
  printf("removed %d %lf\n", heaptop.hid, heaptop.hval); 
  printf("heap before siftdown :\n");
  for (i = 1; i <= heapsize; i++) {
    printf("\t %d  %d  %d  %lf\n",i, hloc[heap[i].hid], heap[i].hid, heap[i].hval);
  }
#endif 

 /* siftdown */  
  while ( hix <  heapsize) {
    min = heap[hix].hval;
    which = hix; 

    lchild = hix*2; 
    if (lchild <= heapsize && heap[lchild].hval < min ) { 
      min = heap[lchild].hval;
      which = lchild; 
    }
    rchild = hix*2 + 1; 
    if (rchild <= heapsize && heap[rchild].hval < min) {
      min = heap[rchild].hval;
      which = rchild; 
    } 
    if (which == hix) break;
    else { /* swap */ 
      tmp = heap[hix];
      heap[hix] = heap[ which ]; 
      hloc[ heap[hix].hid] = hix; 

      heap[ which ] = tmp;
      hloc[ heap[which].hid ] = which;

      hix = which; 
    }
  }/* while */ 

#ifdef HEAP 
  printf("\n heap after siftdown (size %d) to  %d \n" , heapsize, hix); 
  for (i= 1; i <= heapsize; i++) 
    printf("%d  %d  %d  %lf \n", i , hloc[heap[i].hid], heap[i].hid,  heap[i].hval); 
  printf("*\n");
  fflush(stdout); 
#endif

  return heaptop;  }  /* extractmin */ 

/*---siftup from position hix ------------------------------------*/  
void siftup (int hix) { 

  heaptype tmp ; 

#ifdef HEAP
  int i; 
  printf("before siftup from location %d \n", hix);
  for (i = 1; i <= heapsize; i++) 
    printf("\t %d %d %d %lf \n",  i, hloc[heap[i].hid], heap[i].hid, heap[i].hval);
 
#endif 

  while (hix > 1) { 
    int parent = hix/2; 

    if ( heap[hix].hval  <  heap[ parent ].hval ) {
      tmp = heap[hix];
      heap[hix] = heap[parent];
      heap[parent] = tmp;

      hloc[ heap[hix].hid ] = hix;
      hloc[ heap[parent].hid] = parent;

      hix = parent;
    } else break;


  }/*while*/ 

#ifdef HEAP 
  printf("after siftup to location %d \n", hix);
  for (i = 1; i <= heapsize; i++) 
   printf("\t %d %d %d %lf \n",  i, hloc[heap[i].hid], heap[i].hid, heap[i].hval);
#endif 

}

/*---heapinsert------------------------------------------------*/ 
void heapinsert( int nodename, double dist ) {

  heapsize++;
  heap[heapsize].hid = nodename;
  heap[heapsize].hval = dist; 

  hloc[ nodename ] = heapsize;
  siftup( heapsize); 

#ifdef DEBUG_H
  printf("\n hinsert %d  %lf hsize %d \n", nodename, dist, heapsize); 
#endif

 }  /* insert new into heap */ 

/* -heapgetinx--------------------return index for query, update */ 
int heapgetinx (int node) { 
    return  hloc[node];  
} 

/*-heapgetdist----------------return distance for comparison    */ 
double heapgetdist( int inx) {
  return heap[ inx ].hval;
} 

/*heapdecreasekey-----------------------------------------------*/
void heapdecreasekey( int inx, double newval) { 
  heap[ inx ].hval = newval;
  siftup (inx);
} 

/*--------------------------------------------------------------------------*/
/*  Essential Subgraph distance calculation, insert, init                   */
/*--------------------------------------------------------------------------*/ 



/*-----distance: search from s to d in E, return distance or infty      */ 
/*                                                                      */ 
double distance(int s,int d, double ecost)  {

  edgetype *nptr;
  int ename;   
  double edist; 
  double curdist; 

  int  unseen = 0; 
  int  inheap = 1;
  int  seen = 2; 
  int status[MAXNODES];  /* For shortest-path search */ 

  /*V1: memoization*/ 
  if ((dmatrix[s][d] != INFINITY) && (dmatrix[s][d] < ecost)) 
     return dmatrix[s][d]; 
 
  int i; 
  for (i = 1; i <= nodes; i++) {
    status[i] = unseen; 

  } 
  status[s] = inheap;

  /* initalize heap to source vertex */ 
  heapinit (s); 
  heaptype x;  
 
  while ( heapsize != 0 ) {
    x  = heapextract();

   /* V1: distance matrix */ 
   dmatrix[s][x.hid] = x.hval;
   dmatrix[x.hid][s] = x.hval; 
 
   if (x.hid == d) return x.hval;  /* found it */  

   /* V2: abort the loop if this lb on distance to w is too big */ 
   /* V2 is faster if this tuneup is not used */  
   // if (x.hval > ecost) return x.hval; 
   status[x.hid] = seen; 
   curdist = x.hval; 

   /* for each neighbor e of x*/ 
   nptr = nodelist[x.hid].first;  
   while (nptr != NULL) {
     ename = nptr->dest;
     edist = curdist + nptr->cst;

     /* V2: filter the data structure */ 
     if ( edist > ecost );      /* skip it */
     else
       if (status[ename] == unseen) { 
	 heapinsert( ename, edist) ; 
	 status[ename] = inheap; 
     }
     else if (status[ename]==inheap) {
       int heapx = heapgetinx(ename); 
       if (edist < heapgetdist( heapx ) ) {
	 heapdecreasekey (heapx, edist) ; 
       }
     }/* else status = seen */ 
     nptr = nptr -> next;
     
   }/* for each neaighbor */ 
  }/* while heapsize */ 

  return INFINITY;  /* didn't find d reachible from s */  

}/*distance */ 

/*-------------insert---------------------------------------*/
void insert(int v, int w, double cost)  
{
  edgetype *newedge;
  edgetype *lptr; 

#ifdef DEBUGINS 
  printf("\t\t subgrinsert %d %d %lf\n", v, w, cost); 
     fflush(stdout); 
#endif 

  /* insert edge in v list */
   newedge = (edgetype *) malloc( sizeof(  edgetype  ) );
   newedge->dest = w; 
   newedge->cst = cost;
   newedge->next = NULL;
   if (nodelist[v].first == NULL) {
       nodelist[v].first = newedge;
     } else{
       lptr = nodelist[v].last;
       lptr->next = newedge;
     } 
    nodelist[v].last = newedge; 

   /* insert in w list */ 
   newedge = (edgetype *) malloc( sizeof( edgetype ) );
   newedge->dest = v; 
   newedge->cst = cost;
   newedge->next = NULL;
   if (nodelist[w].first == NULL) {
       nodelist[w].first = newedge;
     } else{
       lptr = nodelist[w].last;
       lptr->next = newedge;
     } 
   nodelist[w].last = newedge; 

}/*insert*/ 


/*-------------------------------------------------------------*/
/*       Generation of random edges and costs                  */ 
/*-------------------------------------------------------------*/

/*------------------mydrand-------------------------------------*/
double mydrand()
{
  return(drand48()); /* Reals on (0.0, 1.0]*/ 
} 
/*--------------- nextedge------------------------------------*/
/*  Return the next edge, v, w, cost                          */ 

void nextedge( int* v, int* w, double* cost)
{
  gedgetype  tmp = edgeset[edgeindex++];

  *v = tmp.vv;
  *w = tmp.ww;
  *cost = tmp.cost;
}

/*--dcompare-----------------------------------*/
/* for qsort                                   */ 
int dcompare( const void *a, const void *b) {
    
    gedgetype *aa = (gedgetype* ) a;
    gedgetype *bb = (gedgetype* ) b; 

    if ( aa->cost < bb->cost ) return -1;
    else if (aa-> cost > bb->cost ) return +1;
    else return 0;
} 

/*-----------------graphinit-------------------------------------------------------*/
/* S: initialize nodes (nodelist with edgeset empty                                */ 
/* S is an adjacency list, with edges doubled                                      */ 
/* G: is a list of random edges, in random vertex order                            */ 
/* The graph is undirected, nodes numbered 1..nodes, in array [1..nodes]           */
/*  There is one copy of each edge, oriented to indices (vv=lo, ww = hi)           */ 
/*  Sorted by edge cost                                                            */

void graphinit()
{
   int i, j, k;

   /* initialize node pointers for the subgraph E */ 
   totedges = nodes * (nodes-1) / 2; 
   for (i=1; i<= nodes; i++) {
	nodelist[i].first = NULL;
        nodelist[i].last = NULL;
      }
   
   /* initialize edges in the random input graph */ 
   k=0; 
   for (i=1; i<= nodes-1 ; i++) {
     for (j=i+1; j<= nodes; j++) {
         edgeset[k].vv= i; 
	 edgeset[k].ww = j;
         edgeset[k].cost = mydrand();  /* random uniform 0..1 */
         k++;
    }
   }
   /* sort the edges by cost */ 
   qsort (edgeset, totedges, sizeof(gedgetype), dcompare); 
   edgeindex = 0;     /* first edge returned*/ 

#ifdef DEBUGSRT
   for (i = 0; i < totedges; i++) 
     printf("%d \t %d \t %d \t %lf \n", i,edgeset[i].vv, edgeset[i].ww, edgeset[i].cost); 
     fflush(stdout); 
#endif 

}/*graphinit*/

/*---printsg for output  */ 
void printsg() {
  edgetype *nptr; 
  int ename; 
  double ecost; 
  int i; 
  int ctr; 
  
  /* The subgraph */ 
  for (i = 1; i <= nodes; i++) {
    printf("Node %d: \n ", i ); 
    ctr = 0; 
    nptr = nodelist[i].first; 
    while (nptr != NULL ) {
      ename = nptr->dest;
      ecost = nptr->cst; 
      printf("(%d  %lf) \t", ename, ecost); 
      nptr = nptr->next; 
      if (ctr % 10 == 0) printf("\n"); 
    }
    printf("\n");
  }
} 

/*---freesg for new trial  */ 
void freesg() {
  edgetype *nptr; 
  int i; 
  edgetype *nextedge;   

  /* The free subgraph */ 
  for (i = 1; i <= nodes; i++) {
    nptr = nodelist[i].first;
 
    while (nptr != NULL ) {
      nextedge = nptr->next; 
      free (nptr); 
      nptr = nextedge; 
    }

    nodelist[i].first = NULL;
    nodelist[i].last = NULL; 
  }
} 

/* V1: initialize the distance matrix */ 
void dminit() { 
  int i, j; 
  for (i = 1; i <= nodes; i++) 
    for (j= 1; j <= nodes; j++) dmatrix[i][j] = INFINITY; 
} 


/*----------------------------------------------*/ 
/*---------------------main---------------------*/ 
int main(int argc, char** argv)
{
  int DONE; 
  double ecost; 
  int v,w;
  int tr; 

  double  limit = 1.1;
  double lastcost = 1.1;
  int lastrank = totedges;
  int insertcount = 0;  
  double mindist; 

  tabinit();      
  transinput();   /* get input commands */ 

#ifdef DEBUG
  printf("input %d %d %d\n", nodes, trials, seed);
     fflush(stdout); 
#endif 
  
  for( tr= 0 ; tr< trials; tr++) {

    graphinit();     /* generate random graph, initialize subgraph */ 

    /* V1: initialize the distance matrix*/ 
    dminit(); 

    limit =  1.1;   /* larger than any edge size */ 
    
    edgecount=0;           /* num generated: incremented in nextedge */ 
    insertcount = 0;       /* num in subgraph */
    lastcost = 0;          /* last edge in graph */
    lastrank = 0;          /* rank of last edge in graph */ 
  
    curmax = 1.0;          /* modified in nextcost */  

    DONE = FALSE;  
    while (!DONE) {        /* generate the subgraph */    

      nextedge(&v, &w, &ecost);    
      edgecount++; 
      mindist = distance(v,w, ecost);

#ifdef DEBUG
      printf("edge %d : (%d  %d %lf) dist %lf ", edgecount, v,w,ecost, mindist);
      if (mindist == INFINITY) printf("ins \n"); 
      else if (mindist > ecost) printf("ins \n");
      else printf("out \n"); 
     fflush(stdout); 
#endif 


     if ( (mindist == INFINITY)|| (mindist > ecost)) { 
            insert(v,w,ecost); 
            insertcount++; 
            lastcost = ecost;
	    lastrank = edgecount; 
     } 
   
     
     if (ecost > limit) DONE = TRUE;   
     if (edgecount >= totedges) DONE = TRUE; 

   }/*while still checking edges*/ 

#ifdef SEEOUTPUT 
    printsg();
    fflush(stdout); 
#endif 

  printf("%d %d %d   %d %lf %d  %lf\n", 
	 tr, nodes, totedges, insertcount, lastcost, lastrank,  limit);

  freesg(); 

  }/*for each trial */ 

  return 0; 
}/* main */ 





