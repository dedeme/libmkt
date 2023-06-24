// Copyright 03-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

#include "tqs.h"
#include "kut/DEFS.h"
#include "kut/dec.h"
#include "mkt/quote.h"
#include "mkt/qs.h"

//static char *of_q (Quote *q) { return quote_to_str(q); }
//static Quote *of_s (char *s) { return quote_from_str(s); }

void tqs (void) {
  puts ("qs tests:");

  char *data =
    "20230602:3.947:3.891:3.987:3.824:15902209:false\n"
    "20230601:4:3.921:3.967:3.917:8973463:false\n"
    "20230531:3.91:3.97:3.98:3.908:53083209:false\n"
    "20230530:3.982:3.915:3.988:3.912:6811907:false\n"
    "20230529:3.966:3.973:3.994:3.954:4750999:false\n"
    "20230526:3.94:3.929:3.946:3.895:7086827:false\n"
    "20230525:4.001:3.929:4.011:3.914:8214347:false\n"
    "20230524:1.99:3.996:4.027:3.982:7938446:false"
  ;

  char *mdata =
    "20230601:152.75:154.8:155.4:152.1:80472:false\n"
    "20230531:158.3:151.3:158.5:149.55:231221:false\n"
    "20230530:161.7:158.5:162:158.5:67289:false\n"
    "20230529:160.95:161.3:162.1:160.45:30756:false\n"
    "20230527:161.5:160.1:161.6:158:85878:false\n"
    "20230525:165.3:160.4:165.3:160.1:73500:false\n"
    "20230524:165:165:166.2:163.5:67127:false\n"
    "20230523:166:165.9:167.45:165.1:49554:false\n"
  ;


  char *newdata =
    "20230603:3.948:3.892:3.988:3.825:1590220:false\n"
    "20230602:3.888:3.888:3.987:3.824:15902209:false\n"
    "20230601:4:3.921:3.967:3.917:8973463:false\n"
  ;

  char *merge1_data =
    "20230603:3.948:3.892:3.988:3.825:1590220:false\n"
    "20230602:3.947:3.891:3.987:3.824:15902209:false\n"
    "20230601:4:3.921:4:3.917:8973463:true\n"
    "20230531:3.91:3.97:3.98:3.908:53083209:false\n"
    "20230530:3.982:3.915:3.988:3.912:6811907:false\n"
    "20230529:3.966:3.973:3.994:3.954:4750999:false\n"
    "20230526:3.94:3.929:3.946:3.895:7086827:false\n"
    "20230525:4.001:3.929:4.011:3.914:8214347:false"
  ;

  char *merge2_data =
    "20230601:4:3.921:4:3.917:8973463:true\n"
    "20230531:3.91:3.97:3.98:3.908:53083209:false\n"
    "20230530:3.982:3.915:3.988:3.912:6811907:false\n"
    "20230529:3.966:3.973:3.994:3.954:4750999:false\n"
    "20230527:-1:-1:-1:-1:-1:false\n"
    "20230525:4.001:3.929:4.011:3.914:8214347:false\n"
    "20230524:-1:-1:-1:-1:-1:false\n"
    "20230523:-1:-1:-1:-1:-1:false"
  ;

  // <Qs>
  Rs *qs_rs = qs_from_str(8, data);
  Qs *qs = rs_get(qs_rs);
  if (qs) { TESTI(qs_size(qs), 8); }
  else { TEST(rs_error(qs_rs), "No error in qs_from_str") };

  TEST(qs_to_str(qs), data);

  // <Qs, Arr<char>>
  Tp *correct = qs_correct(qs);
  TEST(
    quote_to_str(arr_get(tp_e1(correct), 1)),
    "20230601:4:3.921:4:3.917:8973463:true"
  );
  TEST(
    quote_to_str(arr_get(tp_e1(correct), 7)),
    "20230524:1.99:3.996:4.027:1.99:7938446:true"
  );
  TEST(
    arr_join(tp_e2(correct), " - "),
    "20230601: Open > Max - 20230525: Open +20% - 20230524: Open < Min"
  );

  // <Qs>
  Rs *mdqs_rs = qs_from_str(8, mdata);
  Qs *mdqs = rs_get(mdqs_rs);
  if (mdqs) { TESTI(qs_size(mdqs), 8); }
  else { TEST(rs_error(mdqs_rs), "No error in qs_from_str") };

  // <Qs, Arr<char>>
  Tp *cdates = qs_correct_dates(mdqs, qs);
  TESTI(arr_size(tp_e1(cdates)), 8);
  assert(quote_is_missing(arr_get(tp_e1(cdates), 4)));
  assert(quote_is_missing(arr_get(tp_e1(cdates), 7)));

  TEST(
    arr_join(tp_e2(cdates), " - "),
    "20230602: Extra quote - 20230527: Missing quote - "
    "20230526: Extra quote - 20230523: Missing quote"
  );

  // <Qs>
  Rs *newqs_rs = qs_from_str(3, newdata);
  Qs *newqs = rs_get(newqs_rs);
  if (newqs) { TESTI(qs_size(newqs), 3); }
  else { TEST(rs_error(newqs_rs), "No error in qs_from_str") };

  Qs *qs2 = (Qs *)arr_copy((Arr *) qs);
  arr_set((Arr *)qs2, 1, quote_new_missing("20230601"));
  // <Qs, Arr<char>>
  Tp *merged = qs_merge((Qs *)arr_new(), newqs, qs);
  TEST(qs_to_str(tp_e1(merged)), merge1_data);
  TEST(arr_join(tp_e2(merged), " - "), "20230601: Open > Max");

  // <Qs, Arr<char>>
  Tp *merged2 = qs_merge(mdqs, newqs, qs);
  TEST(qs_to_str(tp_e1(merged2)), merge2_data);
  TEST(
    arr_join(tp_e2(merged2), " - "),
      "20230603: Extra quote - 20230602: Extra quote - "
      "20230527: Missing quote - 20230526: Extra quote - "
      "20230524: Missing quote - 20230523: Missing quote - "
      "20230601: Open > Max - 20230527: Missing - "
      "20230524: Missing - 20230523: Missing"
    );

  TEST(
    arr_join(arr_new_c(qs_size(qs), (void **)qs_dates(qs)), "-"),
    "20230524-20230525-20230526-20230529-20230530-20230531-20230601-20230602"
  );

  // <char>
  Arr *sum = arr_new();
  double *ds = qs_opens(qs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "1.99|4.001|3.94|3.966|3.982|3.91|4|3.947");

  sum = arr_new();
  ds = qs_closes(qs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "3.996|3.929|3.929|3.973|3.915|3.97|3.921|3.891");

  sum = arr_new();
  ds = qs_maxs(qs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "4.027|4.011|3.946|3.994|3.988|3.98|3.967|3.987");

  sum = arr_new();
  ds = qs_mins(qs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "3.982|3.914|3.895|3.954|3.912|3.908|3.917|3.824");

  Qs *mrqs = tp_e1(merged2);
  sum = arr_new();
  ds = qs_opens(mrqs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "4.001|4.001|4.001|4.001|3.966|3.982|3.91|4");

  sum = arr_new();
  ds = qs_closes(mrqs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "3.929|3.929|3.929|3.929|3.973|3.915|3.97|3.921");

  sum = arr_new();
  ds = qs_maxs(mrqs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "4.011|4.011|4.011|4.011|3.994|3.988|3.98|4");

  sum = arr_new();
  ds = qs_mins(mrqs);
  for (int i = 0; i < qs_size(qs); ++i) arr_push(sum, dec_ftos(ds[i], 4));
  TEST(arr_cjoin(sum, '|'), "3.914|3.914|3.914|3.914|3.954|3.912|3.908|3.917");

  puts("  finished");
}
