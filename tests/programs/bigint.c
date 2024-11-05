#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"

int assert(int b) {
    if (b == 0) {
        printf("assertion error\n");
        exit(1);
    }
}

int main() {
    bigint_t zero = bi_from_str("0");
    bigint_t one = bi_from_str("1");

    bigint_t a   = bi_from_str("1234567890");
    bigint_t b   = bi_from_str("9876543210");
    bigint_t amodb = bi_from_str("1234567890");
    bigint_t apb = bi_from_str("11111111100");

    bigint_t c   = bi_from_str("999999999999999999999999999999");
    bigint_t d   = bi_from_str("123456789012345678901234567890");
    bigint_t cpd = bi_from_str("1123456789012345678901234567889");
    bigint_t csd = bi_from_str("876543210987654321098765432109");

    bigint_t e = bi_from_str("9999999999999999999999999999999");
    bigint_t f = bi_from_tens_power(31);
    bigint_t esf = bi_from_str("-1");
    bigint_t emf = bi_from_str("99999999999999999999999999999990000000000000000000000000000000");

    bigint_t g = bi_from_str("271828182845904523536028747135266249");
    bigint_t h = bi_from_str("4294967296");
    bigint_t gmh = bi_from_str("1167493155454268136125705746661822227707592704");
    bigint_t gdh = bi_from_str("63289930775273713175214069");

    bigint_t i = bi_from_str("115792089237316195423570985008687907853269984665640564039457584007913129639932");
    bigint_t j = bi_from_str("18446744073709551614");
    bigint_t idj = bi_from_str("6277101735386680764516354157049543343102891635622409142280");
    bigint_t imodj = bi_from_str("12");

    bigint_t zhex = bi_from_str("0xffffffff");
    bigint_t zdec = bi_from_str("4294967295");
    bigint_t zbin = bi_from_str("0b11111111111111111111111111111111");
    print_bi_dec(&zhex); puts(" = ");
    print_bi_dec(&zdec); puts(" = ");
    print_bi_dec(&zbin); puts("");
    assert(bi_eq(&zhex, &zdec));
    assert(bi_eq(&zdec, &zbin));

    bigint_t result;

    /* add */
    result = bi_add(&a, &b);
    print_bi_dec(&a); printf(" + "); print_bi_dec(&b); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &apb));

    result = bi_add(&c, &d);
    print_bi_dec(&c); printf(" + "); print_bi_dec(&d); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &cpd));

    /* sub */
    result = bi_sub(&c, &d);
    print_bi_dec(&c); printf(" - "); print_bi_dec(&d); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &csd));

    result = bi_sub(&e, &f);
    print_bi_dec(&e); printf(" - "); print_bi_dec(&f); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &esf));

    /* mul */
    result = bi_mul(&e, &f);
    print_bi_dec(&e); printf(" * "); print_bi_dec(&f); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &emf));

    result = bi_mul(&g, &h);
    print_bi_dec(&g); printf(" * "); print_bi_dec(&h); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &gmh));

    /* div */
    result = bi_div(&g, &h);
    print_bi_dec(&g); printf(" / "); print_bi_dec(&h); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &gdh));

    result = bi_div(&i, &j);
    print_bi_dec(&i); printf(" / "); print_bi_dec(&j); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &idj));

    /* mod */
    result = bi_mod(&i, &j);
    print_bi_dec(&i); printf(" %% "); print_bi_dec(&j); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &imodj));

    result = bi_mod(&a, &b);
    print_bi_dec(&a); printf(" %% "); print_bi_dec(&b); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &amodb));

    /* very big */
    bigint_t very_big_1 = bi_from_tens_power(500);
    bigint_t very_big_2 = bi_from_str("87878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787");
    bigint_t bery_big_1m2 = bi_from_str("8787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878787878700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    bigint_t bery_big_1d2 = bi_from_str("1137931034482758620689655172413793103448275862068965517241379310344827586206896551724137931034482758620689655172413793103448275862068965517241379310344827586206896551724137931034482758620689655172413793103448275862068965517241379310344827586206896");

    printf("very_big_1:\n"); print_bi(&very_big_1); puts("");
    printf("very_big_2:\n"); print_bi(&very_big_2); puts("");

    result = bi_mul(&very_big_1, &very_big_2);
    printf("very_big_1 * very_big_2"); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &bery_big_1m2));

    result = bi_div(&very_big_1, &very_big_2);
    printf("very_big_1 / very_big_2"); puts(" = ");
    print_bi_dec(&result); puts("");
    assert(bi_eq(&result, &bery_big_1d2));

    printf("all passed\n");
    return 0;
}