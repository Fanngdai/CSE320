
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gradedb.h"
#include "sort.h"
#include "error.h"

/*
 * Sort the Class and Section Rosters
 */

Student *getnext(Student *s) {
    return(s->next);
}

Student *getcnext(Student *s) {
    return(s->cnext);
}

void setnext(Student *s, Student *n) {
    s->next = n;
}

void setcnext(Student *s, Student *n) {
    s->cnext = n;
}

void sortrosters(c, compare)
Course *c;
int compare();
{
        Section *s;
        c->roster = sortroster(c->roster, getcnext, setcnext, compare);
        for(s = c->sections; s != NULL; s = s->next)
                s->roster = sortroster(s->roster, getnext, setnext, compare);
}

Student *sortroster(s, gtnxt, stnxt, compare)
Student *s;
Student *gtnxt();
void stnxt();
int compare();
{
        int count = 0, i = 0;
        Student *sp = s, **stab = NULL;

        while(sp != NULL) {     /* Count students */
                count++;
                sp = gtnxt(sp);
        }
        if(count == 0) return(NULL);
        if((stab = (Student **)malloc(count*sizeof(Student))) == NULL)
                warning("Not enough memory to perform sorting.");
        sp = s;
        i = count;
        while(i--) {            /* Put students in table */
                stab[i] = sp;
                sp = gtnxt(sp);
        }
        sorttable(stab, 0, count, compare);
        sp = stab[0];
        for(i = 1; i < count; i++) {    /* Relink students in order */
                stnxt(sp, stab[i]);
                sp = stab[i];
        }
        stnxt(sp, NULL);
        sp = stab[0];

        // YASSS
        if(stab!=NULL)
            free(stab);
        stab = NULL;

        return(sp);
}

/*
 * Quicksort algorithm for sorting table of students.
 */

void sorttable(stab, low, high, compare)
Student **stab;
int low, high;
int compare();
{
        int middle = 0;
        if(low == high)
            return;
        middle = partition(stab, low, high, compare);
        if(low+1 == high)
            return;
        sorttable(stab, low, middle, compare);
        sorttable(stab, middle, high, compare);
}

int partition(stab, low, high, compare)
Student **stab;
int low, high;
int compare();
{
        int l = low, h = high, c = 0, alt = 1;
        Student *temp = NULL, *pivot = stab[(low+high)/2];
        while(l < h) {
                c = compare(stab[l], pivot);
                if(c == 0) {
                        c = alt;
                        alt = -alt;
                }
                if(c > 0) { /* put on high end */
                        temp = stab[h-1];
                        stab[h-1] = stab[l];
                        stab[l] = temp;
                        h--;
                } else {        /* put on low end */
                        l++;
                }
        }
        return(l);
}

int comparename(Student *s1, Student *s2) {
        int c = 0;
        c = strcmp(s1->surname, s2->surname);
        if(c != 0) return(c);
        else return(strcmp(s1->name, s2->name));
}

int compareid(Student *s1, Student *s2) {
        // int c = 0;
        return(strcmp(s1->id, s2->id));
}

int comparescore(s1, s2)
Student *s1, *s2;
{
        if(s1->composite > s2->composite) return(-1);
        else if(s1->composite < s2->composite) return(1);
        else return(0);
}

void checkfordups(Student *sp) {
    while(sp != NULL && sp->cnext != NULL) {
        if(!comparename(sp, sp->cnext))
            warning("Duplicate entry for student: %s, %s.",
                sp->surname, sp->name);
        sp = sp->cnext;
    }
}
