// Copyright 03-Jun-2023 ºDeme
// GNU General Public License - V3 <http://www.gnu.org/licenses/>

#include "tquote.h"
#include "kut/DEFS.h"
#include "mkt/quote.h"

/*
20230602:3.947:3.891:3.987:3.824:15902209:false
20230601:3.957:3.921:3.967:3.917:8973463:false
20230531:3.91:3.97:3.98:3.908:53083209:false
20230530:3.982:3.915:3.988:3.912:6811907:false
20230529:3.966:3.973:3.994:3.954:4750999:false
20230526:3.94:3.929:3.946:3.895:7086827:false
20230525:4.001:3.929:4.011:3.914:8214347:false
20230524:3.99:3.996:4.027:3.982:7938446:false
*/

static char *of_q (Quote *q) { return quote_to_str(q); }
static Quote *of_s (char *s) { return quote_from_str(s); }

void tquote (void) {
  puts ("quote tests:");

  Quote *q1 = of_s("20230602:3.947:3.891:3.987:3.824:15902209:false");
  TEST(of_q(q1), "20230602:3.947:3.891:3.987:3.824:15902209:false");
  TEST(quote_to_js(q1), "[\"20230602\",3.947,3.891,3.987,3.824,15902209,false]");
  TEST(of_q(quote_from_js(
      "[\"20230602\",3.947,3.891,3.987,3.824,15902209,true]"
    )),
    "20230602:3.947:3.891:3.987:3.824:15902209:true"
  );

  Tp *tp0 = quote_correct1(quote_from_str(
    "20230602:3.947:3.891:3.987:3.824:15902209:false"
  ));
  TEST(of_q(tp_e1(tp0)), "20230602:3.947:3.891:3.987:3.824:15902209:false");
  TESTI(arr_size(tp_e2(tp0)), 0);

  Tp *tp1 = quote_correct1(quote_from_str(
    "20230602:4.947:3.891:3.987:3.824:15902209:false"
  ));
  TEST(of_q(tp_e1(tp1)), "20230602:4.947:3.891:4.947:3.824:15902209:true");
  TEST(arr_get(tp_e2(tp1), 0), "Open > Max");

  Tp *tp2 = quote_correct1(quote_from_str(
    "20230602:3.947:4:3.987:3.824:15902209:false"
  ));
  TEST(of_q(tp_e1(tp2)), "20230602:3.947:4:4:3.824:15902209:true");
  TEST(arr_get(tp_e2(tp2), 0), "Close > Max");

  Tp *tp3 = quote_correct1(quote_from_str(
    "20230602:1:3.891:3.987:3.824:15902209:false"
  ));
  TEST(of_q(tp_e1(tp3)), "20230602:1:3.891:3.987:1:15902209:true");
  TEST(arr_get(tp_e2(tp3), 0), "Open < Min");

  Tp *tp4 = quote_correct1(quote_from_str(
    "20230602:3.947:1:3.987:3.824:15902209:false"
  ));
  TEST(of_q(tp_e1(tp4)), "20230602:3.947:1:3.987:1:15902209:true");
  TEST(arr_get(tp_e2(tp4), 0), "Close < Min");

  Tp *tp5 = quote_correct1(quote_from_str(
    "20230602:3.947:1:3.987:3.824:15902209:true"
  ));
  TEST(of_q(tp_e1(tp5)), "20230602:3.947:1:3.987:3.824:15902209:true");
  TESTI(arr_size(tp_e2(tp0)), 0);

  Tp *t1 = quote_correct2(
    of_s("20230602:3.947:3.891:3.987:3.824:15902209:false"),
    of_s("20230601:3.957:3.921:3.967:3.917:8973463:false")
  );
  TEST(of_q(tp_e1(t1)), "20230602:3.947:3.891:3.987:3.824:15902209:false");
  TESTI(arr_size(tp_e2(t1)), 0);

  Tp *t2a = quote_correct2(
    of_s("20230602:4:3.891:3.987:3.824:15902209:false"),
    of_s("20230601:3.957:3.921:3.967:3.917:8973463:false")
  );
  TEST(of_q(tp_e1(t2a)), "20230602:4:3.891:4:3.824:15902209:true");
  TEST(arr_get(tp_e2(t2a), 0), "Open > Max");

  Tp *t2b = quote_correct2(
    of_s("20230602:4:3.891:3.987:3.824:15902209:false"),
    of_s("20230601:4:3.921:3.967:3.917:8973463:false")
  );
  TEST(of_q(tp_e1(t2b)), "20230602:3.987:3.891:3.987:3.824:15902209:true");
  TEST(arr_get(tp_e2(t2b), 0), "Open > Max");

  Tp *t3a = quote_correct2(
    of_s("20230602:3.947:4:3.987:3.824:15902209:false"),
    of_s("20230601:3.957:3.921:3.967:3.917:8973463:false")
  );
  TEST(of_q(tp_e1(t3a)), "20230602:3.947:4:4:3.824:15902209:true");
  TEST(arr_get(tp_e2(t3a), 0), "Close > Max");

  Tp *t3b = quote_correct2(
    of_s("20230602:3.947:4:3.987:3.824:15902209:false"),
    of_s("20230601:3.957:4:3.967:3.917:8973463:false")
  );
  TEST(of_q(tp_e1(t3b)), "20230602:3.947:3.987:3.987:3.824:15902209:true");
  TEST(arr_get(tp_e2(t3b), 0), "Close > Max");

  Tp *t4 = quote_correct3(
    of_s("20230602:3.947:3.891:3.987:3.824:15902209:false"),
    of_s("20230601:1:1:1:1:463:false")
  );
  TEST(of_q(tp_e1(t4)), "20230602:3.947:3.891:3.987:3.824:15902209:true");
  TESTI(arr_size(tp_e2(t4)), 4);
  EACH(tp_e2(t4), char, e) {
    TEST(str_right(e, -5), " +20%");
  }_EACH

  Tp *t5 = quote_correct3(
    of_s("20230602:3.947:3.891:3.987:3.824:15902209:false"),
    of_s("20230601:8:8:8:8:463:false")
  );
  TEST(of_q(tp_e1(t5)), "20230602:3.947:3.891:3.987:3.824:15902209:true");
  TESTI(arr_size(tp_e2(t5)), 4);
  EACH(tp_e2(t5), char, e) {
    TEST(str_right(e, -5), " -20%");
  }_EACH

  puts("  finished");
}
