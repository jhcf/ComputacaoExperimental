/* Copyright (C) 1999 Lucent Technologies */
/* From 'Programming Pearls' by Jon Bentley */

/* markov.c -- generate random text from input document
	Usage: markov k m  <text.in  >text.out 

        k: number of words per phrase
        m: number of words to print  

	History
	  Original from 'Programming Pearls' by Jon Bentley 
	  Modified 6 May 2005 by ccm 
		Change k and m from compile time to command line inputs 
		Change output from one word per line to many per line 
	  Modified 15 August 2005 ccm 
	         Add srand to initialize rng with system time 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char inputchars[4500000];
char *word[800000];
int nword = 0, linelen = 0, k;

/* word comparison */ 
int wordncmp(char *p, char* q)
{	int n = k;
	for ( ; *p == *q; p++, q++)
		if (*p == 0 && --n == 0)
			return 0;
	return *p - *q;
}

/* called by system qsort */ 
int sortcmp(char **p, char **q)
{	return wordncmp(*p, *q);
}

/* skip over words in text */ 
char *skip(char *p, int n)
{	for ( ; n > 0; p++)
		if (*p == 0)
			n--;
	return p;
}

/* print a word */ 
void writeword(char *s)
{	int len = strlen(s);
	if (linelen + len > 70) {
		printf("\n"); 
		linelen = 0;
	} else {
		printf(" ");
	}
	linelen += len + 1;
	printf("%s", s);
}

int main(int argc, char* argv[])
{	int i, wordsleft, l, m, u;
	char *phrase, *p;
	
	k = atoi(argv[1]);
	wordsleft = atoi(argv[2]);
	
	srand((unsigned) time(0));  /*comment this out when debugging*/ 
	
	word[0] = inputchars;
	while (scanf("%s", word[nword]) != EOF) {
	  if(nword <= 50) printf("%s:", word[nword]);

		word[nword+1] = word[nword] + strlen(word[nword]) + 1;
		nword++;
	}
	for (i = 0; i < k; i++)
		word[nword][i] = 0;
	for (i = 0; i < k; i++)
		printf("%s\n", word[i]);
	qsort(word, nword, sizeof(word[0]), sortcmp);
	phrase = inputchars;
	for ( ; wordsleft > 0; wordsleft--) {
		l = -1;
		u = nword;
		while (l+1 != u) {
			m = (l + u) / 2;
			if (wordncmp(word[m], phrase) < 0)
				l = m;
			else
				u = m;
		}
		for (i = 0; wordncmp(phrase, word[u+i]) == 0; i++)
			if (rand() % (i+1) == 0)
				p = word[u+i];
		phrase = skip(p, 1);
		if (strlen(skip(phrase, k-1)) == 0)
			break;
		writeword(skip(phrase, k-1));
	}
	return 0;
}


