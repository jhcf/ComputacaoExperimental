import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random;

// written by ccm 8/2010
// a demo program that generates a random list and
// solves bin packing by branch and bound
// using incremental cost functions 
// Version 4 used branch-bound, propogation, and preprocessing with FFD 

// Usage:  java Binpackv4  n u [seed] 
// n is number of item to be packed
// u is upper bound (<= 1.0) on items
// seed is random seed 

class Binpackv4 {

    static final int TLIM=3; //print first tlim tests to validate 

    static double tests= 0; // report number of packings evaluated
    static double recurs=0; // report number of recursive stages 

    static int n; 
    static double u;
    static double[] list; //of items 
    static double[] optlist; // optimal order 

    static long seed;  
    static Random rng;  
	
    static int optcost = Integer.MAX_VALUE; 

    //----------swap--------------------------
    static void swap (int a, int b){
	double x = list[a];

        list[a] = list[b];
        list[b] = x;
    }

    //---------packBins------------------------------------
    // note list is global 
    // k = index of weight list 
    // sumwts = cumulated sum of weights already packed 
    // bcount = cumulated sum of bins used so far 
    // capacity = how much left in the bin 

    //	binPack(0, sumwts, bins, capacity ); // pack item 0 

    static void packBins(int k, double sumwt, int bcount, double capacity) { 
        double s; 
	int b; 
	double c;

	recurs++;  

	if (k >= n) {  // no more weights; check against optimal 
	    tests++;
	    
	    /*--------------------------
	      System.out.println("opt check this:"); 
		for (int i=0; i < n; i++)
	      
		System.out.print( list [i] + "\t");
		System.out.println(); 
		System.out.println(" k "      + k + "\t" + 
				   "list[k-1] " + list[k-1] + "\t" +
				   "sumwt "    + sumwt + "\t" + 
				   "count "    + bcount + "\t" +
				   "cap   "    + capacity );
				   ----------------*/ 

	    if (bcount < optcost) { 
		//		System.out.println("new opt: " + bcount); 

		optcost = bcount;  
		for (int i=0; i < n; i++) optlist[i] = list[i]; 
		
	    }
	} // recursion stopper 
	    
	else {

	    for (int i=k; i < n; i++) {
		swap (k, i);              //  try it 

		//System.out.println("Try " + k + " " + list[k]) ; 
	 

		if (capacity <  list[k]) { // doesn't fit 
		    b= bcount+1;                 // open new bin 
		    c= 1.0 - list[k];          // new capacity 
		}
                else{
                    b = bcount;                //same bin count 
		    c = capacity - list[k];  //use old bin 

		}

		s = sumwt-list[k];       //  update sumweights 
		// if (b + (s - 1)) >= optcost don't recur

		if ((b + Math.ceil(s - c)) < optcost) {     // check 
		    packBins(k+1, s, b, c);     // recur
		}
	    
		swap(k, i);                     // restore it

	    }//for
	}//else
    }//binPack

    //----------ffd---------------------------
    static int ffd(double sumwt) {
	// sort optlist, then pack with ffd (n^2) 
	// the weights are in list
	// insertion sort ok since n <= 14
	// return number of bins 


	for (int i = 1; i < n; i++){
	    //inv: list[0..i-1] is sorted
	    // insert list[i] in place 
	    double p = optlist[i]; 
            int j; 
	    for (j = i-1; j>=0; j--) {
		// inv: hole is at j+1

		if (optlist[j] <= p) break; 
		else 
		    optlist[j+1] = optlist[j];
	    }
	    optlist[j+1] = p; 
	}//for 

	/*
	System.out.println("Check Sort"); 
	for (int i = 0; i < n; i++) 
	    System.out.print( optlist[i] + "\t"); 
	System.out.println();
	*/

	int bsize = (int) (sumwt*2) + 1; 
	double bins[] = new double[bsize];
	for (int i = 0; i < bsize; i++) bins[i] = 0.0; 

	int lastbin = 0; //open this one 
	
	for (int wix  = 0; wix < n; wix++){ 
	    double wt = optlist[wix]; 
	    // inv: lastbin is last bin open 

            int bix; 
	    for (bix = 0; bix <= lastbin; bix++) {
		if (wt + bins[bix] <= 1.0) {
		    bins[bix] += wt; 
		    break;
		}
	    }//for 
	    if (bix > lastbin) {
		lastbin++;
		bins[lastbin] += wt; 
	    } 

	}// for each weight 

	return lastbin+1;  

    }// ffd 
	
    //-----------------main-----------------
    public static void main(String[] args) {
	double sumwts = 0.0;
        int    bins = 1;
        double capacity = 1.0;
      
	if (args.length < 2) { 
	    System.out.println("Usage:  java Binpack n u seed");

            System.exit(0);
	}
	else {
	    n = Integer.valueOf(args[0].trim()).intValue();
	    u = Double.valueOf(args[1].trim()).doubleValue();

	    seed = 876654391; 
	    if (args.length == 3) 
		seed = Integer.valueOf(args[2].trim()).intValue(); 

	    if (( u <0.0 || u > 1.0)){
		System.out.println("u must be between 0 and 1. Stop.");
		System.exit(0);  
	    }
	}

	list = new double[n];
	optlist= new double[n]; 

	rng = new Random(seed);  

	for (int i = 0; i < n; i++) {
	    // list[i] = Math.random()*u; 
	    list[i] = rng.nextDouble()* u; 
	    sumwts += list[i];

	    optlist[i] = list[i]; // use it for ffd sort 

	    //	    System.out.print( list[i] + " \t"); 
	} 
	//	System.out.println(" sum to: " + sumwts); 

	//---------------ffd packing-----------
	optcost = ffd(sumwts) ; 

	packBins (0, sumwts, bins, capacity ); // pack item 0 

	// System.out.println( "c   n sumweights bins recurs numtested"); 
	System.out.println(n + " " + 
			   sumwts + " " + 
			   optcost + " " +
			   recurs + " " + 
			   tests);

	//	int b = bins; 
	//if (b > 10) b=10; 
       	//for (int i = 0; i < b; i++)System.out.print(list[i] + "\t");
	//System.out.println(); 

    }//main
}//class
