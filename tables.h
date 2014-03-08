#ifndef TABLES_H
#define TABLES_H

struct trecord { int value; char *name; };

extern const struct trecord baud_rates[];
extern const struct trecord control_characters[];
extern const struct trecord input_flags[];
extern const struct trecord local_flags[];
extern const struct trecord control_flags[];

extern int get_flag_mask(char *name, const struct trecord *table);

#endif
