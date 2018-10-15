
/*
 * Allocate storage for the various data structures
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "global.h"
#include "gradedb.h"
#include "stats.h"
#include "allocate.h"
#include "error.h"

char *memerr = "Unable to allocate memory.";

Professor *newprofessor() {
    Professor *p = NULL;
    if((p = (Professor *)malloc(sizeof(Professor))) == NULL)
        fatal(memerr);
    return(p);
}

Assistant *newassistant() {
    Assistant *a = NULL;
    if((a = (Assistant *)malloc(sizeof(Assistant))) == NULL)
        fatal(memerr);
    return(a);
}

Student *newstudent() {
    Student *s = NULL;
    if((s = (Student *)malloc(sizeof(Student))) == NULL)
        fatal(memerr);
    return(s);
}

Section *newsection() {
    Section *s = NULL;
    if((s = (Section *)malloc(sizeof(Section))) == NULL)
        fatal(memerr);
    return(s);
}

Assignment *newassignment() {
    Assignment *a = NULL;
    if((a = (Assignment *)malloc(sizeof(Assignment))) == NULL)
        fatal(memerr);
    return(a);
}

Course *newcourse() {
    Course *c = NULL;
    if((c = (Course *)malloc(sizeof(Course))) == NULL)
        fatal(memerr);
    return(c);
}

Score *newscore() {
    Score *s = NULL;
    if((s = (Score *)malloc(sizeof(Score))) == NULL)
        fatal(memerr);
    return(s);
}

char *newstring(char *tp, int size) {
    char *s, *cp = NULL;
    if((s = (char *)malloc(size)) == NULL)
        fatal(memerr);
    cp = s;
    while(size-- > 0)
        *cp++ = *tp++;
    return(s);
}

Freqs *newfreqs() {
    Freqs *f = NULL;
    if((f = (Freqs *)malloc(sizeof(Freqs))) == NULL)
        fatal(memerr);
    return(f);
}

Classstats *newclassstats() {
    Classstats *c = NULL;
    if((c = (Classstats *)malloc(sizeof(Classstats))) == NULL)
        fatal(memerr);
    return(c);
}

Sectionstats *newsectionstats() {
    Sectionstats *s = NULL;
    if((s = (Sectionstats *)malloc(sizeof(Sectionstats))) == NULL)
        fatal(memerr);
    return(s);
}

Stats *newstats() {
    Stats *s = NULL;
    if((s = (Stats *)malloc(sizeof(Stats))) == NULL)
        fatal(memerr);
    return(s);
}

Ifile *newifile() {
    Ifile *f = NULL;
    if((f = (Ifile *)malloc(sizeof(Ifile))) == NULL)
        fatal(memerr);
    return(f);
}
