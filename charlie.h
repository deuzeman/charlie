#ifndef GUARD_CHARLIE_H
#define GUARD_CHARLIE_H

#include <stdio.h>
#include <string.h>

#include <lime.h>

#include "dml.h"

typedef struct
{
  int lx;
  int ly;
  int lz;
  int lt;
  int prec;
}
paramsIldgFormat;

int parse_ildgformat_xml(char *message, paramsIldgFormat *ildgformat);

#endif
