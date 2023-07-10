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

#ifndef DTOSTRF_H
#define DTOSTRF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <float.h>

/*
 *  ======== DTOSTRF_MAXPREC ========
 *  Maximum precision supported by dtostrf()
 */
#define DTOSTRF_MAXPREC 9

/*
 *  ======== DTOSTRF_BUFLEN ========
 *  Maximum number of characters output by dtostrf()
 *
 *  The expression below computes the maximum output as:
 *      (max integral digits in a float) + max prec + (sign, '.', and '\0')
 */
#define DTOSTRF_BUFLEN   ((FLT_MAX_10_EXP + 1) + DTOSTRF_MAXPREC + 3)  

/*
 *  ======== dtostrf ========
 *  Converts a floating point number to a decimal string
 *
 *  This function is roughly equivalent to:
 *      sprintf(sout, "%*.*f", width, prec, d)
 *
 *  EXCEPT: to keep the code footprint and stack usage to a level appropriate
 *  for embedded systems, we
 *      o limit the precision to just 9 or less
 *      o truncate d to be a float _before_ the conversion
 *      o ignore width
 *
 *  Params:
 *      d     - the value to convert into an ascii string
 *
 *      width - currently ignored (should be the minimum number of
 *              characters to output)
 *
 *      prec  - the minimum number of _significant_ digits after the 
 *              decimal point, _not_ the number of digits, leading 0's are
 *              not counted.  For example, ".00012" has just two significant
 *              digits after the decimal point, not four.
 *
 *              If prec > 9, a precision of exactly 9 will be used.
 *              
 *      sout  - the output string buffer.  This buffer must always be large
 *              enough to hold at least DTOSTRF_BUFLEN characters.
 *  
 *  Returns:
 *      sout, after copying the converted result into sout.  dtostrf()
 *      always '\0' terminates the output string and never writes more than
 *      DTOSTRF_BUFLEN characters (including the terminal '\0')
 */
char *dtostrf (double val, signed char width, unsigned char prec, char *sout);

#ifdef __cplusplus
}
#endif

#endif
