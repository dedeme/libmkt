// Copyright 02-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

#include "mkt/qs.h"
#include "kut/DEFS.h"
#include "mkt/quote.h"

// <Qs>. NOTE: Qs is ordered from after to before.
Rs *qs_from_str (int size, char *s) {
  s = str_trim(s);
  if (!*s) return rs_fail("Quotes string is empty");

  // Quote.
  Arr *qs = arr_new();
  char *err = "";
  TRY {
    EACH(str_csplit(s, '\n'), char, l) {
      arr_push(qs, quote_from_str(l));
    }_EACH
  } CATCH (e) {
    err = exc_msg(e);
  }_TRY

  if (*err) return rs_fail(err);

  if (arr_size(qs) != size) {
    return rs_fail(str_f(
      "Wrong quotes number. Expected %d. Found %d", size, arr_size(qs)
    ));
  }

  return rs_ok(qs);
}

char *qs_to_str (Qs *this) {
  return arr_cjoin(arr_map((Arr *) this, (FMAP)quote_to_str), '\n');
}

int qs_size (Qs *this) {
  return arr_size((Arr *)this);
}

// <Qs, Arr<char>>
Tp *qs_correct (Qs *this) {
  // Arr<char>
  Arr *errs = arr_new();

  // 'new' is Arr<char>
  void append_errs (Quote *q, Arr *new) {
    EACH(new, char, e) {
      arr_push(errs, str_f("%s: %s", q->date, e));
    }_EACH
  }

  if (!arr_size((Arr *)this)) return tp_new(this, errs);

  // <Quote>
  Arr *new_qs = arr_new();
  Quote **pbegin = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  Quote *q1 = NULL;
  Quote *q2 = *pbegin++;
  while (pbegin < pend) {
    q1 = q2;
    q2 = *pbegin++;

    // <Quote, Arr<char>>
    Tp *tp = quote_correct2(q1, q2);
    Quote *q = tp_e1(tp);
    append_errs(q, tp_e2(tp));

    tp = quote_correct3(q, q2);
    q = tp_e1(tp);
    append_errs(q, tp_e2(tp));

    arr_push(new_qs, q);
  }
  // <Quote, Arr<char>>
  Tp *tp = quote_correct1(q2);
  Quote *q = tp_e1(tp);
  arr_push(new_qs, q);
  append_errs(q, tp_e2(tp));

  return tp_new(new_qs, errs);
}

// <Qs, Arr<char>>
Tp *qs_correct_dates (Qs *mdqs, Qs *this) {
  // Arr<char>
  Arr *errs = arr_new();

  // 'new' is Arr<char>
  void append_err (Quote *q, char *e) {
    arr_push(errs, str_f("%s: %s", q->date, e));
  }

  // <Quote>
  Arr *new_qs = arr_new();
  int mdi = 0;
  int mdqs_sz = qs_size(mdqs);
  int i = 0;
  int this_sz = qs_size(this);
  for (;;) {
    if (mdi < mdqs_sz) {
      Quote *mdq = arr_get((Arr *)mdqs, mdi);
      if (i < this_sz) {
        Quote *q = arr_get((Arr *)this, i);
        if (strcmp(mdq->date, q->date) > 0) {
          append_err(mdq, "Missing quote");
          arr_push(new_qs, quote_new_missing(mdq->date));
          ++mdi;
          continue;
        }
        if (strcmp(mdq->date, q->date) < 0) {
          append_err(q, "Extra quote");
          ++i;
          continue;
        }
        arr_push(new_qs, q);
        ++i;
        ++mdi;
        continue;
      }
      append_err(mdq, "Missing quote");
      arr_push(new_qs, quote_new_missing(mdq->date));
      ++mdi;
      continue;
    }
    if (i < this_sz) {
      append_err(arr_get((Arr *)this, i), "Extra quote");
      ++i;
      continue;
    }
    break;
  }

  return tp_new(new_qs, errs);
}

// <Qs, Arr<char>>
Tp *qs_merge (Qs *mdqs, Qs *new_qs, Qs *this) {
  int iqs = 0;
  int qs_sz = qs_size(this);
  // Arr<Qs>
  Arr *qs = arr_new_bf(qs_sz);
  int ins = 0;
  int ns_sz = qs_size(new_qs);
  int ithis = 0;

  for (;;) {
    if (iqs >= qs_sz) break;

    if (ins < ns_sz) {
      Quote *nq = arr_get((Arr *)new_qs, ins);
      if (ithis < qs_sz) {
        Quote *oldq = arr_get((Arr *)this, ithis);
        if (strcmp(nq->date, oldq->date) > 0) {
          arr_push(qs, nq);
          ++ins;
          ++iqs;
          continue;
        }
        if (strcmp(nq->date, oldq->date) > 0) {
          arr_push(qs, oldq);
          ++ithis;
          ++iqs;
          continue;
        }
        if (quote_is_missing(oldq)) arr_push(qs, nq);
        else arr_push(qs, oldq);
        ++ins;
        ++ithis;
        ++iqs;
        continue;
      }
      arr_push(qs, nq);
      ++ins;
      ++iqs;
      continue;
    }

    if (ithis < qs_sz) {
      arr_push(qs, arr_get((Arr *)this, ithis));
      ++ithis;
      ++iqs;
      continue;
    }

    break;
  }

  Qs *rqs = (Qs *)qs;
  // <char>
  Arr *errs = arr_new();

  if (qs_size(mdqs) != 0) {
    if (qs_sz != qs_size(mdqs))
      return tp_new(this, arr_new_from(str_f(
          "Company model (%d) has diffent number of quotes from 'this' (%s)",
          qs_size(mdqs), qs_sz
        ), NULL));

    Tp *tp = qs_correct_dates(mdqs, rqs);
    rqs = tp_e1(tp);
    //<char>
    arr_cat(errs, tp_e2(tp));
  }

  Tp *tp = qs_correct(rqs);
  rqs = tp_e1(tp);
  //<char>
  arr_cat(errs, tp_e2(tp));

  return tp_new(rqs, errs);
}

// <Quote>
Arr *qs_quotes (Qs *this) {
  Arr *r = arr_copy((Arr *)this);
  arr_reverse(r);
  return r;
}

char **qs_dates (Qs *this) {
  int size = arr_size((Arr *)this);
  char **r = (char **)GC_MALLOC(sizeof(char *) * size);
  char **pr = r + size;
  Quote **pthis = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  while (pthis < pend) *(--pr) = (*pthis++)->date;
  return r;
}

static void normalize(int size, double *vs) {
  double start = -1;
  for (int i = 0; i < size; ++i) {
    if (vs[i] >= 0) {
      start = vs[i];
      break;
    }
  }
  if (start < 0)
    EXC_ILLEGAL_STATE("Every quote is < 0");

  vs[0] = start;
  for (int i = 1; i < size; ++i) if (vs[i] < 0) vs[i] = vs[i - 1];
}

double *qs_opens (Qs *this) {
  int size = arr_size((Arr *)this);
  double *r = (double *)ATOMIC(sizeof(double) * size);
  double *pr = r + size;
  Quote **pthis = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  while (pthis < pend) *(--pr) = (*pthis++)->open;
  normalize(size, r);
  return r;
}

double *qs_closes (Qs *this) {
  int size = arr_size((Arr *)this);
  double *r = (double *)ATOMIC(sizeof(double) * size);
  double *pr = r + size;
  Quote **pthis = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  while (pthis < pend) *(--pr) = (*pthis++)->close;
  normalize(size, r);
  return r;
}

double *qs_maxs (Qs *this) {
  int size = arr_size((Arr *)this);
  double *r = (double *)ATOMIC(sizeof(double) * size);
  double *pr = r + size;
  Quote **pthis = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  while (pthis < pend) *(--pr) = (*pthis++)->max;
  normalize(size, r);
  return r;
}

double *qs_mins (Qs *this) {
  int size = arr_size((Arr *)this);
  double *r = (double *)ATOMIC(sizeof(double) * size);
  double *pr = r + size;
  Quote **pthis = (Quote **)arr_begin((Arr *)this);
  Quote **pend = (Quote **)arr_end((Arr *)this);
  while (pthis < pend) *(--pr) = (*pthis++)->min;
  normalize(size, r);
  return r;
}

double qs_volume_avg (Qs *this, int samples) {
  double fvol (Quote *q) {
    return (q->max + q->min) * q->vol / 2.0;
  }

  if (qs_size(this) < 81)
    EXC_ILLEGAL_ARGUMENT("Wrong quotes number", " > 81", str_f("%d", qs_size(this)));

  double maxs[3];
  double mins[3];
  double sum = 0;
  int ix = 0;
  int mmix = 0;
  EACH((Arr *)this, Quote, q) {
    if (ix >= samples) break;
    if (quote_is_missing(q)) continue;
    double v = fvol(q);
    if (mmix < 3) {
      maxs[mmix] = v;
      mins[mmix] = v;
      ++mmix;
      continue;
    }
    for (int i = 0; i < 3; ++i) {
      if (v > maxs[i]) {
        double tmp = v;
        v = maxs[i];
        maxs[i] = tmp;
      } else if (v < mins[i]) {
        double tmp = v;
        v = mins[i];
        mins[i] = tmp;
      }
    }
    sum += v;
    ++ix;
  }_EACH

  if (ix > 0) return sum / ix;
  return 0;
}
