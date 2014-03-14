/*
 * sttyl.c
 *
 * Author: David Chouinard
 *   Date: March 5 2014
 *   Desc: sttyl prints or changes current terminal settings. It operates on
 *         control chracters and a subset of input, output and local flags.
 *         It is meant as a replacement for the GNU stty utility (but only provides a subset of functionality).
 *  Usage:
 *         sttyl [SETTING]...
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#include "tables.h"


#define CHAR_HIGH_ORDER_BIT (1 << 7)  /* High-order bit of an 8-bit char */


/* prototypes */
void print_settings(struct termios *ttyp);
void print_baud(int speed);
void print_characters(cc_t cc[]);
void print_flags(struct termios *ttyp);
void print_flags_from_table(tcflag_t flag, const struct trecord *table);
void parse_args(struct termios *ttyp, int argc, char* argv[]);
void argument_error(char *message, char *argument);
bool is_valid_flag_name(char *flag_name);
void update_flag(struct termios *ttyp, char *flag_name, bool enable_flag);
tcflag_t update_flag_struct(tcflag_t flag, char *flag_name, bool enable_flag,
        const struct trecord *table);
void save_tty(struct termios *ttyp);


int main(int argc, char* argv[]) {
    struct termios ttyp;   // working tty termios struct

    if (tcgetattr(0 , &ttyp) == -1) {
        perror("Unable to retrieve terminal settings");
        exit(EXIT_FAILURE);
    }

    if (argc <= 1)
        print_settings(&ttyp); // user just wants to see current setting status

    // Parse args and call appropriate functions for processing
    parse_args(&ttyp, argc, argv);

    // Update settings and verify the save
    save_tty(&ttyp);
}

/*
 * Prints relevant terminal settings in human-readable format
 *
 * Arguments:
 *    struct termios *ttyp: termios structure of the relevant terminal
 */
void print_settings(struct termios *ttyp) {
    print_baud(cfgetospeed(ttyp));
    putchar('\n');

    print_characters(ttyp->c_cc);
    putchar('\n');

    print_flags(ttyp);
    putchar('\n');

    exit(EXIT_SUCCESS);
}

/*
 * Prints the baud rate in human-readable format given a speed from cfgetospeed
 *
 * Arguments:
 *    int speed: speed given by cfgetospeed and the termios structure
 */
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

/*
 * Prints a subset of relevant control characters from the termios structure.
 * Like stty, prepends M- to non-ASCII characters and prints <undef> for
 * undefined control characters.
 *
 * Arguments:
 *    cc_t cc[]: control character array from the termios structure
 */
void print_characters(cc_t cc[]) {
    cc_t ch;   // current working character

    for (int i=0; control_characters[i].name != NULL; i++) {
        ch = cc[control_characters[i].value];

        printf("%s = ", control_characters[i].name);

        if (ch == _POSIX_VDISABLE) {
            printf("<undef>");  // control character is disabled
        } else {
            if (!isascii(ch)) {
                ch &= ~CHAR_HIGH_ORDER_BIT; // get the ascii part of the character
                printf("M-");
            }
            if (iscntrl(ch)) {
                printf("^");
                ch ^= ('A' - 1);  // make suitable for printing
            }

            putchar(ch);
        }

        printf("; ");
    }
}

/*
 * Prints a susbset of relevant input, output and local modes/flags (each on one
 * line) in human-readable format.
 *
 * Arguments:
 *    struct termios *ttyp: termios structure of the relevant terminal
 */
void print_flags(struct termios *ttyp) {
    print_flags_from_table(ttyp->c_iflag, input_flags);
    putchar('\n');
    print_flags_from_table(ttyp->c_oflag, output_flags);
    putchar('\n');
    print_flags_from_table(ttyp->c_lflag, local_flags);
}

/*
 * Prints flags in human-readable format given a flag table and flags from the
 * termios structure
 *
 * Arguments:
 *    tcflag_t flag: flag element from the termios structure
 *    const struct trecord table[]: relevant flag table for this tcflag_t
 */
void print_flags_from_table(tcflag_t flag, const struct trecord *table) {
    for (int i=0; table[i].name != NULL; i++) {
        printf("%s%s; ", (flag & table[i].value) ? "" : "-", table[i].name);
    }
}

/*
 * Parses command-line arguments, printing validation errors to stderr and
 * calling the appropriate processing functions on the arguments
 *
 * Arguments:
 *    struct termios *ttyp: termios structure of the relevant terminal
 *    int argc: argument count, to be passed from main
 *    char* argv[]: argument array, to be passed from main
 */
void parse_args(struct termios *ttyp, int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        cc_t cc_i = '\0';  // index of control character to operate on

        if (strcmp(argv[i], "erase") == 0)
            cc_i = VERASE;
        else if (strcmp(argv[i], "kill") == 0)
            cc_i = VKILL;

        if (cc_i != '\0') {
            if (i + 1 >= argc)
                argument_error("missing argument to `%s'", argv[i]);
            if (strlen(argv[i + 1]) > 1)
                argument_error("invalid integer argument `%s'", argv[i + 1]);

            ttyp->c_cc[cc_i] = argv[++i][0];
        } else {
            bool enable_flag = (argv[i][0] != '-');  // false to unset flag

            // If flag is disabled, the flag name starts at the second char
            char *flag_name = (enable_flag) ? argv[i] : (argv[i] + 1);

            if (!is_valid_flag_name(flag_name))
                argument_error("invalid argument `%s'", argv[i]);

            update_flag(ttyp, flag_name, enable_flag);
        }
    }
}

/*
 * Prints an argument parsing error message to stderr and exits
 *
 * Arguments:
 *    char *message: error message with a single %s token
 *    char *argument: faulty argument (will replace %s in message)
 */
void argument_error(char *message, char *argument) {
    fputs("sttyl: ", stderr);
    fprintf(stderr, message, argument);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

/*
 * Validates that the passed flag name is known
 *
 * Arguments:
 *    char *flag_name: flag name to validate
 */
bool is_valid_flag_name(char *flag_name) {
    return (get_flag_mask(flag_name, input_flags).name != NULL ||
            get_flag_mask(flag_name, output_flags).name != NULL ||
            get_flag_mask(flag_name, local_flags).name != NULL);
}

/*
 * Updates the passed flag as specified from the appropriate termios field
 *
 * Arguments:
 *    struct termios *ttyp: termios structure of the relevant terminal
 *    char *flag_name: flag name to update
 *    char *enable_flag: if true, enable that flag, else disable it
 */
void update_flag(struct termios *ttyp, char *flag_name, bool enable_flag) {
    ttyp->c_iflag = update_flag_struct(ttyp->c_iflag, flag_name, enable_flag,
            input_flags);
    ttyp->c_oflag = update_flag_struct(ttyp->c_oflag, flag_name, enable_flag,
            output_flags);
    ttyp->c_lflag = update_flag_struct(ttyp->c_lflag, flag_name, enable_flag,
            local_flags);
}

/*
 * Returns an updated flag using the passed flag table (and returns the
 * unmodified input flag if not present in the flag table)
 *
 * Arguments:
 *    tcflag_t flag: flag element from the termios structure
 *    char *flag_name: flag name to update
 *    char *enable_flag: if true, enable that flag, else disable it
 *    const struct trecord *table: relevant flag table for this tcflag_t
 */
tcflag_t update_flag_struct(tcflag_t flag, char *flag_name, bool enable_flag,
        const struct trecord *table) {
    struct trecord record = get_flag_mask(flag_name, table);

    if (record.name == NULL)
        return flag;  // we don't know about that, nothing to do here

    if (enable_flag)
        return (flag |= record.value);   // turn on masked bit
    else
        return (flag &= ~record.value);  // turn off masked bit
}

/*
 * Saves the passed termios structure and verify that the save was successful
 * (prints to stderr and exits if it wasn't)
 *
 * Arguments:
 *    struct termios *ttyp: termios structure to save
 */
void save_tty(struct termios *ttyp) {
    if (tcsetattr(0, TCSANOW, ttyp) == -1) {
        perror("Unable to update terminal settings");
        exit(EXIT_FAILURE);
    }

    // Thoroughly verify that the changes were written. From the man page:
    //
    //   Note that tcsetattr() returns success if any of the requested changes
    //   could be successfully carried out. Therefore, when making multiple
    //   changes it may be necessary to follow this call with a further call to
    //   tcgetattr() to check that all changes have been performed successfully.
    struct termios ttyp_current;

    if (tcgetattr(0, &ttyp_current) == -1) {
        perror("Unable to retrieve terminal settings for write verification");
        exit(EXIT_FAILURE);
    }

    // TODO: compare array
    if (ttyp->c_iflag != ttyp_current.c_iflag ||
            ttyp->c_oflag != ttyp_current.c_oflag ||
            ttyp->c_lflag != ttyp_current.c_lflag ||
            memcmp(ttyp->c_cc, ttyp_current.c_cc, sizeof(ttyp->c_cc))) {
        fputs("Some (but not all) terminal setting changes were not correctly saved", stderr);
        exit(EXIT_FAILURE);
    }
}
