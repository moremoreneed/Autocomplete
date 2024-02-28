#include "autocomplete.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int find_pos(term *terms, char *str, int len,int start_pos, int end_pos)
{
    if(start_pos >= end_pos ) {
        return -1;
    }
    else {
        int mid = (start_pos+end_pos)/2;
        int val = strncmp(terms[mid].term, str, len);
        if(val == 0 ) {
            return mid;
        }
        else  if(val <0){
            return find_pos(terms, str,len,  mid + 1, end_pos);
        }
        else if(val > 0)
            return find_pos(terms, str,len, start_pos, mid - 1);
    }
}
int find_not_equ_low_pos(term *terms, char *str, int len, int start_pos, int end_pos)
{
    if(start_pos >= end_pos ) {
        return start_pos;
    }
    else {
        int mid = (start_pos+end_pos)/2;
        int val = strncmp(terms[mid].term, str, len);
        if( val == 0)  {
            return find_not_equ_low_pos(terms, str, len, start_pos, mid);
        }
        else  {
            return find_not_equ_low_pos(terms, str, len, mid + 1 , end_pos);
        }
	}
}
 int find_not_equ_high_pos(term *terms, char *str, int len, int start_pos, int end_pos)
{
    if(start_pos >= end_pos ) {
        return start_pos;
    }
    else {
        int mid = (start_pos+end_pos+1)/2;
        int val = strncmp(terms[mid].term, str, len);
        if( val == 0)  {
            return find_not_equ_high_pos(terms, str, len, mid, end_pos);
        }
        else  {
            return find_not_equ_high_pos(terms, str, len, start_pos, mid -1 );
        }
	}
}
int cmp_term(const void *a, const void *b) {
    struct term *c = (term*)a;
    struct term *d = (term*)b;
    return (strcmp(c->term, d->term));
}
 
void read_in_terms(term **terms, int *pnterms, char *filename) {
    FILE *fp = fopen(filename, "r");
    int cnt = 0;
    char str[200];
	fgets(str, 200, fp);
	cnt=  atoi(str);
   
    *terms = (term*)malloc(cnt * sizeof(term));

    char *token;
    cnt = 0;
    //fgets(str, 1000, fp);
    while((fgets(str,200,fp)) != NULL){
        token = strtok(str," \t\n\0" );
        (*terms)[cnt].weight = atof(token);
        token = strtok(NULL,"\n");
        //if(token != NULL) {
            strcpy((*terms)[cnt].term, token);
        //}
        //else {
        //    strcpy((*terms)[cnt].term, " ");
        //}
        //printf("tmp.term is %s\n",tmp.term);
        
        cnt++;
    }
    qsort(*terms, cnt, sizeof(term), cmp_term);
	fclose(fp);
    *pnterms = cnt;	
}
 
 
int lowest_match(term *terms,  int nterms, char *substr)
{
    int len = strlen(substr);
    int pos = find_pos(terms, substr, len, 0, nterms);
	int low_pos;
    if(pos > 0) {
		low_pos = find_not_equ_low_pos(terms, substr, len, 0, pos);
    }
    else{
        return -1;
    }
 
    return low_pos;
}
 
int highest_match(term *terms,  int nterms, char *substr)
{
    int len = strlen(substr);
    int pos = find_pos(terms, substr, len, 0, nterms);
	int high_pos;
    if(pos >0) {
		high_pos = find_not_equ_high_pos(terms, substr, len, pos, nterms-1);
    }
    else
        return -1;
    return high_pos;
 
}
 
int cmp(const void *a, const void *b) {
    struct term *c = (term*)a;
    struct term *d = (term*)b;
    return ((d->weight - c->weight) > 0.0);
}
void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr)
{
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);
    if((low >= 0) && (high < nterms)) {
        *answer = (term*)malloc((high-low +1) * sizeof(term));
        for(int i = low; i<= high; i++) {
            (*answer)[i-low] = terms[i];
            //printf("weight:%d\n", terms[i].weight);
        }
    }
    //printf("%s  low is %d, high is %d\n", substr, low, high);
    if((low >=0) &&(high>=0)) {
        qsort(*answer, (high-low+1), sizeof(term), cmp);
        *n_answer = high - low + 1 ;
		/*
        char name[100];
        strcpy(name, substr);
        strcat(name,"_after_sort.txt");
        FILE *fp;
        fp = fopen(name, "w");
        for(int i = 0;i<*n_answer;i++) {
            fprintf(fp, "%s ;  %.2f\n", (*answer)[i].term, (*answer)[i].weight);
        }
        fclose(fp);
		*/
    }
    else {
        *answer = (term*)malloc(1*sizeof(term));
        *n_answer = 0;
    }
}

/*

int main(void)
{
    struct term *terms;
    int nterms;
	char name[100];
	strcpy(name,"cities.txt");
    read_in_terms(&terms, &nterms, name);
	strcpy(name,"Tor");
    int low = lowest_match(terms, nterms, name);
    int high = highest_match(terms, nterms, name);
    printf("low is %d, high is %d\n", low, high);

    struct term *answer;
    int n_answer;
    autocomplete(&answer, &n_answer, terms, nterms, name);
    printf("Term %s\n", answer[0].term);
    //free allocated blocks here -- not required for the project, but good practice
    return 0;
}
*/
