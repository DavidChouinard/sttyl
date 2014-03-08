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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>

#include "tables.h"

/* prototypes */
void print_settings(struct termios *ttyp);
void print_baud(int speed);
void print_characters(cc_t cc[]);
void print_all_flags(struct termios *ttyp);
void print_flag(int flag, const struct trecord table[]);
void parse_args(struct termios *ttyp, int argc, char* argv[]);
tcflag_t update_flag(tcflag_t flag, char *flag_name, bool enable_flag, const struct trecord *table);


int main(int argc, char* argv[]) {
    struct termios ttyp;   // working tty termios struct

    if (tcgetattr(0 , &ttyp) == -1) {
        perror("Unable to retrieve terminal settings");
        exit(1);
    }

    if (argc <= 1)
        print_settings(&ttyp);

    parse_args(&ttyp, argc, argv);

    // Update settings
    if (tcsetattr(0, TCSANOW, &ttyp) == -1) {
        perror("Unable to update terminal settings");
        exit(1);
    }
}

void print_settings(struct termios *ttyp) {
    print_baud(cfgetospeed(ttyp));

    print_characters(ttyp->c_cc);
    puts("");

    print_all_flags(ttyp);
    puts("");

    exit(0);
}

void print_baud(int speed) {
    char *baud = NULL;

    for (int i=0; baud_rates[i].name != NULL; i++) {
        if (speed == baud_rates[i].value)
            baud = baud_rates[i].name;
    }

    if (baud == NULL)
        baud = "unrecognized";

    printf("speed %s baud; ", baud);
}

void print_characters(cc_t cc[]) {
    for (int i=0; control_characters[i].name != NULL; i++) {
        printf("%s = ^%c; ", control_characters[i].name,
                cc[control_characters[i].value] - 1 + 'A');
    } // TODO: right way to show?
}

void print_all_flags(struct termios *ttyp) {
    print_flag(ttyp->c_iflag, input_flags);
    print_flag(ttyp->c_lflag, local_flags);
    print_flag(ttyp->c_cflag, control_flags);
}

void print_flag(int flag, const struct trecord table[]) {
    for (int i=0; table[i].name != NULL; i++) {
        printf("%s%s; ", (flag & table[i].value) ? "" : "-", table[i].name);
    }
}

void parse_args(struct termios *ttyp, int argc, char* argv[]) {
    char *flag_name;
    bool enable_flag;    // true if we want to set the flag, false to unset

    // TODO: argument validation
    for (int i = 1; i < argc; ++i) {
        enable_flag = (argv[i][0] != '-');

        // If to flag is to be disabled, the actual flag name starts at the
        // second character
        flag_name = (enable_flag) ? argv[i] : (argv[i] + 1);

        /*tcflag_t c_iflag;      [> input modes <]*/
           /*tcflag_t c_oflag;      [> output modes <]*/
           /*tcflag_t c_cflag;      [> control modes <]*/

        ttyp->c_iflag = update_flag(ttyp->c_iflag, flag_name, enable_flag, input_flags);
        ttyp->c_lflag = update_flag(ttyp->c_lflag, flag_name, enable_flag, local_flags);
    }
}

tcflag_t update_flag(tcflag_t flag, char *flag_name, bool enable_flag, const struct trecord *table) {
    int mask = get_flag_mask(flag_name, table);

    if (mask == -1)
        return flag;  // we don't know about that floag, nothing to do here

    if (enable_flag)
        return (flag |= mask);   // turn on masked bit
    else
        return (flag &= ~mask);  // turn off masked bit
}

