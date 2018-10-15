/*
 * Type definitions for memory allocation functions
 */

Professor *newprofessor();
Assistant *newassistant();
Student *newstudent();
Section *newsection();
Assignment *newassignment();
Course *newcourse();
Score *newscore();
char *newstring();

Freqs *newfreqs();
Classstats *newclassstats();
Sectionstats *newsectionstats();
Stats *newstats();
Ifile *newifile();

// Moved to allocate.c
// char *memerr = "Unable to allocate memory.";