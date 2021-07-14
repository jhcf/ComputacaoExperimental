#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAXNODES  100
#define MAXCOLORS 100    
#define MAXEDGES 100*(100-1)/2  

/* SIG Simplified Iterated Greedy graph coloring algorithm C.C. McGeoch 2011
   Usage: sig <filename 

   filename:   input file containing algorithm parameters. See
          getinput.c for details. 
*/ 

#define REPORTSTATS  1
#define REPORTCOLORS 1
#define DEBUG 1
#define ASSERT(cond, msg) if (! (cond)) {printf("msg \n"); exit(1);} ;

#include "getinput.c" 

int numnodes=0;    /* number of nodes in the graph */
int numcolors=0;   /* initialized to equal numnodes, then decreases  */ 
int numedges=0;    /* total edges */

/*-------------------------------------------------------------*/
/* Graph and coloring representation                           */
/* The graph is an adjacency list in two arrays                */ 
/*-------------------------------------------------------------*/ 

/* vertices named/indexed 1..n; colors named/indexed 1..n */ 
int edgex[MAXNODES+1];      /* vertex v: index to neighbor array */ 
int ecount[MAXNODES+1];     /* vertex v: number of neighbors */ 
int neighbor[MAXEDGES+1];   /* list of adjacent nodes */ 
int colorof[MAXNODES+1];     /* vertex v: current color */ 

int curcolors;               /* number distinct colors in coloring*/ 
int curcolorscore;           /* sum of current colors in coloring + (curcolors*numnodes) */ 
int colorcount[MAXNODES+1];  /* color c: number of vertices this color */ 

int bestcoloring[MAXNODES+1]; /* save best coloring: i is col(v_i) */ 
int bestcolors; 
int bestcolorscore = MAXNODES+1;            

int vperm[MAXNODES+1];       /* vertex permutation: pos i is name of v_i */ 
int cperm[MAXNODES+1];       /* color permutation:  pos i is name of c_i */ 

/*-------------------------------------------------------*/
/* algorithm statistics                                  */ 

int checkcount; 

/*-----------------------------------------------------------*/
/* Utility functions                                         */

/*---printcolors---------------------------------------------*/ 
void printcolors(int which) 
{
  int v; 

  if (which==0) {// print best 
    printf("best coloring:\t"); 
    for (v =1; v<= numnodes; v++) {
      printf("%d %d\t", v, bestcoloring[v]); 
      if (v % 10 == 0) printf("\n"); 
    }
    printf("\n best  %d  %d \n", bestcolors, bestcolorscore); 
  }
  else { // print current 
    printf("current coloring:\t"); 
    for (v =1; v<= numnodes; v++) {
      printf("%d %d\t", v, colorof[v]); 
      if (v % 10 == 0) printf("\n"); 
    }
    printf("\n curcolors,score  %d  %d \n", curcolors, curcolorscore); 
  }
}
/*---printgraph----------------------------------------------*/ 
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
/*--------------------------------------------------------------*/ 
/* getgraph: Read input file and build graph.                   */ 
/* Input Format:  
      First line is integer number of nodes. 
      Then (nodes are named 1..n):  
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

  //fp is declared, opened during getinput 

    fscanf(fp,"%d", &numnodes);  /* number of nodes in graph */
    numcolors = numnodes; 
    if (numnodes >MAXNODES) {
      printf("too many nodes: recompile and try again\n");
      exit(1); 
    }
 
    for (i=1; i <= numnodes; i++) { 
      edgex[i] = edgeindex;  
      fscanf(fp, "%d", &nodeval); 
      edgecount=0; 

      while (nodeval != 0) {  
	numedges++;         /* total for accounting */
        edgecount++;        /* total neighbors this node */ 
        neighbor[edgeindex++] = nodeval; 
	fscanf(fp, "%d", &nodeval);        

      } // for each neighbor 
      ecount[i] = edgecount; 
    } // for each vertex 
} //getgraph 

/*--------------------------------------------------------------*/
/* Greedy coloring                                              */

/*--checkcolorok----------------------------------------------*/ 
int checkcolorok(int thenode, int thecolor)
{
  int ex = edgex[thenode];  /* index to neighbor list */ 
  int edges = ecount[thenode]; /* number of neighbors */ 
  int e=1;     
  while (e++ <= edges){
    if (colorof [neighbor[ex++] ] == thecolor) return 0; /* false */ 
  }
  return  1; 
}
/*---colorit---------------------------------------------------*/ 
void colorit (int thenode, int thecolor) 
{
  colorof[thenode] = thecolor;
  colorcount[thecolor]++; 
}   
/*---greedycolorgraph-----------------------------------------------------------*/ 
/* recolors the graph respecting vperm, cperm, and current colors               */ 
void greedycolorgraph()
{
  int v , vertex; 
  int k, c; 

  for (k = 1; k <= numnodes; k++) colorcount[k] = 0; // initialize 
  curcolors=0; 
  curcolorscore=0; 

#ifdef DEBUG
  printf("\nbefore greedy color: ");
  printcolors(1); 
  printf("\n vperm is: \t");
  for (v = 1; v <= numnodes; v++) printf("%d \t", vperm[v]);
  printf("\n cperm is: \t");
  for (c = 1; c <= numcolors; c++) printf("%d \t", cperm[c]);
#endif 

  for (v = 1; v <= numnodes; v++) { // for each vertex 
    vertex = vperm[v];  // respecting vperm 

    for (k = 1; k <= numcolors; k++ ) { // for each color  
      int color = cperm[k] ;   // respecting cperm 

      if (checkcolorok(vertex, color)) {   // if ok 
	colorit(vertex, color);            // set colorof
	curcolorscore += color;            // sum of colors assigned in coloring  
	if (curcolors < k) curcolors=k;    // number of distinct colors assigned 
	break; 
      }//if 
    }//for k
  }//for v

  curcolorscore += curcolors*numnodes;  //finish curcolorscore  
  if (curcolors < numcolors) numcolors = curcolors;  // reduce size of color set 

#ifdef DEBUG
  printf("\nafter greedy color:  ");
  printcolors(1);
  printf("curcolors %d curcolorscore %d  numcolors %d\n", curcolors, curcolorscore, numcolors);
#endif 

} // greedycolorgraph 

/*---saveAsBest---------------------------------------------------*/ 
void saveAsBest() {
  int v; 

  for (v=1; v<=numnodes; v++)  bestcoloring[v] = colorof[v];
  bestcolors=curcolors;  // should equal numcolors 
  bestcolorscore=curcolorscore; 
#ifdef DEBUG 
  printf("save best\n"); 
#endif 
} 
/*---revertToBest-------------------------------------------------*/ 
void revertToBest() {
  int v;
  for (v=1; v<= numnodes; v++) colorof[v] = bestcoloring[v];
  curcolors=bestcolors;
  bestcolorscore= curcolorscore;
#ifdef DEBUG
  printf("revert \n");
#endif 

}

/*--pickRandomColorRule------------------------------------------------*/
int pickRandomColorRule() {
  
  double  pr = drand48(); 
  int i;  
  for (i=0; pr > ccutoffs[i] ; i++); 
  return i; 
}
/*--pickRandomVertexRule-----------------------------------------------*/ 
int pickRandomVertexRule() {
  double pr = drand48();
  int i;
  for (i=0; pr > vcutoffs[i]; i++);
  return i; 
} 

/*---vertexreorder--------------------------------------*/ 

void vertexreorder(int vr) { 
   /* vertex re-order puts vertices in groups according to color, by rule: 
        0: 1..k  colors going up 
        1: k..1  colors going down 
        2: random color order
        3: by decreasing total degree in color set  
   builds vperm[1..n] vperm[i] names the vertex to be considered ith in turn  
  */ 
  int i;
  int colorhead[numnodes+1];  // start of each color list 
  int next[numnodes+1];       // next vertex in color list 
  int order[numnodes+1];      // temp for ordering colors 
  int c, k, cc, r; 
  int vpermx=0;
  int vx; 
  int tmp; 
  int max; 
  int v; 

 // gather vertices according to color
  for (k=1; k <= numnodes; k++) { //numnodes = max colors 
    colorhead[k] = 0;  
    next[k] = 0;
  }
  for(v=1; v<= numnodes;v++) {
    c = colorof[v];
    next[v] = colorhead[c];
    colorhead[c] = v; 
  }

#ifdef DEBUG
  printf("vertex rule %d: ", vr);
  switch(vr) {
  case 0: printf("1..k\n"); break;
  case 1: printf("k..1\n"); break;
  case 2: printf("random\n"); break;
  case 3: printf("decreasing vcount in colorset\n"); break;
  }
#endif 

  //copy into vperm by color group and rule 
  vpermx = 1; // index into vperm 

  switch(vr) {
  case 0: // vertices by increasing colors (skipping unused colors)   
    for (c = 1; c <= numnodes; c++) {
      vx = colorhead[c];
      while (vx != 0) { 
	vperm[vpermx++] = vx;
	vx = next[vx];
      }
    }// for 
    break; 
  case 1: // vertices by decreasing colors (skipping unused colors) 
    for (c = numnodes; c>= 1; c--) {
      vx = colorhead[c];
      while (vx != 0) {
	vperm[vpermx++] = vx;
	vx = next[vx];
      }
    }//for 
    break; 

  case 2:  //vertices by random color gropus 
    for (i = 1; i <= numnodes; i++) order[i] = i; // initialize order 
    for (i = numnodes;  i >= 2; i--){             // random permute 
      r =  (int) (drand48() * i) +1  ;            //1..i inclusive 
      tmp= order[i];
      order[i] = order[r];
      order[r]=tmp;
    }

    for (c = 1; c <= numnodes; c++) {//build vperm 
      cc = order[c]; 
      vx = colorhead[cc];
      while (vx != 0){
	vperm[vpermx++] = vx;
	vx = next[vx];
      }
    }//for 
    break;

  case 3: //vertices by decreasing vcount in colorset 
    for (c = 1; c <= numnodes; c++) order[c] = colorcount[c]; // copy it 
    while ( TRUE ) {  // at most maxnodes passes, usually fewer 
      max = -1; 
      for (c = 1; c <= numnodes; c++) if (max < order[c]) max = order[c]; 

      for (c = 1; c <= numnodes; c++) { // copy all groups with max colors 
	if (max == order[c] ) {
	  vx  = colorhead[c]; 
	  while (vx != 0) {
	    vperm[vpermx++] = vx;
	    vx=next[vx];
	  }
	  order[c] = -1 ;  //remove it from copy 
	}//if max 
      }// c passes 
       if (max==0) break;  // last colorcount was 0, done       
    }// while true 
    break;
  }//switch 

}//vertexreorder 

/*-----------------------------------------------------------------*/ 
/* colorreroder sets the colors in order to be considered         */ 
void colorreorder (int cr) { 
  /*re-order colors in cperm[1..numcolors] by color rule  
    0: 1..k up 
    1: largest color set first 
    2: smallest color set first 
    3: random colors 
    4: down k..1 
    5: reverse current order 
result is array cperm[1..numcolors] (just the front): cperm[i] names the ith color 
  */ 
  int i, c, cx; 
  int r; 
  int tmp; 
  int order[numnodes+1];     
  int coloruse[numnodes+1] ;  // which colors are in the mix? 
  int newcperm[numnodes+1];  
  int newx=1;  
  int max; 
  int min; 

  for (c = 1; c <= numnodes; c++)  coloruse[c] = FALSE; 
  for (c = 1; c <= numcolors; c++) coloruse[ cperm[c] ] = TRUE; //use this color 

#ifdef DEBUG
  printf("color reorde rules  %d number colors is %d: ", cr, numcolors);
  switch(cr) {
  case 0: printf("1..k\n"); break; 
  case 1: printf("largest first\n"); break;
  case 2: printf("smallest first\n"); break;
  case 3: printf("random\n"); break;
  case 4: printf("k..1\n"); break; 
  case 5: printf("reverse current\n"); break; 
  }
  printf("use these colors: "); 
  for(c =1; c <= numcolors; c++) printf(" %d\t" , cperm[c]); 
  printf("\n"); 
#endif 

  switch (cr) {
  case 0: // 1..k up 
    cx = 1; 
    for (c = 1; c<= numnodes; c++)               // for all colors in up order 
      if (coloruse[c] == TRUE) cperm[cx++] = c;  // if in use, put it next in cperm 
    break; 

  case 1:  // largest color set first among those in front of cperm 
    for (c = 1; c <= numcolors; c++) {
      order[ cperm[c] ] = colorcount[ cperm[c] ]; // copy 
    }
    newx=1; 
    
    while (TRUE) {
      max = -1; 
      for (c = 1; c <= numcolors; c++) {
	if (order[cperm[c] ] > max) max = order[cperm[c] ];
      }
      if (max<=0) break; // ignore unused colors 
      for (c = 1; c <= numcolors; c++) {
	if (order[cperm[c]] == max) { // copy all colors with max count 
	    newcperm[newx++] = cperm[c]; 
	    order[cperm[c] ] = -1;  // reset 
	  }
      }
    }//while 
    for (c = 1; c <= numcolors; c++) cperm[c] = newcperm[c];  
    break;
    
  case 2: // smallest colorset first, not including 0 counts  
    for (c = 1; c <= numcolors; c++) {
        order[ cperm[c] ] = colorcount[ cperm[c] ]; // copy `
    } 
    newx=1; 
    while (TRUE) {
      min = numnodes+1 ; 
      for (c = 1; c <= numcolors; c++) 
	if (order[cperm[c] ] < min ) min = order[cperm[c] ];
      if (min==numnodes+1) break; // done finding colors to copy 
      for (c = 1; c <= numcolors; c++) {
	if (order[cperm[c]] == min) {
	    newcperm[newx++] = cperm[c]; 
	    order[cperm[c] ] = numnodes+2;  // reset to higher number
	  }
      }
    }// while 
    for (c = 1; c <= numcolors; c++) cperm[c] = newcperm[c];  
    break;

  case 3: // random  order among front colors  
    for (i = numcolors;  i >= 2; i--){
      r =  (int) (drand48() * i) +1 ; //1..i inclusive 
      tmp= cperm[i];
      cperm[i] = cperm[r];
      cperm[r]=tmp;
    }
    break;

  case 4: // down k..1 among front of cperm 
   cx = 1; 
   for (c = numnodes; c>= 1; c--) {          // for all colors in down order 
     if (coloruse[c] == 1) cperm[cx++] = c;  // if in use, put it next in cperm 
   }
   break; 

  case 5: // reverse current order : 
    cx = 1; 
    for (c = numcolors; c>= 1; c--) newcperm[cx++] = cperm[c]; 
    for (c = 1 ; c <= numcolors; c++) cperm[c] = newcperm[c]; 
    break; 
  }//switch 

  /* now cperm[1..numcolors] has the new color permutation  */ 

} // reordercolor 

/*---initialcoloring()--------------------------------------------------*/ 
void initialcoloring() 
 {
   int v,i,r, c, tmp; 
   for (i = 1; i<= numnodes; i++)  vperm[i] = i;   //initialize vertex permutation
   for (i = 1; i<= numcolors; i++) cperm[i] = i;   //initialize  color permutation 

   switch (initpolicy) {
   case 1:  // n distinct colors 
     for (v=1; v<=numnodes; v++) {
       colorof[v] = v;  
       colorcount[v]=1; 
       curcolorscore += v;
     }
     curcolors=numnodes; 
     curcolorscore += curcolors * numnodes;  
     numcolors=curcolors; 
     break; 
  
   case 2: // greedy with random vertex order, color order increasing  
     for (i = numnodes;  i >= 2; i--){
       r =  (int) (drand48() * i) +1 ; //1..i inclusive 
       tmp= vperm[i];
       vperm[i] = vperm[r];
       vperm[r]=tmp;
     }
     for (v=1; v<= numnodes; v++) {
       colorof[v] = 0;  //initialize vertices to not colored 
     }
     greedycolorgraph(); 
     break; 
  
   }//switch  

}//initialcoloring 

/*-----------------------------------------------------------*/ 
/* SIG: simple iterated greedy coloring                      */

void sigcolor() {

  initialcoloring(); 
  saveAsBest(); 

#ifdef DEBUG 
  printf("after initialize\n"); 
  printcolors(0); //best
  printcolors(1); //current
#endif 

  int t = 0; // total iterations 
  int b = 0; // iterations since last save (new best) 
  int r = 0; // iterations since most recent (savebest, revert)   

  int cr=0; 
  int vr=1; //rules 
  
  while ((b < maxiter) && (bestcolors > target)) {
    t++;  b++;  r++;  

    // cr = pickRandomColorRule();
    //vr = pickRandomVertexRule();

    vr = (vr+1) % 4; 
    cr = (cr+1) % 6;  

#ifdef DEBUG
    printf("\niteration %d color rule %d vertex rule %d\n", t, cr, vr); 
#endif 

    vertexreorder(vr);
    colorreorder(cr); 
    greedycolorgraph(); // recolor, respecting old colors

    if (curcolors < bestcolors) {// if this is a new min color count 
      saveAsBest();
      b = 0;  
      r = 0;                    // new improvement 
    } 
    else if (curcolorscore < bestcolorscore) { // if this is a new min color score 
      saveAsBest();  
      b = 0; 
      r = 0; // new improvement  
    }
    if (r > revert) {
      revertToBest();
      r=0;
    } 
  }//while 

}//sigcolor

/*--main---------------------------------------------------------*/ 
int  main(int argc, char* argv[])
{

  transinput();  // command file/parameters from standard in 
  getgraph();    // input graph  (from file named in command file) 

#ifdef DEBUG  
    printf("the graph:\n"); 
    printgraph(); 
#endif 

    int i;  

    for (i = 0; i < trials; i++) { 
      checkcount=0;
      bestcolors=0;
      bestcolorscore=0; 
      
      // what else to initialize: maxcolor=0; colorcount=0; checkcou
    
      sigcolor() ; 

#ifdef REPORTSTATS 
      printf("i n  m  it  compares maxcolor  colorcount  \n");
      printf("%d  %d %d  %d  %d      %d       %d          \n", 
	     i, numnodes, numedges, trials, 
	     checkcount, bestcolors, bestcolorscore);
#endif 

#ifdef REPORTCOLORS
      printcolors(0);
      printcolors(1);
#endif 
    }//for trials 

    return(0); 
} /* end main */ 

