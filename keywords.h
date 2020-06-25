#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "symbols.h"

/* Check if is keyword */
int sym2key(const char *id);
/* Transform from number to SYMBOL */
SYMBOL key2num(int num);

#endif /* KEYWORDS_H */
