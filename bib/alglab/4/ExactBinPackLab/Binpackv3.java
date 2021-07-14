import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random; 

// written by ccm 2/2010
// a demo program that generates a random list and
// solves bin packing by branch and bound
// using incremental cost functions 
// Version 3 :   branch-and-bound plus propogation

// Usage:  java Binpackv3  n u 
// n is number of item to be packed
// u is upper bound (<= 1.0) on items

class Binpackv3 {

    static final int TLIM=3; //print first tlim tests to validate 

    static double tests= 0; // report number of packings evaluated
    static double recurs=0; // report number of recursive stages 

    static int n; 
    static double u;
    static double[] list; //of items 
    static double[] optlist; // optimal order 

    static Random rng;
    static long seed; 

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

	if (k == n) {  // no more weights; check against optimal 
	    tests++;

	    /*-----------------------------------
		System.out.println("check this:"); 
		for (int i=0; i < n; i++)
		    System.out.print( list [i] + "\t");
		System.out.println(); 
		System.out.println(" k "      + k + "\t" + 
				   "list[n-1] " + list[n-1] + "\t" +
				   "sumwt "    + sumwt + "\t" + 
				   "count "    + bcount + "\t" +
				   "cap   "    + capacity );


	    --------------------------------------*/
	    if (bcount < optcost) { 
		//System.out.println("new old opt: " + bcount + " " + optcost); 

	    optcost = bcount;  
	    for (int i=0; i < n; i++) optlist[i] = list[i]; 
	    }
	} // recursion stopper 
	    
	else {
	    for (int i=k; i < n; i++) {
		swap (k, i);              //  try it 
	 
		if (capacity <  list[k]) { // doesn't fit 
		    b= bcount+1;                 // open new bin 
		    c= 1.0 - list[k];          // new capacity 
		}
                else{
                    b = bcount;                //same bin count 
		    c = capacity - list[k];  //use old bin 

		}
		s = sumwt-list[k];       //  update sumweights 
		// if (b + (s - c) ) >= optcost don't recur

		double bound = b+ Math.ceil(s - c) ;

		if (bound < optcost) {     // check 
		    packBins(k+1, s, b, c);     // recur
		}
	    
		swap(k, i);                     // restore it

	    }//for
	}//else
    }//binPack

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
	    //	    list[i] = Math.random()*u; 
	    list[i] = rng.nextDouble() * u;  
	    sumwts += list[i];

	    //	    System.out.print( list[i] + " \t"); 
	} 
	//	System.out.println(" sum to: " + sumwts); 


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
