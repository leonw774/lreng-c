#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynarr.h"
#include "number.h"

void
number_copy(number_t* dst, number_t* src) {
    dst->sign = src->sign;
    dst->zero = src->zero;
    dst->nan = src->nan;
    if (src->nan) {
        dst->numer = NAN_BIGINT();
        dst->denom = NAN_BIGINT();
    }
    else if (src->zero) {
        dst->numer = ZERO_BIGINT();
        dst->denom = ZERO_BIGINT();
    }
    else {
        bi_copy(&dst->numer, &src->numer);
        bi_copy(&dst->denom, &src->denom);
    }
}

void
number_free(number_t* x) {
    x->sign = x->zero = x->nan = 0;
    bi_free(&x->numer);
    bi_free(&x->denom);
}

void
number_normalize(number_t* x) {
    bigint_t a, b, t1 = ZERO_BIGINT(), t2 = ZERO_BIGINT(), one = ONE_BIGINT();
    /* flags */
    if (x->nan || x->numer.nan || x->denom.nan) {
        bi_free(&x->numer);
        bi_free(&x->denom);
        x->numer = NAN_BIGINT();
        x->denom = NAN_BIGINT();
        x->sign = x->zero = 0;
    }

    /* normalize the sign */
    if (x->numer.sign != x->denom.sign) {
        x->sign = !x->sign;
    }
    x->numer.sign = x->denom.sign = 0;

    /* special cases */
    /* n = 0 */
    if (x->numer.size == 0) {
        bi_free(&x->numer);
        bi_free(&x->denom);
        *x = ZERO_NUMBER();
        return;
    }
    /* n == 1 or d = 1 */ 
    if (bi_eq(&x->numer, &one) || bi_eq(&x->denom, &one)) {
        return;
    }
    /* n */
    if (x->denom.size == 0) {
        bi_free(&x->numer);
        bi_free(&x->denom);
        *x = NAN_NUMBER();
        return;
    }
    if (bi_eq(&x->numer, &x->denom)) {
        bi_free(&x->numer);
        bi_free(&x->denom);
        x->numer = ONE_BIGINT();
        x->denom = ONE_BIGINT();
        return;
    }
    
    /* euclidian algorithm */
    bi_copy(&a, &x->numer);
    bi_copy(&b, &x->denom);
    while (b.size != 0) {
        bi_free(&t1);
        bi_copy(&t1, &b); /* t1 = b */
        t2 = bi_mod(&a, &b); /* t2 = a mod b */
        bi_free(&b);
        bi_copy(&b, &t2);  /* b = t2 */
        bi_free(&a);
        bi_copy(&a, &t1); /* a = t1 */
        // printf("a "); print_bigint(&a); puts("");
        // printf("b "); print_bigint(&b); puts("");
    }

    /* a is gcd of numer & denom */
    if (!bi_eq(&a, &one)) {
        bi_free(&t1);
        t1 = bi_div(&x->numer, &a);
        bi_free(&x->numer);
        x->numer = t1;
        bi_free(&t2);
        t2 = bi_div(&x->denom, &a);
        bi_free(&x->denom);
        x->denom = t2;
        /* dont need to free t1 and t2 because they are own by x now */
    }
    else {
        bi_free(&t1);
        bi_free(&t2);
    }
    bi_free(&one);
}

int
number_eq(number_t* a, number_t* b) {
    if (a->zero == b->zero) {
        return 1;
    }
    /*  nan != naything */
    if (a->nan || b->nan) {
        return 0;
    }
    return (a->sign == b->sign
        && bi_eq(&a->numer, &b->numer) && bi_eq(&a->denom, &b->denom));
}

int
number_lt(number_t* a, number_t* b) {
    /* obvious cases */
    if ((a->sign != b->sign)) {
        return a->sign;
    }
    /* if one of them is nan: always false */
    if (a->nan || b->nan) {
        return 0;
    }

    if (bi_eq(&a->denom, &b->denom)) {
        if (a->sign) {
            return bi_lt(&b->numer, &a->numer);
        }
        else {
            return bi_lt(&a->numer, &b->numer);
        }
    }
    if (bi_eq(&a->numer, &b->numer)) {
        if (a->sign) {
            return bi_lt(&a->denom, &b->denom);
        }
        else {
            return bi_lt(&b->denom, &a->denom);
        }
    }
    bigint_t l = bi_mul(&a->numer, &b->denom),
        r = bi_mul(&b->numer, &a->denom);
    int res = bi_lt(&l, &r);
    bi_free(&l);
    bi_free(&r);
    return res;
}


number_t
number_add(number_t* a, number_t* b) {
    number_t res = EMPTY_NUMBER();
    bigint_t n, d, t1, t2;
    if (a->nan || b->nan) {
        return NAN_NUMBER();
    }
    t1 = bi_mul(&a->numer, &b->denom);
    t1.sign = a->sign;
    t2 = bi_mul(&b->numer, &a->denom);
    t2.sign = b->sign;
    n = bi_add(&t1, &t2);
    d = bi_mul(&a->denom, &b->denom);
    bi_free(&t1);
    bi_free(&t2);
    res.numer = n;
    res.denom = d;
    number_normalize(&res);
    return res;
}

number_t
number_sub(number_t* a, number_t* b) {
    number_t res = EMPTY_NUMBER();
    bigint_t n, d, t1, t2;
    if (a->nan || b->nan) {
        return NAN_NUMBER();
    }
    t1 = bi_mul(&a->numer, &b->denom);
    t1.sign = a->sign;
    t2 = bi_mul(&b->numer, &a->denom);
    t2.sign = b->sign;
    n = bi_sub(&t1, &t2);
    d = bi_mul(&a->denom, &b->denom);
    bi_free(&t1);
    bi_free(&t2);
    bi_copy(&res.numer, &n);
    bi_copy(&res.denom, &d);
    number_normalize(&res);
    return res;
}

number_t
number_mul(number_t* a, number_t* b) {
    number_t res = EMPTY_NUMBER();
    bigint_t n, d;
    if (a->nan || b->nan) {
        return NAN_NUMBER();
    }
    n = bi_mul(&a->numer, &b->numer);
    d = bi_mul(&a->denom, &b->denom);
    res.sign = a->sign != b->sign;
    res.numer = n;
    res.denom = d;
    number_normalize(&res);
    return res;
}

number_t
number_div(number_t* a, number_t* b) {
    number_t res = EMPTY_NUMBER();
    bigint_t n, d;
    if (a->nan || b->nan) {
        return NAN_NUMBER();
    }
    n = bi_mul(&a->numer, &b->denom);
    d = bi_mul(&a->denom, &b->numer);
    res.sign = a->sign != b->sign;
    res.numer = n;
    res.denom = d;
    number_normalize(&res);
    return res;
}

number_t
number_mod(number_t* a, number_t* b) {
    number_t res = EMPTY_NUMBER();
    bigint_t n, d, t1, t2;
    t1 = bi_mul(&a->numer, &b->denom);
    t1.sign = a->sign;
    t2 = bi_mul(&b->numer, &a->denom);
    t2.sign = b->sign;
    n = bi_mod(&t1, &t2);
    d = bi_mul(&a->denom, &b->denom);
    bi_free(&t1);
    bi_free(&t2);
    res.numer = n;
    res.denom = d;
    number_normalize(&res);
    return res;
}


int print_number_struct(number_t* x) {
    printf("[Number] sign=%u zero=%u nan=%u\n", x->sign, x->zero, x->nan);
    printf("\tnumer="); print_bigint(&x->numer); puts("");
    printf("\tdenom="); print_bigint(&x->denom); puts("");
    return 0;
}

int
print_number_frac(number_t* x) {
    int i, printed_byte_count = 0;
    printed_byte_count += printf("[Number] ");
    if (x->nan & NUMBER_FLAG_NAN) {
        printed_byte_count += printf("NaN");
        return printed_byte_count;
    }
    putchar('(');
    if (x->sign) {
        putchar('-');
        printed_byte_count++;
    }
    printed_byte_count += print_bigint_dec(&x->numer);
    printf(", ");
    printed_byte_count += print_bigint_dec(&x->denom);
    putchar(')');
    return printed_byte_count + 4;
}

int
print_number_dec(number_t* x, int precision) {
    int i, n_exp, d_exp, m, e;
    dynarr_t n_str, d_str, res_str = new_dynarr(1);
    char *n_cstr, *d_cstr, *res_cstr;
    char dot = '.' - '0';
    bigint_t ten_to_abs_m, one = ONE_BIGINT();
    number_t _x, ten = number_from_str("10"), ten_to_m = EMPTY_NUMBER(),
        t = EMPTY_NUMBER(), q = EMPTY_NUMBER(), r = EMPTY_NUMBER();

    if (x->zero) {
        return printf("[Number] 0");
    }

    /* find the lowest m such that 10^m >= abs(n/d) */
    n_str = bigint_to_dec_string(&x->numer);
    d_str = bigint_to_dec_string(&x->denom);
    n_cstr = to_str(&n_str);
    d_cstr = to_str(&d_str);
    n_exp = strlen(n_cstr);
    d_exp = strlen(d_cstr);
    m = n_exp - d_exp + (strcmp(n_cstr, d_cstr) >= 0);
    e = m - precision;
    free(n_cstr);
    free(d_cstr);

    /* round the number to 10^(m - precision):
       for each exponent i := m-1 ~ precision,
         q = x - (x / 10^(m-i)) % 1
         push q into digit stack
         x -= q * 10^(m-i)
    */
    i = 1;
    ten_to_abs_m = bigint_from_tens_power((m < 0) ? -m - i : m - i);
    if (m < 0) {
        ten_to_m.numer = ONE_BIGINT();
        ten_to_m.denom = ten_to_abs_m;
    }
    else {
        ten_to_m.numer = ten_to_abs_m;
        ten_to_m.denom = ONE_BIGINT();
    }
    number_copy(&_x, x);
    while (1) {
        number_free(&r);
        number_free(&t);
        number_free(&q);
        r = number_mod(&_x, &ten_to_m);
        t = number_sub(&_x, &r);
        q = number_div(&t, &ten_to_m);
        // printf("_x   "); print_number_struct(&_x); puts("");
        // printf("10^m "); print_number_struct(&ten_to_m); puts("");
        // printf("q    "); print_number_struct(&q); puts("");
        // printf("r    "); print_number_struct(&r); puts("");
        if (q.numer.size != 1 || q.numer.digit[0] >= 10) {
            printf("print_number_dec: q's numer >= 10\n");
            exit(1);
        }
        append(&res_str, &(q.numer.digit[0]));
        i++;
        if (m - i == -1) {
            append(&res_str, &dot);
        }
        if (i > precision) {
            break;
        }
        /* x -= q * 10^(m-i) */
        number_free(&r);
        r = number_mul(&q, &ten_to_m);
        // printf("q*10^(m-i)   "); print_number_struct(&r); puts("");
        number_free(&q);
        q = number_sub(&_x, &r);
        // printf("x-q*10^(m-i) "); print_number_struct(&q); puts("");
        number_free(&_x);
        number_copy(&_x, &q);
        /* ten_to_m /= 10 */
        number_free(&t);
        t = number_div(&ten_to_m, &ten);
        number_free(&ten_to_m);
        number_copy(&ten_to_m, &t);
    }
    number_free(&t);
    res_cstr = to_str(&res_str);
    for (i = 0; i < res_str.size; i++) {
        res_cstr[i] += '0';
    }
    return printf("[Number] %s", res_cstr);
}

number_t
number_from_str(const char* str) {
    number_t n = EMPTY_NUMBER();
    size_t str_length = strlen(str);
    u32 i, j, dot_pos = str_length, is_neg = 0, is_less_one = 0;
    
    if (str[0] == '-') {
        str_length--;
        str++;
        is_neg = 1;
    }
    if (str[0] == '0') {
        if (str_length == 2) {
            return ZERO_NUMBER();
        }
        if (str[1] == 'b' || str[1] == 'x') {
            n.numer = bigint_from_str(str);
            n.denom = ONE_BIGINT();
            n.sign = is_neg;
            return n;
        }
        else if (str[1] == '.') {
            is_less_one = 1;
        }
        else {
            printf("number_from_str: bad format\n");
            return NAN_NUMBER();
        }
    }

    char* str_no_dot = (char*) malloc(str_length + 1);
    i = j = 0;
    for (i = 0; i < str_length; i++) {
        if (is_less_one && i == 0) {
            continue;
        }
        if (str[i] != '.') {
            str_no_dot[j] = str[i];
            j++;
        }
        else {
            dot_pos = i + 1;
        }
    }
    str_no_dot[j] = '\0';
    // printf("%s %d %d\n", str_no_dot, dot_pos, str_length - dot_pos);
    n.sign = is_neg;
    n.numer = bigint_from_str(str_no_dot);
    n.denom = bigint_from_tens_power(str_length - dot_pos);
    // print_bigint_dec(&n.numer); puts("");
    // print_bigint_dec(&n.denom); puts("");
    free(str_no_dot);
    number_normalize(&n);
    return n;
}

number_t
number_from_bigint(bigint_t* n);
