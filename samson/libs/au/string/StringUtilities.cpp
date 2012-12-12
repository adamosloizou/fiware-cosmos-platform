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

#include "StringUtilities.h"     // Own definitions

#include <cstdarg>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include <sys/ioctl.h>

#include "au/TemporalBuffer.h"

namespace au {
std::string str_tabs(int t) {
  std::ostringstream output;

  for (int i = 0; i < t; i++) {
    output << "\t";
  }
  return output.str();
}

std::string str_percentage(double p) {
  if (p > 1) {
    return au::str(p);
  }
  return au::str("%05.1f%%", p * 100);
}

std::string str_simple_percentage(double value, double total) {
  if (total == 0) {
    return str_simple_percentage(0);
  } else {
    return str_simple_percentage(value / total);
  }
}

std::string str_simple_percentage(double p) {
  if (p > 1) {
    return au::str(p);
  }
  return au::str("%03.0f%%", p * 100);
}

std::string str_percentage(double value, double total) {
  if (total == 0) {
    return str_percentage(0);
  } else {
    return str_percentage(value / total);
  }
}

std::string str_timestamp(time_t t) {
  struct tm timeinfo;
  char buffer_time[1024];

  localtime_r(&t, &timeinfo);
  strftime(buffer_time, 1024, "%d/%m/%Y (%X)", &timeinfo);

  return std::string(buffer_time);
}

std::string str_time_simple(size_t seconds) {
  int years = seconds / (365 * 24 * 3600);

  seconds -= (365 * 24 * 3600) * years;

  int days = seconds / (24 * 3600);
  seconds -= (24 * 3600) * days;

  int hours = seconds / 3600;
  seconds -= 3600 * hours;

  int minutes = seconds / 60;
  seconds -= 60 * minutes;


  if (years > 0) {
    return au::str("%6dy", years);
  }
  if (days > 100) {
    return au::str("%6dd", days);
  }
  if (days > 0) {
    return au::str("%2dd%2dh", days, hours);
  }
  if (hours > 0) {
    return au::str("%2dh%2dm", hours, minutes);
  }
  if (minutes > 0) {
    return au::str("%2dm%2ds", minutes, seconds);
  }

  return au::str("%6ds", seconds);
}

std::string str_time(size_t seconds) {
  if (seconds > (3600ULL * 24ULL * 265ULL * 100ULL)) {
    return "   >100y";
  }

  int days = 0;
  while (seconds > (3600 * 24)) {
    days++;
    seconds -= (3600 * 24);
  }

  int minutes = seconds / 60;
  seconds -= minutes * 60;

  int hours = minutes / 60;
  minutes -= hours * 60;

  if (days > 0) {
    return au::str("%02dd %02d:%02d", days, hours, minutes);
  }

  return au::str(" %02d:%02d:%02d", hours, minutes, (int)seconds);
}

std::string str_progress_bar(double p, int len) {
  std::ostringstream output;


  if (p < 0) {
    p = 0;
  }
  if (p > 1) {
    p = 1;
  }
  int pos = len * p;

  output << " [ ";
  for (int s = 0; s < pos; s++) {
    output << "*";
  }
  for (int s = pos; s < len; s++) {
    output << ".";
  }
  output << " ] ";

  return output.str();
}

std::string str_progress_bar(double p, int len, char c, char c2) {
  std::ostringstream output;


  if (p < 0) {
    p = 0;
  }
  if (p > 1) {
    p = 1;
  }
  int pos = len * p;

  output << " [ ";
  for (int s = 0; s < pos; s++) {
    output << c;
  }
  for (int s = pos; s < len; s++) {
    output << c2;
  }
  output << " ] ";

  return output.str();
}

std::string str_double_progress_bar(double p1, double p2, char c1, char c2, char c3, int len) {
  std::ostringstream output;


  if (p1 < 0) {
    p1 = 0;
  }
  if (p1 > 1) {
    p1 = 1;
  }
  if (p2 < 0) {
    p2 = 0;
  }
  if (p2 > 1) {
    p2 = 1;
  }
  if (p2 < p1) {
    p2 = p1;        // no sense
  }
  int pos1 = (double)len * p1;
  int pos2 = (double)len * p2;

  output << " [ ";

  for (int s = 0; s < pos1; s++) {
    output << c1;
  }

  for (int s = pos1; s < pos2; s++) {
    output << c2;
  }

  for (int s = pos2; s < len; s++) {
    output << c3;
  }

  output << " ] ";

  return output.str();
}

void FindAndReplaceInString(std::string &source, const std::string& find, const std::string& replace) {
  size_t pos = 0;

  // LOG_SM(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
  pos = source.find(find, pos);
  // LOG_SM(("Position found %lu bytes" , find.length() ));

  while (pos != std::string::npos) {
    source.replace(pos, find.length(), replace);

    // Go forward in the input string
    pos += replace.length();

    // LOG_SM(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
    pos = source.find(find, pos);
    // LOG_SM(("Position found %lu bytes" , find.length() ));
  }
}

std::string str_indent(const std::string& txt) {
  // Copy input string
  std::string copied_txt = txt;

  // Replace all "returns" by "return and tab"
  FindAndReplaceInString(copied_txt, "\n", "\n\t");
  // Insert the first tab
  copied_txt.insert(0, "\t");
  return copied_txt;
}

std::string str_indent(const std::string& txt, int num_spaces) {
  // Copy input string
  std::string copied_txt = txt;

  // Create separator string
  std::string sep;

  for (int i = 0; i < num_spaces; i++) {
    sep.append(" ");
  }

  // Replace all "returns" by "return and tab"
  FindAndReplaceInString(copied_txt, "\n", "\n" + sep);

  // Insert the first tab
  copied_txt.insert(0, sep);
  return copied_txt;
}

std::string str(double value, char letter) {
  if (value < 10) {
    return au::str("%4.2f%c", value, letter);
  } else if (value < 100) {
    return au::str("%4.1f%c", value, letter);
  }

  return au::str("%4.0f%c", value, letter);
}

std::string str(const char *format, ...) {
  va_list args;
  char vmsg[2048];

  /* "Parse" the varible arguments */
  va_start(args, format);

  /* Print message to variable */
  vsnprintf(vmsg, sizeof(vmsg), format, args);
  // vmsg[2047] = 0;
  va_end(args);

  return std::string(vmsg);
}

std::string str_color(int main_color_code, int color_code, const std::string message) {
  return au::str("\033[%d;%dm", main_color_code, color_code) + message + std::string("\033[0m");
}

std::string str(Color color, const std::string& message) {
  switch (color) {
    case Normal: return message;

    case Black: return str_color(0, 30, message);

    case Red: return str_color(0, 31, message);

    case Green: return str_color(0, 32, message);

    case Yellow: return str_color(0, 33, message);

    case Blue: return str_color(0, 34, message);

    case Magenta: return str_color(0, 35, message);

    case Cyan: return str_color(0, 36, message);

    case White: return str_color(0, 37, message);

    case BoldBlack: return str_color(1, 30, message);

    case BoldRed: return str_color(1, 31, message);

    case BoldGreen: return str_color(1, 32, message);

    case BoldYellow: return str_color(1, 33, message);

    case BoldBlue: return str_color(1, 34, message);

    case BoldMagenta: return str_color(1, 35, message);

    case BoldCyan: return str_color(1, 36, message);

    case BoldWhite: return str_color(1, 37, message);
  }

  // Default mode ( just in case )
  return message;
}

std::string str(Color color, const char *format, ...) {
  va_list args;
  char vmsg[2048];

  /* "Parse" the varible arguments */
  va_start(args, format);

  /* Print message to variable */
  vsnprintf(vmsg, sizeof(vmsg), format, args);
  // vmsg[2047] = 0;
  va_end(args);

  // Real message to print
  std::string message = std::string(vmsg);
  return str(color, message);
}

std::string str_double(double value, char letter) {
  if (value < 10) {
    return au::str(" %4.2f%c", value, letter);
  } else if (value < 100) {
    return au::str(" %4.1f%c", value, letter);
  }

  return au::str(" %4.0f%c", value, letter);
}

std::string str(const std::vector<std::string>& hosts) {
  std::ostringstream output;

  for (size_t i = 0; i < hosts.size(); ++i) {
    output << hosts[i];
    if (i != (hosts.size() - 2)) {
      output << " ";
    }
  }
  return output.str();
}

std::string str_grouped(const std::vector<std::string>& names) {
  au::map<std::string, std::vector<std::string> > grouped_names;

  for (size_t i = 0; i < names.size(); i++) {
    std::string category;
    std::string name;
    size_t pos = names[i].find("::");
    if (pos == std::string::npos) {
      category = names[i];
    } else {
      category = names[i].substr(0, pos);
      name = names[i].substr(pos + 2);
    }
    grouped_names.findOrCreate(category)->push_back(name);
  }

  std::ostringstream output;
  au::map<std::string, std::vector<std::string> >::iterator iter;
  for (iter = grouped_names.begin(); iter != grouped_names.end(); ++iter) {
    if (iter->second->size() == 1) {
      std::string name = iter->second->at(0);
      if (name == "") {
        output << iter->first;
      } else {
        output << iter->first << "::" << name;
      }
    } else {
      output << iter->first << "::{ ";

      for (size_t i = 0; i < iter->second->size(); i++) {
        std::string name = iter->second->at(i);
        if (name == "") {
          output << "_ ";
        } else {
          output << name << " ";
        }
      }
      output << "}";
    }
    output << " ";
  }
  return output.str();
}

std::string str(double value) {
  if (value == 0) {
    return "    0 ";
  }

  if (value < 0) {
    std::string tmp = str(-value);
    size_t pos = tmp.find_first_not_of(" ");
    if ((pos == std::string::npos) || (pos == 0)) {
      return "ERROR";
    }

    tmp[pos - 1] = '-';
    return tmp;
  }

  if (value < 0.000000000000001) {
    return "  EPS ";
  } else if (value < 0.000000000001) {
    return au::str_double(value * 1000000000000000.0, 'f');
  } else if (value < 0.000000001) {
    return au::str_double(value * 1000000000000.0, 'p');
  } else if (value < 0.000001) {
    return au::str_double(value * 1000000000.0, 'n');
  } else if (value < 0.001) {
    return au::str_double(value * 1000000.0, 'u');
  } else if (value < 1) {
    return au::str_double(value * 1000.0, 'm');
  } else if (value < 1000) {
    return au::str_double(value, ' ');
  } else if (value < 1000000) {
    return au::str_double(value / 1000.0, 'K');
  } else if (value < 1000000000) {
    return au::str_double(value / 1000000.0, 'M');
  }

#ifdef __LP64__
  else if (value < 1000000000000) {
    return au::str_double(value / 1000000000.0, 'G');
  } else if (value < 1000000000000000) {
    return au::str_double(value / 1000000000000.0, 'T');
  } else if (value < 1000000000000000000) {
    return au::str_double(value / 1000000000000000.0, 'P');
  }

#endif  // __LP64__
  else {
    return "  INF ";
  }
}

std::string str(double value, const std::string& postfix) {
  return str(value) + postfix;
}

std::string str_detail(size_t value) {
  if (value < 1000) {
    return str(value);
  }

  return str("%lu (%s)", value, au::str(value).c_str());
}

std::string str_detail(size_t value, const std::string& postfix) {
  if (value < 1000) {
    return str(value, postfix);
  }

  return str("%lu %s (%s)", value, postfix.c_str(), au::str(value, postfix).c_str());
}

bool IsOneOf(char c, const std::string& s) {
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == c) {
      return true;
    }
  }

  return false;
}

std::vector<std::string> split_using_multiple_separators(const std::string& input, const std::string& separators) {
  std::vector<std::string> components;
  size_t pos = 0;


  while (pos < input.length()) {
    size_t s = input.find_first_of(separators, pos);


    if (s == std::string::npos) {
      components.push_back(input.substr(pos, input.length() - pos));
      break;
    } else {
      components.push_back(input.substr(pos, s - pos));
      pos = s + 1;
    }
  }


  // Return found components
  return components;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;

  while (std::getline(ss, item, delim)) {
    if (item.length() > 0) {
      elems.push_back(item);
    }
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  return split(s, delim, elems);
}

int GetTerminalSize(int fd, int *x, int *y) {
#ifdef TIOCGSIZE
  struct ttysize win;

#elif defined(TIOCGWINSZ)
  struct winsize win;

#endif

#ifdef TIOCGSIZE
  if (ioctl(fd, TIOCGSIZE, &win)) {
    return 0;
  }


  if (y) {
    *y = win.ts_lines;
  }
  if (x) {
    *x = win.ts_cols;
  }

#elif defined TIOCGWINSZ
  if (ioctl(fd, TIOCGWINSZ, &win)) {
    return 0;
  }





  if (y) {
    *y = win.ws_row;
  }
  if (x) {
    *x = win.ws_col;
  }

#else
  {
    const char *s;
    s = getenv("LINES");
    if (s) {
      *y = strtol(s, NULL, 10);
    } else {
      *y = 25;
    }
    s = getenv("COLUMNS");
    if (s) {
      *x = strtol(s, NULL, 10);
    } else {
      *x = 80;
    }
  }
#endif  // ifdef TIOCGSIZE

  return 1;
}

int GetTerminalWidth() {
  int x, y;

  if (GetTerminalSize(0, &x, &y)) {
    return x;
  } else {
    return 0;
  }
}

std::string StringRepeatingCharInConsole(char c) {
  std::ostringstream output;

  for (int i = 0; i < GetTerminalWidth(); i++) {
    output << c;
  }
  return output.str();
}

std::string StringWithMaxLineLength(const std::string& txt, int max_line_length) {
  std::istringstream input_stream(txt);
  std::ostringstream output;

  char line[1024];

  while (input_stream.getline(line, 1023)) {
    int line_length = (int)strlen(line);

    if (line_length > max_line_length) {
      line[max_line_length - 4] = '.';
      line[max_line_length - 3] = '.';
      line[max_line_length - 2] = '.';
      line[max_line_length - 1] = 0;

      // LOG_SM(("Exesive line %d / %d ", line_length , max_line_length ));
    } else {
      // LOG_SM(("Normal line %d / %d", line_length , max_line_length ));
    }

    output << line << "\n";
  }
  return output.str();
}

std::string StringInConsole(const std::string& txt) {
  return StringWithMaxLineLength(txt, GetTerminalWidth());
}

// Backward look up for a sequence
const char *laststrstr(const char *source, const char *target) {
  return laststrstr(source, strlen(source), target);
}

const char *laststrstr(const char *source, size_t source_length, const char *target) {
  const char *sp;

  /* Step backward through the source string - one character at a time */
  for (sp = source + source_length - strlen(target); sp >= source; sp--) {
    const char *tp, *spt;
    /* go forward through the search string and source checking characters
     * Stop the loop if the character are unequal or you reach the end of the target (or string) */
    for (tp = target, spt = sp; (*tp == *spt)  && (*spt != '\0') && (*tp != '\0'); spt++, tp++) {
      ;
    }

    /* if the loop above gets to the end of the target string then it must have matched all the characters */
    if (*tp == '\0') {
      break;
    }
  }
  /* If the outer loop finished before getting to the start of the source string then
   * it must have found a matching sub string */
  return (sp < source) ? NULL : sp;
}

// Similar to strnstr(), but limits the length of the pattern to be searched
// and assumes that text string is null terminated
const char *strnstr_limitpattern(const char *text, const char *pattern, size_t max_length) {
  char cpat, ctxt;
  size_t len;

  // Check for an empty pattern
  if ((cpat = *pattern++) != '\0') {
    len = strlen(text);
    do {
      // Locate the beginning of a possible match
      do {
        if ((len-- < max_length) || (ctxt = *text++) == '\0') {
          return ((const char *)NULL);
        }
      } while (ctxt != cpat);
    } while (strncmp(text, pattern, max_length - 1) != 0);
    text--;
  }
  return (text);
}

// match a string against a simple pattern with wildcard characters meaning
// any sequence of characters
// Could be done with regexp library, but it is too much powerful (and slow)
bool MatchPatterns(const char *inputString, const char *pattern, char wildcard) {
  const char *p_input;
  const char *p_pattern;
  char *p_wildcard;
  const char *p_match;
  bool first_wildcard = false;

  // First, lets skip trivial situations
  if (inputString == NULL) {
    return false;
  }

  if (*inputString == '\0') {
    return false;
  }

  if (pattern == NULL) {
    return false;
  }

  if (*pattern == '\0') {
    return false;
  }

  p_input = inputString;
  p_match = p_input;
  p_pattern = pattern;
  if (p_pattern[0] == wildcard) {
    first_wildcard = true;
  }

  while ((p_wildcard = strchr((char *)p_pattern, wildcard)) != NULL) {
    // First locate the next wildcard
    size_t len_pattern = p_wildcard - p_pattern;

    // and check the current portion of inputString against the fix section in the pattern
    if (first_wildcard == false) {
      if (strncmp(p_input, p_pattern, len_pattern) != 0) {
        return false;
      }
      first_wildcard = true;
    } else
    if (len_pattern > 0) {
      if ((p_match = strnstr_limitpattern(p_input, p_pattern, len_pattern)) == NULL) {
        return false;
      }
    }
    p_input = p_match + len_pattern;
    p_pattern = p_wildcard + 1;
    if (*p_pattern == '\0') {
      return true;
    }
  }

  // processing the last portion of the pattern
  // (or all of it, if there was no wildcard)
  if (first_wildcard == false) {
    if (strcmp(p_input, p_pattern) != 0) {
      return false;
    }
  } else
  if (strlen(p_pattern) > 0) {
    if (((p_match = strstr(p_input, p_pattern)) == NULL) || (*(p_match + strlen(p_pattern))) != '\0') {
      return false;
    }
  }
  return true;
}

std::vector<char *> SplitInWords(char *line, char separator) {
  std::vector<char *> words;
  size_t pos = 0;
  size_t previous = 0;
  bool finish = false;

  while (!finish) {
    if ((line[pos] == separator) || (line[pos] == '\0')) {
      if ((line[pos] == '\0') || (line[pos] == '\n')) {
        finish = true;
      }

      // Artificial termination of string
      line[pos] = '\0';

      // Add the found word
      words.push_back(&line[previous]);

      // Point to the next words
      pos++;
      previous = pos;
    } else {
      pos++;
    }
  }

  return words;
}

int GetCommonChars(const std::string& txt, const std::string& txt2) {
  size_t l = std::min(txt.length(), txt2.length());

  for (size_t i = 0; i < l; i++) {
    if (txt[i] != txt2[i]) {
      return i;
    }
  }
  return l;
}

bool IsCharInRange(char c, char lower, char higher) {
  return ((c >= lower) && (c <= higher));
}

Color GetColor(const std::string color_name) {
  if ((color_name == "red") || (color_name == "r")) {
    return BoldRed;
  }
  if ((color_name == "magenta") || (color_name == "m")) {
    return BoldMagenta;
  }

  return Normal;
}

std::string string_in_color(const std::string& message, const std::string& color_name) {
  Color color = GetColor(color_name);

  return au::str(color, "%s", message.c_str());

  std::ostringstream output;
}

bool CheckIfStringsBeginWith(const std::string& str, const std::string& prefix) {
  return str.substr(0, prefix.length()) == prefix;
}

bool CheckIfStringsEndsWith(const std::string& str, const std::string& postfix) {
  if (postfix.length() > str.length()) {
    return false;
  }
  return str.substr(str.length() - postfix.length()) == postfix;
}

bool CheckIfStringBeginsAndEndsWith(const std::string& str, const std::string& prefix, const std::string& postfix) {
  if (postfix.length() > str.length()) {
    return false;
  }

  if (prefix.length() > str.length()) {
    return false;
  }

  if (str.substr(0, prefix.length()) != prefix) {
    return false;
  }
  if (str.substr(str.length() - postfix.length()) != postfix) {
    return false;
  }
  return true;
}

std::string ReverseLinesOrder(const std::string& txt) {
  std::istringstream input_stream(txt);

  char line[1024];

  std::vector<std::string> lines;

  while (input_stream.getline(line, 1023)) {
    lines.push_back(line);
  }

  // Generate output
  std::ostringstream output;
  size_t num = lines.size();
  for (size_t i = 0; i < num; i++) {
    output << lines[num - i - 1] << "\n";
  }
  return output.str();
}

int HashString(const std::string& str, int max_num_partitions) {
  static const size_t InitialFNV = 2166136261U;
  static const size_t FNVMultiple = 16777619;

  size_t hash = InitialFNV;

  size_t str_length = str.length();

  for (size_t i = 0; i < str_length; ++i) {
    hash ^= str[i];
    hash *= FNVMultiple;
  }
  return static_cast<int>(hash % max_num_partitions);
}

std::string StripString(const std::string& line) {
  size_t begin_pos = line.find_first_not_of(" \t");

  if (begin_pos == std::string::npos) {
    return "";
  }
  size_t end_pos = line.find_last_not_of(" \t\n\r");
  if (end_pos == std::string::npos) {
    return "";   // Theoretically impossible
  }
  return line.substr(begin_pos, end_pos - begin_pos + 1);
}

void ClearTerminalLine() {
  int terminal_width = GetTerminalWidth();
  au::TemporalBuffer buffer(terminal_width + 1);
  char *data = buffer.data();

  memset(buffer.data(), 0x20, GetTerminalWidth());
  data[terminal_width] = '\0';
  printf("\r%s\r", buffer.data());
  fflush(stdout);
}
}

