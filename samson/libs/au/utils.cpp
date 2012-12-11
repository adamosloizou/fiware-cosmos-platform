/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "au/utils.h"  // Own interface

#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "au/string/StringUtilities.h"


namespace au {
const char *valid_chars = "01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTSUVXYZ";

int getTerminalColumns() {
  int x, y;

  GetTerminalSize(0, &x, &y);
  return x;
}

void ClearTerminalLine() {
  printf("\r");
  for (int i = 0; i < getTerminalColumns(); i++) {
    printf(" ");
  }
  printf("\r");
  fflush(stdout);
}

size_t code64_rand() {
  size_t v;
  char *c = (char *)&v;

  for (size_t i = 0; i < sizeof(size_t) / sizeof(char); i++) {
    c[i] = rand() % strlen(valid_chars);
  }
  return v;
}

bool code64_is_valid(size_t v) {
  char *c = (char *)&v;

  for (size_t i = 0; i < sizeof(size_t) / sizeof(char); i++) {
    if (c[i] >= (int)strlen(valid_chars)) {
      return false;
    }
  }
  return true;
}

std::string code64_str(size_t v) {
  char str[sizeof(size_t) + 1];
  char *c = (char *)&v;

  for (size_t i = 0; i < sizeof(size_t); i++) {
    int p = c[i];
    if (p >= (int)strlen(valid_chars)) {
      str[i] = '?';
    } else {
      str[i] = valid_chars[p];
    }
  }

  str[sizeof(size_t)] = '\0';

  return str;
}

void remove_return_chars(char *line) {
  while (true) {
    size_t l = strlen(line);

    if (l == 0) {
      return;
    }

    if ((line[l - 1] == '\n') || (line[l - 1] == '\r')) {
      line[l - 1] = '\0';
    } else {
      return;
    }
  }
}
}
