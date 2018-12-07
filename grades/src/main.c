
/*
 * short options should not rely on the option_info
 * still reachable mem leaks
 * stat = stats?
 *
 * # works now
 *    -r does not work
 *    2nd flag seg faults when the value is not correct
 *    Finish output
 *    read.c tokenbuff is hardcoded
 *
 * Flags can only be called once? Nah he said no need to fix any issues other than what was specified
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "version.h"
#include "global.h"
#include "gradedb.h"
#include "stats.h"
#include "read.h"
#include "write.h"
#include "normal.h"
#include "sort.h"
#include "error.h"
#include "report.h"

/*
 * Course grade computation program
 */

#define REPORT          0
#define COLLATE         1
#define FREQUENCIES     2
#define QUANTILES       3
#define SUMMARIES       4
#define MOMENTS         5
#define COMPOSITES      6
#define INDIVIDUALS     7
#define HISTOGRAMS      8
#define TABSEP          9
#define ALLOUTPUT      10
#define SORTBY         11
#define NONAMES        12
#define OUTPUT         13
#define INVALIDFLAG    14

static struct option_info {
    unsigned int val;
    char *name;
    char chr;
    int has_arg;
    char *argname;
    char *descr;
} option_table[] = {
 {REPORT,       "report",    'r',      no_argument, NULL,
                "Process input data and produce specified reports."},
 {COLLATE,      "collate",   'c',      no_argument, NULL,
                "Collate input data and dump to standard output."},
 {FREQUENCIES,  "freqs",     0,        no_argument, NULL,
                "Print frequency tables."},
 {QUANTILES,    "quants",    0,        no_argument, NULL,
                "Print quantile information."},
 {SUMMARIES,    "summaries", 0,        no_argument, NULL,
                "Print quantile summaries."},
 {MOMENTS,      "stats",     0,        no_argument, NULL,
                "Print means and standard deviations."},
 {COMPOSITES,   "comps",     0,        no_argument, NULL,
                "Print students' composite scores."},
 {INDIVIDUALS,  "indivs",    0,        no_argument, NULL,
                "Print students' individual scores."},
 {HISTOGRAMS,   "histos",    0,        no_argument, NULL,
                "Print histograms of assignment scores."},
 {TABSEP,       "tabsep",    0,        no_argument, NULL,
                "Print tab-separated table of student scores."},
 {ALLOUTPUT,    "all",       'a',      no_argument, NULL,
                "Print all reports."},
 {SORTBY,       "sortby",    'k',      required_argument, "key",
                "Sort by {name, id, score}."},
 {NONAMES,      "nonames",   'n',      no_argument, NULL,
                "Suppress printing of students' names."},
 {OUTPUT,       "output",    'o',      required_argument, "outfile",
                "Write output to file, rather than standard output."},
 {INVALIDFLAG,       0,           0,  no_argument, NULL,
                "Invalid flags." }
};

#define NUM_OPTIONS (15)

// : means needs required argument
static char *short_options = "rcank:o:";
static struct option long_options[NUM_OPTIONS];

static void init_options() {
    for(unsigned int i = 0; i < NUM_OPTIONS; i++) {
        struct option_info *oip = &option_table[i];
        struct option *op = &long_options[i];
        op->name = oip->name;
        op->has_arg = oip->has_arg;
        op->flag = NULL;
        op->val = oip->val;
    }
}

static int report=0, collate=0, freqs=0, quantiles=0, summaries=0, moments=0,
    scores=0, composite=0, histograms=0, tabsep=0, nonames=0,
    all = 0, sortby = 0;
char* output_file;

static void usage();
void duplicatevalue(int value, int name);
// void incorrectvalue(char *str1, char *str2);

int main(int argc, char *argv[]) {
    Course *c = NULL;
    Stats *s = NULL;
    output_file = NULL;
    extern int errors, warnings;
    char optval = 0;

    int (*compare)() = comparename;

    fprintf(stderr, BANNER);
    init_options();
    if(argc <= 1)
        usage(argv[0]);

    int argcounter = 0;

    while(optind < argc) {
        ++argcounter;
        if((optval = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
            switch(optval) {
            case REPORT:
                // incorrectvalue(option_table[REPORT].name, argv[argcounter]);
            case 'r':
                duplicatevalue(report, REPORT);
                report++;
                break;
            case COLLATE:
                // incorrectvalue(option_table[COLLATE].name, argv[argcounter]);
            case 'c':
                duplicatevalue(collate, COLLATE);
                collate++;
                break;
            case TABSEP:
                // incorrectvalue(option_table[TABSEP].name, argv[argcounter]);
                duplicatevalue(tabsep, TABSEP);
                tabsep++;
                break;
            case NONAMES:
                // incorrectvalue(option_table[NONAMES].name, argv[argcounter]);
            case 'n':
                duplicatevalue(nonames, NONAMES);
                nonames++;
                break;
            case SORTBY:
                // incorrectvalue(option_table[SORTBY].name, argv[argcounter]);
            case 'k':
                duplicatevalue(sortby, SORTBY);
                sortby++;

                if(!strcmp(optarg, "name")) {
                    compare = comparename;
                } else if(!strcmp(optarg, "id")) {
                    compare = compareid;
                } else if(!strcmp(optarg, "score")) {
                    compare = comparescore;
                } else {
                    char *temp;
                    if(optval==SORTBY) {
                        temp = option_table[SORTBY].name;
                    } else {
                        temp = "k";
                    }
                    fprintf(stderr, "Option '%s' requires argument from {name, id, score}.\n\n", temp);
                    usage(argv[0]);
                }
                break;
            case FREQUENCIES:
                // incorrectvalue(option_table[FREQUENCIES].name, argv[argcounter]);
                duplicatevalue(freqs, FREQUENCIES);
                freqs++;
                break;
            case QUANTILES:
                // incorrectvalue(option_table[QUANTILES].name, argv[argcounter]);
                duplicatevalue(quantiles, QUANTILES);
                quantiles++;
                break;
            case SUMMARIES:
                // incorrectvalue(option_table[SUMMARIES].name, argv[argcounter]);
                duplicatevalue(summaries, SUMMARIES);
                summaries++;
                break;
            case MOMENTS:
                // incorrectvalue(option_table[MOMENTS].name, argv[argcounter]);
                duplicatevalue(moments, MOMENTS);
                moments++;
                break;
            case COMPOSITES:
                // incorrectvalue(option_table[COMPOSITES].name, argv[argcounter]);
                duplicatevalue(composite, COMPOSITES);
                composite++;
                break;
            case INDIVIDUALS:
                // incorrectvalue(option_table[INDIVIDUALS].name, argv[argcounter]);
                duplicatevalue(scores, INDIVIDUALS);
                scores++;
                break;
            case HISTOGRAMS:
                // incorrectvalue(option_table[HISTOGRAMS].name, argv[argcounter]);
                duplicatevalue(histograms, HISTOGRAMS);
                histograms++;
                break;
            case ALLOUTPUT:
                // incorrectvalue(option_table[ALLOUTPUT].name, argv[argcounter]);
            case 'a':
                duplicatevalue(all, ALLOUTPUT);
                // Not used other than checking
                all++;
                break;
            case OUTPUT:
                // incorrectvalue(option_table[OUTPUT].name, argv[argcounter]);
            case 'o':
                // Make sure -o was called once
                if(output_file!=NULL) {
                    fprintf(stderr, "%s duplicated\n", option_table[OUTPUT].name);
                    exit(EXIT_FAILURE);
                }else if(optarg!=NULL) {
                    output_file = optarg;
                } else {
                    fprintf(stderr, "Option '%s' requires files.\n\n", option_table[OUTPUT].name);
                    usage(argv[0]);
                }
                break;
            // case ':':           // missing argument and the first character of optstring was a colon
            //     printf("@@@@@@@@@@@@@\n");
            //     break;
            case '?':           // value is not what we want it to be
                usage(argv[0]);
                break;
            default:
                break;
            }
        } else {
            break;
        }
    }

    if(all) {
        freqs++;
        quantiles++;
        summaries++;
        moments++;
        composite++;
        scores++;
        histograms++;
        tabsep++;
    }

    if(optind == argc) {
        fprintf(stderr, "No input file specified.\n\n");
        usage(argv[0]);
    }

    char *ifile = argv[optind];
    if(report == collate) {
        fprintf(stderr, "Exactly one of '%s' or '%s' is required.\n\n",
            option_table[REPORT].name, option_table[COLLATE].name);
        usage(argv[0]);
    }

    // Modified added if statement
    if(strcmp(*(argv+1), "--report") && strcmp(*(argv+1), "--collate")
        && strcmp(*(argv+1), "-r") && strcmp(*(argv+1), "-c")) {
        fprintf(stderr, "'%s' or '%s' must appear first.\n\n",
            option_table[REPORT].name, option_table[COLLATE].name);
        usage(argv[0]);
    }

    fprintf(stderr, "Reading input data...\n");
    c = readfile(ifile);
    if(errors) {
        printf("%d error%s found, so no computations were performed.\n",
            errors, errors == 1 ? " was": "s were");
        exit(EXIT_FAILURE);
    }

    FILE * fp;
    if(output_file!=NULL) {
        fp = fopen(output_file,"w");
    } else {
        fp = stdout;
    }

    fprintf(stderr, "Calculating statistics...\n");
    s = statistics(c);
    if(s == NULL)
        fatal("There is no data from which to generate reports.");
    // modified
    normalize(c/*, s*/);
    composites(c);
    sortrosters(c, comparename);
    checkfordups(c->roster);
    if(collate) {
        fprintf(stderr, "Dumping collated data...\n");
        writecourse(fp, c);
        exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
    }
    sortrosters(c, compare);

    fprintf(stderr, "Producing reports...\n");
    reportparams(fp, ifile, c);
    if(moments)
        reportmoments(fp, s);
    if(composite)
        reportcomposites(fp, c, nonames);
    if(freqs)
        reportfreqs(fp, s);
    if(quantiles)
        reportquantiles(fp, s);
    if(summaries)
        reportquantilesummaries(fp, s);
    if(histograms)
        reporthistos(fp, c, s);
    if(scores)
        reportscores(fp, c, nonames);
    if(tabsep)
        reporttabs(fp, c/*, nonames*/);

    fprintf(stderr, "\nProcessing complete.\n");
    printf("%d warning%s issued.\n", warnings+errors,
        warnings+errors == 1? " was": "s were");

    fclose(fp);


    // Stats
    if(s != NULL)
        free(s);
    s = NULL;
    // Course
    // free(c->roster = NULL);
    // c->roster = NULL;
    // free(c);
    // c = NULL;

    exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
}

void incorrectvalue(char *str1, char *str2) {
    if(!strcmp(str1, str2)) {
        fprintf(stderr, "Invalid flag %s.\n", str1);
        exit(EXIT_FAILURE);
    }
}

void duplicatevalue(int value, int name) {
    if(value) {
        fprintf(stderr, "%s flag duplicated.\n", option_table[name].name);
        exit(EXIT_FAILURE);
    }
}

void usage(char *name) {
    struct option_info *opt;

    fprintf(stderr, "Usage: %s [options] <data file>\n", name);
    fprintf(stderr, "Valid options are:\n");

    // -a to remove the last null value
    for(unsigned int i = 0; i < NUM_OPTIONS-1; i++) {
        opt = &option_table[i];
        char optchr[5] = {' ', ' ', ' ', ' ', '\0'};
        if(opt->chr)
            sprintf(optchr, "-%c, ", opt->chr);

        char arg[32];
        if(opt->has_arg)
            sprintf(arg, " <%.10s>", opt->argname);
        else
            sprintf(arg, "%.13s", "");
        fprintf(stderr, "\t%s--%-10s%-13s\t%s\n",
            optchr, opt->name, arg, opt->descr);
        opt++;
    }
    exit(EXIT_FAILURE);
}
