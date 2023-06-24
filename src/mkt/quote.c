// Copyright 02-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

#include "mkt/quote.h"
#include "kut/DEFS.h"
#include "kut/dec.h"
#include "kut/js.h"

Quote *quote_new (
  char *date,
   double open, double close, double max, double min, int vol, int force
) {
  if (open < 0 || close < 0 || max < 0 || min < 0 || vol < 0) {
    Quote *this = quote_new_missing(date);
    this->force = force;
  }

  Quote *this = MALLOC(Quote);
  this->date = date;
  this->open = open;
  this->close = close;
  this->max = max;
  this->min = min;
  this->vol = vol;
  this->force = force;
  return this;
}

Quote *quote_new_missing (char *date) {
  Quote *this = MALLOC(Quote);
  this->date = date;
  this->open = -1;
  this->close = -1;
  this->max = -1;
  this->min = -1;
  this->vol = -1;
  this->force =  FALSE;
  return this;
}

/// Returns 1 if 'this' is a missing quote.
int quote_is_missing (Quote *this) {
  return this->vol < 0;
}

// <Quote, Arr<char>>
Tp *quote_correct1 (Quote *q) {
  // <char>
  Arr *errs = arr_new();

  if (q->force) return tp_new(q, errs);

  if (quote_is_missing(q)) {
    arr_push(errs, "Missing");
    return tp_new(q, errs);
  }

  char *date = q->date;
  double open = q->open;
  double close = q->close;
  double max = q->max;
  double min = q->min;
  int vol = q->vol;

  if (open > max) {
    max = open;
    arr_push(errs, "Open > Max");
  }
  if (close > max) {
    max = close;
    arr_push(errs, "Close > Max");
  }
  if (open < min) {
    min = open;
    arr_push(errs, "Open < Min");
  }
  if (close < min) {
    min = close;
    arr_push(errs, "Close < Min");
  }

  if (arr_size(errs))  q = quote_new(date, open, close, max, min, vol, TRUE);
  return tp_new(q, errs);
}

// <Quote, Arr<char>>
Tp *quote_correct2 (Quote *current, Quote *previous) {
  // <char>
  Arr *errs = arr_new();

  if (current->force) return tp_new(current, errs);

  if (quote_is_missing(current)) return tp_new(current, errs);

  char *date = current->date;
  double open = current->open;
  double close = current->close;
  double max = current->max;
  double min = current->min;
  int vol = current->vol;

  if (quote_is_missing(previous)) return quote_correct1(current);

  double open0 = previous->open;
  double close0 = previous->close;
  double max0 = previous->max;
  double min0 = previous->min;

  if (max0 < min0) return quote_correct1(current);

  if (open > max) {
    if (open == open0 && max != max0) open = max;
    else max = open;
    arr_push(errs, "Open > Max");
  }
  if (close > max) {
    if (close == close0 && max != max0) close = max;
    else max = close;
    arr_push(errs, "Close > Max");
  }
  if (open < min) {
    if (open == open0 && min != min0)  open = min;
    else min = open;
    arr_push(errs, "Open < Min");
  }
  if (close < min) {
    if (close == close0 && min != min0) close = min;
    else min = close;
    arr_push(errs, "Close < Min");
  }

  if (arr_size(errs))  current = quote_new(date, open, close, max, min, vol, TRUE);
  return tp_new(current, errs);
}

// <Quote, Arr<char>>
Tp *quote_correct3 (Quote *current, Quote *previous) {
  // <char>
  Arr *errs = arr_new();

  if (current->force) return tp_new(current, errs);

  if (quote_is_missing(current) || quote_is_missing(previous))
    return tp_new(current, errs);

  char *date = current->date;
  double open = current->open;
  double close = current->close;
  double max = current->max;
  double min = current->min;
  int vol = current->vol;

  double open0 = previous->open;
  double close0 = previous->close;
  double max0 = previous->max;
  double min0 = previous->min;

  if (open > open0 * 1.2) arr_push(errs, "Open +20%");
  if (close > close0 * 1.2)  arr_push(errs, "Close +20%");
  if (max > max0 * 1.2) arr_push(errs, "Max +20%");
  if (min > min0 * 1.2) arr_push(errs, "Min +20%");

  if (open < open0 * 0.8) arr_push(errs, "Open -20%");
  if (close < close0 * 0.8) arr_push(errs, "Close -20%");
  if (max < max0 * 0.8) arr_push(errs, "Max -20%");
  if (min < min0 * 0.8) arr_push(errs, "Min -20%");

  if (arr_size(errs))  current = quote_new(date, open, close, max, min, vol, TRUE);
  return tp_new(current, errs);
}

char *quote_to_str (Quote *this) {
  return str_f("%s:%s:%s:%s:%s:%s:%s",
    this->date,
    dec_ftos(this->open, 4), dec_ftos(this->close, 4),
    dec_ftos(this->max, 4), dec_ftos(this->min, 4), dec_itos(this->vol),
    this->force ? "true" : "false"
  );
}

Quote *quote_from_str (char *s) {
  s = str_trim(s);
  if (!*s) EXC_GENERIC("Quote string is empty");

  // <char>
  Arr *a = str_csplit_trim(s, ':');

  char *date = arr_get(a, 0);
  if (strlen(date) != 8 || !dec_digits(date))
    EXC_ILLEGAL_ARGUMENT("Bad date", "Date string", date);

  char *force = arr_get(a, 6);
  if (strcmp(force, "true") && strcmp(force, "false"))
    EXC_ILLEGAL_ARGUMENT("Bad force value", "Boolean", force);

  return quote_new(
    date,
    dec_stod(arr_get(a, 1)),
    dec_stod(arr_get(a, 2)),
    dec_stod(arr_get(a, 3)),
    dec_stod(arr_get(a, 4)),
    dec_stoi(arr_get(a, 5)),
    str_eq(force, "true") ? TRUE : FALSE
  );
}

char *quote_to_js (Quote *this) {
  return js_wa(arr_new_from(
    js_ws(this->date),
    js_wf(this->open, 4),
    js_wf(this->close, 4),
    js_wf(this->max, 4),
    js_wf(this->min, 4),
    js_wi(this->vol),
    js_wb(this->force),
    NULL
  ));
}

Quote *quote_from_js (char *js) {
  Arr *a = js_ra(js);
  return quote_new(
    js_rs(arr_get(a, 0)),
    js_rd(arr_get(a, 1)),
    js_rd(arr_get(a, 2)),
    js_rd(arr_get(a, 3)),
    js_rd(arr_get(a, 4)),
    js_ri(arr_get(a, 5)),
    js_rb(arr_get(a, 6))
  );
}
