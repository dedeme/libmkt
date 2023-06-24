// Copyright 02-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

/// Market quote.

#ifndef MKT_QUOTE_H
  #define MKT_QUOTE_H

#include "kut/tp.h"

/// Quote definition.
typedef struct quote_Quote {
    char *date; // Date of quote in format "YYYYMMDD".
    double open; // Open value.
    double close; // Close value.
    double max; // Maximum value.
    double min; // Minimum value.
    int vol; // Volume value.
    int force; // 'TRUE' if quote was modified manually.
  } Quote;

/// Create a new quote.
///   date : Date of quote in format "YYYYMMDD".
///   open : Open value.
///   close: Close value.
///   max  : Maximum value.
///   min  : Minimum value.
///   vol  : Valume value.
///   force: 'TRUE' if quote was modified manually.
Quote *quote_new (
  char *date,
  double open, double close, double max, double min, int vol, int force
);

/// Create a missing quote (every numeric value is < 0 and force == FALSE)
Quote *quote_new_missing (char *date);

/// Returns 1 if 'this' is a missing quote.
int quote_is_missing (Quote *this);

/// Checks maximum and minimum and returns a new quote corrected, if it is
/// necessary.
///
/// If this->force = TRUE, quote will not be corrected.
///
/// If quote was corrected, its 'force' field is set to TRUE.
///
///   this: Quote to correct.
///   ----
///   Returns Tp<Quote, Arr<char>>:
///     e1 : Corrected quote.
///     e2 : Array of corrections (e.g. Close > Max).
Tp *quote_correct1 (Quote *this);

/// Checks maximum and minimum of 'current', taken account 'previous' and
/// returns a new quote corrected, if it is necessary.
///
/// If last->force = TRUE, quote will not be corrected.
///
/// If quote was corrected, its 'force' field is set to TRUE.
///
///   current : Quote to correct.
///   previous: Quote previous to "current"
///    ----
///   Returns Tp<Quote, Arr<char>>:
///     e1 : Corrected quote.
///     e2 : Array of corrections (e.g. Close > Max).
Tp *quote_correct2 (Quote *current, Quote *previous);

/// Checks increment (+-20%) and returns a new quote equals to 'current', setting
/// current->force to TRUE if such increment has happened.
///
///   current : Quote to correct.
///   previous: Quote previous to "current"
///    ----
///   Returns Tp<Quote, Arr<char>>:
///     e1 : Corrected quote.
///     e2 : Array of corrections (e.g. Close > Max).
Tp *quote_correct3 (Quote *current, Quote *previous);

/// Returns a string representation of "this". Used to write in company tables.
char *quote_to_str (Quote *this);

/// Returns a Quote from its string representation. Used to read in company tables.
/// THROWS exception if it fails.
Quote *quote_from_str (char *s);

///
char *quote_to_js (Quote *this);

///
Quote *quote_from_js (char *js);

#endif
