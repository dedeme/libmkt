// Copyright 02-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

/// Array of quotes.

#ifndef MKT_QS_H
  #define MKT_QS_H

#include "kut/tp.h"
#include "kut/rs.h"
#include "kut/arr.h"

/// Opaque structure.
typedef struct qs_Qs Qs;

/// Returns a Rs<Qs> with quotes from the content of a 'NICK.tb' file.
/// If the final number of quotes is different to 'size', or
/// same quote can not be read, the first error found is returned.
///   size: Number of quotes to return.
///   s   : Text from a 'NICK.tb' file.
Rs *qs_from_str (int size, char *s);

/// Returns text to write in a 'NICK.tb' file.
char *qs_to_str (Qs *this);

/// Returns the quotes number of 'this'
int qs_size (Qs *this);

/// Corrects "this" and returns a list with corrections.
///   this: Historic quotes.
///   ----
///   Returns Tp<Qs, Arr<char>>:
///     e1 : Quotes, corrected if necessary.
///     e2 : Array with corrections.
Tp *qs_correct (Qs *this);

/// Corrects extra o missing quotes in "this" and returns a list with corrected
/// quotes.
///   mdqs: Historic quotes of company model
///   this: Historic quotes of company.
///   ----
///   Returns Tp<Qs, Arr<char>>:
///     e1 : Quotes, corrected if necessary.
///     e2 : Array with corrections.
Tp *qs_correct_dates (Qs *mdqs, Qs *this);

/// Merges new quotes with others already existent.
///   mdqs : Model quotes. When quotes are of company model, it is an
///          empty array.
///   newQs: Last quotes read from the Internet. It can not be empty.
///   this : Existent quotes in file system.
///   ----
///   Returns Tp<Qs, Arr<char>>:
///     e1 : Quotesmade with the following process:
///           1. Every quote on top with 'open = -1' is removed from "this"
///              in the range of "newQs" dates.
///           2. If there are new and old quotes for the same date, the old
///              is selected.
///           3. The return array is corrected in the range of new dates
///              and added or removed quotes maching model quotes.
///     e2 : Array with Errors returned by 'correct' and 'correctDates' in format
///         "date: error".
Tp *qs_merge (Qs *mdqs, Qs *new_qs, Qs *this);

/// Returns an Arr<Quote>, copy of this, ordered from before to after.
///   this: Historic quotes.
Arr *qs_quotes (Qs *this);

/// Returns a C array of dates with 'size == qs_size(this)', ordered from
/// before to after
///   this: Historic quotes.
char **qs_dates (Qs *this);

/// Returns a C array of opens with 'size = qs_size(this)', normalized and
/// ordered from before to after
///   this: Historic quotes.
double *qs_opens (Qs *this);

/// Returns a C array of closes with 'size = qs_size(this)', normalized and
/// ordered from before to after
///   this: Historic quotes.
double *qs_closes (Qs *this);

/// Returns a C array of maxs with 'size = qs_size(this)', normalized and
/// ordered from before to after
///   this: Historic quotes.
double *qs_maxs (Qs *this);

/// Returns a C array of mins with 'size = qs_size(this)', normalized and
/// ordered from before to after
///   this: Historic quotes.
double *qs_mins (Qs *this);

/// Returns the volume average (in €) of a company quotes.
///   this   : Historic quotes.
///   samples: Number of quotes to take in account.
double qs_volume_avg (Qs *this, int samples);

#endif
