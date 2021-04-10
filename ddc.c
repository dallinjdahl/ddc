#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DEPTH 16
uint64_t sa[DEPTH];
uint64_t sb[DEPTH];
uint64_t mask = 0xffff;

char c;

char out[65] = {0};

uint8_t inbase = 16, outbase = 16, sign = 0;

uint16_t ax = 0, bx = 0;

int8_t dig(char c) {
	int8_t res = c;
	if (res < '0') return -1;
	res -= '0';
	if (res > 9) res -= ('A'-'9')-1;
	if (res > 15) return -1;
	return res;
}

char inext() {
	int res = getchar();
	if (res == EOF) exit(0);
	return (char) res;
}

char (*next)() = inext;

void num() {
	uint8_t neg = 1;
	uint64_t res = 0;
	int64_t d = 0;
	if (c == '_') {
		neg = -1;
		c = next();
	}
	while((d = dig(c)) >= 0) {
		res *= inbase;
		res += d;
		c = next();
	}
	res *= neg;
	sa[ax++] = res;
}

void obase() {
	int64_t val = sa[--ax];
	if(val < 0) {
		val *= -1;
	}
	outbase = val;
	c = next();
}

void ibase() {
	int64_t val = sa[--ax];
	if(val < 0) {
		val *= -1;
	}
	inbase = val;
	c = next();
}

void print() {
	uint64_t val = sa[ax-1];
	int64_t sval = val|~mask;
	uint64_t rem = 0;
	if (sign && sval < 0) {
		putchar('_');
		val = sval*-1;
	}
	char d;
	uint8_t cursor = 65;
	lldiv_t inter = {0, 0};
	while(val) {
		//note that this has to use the modulus and division
		//operators, since the lldiv function only does signed
		//division
		rem = val % outbase;
		val /= outbase;
		d = rem + '0';
		if (d > '9') d += 'A'-'9'-1;
		out[--cursor] = d;
	}
	fputs(out+cursor, stdout);
	c = next();
}

void wid() {
	int8_t w = sa[--ax];
	sign = 0;
	if(w < 0) {
		w *= -1;
		sign = 1;
	}
	uint64_t nmask = (1 << w) - 1;
	if (w == 64) nmask = -1;
	if (w > 64) nmask = mask;
	if(nmask > mask && sign) {
		mask ^= mask >> 1;
		for(uint8_t i = 0; i < ax; i++) {
			sa[i] = (sa[i]^mask)-mask;
			sa[i] &= nmask;
		}
		for(uint8_t i = 0; i < bx; i++) {
			sb[i] = (sb[i]^mask)-mask;
			sb[i] &= nmask;
		}
		mask = nmask;
		c = next();
		return;
	}
	for(uint8_t i = 0; i < ax; i++) {
		sa[i] &= nmask;
	}
	for(uint8_t i = 0; i < bx; i++) {
		sb[i] &= nmask;
	}
	mask = nmask;
	c = next();
	return;
}

uint64_t ipow(uint64_t base, uint64_t exp) {
	uint64_t result = 1;
	while (1) {
		if (exp & 1) result *= base;
		exp >>= 1;
		if (!exp) break;
		base *= base;
	}
	return result;
}

//note: don't put in expressions with side-effects for a or b
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

void slice() {
	uint64_t msbin = max(sa[ax-1], sa[ax-2]);
	uint64_t lsbin = min(sa[ax-1], sa[ax-2]);
	uint64_t val = sa[ax-3];
	ax--;
	ax--;
	uint64_t msbmsk = (1 << (msbin+1)) - 1;
	if (msbin+1 == 64) msbmsk = -1;
	uint64_t lsbmsk = (1 << lsbin) - 1;
	uint64_t ms = msbmsk ^ lsbmsk;
	val &= ms;
	val >>= lsbin;
	sa[ax-1] = val;
	c = next();
}

void comment() {
	while((c = next()) != '\n');
}

int main(int argc, char **argv) {
	uint64_t tmp = 0; 
	c = next();
	while(1) {
	if(dig(c) >= 0 || c == '_') {
		num();
	}
	switch (c) {
		case 'q':
			exit(0);
		case 'd':
			sa[ax] = sa[ax-1];
			ax++;
			c = next();
			continue;
		case 'p':
			print();
			putchar('\n');
			continue;
		case 'n':
			print();
			ax--;
			continue;
		case 'o':
			obase();
			continue;
		case 'i':
			ibase();
			continue;
		case 'k':
			wid();
			continue;
		case 'r':
			tmp = sa[ax-1];
			sa[ax-1] = sa[ax-2];
			sa[ax-2] = tmp;
			c = next();
			continue;
		case 'v':
			sa[ax] = sa[ax-2];
			ax++;
			c = next();
			continue;
		case '+':
			ax--;
			sa[ax-1] += sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '-':
			ax--;
			sa[ax-1] -= sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '*':
			ax--;
			sa[ax-1] *= sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '^':
			ax--;
			sa[ax-1] = ipow(sa[ax-1], sa[ax]);
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '|':
			ax--;
			sa[ax-1] |= sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '`':
			ax--;
			sa[ax-1] ^= sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '&':
			ax--;
			sa[ax-1] &= sa[ax];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '~':
			sa[ax-1] = ~sa[ax-1];
			sa[ax-1] &= mask;
			c = next();
			continue;
		case '#':
			comment();
			continue;
		case '>':
			ax--;
			sb[bx] = sa[ax];
			bx++;
			c = next();
			continue;
		case '<':
			bx--;
			sa[ax] = sb[bx];
			ax++;
			c = next();
			continue;
		case 'b':
			slice();
			continue;
		case '\n':
		case ' ':
		case '\t':
			c = next();
			continue;
		default:
			fputs("?\n", stdout);
			c = next();
	}}
	return 0;
}
