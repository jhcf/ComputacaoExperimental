import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Random; 

// written by ccm 2/2010
// a demo program that generates a random list and
// solves bin packing by exhaustive search

// First generation simple version  no improvements 
// Usage: java Binpackv1   n  u
// n is number of item to be packed
// u is upper bound (<= 1.0) on items

class Binpackv1 {

    static final int TLIM = -1;  // print first tlim tests to validate 
    static boolean verbose = false; 

    static double tests = 0;  // number of packings evaluated. 
    static double recurs = 0; // number of recursive stages 
  
    static int n; 
    static double u;
    static double[] list; //list of items 
    static double[] optlist; // optimal order of list 

    static Random rng; 
    static long seed; 

    static int optcost = Integer.MAX_VALUE;         // best found so far

    //----------swap----------------- 
    static void swap (int a, int b){
	double x = list[a];
        list[a] = list[b];
        list[b] = x;
    }

    //--------binCount----------------------------
    // run through (global) list calculating next fit cost 


    static int binCount () { 
	int count = 1; 

	double curcap = 1.0; 
	tests++;  // count total number evaluated 

	if (tests <  TLIM ) System.out.print("try: "); 
	for (int i = 0 ; i < list.length; i++) { 
	    // inv: current bin is open, has been counted 
 
	    if (curcap < list[i]) {              // make a new bin  
		count++;
		curcap = 1.0 - list[i];
		if (tests < TLIM) System.out.print("/"); 
	    } 
	    else curcap = curcap - list[i];      // put in current bin 

	    if (tests < TLIM ) System.out.print(list[i] + " ");

	}//for 
	    
	if (tests < TLIM) System.out.println(); 
	return count;  
    }

    //---------packBins----------------------
    // k = index of weight list 

    static void packBins (int k) { 
        double s; 
	int b; 
	double c; 
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
		swap (k, i);                // swap it
		packBins(k+1);      // recur
		swap(k, i);                 // restore it
	    }//for
	}//else
    }//binPack

    //-----------------main-----------------
    public static void main(String[] args) {
	double sumwts = 0.0;
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
