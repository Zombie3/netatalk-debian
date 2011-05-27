#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <atalk/logger.h>
#include <errno.h>

#include <netatalk/endian.h>

#include <atalk/unicode.h>
#include "ucs2_casetable.h"
#include "precompose.h"
#include "byteorder.h"

/*******************************************************************
 Convert a wide character to upper/lower case.
********************************************************************/
ucs2_t toupper_w(ucs2_t val)
{
	if ( val >= 0x0040 && val <= 0x007F)
		return upcase_table_1[val-0x0040];
	if ( val >= 0x00C0 && val <= 0x02BF)
		return upcase_table_2[val-0x00C0];
	if ( val >= 0x0380 && val <= 0x04FF)
		return upcase_table_3[val-0x0380];
	if ( val >= 0x0540 && val <= 0x05BF)
		return upcase_table_4[val-0x0540];
	if ( val >= 0x1E00 && val <= 0x1FFF)
		return upcase_table_5[val-0x1E00];
	if ( val >= 0x2140 && val <= 0x217F)
		return upcase_table_6[val-0x2140];
	if ( val >= 0x24C0 && val <= 0x24FF)
		return upcase_table_7[val-0x24C0];
	if ( val >= 0xFF40 && val <= 0xFF7F)
		return upcase_table_8[val-0xFF40];

	return (val);
}


ucs2_t tolower_w(ucs2_t val)
{
	if ( val >= 0x0040 && val <= 0x007F)
		return lowcase_table_1[val-0x0040];
	if ( val >= 0x00C0 && val <= 0x023F)
		return lowcase_table_2[val-0x00C0];
	if ( val >= 0x0380 && val <= 0x057F)
		return lowcase_table_3[val-0x0380];
	if ( val >= 0x1E00 && val <= 0x1FFF)
		return lowcase_table_4[val-0x1E00];
	if ( val >= 0x2140 && val <= 0x217F)
		return lowcase_table_5[val-0x2140];
	if ( val >= 0x2480 && val <= 0x24FF)
		return lowcase_table_6[val-0x2480];
	if ( val >= 0xFF00 && val <= 0xFF3F)
		return lowcase_table_7[val-0xFF00];

	return (val);
}

/*******************************************************************
 Convert a string to lower case.
 return True if any char is converted
********************************************************************/
int strlower_w(ucs2_t *s)
{
	int ret = 0;
	while (*s) {
		ucs2_t v = tolower_w(*s);
		if (v != *s) {
			*s = v;
			ret = 1;
		}
		s++;
	}
	return ret;
}

/*******************************************************************
 Convert a string to upper case.
 return True if any char is converted
********************************************************************/
int strupper_w(ucs2_t *s)
{
	int ret = 0;
	while (*s) {
		ucs2_t v = toupper_w(*s);
		if (v != *s) {
			*s = v;
			ret = 1;
		}
		s++;
	}
	return ret;
}


/*******************************************************************
determine if a character is lowercase
********************************************************************/
int islower_w(ucs2_t c)
{
	return ( c == tolower_w(c));
}

/*******************************************************************
determine if a character is uppercase
********************************************************************/
int isupper_w(ucs2_t c)
{
	return ( c == toupper_w(c));
}


/*******************************************************************
 Count the number of characters in a ucs2_t string.
********************************************************************/
size_t strlen_w(const ucs2_t *src)
{
	size_t len;

	for(len = 0; *src++; len++) ;

	return len;
}

/*******************************************************************
 Count up to max number of characters in a ucs2_t string.
********************************************************************/
size_t strnlen_w(const ucs2_t *src, size_t max)
{
	size_t len;

	for(len = 0; *src++ && (len < max); len++) ;

	return len;
}

/*******************************************************************
wide strchr()
********************************************************************/
ucs2_t *strchr_w(const ucs2_t *s, ucs2_t c)
{
	while (*s != 0) {
		if (c == *s) return (ucs2_t *)s;
		s++;
	}
	if (c == *s) return (ucs2_t *)s;

	return NULL;
}

ucs2_t *strcasechr_w(const ucs2_t *s, ucs2_t c)
{
	while (*s != 0) {
/*		LOG(log_debug, logtype_default, "Comparing %X to %X (%X - %X)", c, *s, toupper_w(c), toupper_w(*s));*/
		if (toupper_w(c) == toupper_w(*s)) return (ucs2_t *)s;
		s++;
	}
	if (c == *s) return (ucs2_t *)s;

	return NULL;
}


int strcmp_w(const ucs2_t *a, const ucs2_t *b)
{
	while (*b && *a == *b) { a++; b++; }
	return (*a - *b);
	/* warning: if *a != *b and both are not 0 we retrun a random
	   greater or lesser than 0 number not realted to which
	   string is longer */
}

int strncmp_w(const ucs2_t *a, const ucs2_t *b, size_t len)
{
	size_t n = 0;
	while ((n < len) && *b && *a == *b) { a++; b++; n++;}
	return (len - n)?(*a - *b):0;
}

/*******************************************************************
wide strstr()
********************************************************************/
ucs2_t *strstr_w(const ucs2_t *s, const ucs2_t *ins)
{
	ucs2_t *r;
	size_t slen, inslen;

	if (!s || !*s || !ins || !*ins) return NULL;
	slen = strlen_w(s);
	inslen = strlen_w(ins);
	r = (ucs2_t *)s;
	while ((r = strchr_w(r, *ins))) {
		if (strncmp_w(r, ins, inslen) == 0) return r;
		r++;
	}
	return NULL;
}

ucs2_t *strcasestr_w(const ucs2_t *s, const ucs2_t *ins)
{
	ucs2_t *r;
	size_t slen, inslen;

	if (!s || !*s || !ins || !*ins) return NULL;
	slen = strlen_w(s);
	inslen = strlen_w(ins);
	r = (ucs2_t *)s;
	while ((r = strcasechr_w(r, *ins))) {
		if (strncasecmp_w(r, ins, inslen) == 0) return r;
		r++;
	}
	return NULL;
}




/*******************************************************************
case insensitive string comparison
********************************************************************/
int strcasecmp_w(const ucs2_t *a, const ucs2_t *b)
{
	while (*b && toupper_w(*a) == toupper_w(*b)) { a++; b++; }
	return (tolower_w(*a) - tolower_w(*b));
}

/*******************************************************************
case insensitive string comparison, lenght limited
********************************************************************/
int strncasecmp_w(const ucs2_t *a, const ucs2_t *b, size_t len)
{
	size_t n = 0;
	while ((n < len) && *b && (toupper_w(*a) == toupper_w(*b))) { a++; b++; n++; }
	return (len - n)?(tolower_w(*a) - tolower_w(*b)):0;
}

/*******************************************************************
duplicate string
********************************************************************/
/* if len == 0 then duplicate the whole string */
ucs2_t *strndup_w(const ucs2_t *src, size_t len)
{
	ucs2_t *dest;

	if (!len) len = strlen_w(src);
	dest = (ucs2_t *)malloc((len + 1) * sizeof(ucs2_t));
	if (!dest) {
		LOG (log_error, logtype_default, "strdup_w: out of memory!");
		return NULL;
	}

	memcpy(dest, src, len * sizeof(ucs2_t));
	dest[len] = 0;

	return dest;
}

ucs2_t *strdup_w(const ucs2_t *src)
{
	return strndup_w(src, 0);
}

/*******************************************************************
copy a string with max len
********************************************************************/

ucs2_t *strncpy_w(ucs2_t *dest, const ucs2_t *src, const size_t max)
{
	size_t len;

	if (!dest || !src) return NULL;

	for (len = 0; (src[len] != 0) && (len < max); len++)
		dest[len] = src[len];
	while (len < max)
		dest[len++] = 0;

	return dest;
}


/*******************************************************************
append a string of len bytes and add a terminator
********************************************************************/

ucs2_t *strncat_w(ucs2_t *dest, const ucs2_t *src, const size_t max)
{
	size_t start;
	size_t len;

	if (!dest || !src) return NULL;

	start = strlen_w(dest);
	len = strnlen_w(src, max);

	memcpy(&dest[start], src, len*sizeof(ucs2_t));
	dest[start+len] = 0;

	return dest;
}


ucs2_t *strcat_w(ucs2_t *dest, const ucs2_t *src)
{
	size_t start;
	size_t len;

	if (!dest || !src) return NULL;

	start = strlen_w(dest);
	len = strlen_w(src);

	memcpy(&dest[start], src, len*sizeof(ucs2_t));
	dest[start+len] = 0;

	return dest;
}


/*******************************************************************
binary search for pre|decomposition
********************************************************************/

static ucs2_t do_precomposition(unsigned int base, unsigned int comb) 
{
	int min = 0;
	int max = PRECOMP_COUNT - 1;
	int mid;
	u_int32_t sought = (base << 16) | comb, that;

	/* binary search */
	while (max >= min) {
		mid = (min + max) / 2;
		that = (precompositions[mid].base << 16) | (precompositions[mid].comb);
		if (that < sought) {
			min = mid + 1;
		} else if (that > sought) {
			max = mid - 1;
		} else {
			return precompositions[mid].replacement;
		}
	}
	/* no match */
	return 0;
}

/* ------------------------ */
static u_int32_t do_precomposition_sp(unsigned int base_sp, unsigned int comb_sp) 
{
	int min = 0;
	int max = PRECOMP_SP_COUNT - 1;
	int mid;
	u_int64_t sought_sp = ((u_int64_t)base_sp << 32) | (u_int64_t)comb_sp, that_sp;

	/* binary search */
	while (max >= min) {
		mid = (min + max) / 2;
		that_sp = ((u_int64_t)precompositions_sp[mid].base_sp << 32) | ((u_int64_t)precompositions_sp[mid].comb_sp);
		if (that_sp < sought_sp) {
			min = mid + 1;
		} else if (that_sp > sought_sp) {
			max = mid - 1;
		} else {
			return precompositions_sp[mid].replacement_sp;
		}
	}
	/* no match */
	return 0;
}

/* -------------------------- */
static u_int32_t do_decomposition(ucs2_t base) 
{
	int min = 0;
	int max = DECOMP_COUNT - 1;
	int mid;
	u_int32_t sought = base;
	u_int32_t result, that;

	/* binary search */
	while (max >= min) {
		mid = (min + max) / 2;
		that = decompositions[mid].replacement;
		if (that < sought) {
			min = mid + 1;
		} else if (that > sought) {
			max = mid - 1;
		} else {
			result = (decompositions[mid].base << 16) | (decompositions[mid].comb);
			return result;
		}
	}
	/* no match */
	return 0;
}

/* -------------------------- */
static u_int64_t do_decomposition_sp(unsigned int base_sp) 
{
	int min = 0;
	int max = DECOMP_SP_COUNT - 1;
	int mid;
	u_int32_t sought_sp = base_sp;
	u_int32_t that_sp;
	u_int64_t result_sp;

	/* binary search */
	while (max >= min) {
		mid = (min + max) / 2;
		that_sp = decompositions_sp[mid].replacement_sp;
		if (that_sp < sought_sp) {
			min = mid + 1;
		} else if (that_sp > sought_sp) {
			max = mid - 1;
		} else {
			result_sp = ((u_int64_t)decompositions_sp[mid].base_sp << 32) | ((u_int64_t)decompositions_sp[mid].comb_sp);
			return result_sp;
		}
	}
	/* no match */
	return 0;
}

/*******************************************************************
pre|decomposition

   we can't use static, this stuff needs to be reentrant
   static char comp[MAXPATHLEN +1];

   We don't implement Singleton and Canonical Ordering.
   We ignore CompositionExclusions.txt.
   because they cause the problem of the roundtrip
   such as Dancing Icon.

   exclude U2000-U2FFF, UFE30-UFE4F and U2F800-U2FA1F ranges
   in precompose.h from composition according to AFP 3.x spec
********************************************************************/

size_t precompose_w (ucs2_t *name, size_t inplen, ucs2_t *comp, size_t *outlen)
{
	size_t i;
	ucs2_t base, comb;
	u_int32_t base_sp, comb_sp;
	ucs2_t *in, *out;
	ucs2_t lindex, vindex;
	ucs2_t result;
	u_int32_t result_sp;
	size_t o_len = *outlen;
	
	if (!inplen || (inplen & 1) || inplen > o_len)
		return (size_t)-1;
	
	i = 0;
	in  = name;
	out = comp;
	
	base = *in;
	while (*outlen > 2) {
		i += 2;
		in++;

		if (i == inplen) {
			*out = base;
			out++;
			*out = 0;
			*outlen -= 2;
			return o_len - *outlen;
		}

		comb = *in;
		result = 0;

		/* Non-Combination Character */
		if (comb < 0x300) ;
		
		/* Unicode Standard Annex #15 A10.3 Hangul Composition */
		/* Step 1 <L,V> */
		else if ((VBASE <= comb) && (comb <= VBASE + VCOUNT)) {
			if ((LBASE <= base) && (base < LBASE + LCOUNT)) {
				result = 1;
				lindex = base - LBASE;
				vindex = comb - VBASE;
				base = SBASE + (lindex * VCOUNT + vindex) * TCOUNT;
			}
		}
		
		/* Step 2 <LV,T> */
		else if ((TBASE < comb) && (comb < TBASE + TCOUNT)) {
			if ((SBASE <= base) && (base < SBASE + SCOUNT) && (((base - SBASE) % TCOUNT) == 0)) {
				result = 1;
				base += comb - TBASE;
			}
		}
		
		/* Binary Search for Surrogate Pair */
		else if ((0xD800 <= base) && (base < 0xDC00)) {
			if ((0xDC00 <= comb) && (comb < 0xE000) && (i + 4 <= inplen)) {
				base_sp = ((u_int32_t)base << 16) | (u_int32_t)comb;
				do {
					comb_sp = ((u_int32_t)in[1] << 16) | (u_int32_t)in[2];
					if (result_sp = do_precomposition_sp(base_sp, comb_sp)) {
						base_sp = result_sp;
						i += 4;
						in +=2;
					}
				} while ((i + 4 <= inplen) && result_sp) ;

				*out = base_sp >> 16;
				out++;
				*outlen -= 2;

				if (*outlen <= 2) {
					errno = E2BIG;
					return (size_t)-1;
				}

				*out = base_sp & 0xFFFF;
				out++;
				*outlen -= 2;

				i += 2;
				in++;
				base = *in;

				result = 1;
			}
		}

		/* Binary Search for BMP */
		else if (result = do_precomposition(base, comb)) {
			base = result;
		}
		
		if (!result) {
			*out = base;
			out++;
			*outlen -= 2;
			base = comb;
		}
	}

	errno = E2BIG;
	return (size_t)-1;
}

/* --------------- */
size_t decompose_w (ucs2_t *name, size_t inplen, ucs2_t *comp, size_t *outlen)
{
	size_t i;
	size_t comblen;
	ucs2_t base, comb[COMBBUFLEN];
	u_int32_t base_sp;
	ucs2_t sindex, tjamo;
	ucs2_t *in, *out;
	unsigned int result;
	u_int64_t result_sp;
	size_t o_len = *outlen;

	if (!inplen || (inplen & 1))
		return (size_t)-1;
	i = 0;
	in  = name;
	out = comp;

	while (i < inplen) {
		base = *in;
		comblen = 0;
		
		/* check ASCII first. this is frequent. */
		if (base <= 0x007f) ;
		
		/* Unicode Standard Annex #15 A10.2 Hangul Decomposition */
		else if ((SBASE <= base) && (base < SBASE + SCOUNT)) {
			sindex = base - SBASE;
			base = LBASE + sindex / NCOUNT;
			comb[COMBBUFLEN-2] = VBASE + (sindex % NCOUNT) / TCOUNT;
			
			/* <L,V> */
			if ((tjamo = TBASE + sindex % TCOUNT) == TBASE) {
				comb[COMBBUFLEN-1] = comb[COMBBUFLEN-2];
				comblen = 1;
			}
			
			/* <L,V,T> */
			else {
				comb[COMBBUFLEN-1] = tjamo;
				comblen = 2;
			}
		}
		
		/* Binary Search for Surrogate Pair */
		else if ((0xD800 <= base) && (base < 0xDC00)) {
			if (i + 2 < inplen) {
				base_sp =  ((u_int32_t)base << 16) | (u_int32_t)in[1];
				do {
					if ( !(result_sp = do_decomposition_sp(base_sp))) break;
					comblen += 2;
					base_sp = result_sp >> 32;
					comb[COMBBUFLEN-comblen] = (result_sp >> 16) & 0xFFFF;  /* hi */
					comb[COMBBUFLEN-comblen+1] = result_sp & 0xFFFF;        /* lo */
				} while (comblen < MAXCOMBSPLEN);

				if (*outlen < (comblen + 1) << 1) {
					errno = E2BIG;
					return (size_t)-1;
				}

				*out = base_sp >> 16;   /* hi */
				out++;
				*outlen -= 2;
				
				base = base_sp & 0xFFFF; /* lo */
				
				i += 2;
				in++;
			}
		}
			
		/* Binary Search for BMP */
		else {
			do {
				if ( !(result = do_decomposition(base))) break;
				comblen++;
				base = result  >> 16;
				comb[COMBBUFLEN-comblen] = result & 0xFFFF;
			} while ((0x007f < base) && (comblen < MAXCOMBLEN));
		}
		
		if (*outlen < (comblen + 1) << 1) {
			errno = E2BIG;
			return (size_t)-1;
		}
		
		*out = base;
		out++;
		*outlen -= 2;
		
		while ( comblen > 0 ) {
			*out = comb[COMBBUFLEN-comblen];
			out++;
			*outlen -= 2;
			comblen--;
		}
		
		i += 2;
		in++;
	}
	
	*out = 0;
	return o_len-*outlen;
}

/*******************************************************************
length of UTF-8 character and string
********************************************************************/

size_t utf8_charlen ( char* utf8 )
{
	unsigned char *p;

	p = (unsigned char*) utf8;
	
	if ( *p < 0x80 )
		return (1);
	else if ( *p > 0xC1 && *p < 0xe0 && *(p+1) > 0x7f && *(p+1) < 0xC0)
		return (2);
	else if ( *p == 0xe0 && *(p+1) > 0x9f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0)
		return (3);
	else if ( *p > 0xe0  && *p < 0xf0 && *(p+1) > 0x7f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0)
		return (3);
	else if ( *p == 0xf0 && *(p+1) > 0x8f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
		return (4);
	else if ( *p > 0xf0 && *p < 0xf4 && *(p+1) > 0x7f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
		return (4);
	else if ( *p == 0xf4 && *(p+1) > 0x7f && *(p+1) < 0x90 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
		return (4);
	else
		return ((size_t) -1);
}


size_t utf8_strlen_validate ( char * utf8 )
{
	size_t len;
	unsigned char *p;

	p = (unsigned char*) utf8;
	len = 0;

	/* see http://www.unicode.org/unicode/reports/tr27/ for an explanation */

	while ( *p != '\0')
	{
		if ( *p < 0x80 )
			p++;

		else if ( *p > 0xC1 && *p < 0xe0 && *(p+1) > 0x7f && *(p+1) < 0xC0)
			p += 2;

		else if ( *p == 0xe0 && *(p+1) > 0x9f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0)
			p += 3;

		else if ( *p > 0xe0  && *p < 0xf0 && *(p+1) > 0x7f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0)
			p += 3;

		else if ( *p == 0xf0 && *(p+1) > 0x8f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
			p += 4;

		else if ( *p > 0xf0 && *p < 0xf4 && *(p+1) > 0x7f && *(p+1) < 0xc0 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
			p += 4;

		else if ( *p == 0xf4 && *(p+1) > 0x7f && *(p+1) < 0x90 && *(p+2) > 0x7f && *(p+2) < 0xc0 && *(p+3) > 0x7f && *(p+3) < 0xc0 )
			p += 4;

		else
			return ((size_t) -1);

		len++;
	}

	return (len);
}
