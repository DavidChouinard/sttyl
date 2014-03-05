/*
 * pfind.c
 *
 * Author: David Chouinard
 *   Date: Feb 28 2014
 *   Desc: pfind recursively traverses the passed directory and prints all
 *         files that fullfil the passed name pattern and file type conditions.
 *         It is meant as a drop-in replacement for the GNU find utility (but
 *         only provides a subset of functionality).
 *  Usage:
 *         pfind starting_dir [-name filename-or-pattern] [-type {f|d|b|c|p|l|s}]
 */

#include  <stdlib.h>
#include  <stdio.h>

/* prototypes */


int main(int argc, char* argv[]) {
    char *dirname = NULL;
    char *findme = NULL;
    char type = '\0';

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {  // Skip argv[0] (program name)
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-name") == 0) {
                if (i + 1 <= argc - 1) {
                    i++;
                    findme = argv[i];
                } else usage_error("missing argument to `%s'", argv[i]);
            } else if (strcmp(argv[i], "-type") == 0) {
                if (i + 1 <= argc - 1) {
                    i++;
                    if (validate_type(argv[i]))
                        type = argv[i][0];
                    else
                        usage_error("invalid argument `%s' to `-type'", argv[i]);
                } else usage_error("missing argument to `%s'", argv[i]);
            } else usage_error("invalid predicate `%s'", argv[i]);
        } else {
            if (dirname)
                print_usage(argv[0]);
            else
                dirname = argv[i];
        }
    }

    if (!dirname) print_usage(argv[0]);

    // Remove trailing slash from passed directory, if any
    if (dirname[strlen(dirname) - 1] == '/')
        dirname[strlen(dirname) - 1] = '\0';

    searchdir(dirname, findme, type);
}

