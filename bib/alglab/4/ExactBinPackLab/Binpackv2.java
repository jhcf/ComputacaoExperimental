import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random; 

// written by ccm 2/2010
// a demo program that generates a random list and
// solves bin packing by branch and bound (no propogation). 
// This version calculates partial costs fresh each level. 

// Version 2: branch and bound, no propogation 
// Usage: java Binpackv2  n  u
// n is number of item to be packed
// u is upper bound (<= 1.0) on items

class Binpackv2 {

    static final int TLIM = -1;  // print first tlim tests to validate 
    static boolean verbose = false; 

    static double tests = 0;  // number of packings evaluated. 
    static double recurs = 0; // number of recursive stages 
  
    static int n; 
    static double u;
    static double[] list; //list of items 
    static double[] optlist; // optimal order of list 

    static double sumwts = 0.0;

    static Random rng; 
    static long seed; 

    static int optcost = Integer.MAX_VALUE;         // best found so far

    //----------swap----------------- 
    static void swap (int a, int b){
	double x = list[a];
        list[a] = list[b];
        list[b] = x;
    }

    //--------binCount---------------------------
    // calculate next fit cost of list[0..k] inclusive
    static int binCount( int k) {
	int count = 1; 
	double curcap = 1.0;  //capacity of open bin 
	tests++;              // number of calls to bincount
	
	for (int i = 0; i <= k ; i++) {
	    // inv: current bin is open, has been counted
	    if (curcap < list[i]) {
		count++;                  // make new bin 
		curcap = 1.0 - list[i];  
	    } else {
		curcap = curcap - list[i]; //put in current bin 
	    }
	}// for
	return count;
    }// bincount 

    //--------binCount----------------------------
    // run through (full) list calculating next fit cost 
    static int binCount () { 
	return binCount( list.length-1 ); 
    }


    // ------weightSum-----------------------------
    // return sum of all weights in list[s=k+1 .. n-1], inclusive 

    static double sumWeights(int s) { 
	double sum = 0.0;
	for (int i=s; i< list.length; i++) 
	    sum += list[i];
	return sum; 
    }

    //---------packBins----------------------
    // k = index of weight list 

    static void packBins (int k) { 
        double s; 
	int b; 
	double c; 
	double bound; 
	int bcount; 

	recurs++;       // count recursive stages 

	if (k == n) { // last item 
	    bcount = binCount(); 
	    if (bcount < optcost) {
		optcost = bcount;  
	        for (int i = 0; i < n; i++ ) optlist[i] = list[i]; 
	    }
	} else {
	    for (int i=k; i < n; i++) { // for all later list items 
		swap (k, i);                // try it 

		b = binCount(k);
		s = sumWeights(k+1); 
		c = 1.0 - list[k] ;   // this is upper bound on real cap 

		bound = b + Math.ceil(s - c); // lower bound on bins 

		if (bound < optcost ) packBins(k+1); // if lb >= opt don't recur

		swap(k, i);                 // restore it
	    }//for
	}//else
    }//binPack

    //-----------------main-----------------
    public static void main(String[] args) {

        int bins = 1;
        double capacity = 1.0;
      
	if (args.length < 2) { 
	    System.out.println("Usage:  java Binpack n u");
            System.exit(0);
	}
	else {
	    n = Integer.valueOf(args[0].trim()).intValue();
	    u = Double.valueOf(args[1].trim()).doubleValue();

	    seed = 876654391; 
            if (args.length == 3) 
		seed = Integer.valueOf(args[2].trim()).intValue(); 

	    if (( u < 0.0) || (u > 1.0) ){
		System.out.println("U must be between 0 and 1. Stop");
		System.exit(0);
	    }
	}

	list = new double[n];
	optlist =  new double[n];

	rng = new Random(seed);  

	for (int i = 0; i < n; i++) {
	    //	    list[i] = Math.random()*u; 
	    list[i] = rng.nextDouble()*u ; 
	    sumwts += list[i];
	} 

	packBins(0);  // start at index 0 

	//System.out.println( "n\t sum \t opt \t recurs \t tested"); 
	System.out.println(n + " " + 
			   sumwts + " \t" + 
			   optcost + " \t" + 
			   recurs + " \t" + 
			   tests  );

	//	System.out.println("List:"); 
        //for (int i=0; i < n; i++) System.out.print(list[i] + "\t" ); 
	//System.out.println(); 

    }//main
}//class
