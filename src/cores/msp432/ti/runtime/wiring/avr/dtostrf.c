/****************************************************************************/
/*                                                                          */
/* Copyright (c) 1995-2017 Texas Instruments Incorporated                   */
/* http://www.ti.com/                                                       */
/*                                                                          */
/*  Redistribution and  use in source  and binary forms, with  or without   */
/*  modification,  are permitted provided  that the  following conditions   */
/*  are met:                                                                */
/*                                                                          */
/*     Redistributions  of source  code must  retain the  above copyright   */
/*     notice, this list of conditions and the following disclaimer.        */
/*                                                                          */
/*     Redistributions in binary form  must reproduce the above copyright   */
/*     notice, this  list of conditions  and the following  disclaimer in   */
/*     the  documentation  and/or   other  materials  provided  with  the   */
/*     distribution.                                                        */
/*                                                                          */
/*     Neither the  name of Texas Instruments Incorporated  nor the names   */
/*     of its  contributors may  be used to  endorse or  promote products   */
/*     derived  from   this  software  without   specific  prior  written   */
/*     permission.                                                          */
/*                                                                          */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS   */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT   */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT   */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT   */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT   */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    */
/*                                                                          */
/****************************************************************************/
#include <math.h>
#include <string.h> /* for memcpy() */

#include "dtostrf.h"

#define MAXLONGLEN 20 /* number of decimal digits in 0x7FFFFFFF */

/*
 *  ======== ltoa ========
 *  Convert a long POSITIVE int < 0x7FFFFFFF to ascii
 */
static int ltoa(long val, char *buffer)
{
    char           tempc[MAXLONGLEN];
    register char *bufptr;
    register long  uval = val;

    bufptr = &tempc[MAXLONGLEN];

    do {
        *--bufptr = (uval % 10) + '0';
    } while (uval /= 10);

    uval = (tempc + MAXLONGLEN) - bufptr;
    memcpy(buffer, bufptr, uval);
    return uval;
}

/*
 *  ======== ftoa ========
 *  value  - number to convert
 *  ndigit - number of fractional digits to output
 *  decpt  - output location of decimal point in output buffer
 *  sign   - output 0 if value is positive, non-zero if negative
 */
static char *ftoa(long double value, int ndigit, int *decpt, int *sign, char *out)
{
    int   digits = 0;         /* NUMBER OF DIGITS BEFORE .       */
    char *pos    = out + 1;     
    int   temp;

    out[0] = '0';             /* JUST IN CASE WE ROUND.          */
    ndigit++;                 /* DO ONE EXTRA DIGIT FOR ROUNDING */

    /*--------------------------------------------------------------------*/
    /* PERFORM PRESCALING - MAKE SURE NUMBER HAS NON-0 INTEGRAL PART      */
    /*--------------------------------------------------------------------*/
    if ((*sign = (value < 0))) value = -value;
    while (value > 0x7FFFFFFF) { value /= 10; digits++; }
    while (value && value < 1) { value *= 10; digits--; }

    /*--------------------------------------------------------------------*/
    /* WRITE OUT INTEGRAL PART OF NUMBER.                                 */
    /*--------------------------------------------------------------------*/
    temp = ltoa((long)value, pos);
    pos += temp;
    *decpt = digits + temp; 

    /*--------------------------------------------------------------------*/
    /* ndigit MAY BE AFFECTED BY PRE-SCALING ...                          */
    /*--------------------------------------------------------------------*/
    ndigit += digits;

    /*--------------------------------------------------------------------*/
    /* WRITE OUT FRACTIONAL PART OF SCALED NUMBER                         */
    /*--------------------------------------------------------------------*/
    if (ndigit > 0) {
        do {
            value -= (int)value;
            *pos++ = (int)(value *= 10.0) + '0';
        }
        while (--ndigit);
    }

    /*--------------------------------------------------------------------*/
    /* PERFORM ROUNDING.  NOTE THAT --pos IS CURRENTLY POINTING AT AN     */
    /* EXTRA DIGIT WHICH WAS CONVERTED FOR THIS PURPOSE.                  */
    /*--------------------------------------------------------------------*/
    if (*--pos >= '5') {
       char *ptr = pos;
       while ((*--ptr += 1) > '9') { /* leading '0' ensures this stops */
           *ptr = '0';
       }
       
       if (ptr == out) {    /* if we carried into the leading '0' */
           if (pos > out) { /* need to keep all digits */
               *pos   = 0;
           }
           else {           /* pos == out, nothing but 0's after out[0] */
               out[1] = 0;
           }
          
           *decpt += 1;    /* adjust for the added leading digit */
           return out;
       }
    }

    *pos = 0;
    return out + 1; /* drop the extra '0' added for rounding at the start */
}

/*
 *  ======== dtostrf ========
 *  Portable implementation of dtostrf() needed by energia
 *
 *  This implementation is a re-implementation the "baseline" Arduino
 *  implementation:
 *      sprintf(sout, "%*.*f", width, prec, d)
 *
 *  EXCEPT: to keep the code footprint and stack usage to a level appropriate
 *  for embedded systems, we
 *      o limit the precision to just 9 or less
 *      o truncate d to be a float
 *      o ignore width
 *
 *  This implementation uses a reentrant variant of fcvt() and, as a 
 *  result, it can probably be made much smaller by "fixing" ftoa()
 *  to write the output directly rather than "compressing" leading
 *  zeros and leaving the formatting to the caller.
 */
char *dtostrf(double d, signed char width, unsigned char prec, char *sout)
{
    int decpt, sign;
    char *buf;
    int i;
    char *optr = sout;
    
    /* work buffer[max integral digits + max prec + (sign, '.', and '\0')] */
    char buffer[DTOSTRF_BUFLEN];

    /* limit precision to keep the max size of buffer "small" */
    float f = d; /* truncate range of values converted to that of a float */
    if (prec > DTOSTRF_MAXPREC) {
        prec = DTOSTRF_MAXPREC;
    }

    if (isnan(f)) {
        strcpy(optr, "nan");
    }
    else if (isinf(f)) {
        strcpy(optr, "-inf" + (d < 0 ? 0 : 1));
    }
    else {
        buf = ftoa(f, prec, &decpt, &sign, buffer);
        
        if (sign) {
            *optr++ = '-';
        }

        if (decpt <= 0) {
            *optr++ = '0';
            if (prec) {
                *optr++ = '.';
                decpt = -1 * decpt;
                for (i = decpt > prec ? prec : decpt; i > 0; i--) {
                    *optr++ = '0';
                }
                if (decpt >= prec) {
                    *optr = '\0';
                    return (sout);
                }
            }
            else {
                *optr = '\0';
                return (sout);
            }
        }
        else {
            for (i = decpt; i > 0; i--) {
                *optr++ = *buf++;
            }
            if (prec) {
                *optr++ = '.';
            }
        }
        while (*buf != '\0') {
            *optr++ = *buf++;
        }
        *optr = '\0';
    }

    return (sout);
}
