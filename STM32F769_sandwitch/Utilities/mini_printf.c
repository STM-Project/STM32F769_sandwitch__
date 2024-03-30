/*
 * mini_printf.c
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */


/*
 * The Minimal snprintf() implementation
 *
 * Copyright (c) 2013,2014 Michal Ludvig <michal@logix.cz>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the auhor nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ----
 *
 * This is a minimal snprintf() implementation optimised
 * for embedded systems with a very limited program memory.
 * mini_snprintf() doesn't support _all_ the formatting
 * the glibc does but on the other hand is a lot smaller.
 * Here are some numbers from my STM32 project (.bin file size):
 *      no snprintf():      10768 bytes
 *      mini snprintf():    11420 bytes     (+  652 bytes)
 *      glibc snprintf():   34860 bytes     (+24092 bytes)
 * Wasting nearly 24kB of memory just for snprintf() on
 * a chip with 32kB flash is crazy. Use mini_snprintf() instead.
 *
 */

#include <string.h>
#include <stdarg.h>
#include "mini_printf.h"

unsigned int mini_strlen(const char *s)
{
	unsigned int len = 0;
	while (s[len] != '\0') len++;
	return len;
}

void StrBuffCopy(char *dest, char *src)
{
	int i,len=mini_strlen(src);
	for(i=0;i<len;++i) dest[i]=src[i];	 dest[i]=0;
}
void StrBuffCopylimit(char *dest, char *src, int lenSrc)
{
	int i;
	for(i=0;i<lenSrc;++i) dest[i]=src[i];	 dest[i]=0;
}

static unsigned int mini_itoa(int value, unsigned int radix, unsigned int uppercase, unsigned int unsig, char *buffer, unsigned int zero_pad)
{
	char	*pbuffer = buffer;
	int	negative = 0;
	unsigned int	i, len;

	/* No support for unusual radixes. */
	if (radix > 16)
		return 0;

	if (value < 0 && !unsig) {
		negative = 1;
		value = -value;
	}

	/* This builds the string back to front ... */
	do {
		int digit = value % radix;
		*(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
	} while (value > 0);

	for (i = (pbuffer - buffer); i < zero_pad; i++)
		*(pbuffer++) = '0';

	if (negative)
		*(pbuffer++) = '-';

	*(pbuffer) = '\0';

	/* ... now we reverse it (could do it recursively but will
	 * conserve the stack space) */
	len = (pbuffer - buffer);
	for (i = 0; i < len / 2; i++) {
		char j = buffer[i];
		buffer[i] = buffer[len-i-1];
		buffer[len-i-1] = j;
	}

	return len;
}

int mini_vsnprintf(char *buffer, unsigned int buffer_len, const char *fmt, va_list va)
{
	char *pbuffer = buffer;
	char bf[24];
	char ch;

	int _putc(char ch)
	{
		if ((unsigned int)((pbuffer - buffer) + 1) >= buffer_len)
			return 0;
		*(pbuffer++) = ch;
		*(pbuffer) = '\0';
		return 1;
	}

	int _puts(char *s, unsigned int len)
	{
		unsigned int i;

		if (buffer_len - (pbuffer - buffer) - 1 < len)
			len = buffer_len - (pbuffer - buffer) - 1;

		/* Copy to buffer */
		for (i = 0; i < len; i++)
			*(pbuffer++) = s[i];
		*(pbuffer) = '\0';

		return len;
	}

	while ((ch=*(fmt++))) {
		if ((unsigned int)((pbuffer - buffer) + 1) >= buffer_len)
			break;
		if (ch!='%')
			_putc(ch);
		else {
			char zero_pad = 0;
			char *ptr;
			unsigned int len;

			ch=*(fmt++);

			/* Zero padding requested */
			if (ch=='0') {
				ch=*(fmt++);
				if (ch == '\0')
					goto end;
				if (ch >= '0' && ch <= '9')
					zero_pad = ch - '0';
				ch=*(fmt++);
			}

			switch (ch) {
				case 0:
					goto end;

				case 'u':
				case 'd':
					len = mini_itoa(va_arg(va, unsigned int), 10, 0, (ch=='u'), bf, zero_pad);
					_puts(bf, len);
					break;

				case 'x':
				case 'X':
					len = mini_itoa(va_arg(va, unsigned int), 16, (ch=='X'), 1, bf, zero_pad);
					_puts(bf, len);
					break;

				case 'c' :
					_putc((char)(va_arg(va, int)));
					break;

				case 's' :
					ptr = va_arg(va, char*);
					_puts(ptr, mini_strlen(ptr));
					break;

				default:
					_putc(ch);
					break;
			}
		}
	}
end:
	return pbuffer - buffer;
}

int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...)
{
	int ret;
	va_list va;
	va_start(va, fmt);
	ret = mini_vsnprintf(buffer, buffer_len, fmt, va);
	va_end(va);

	return ret;
}

#define SCRATCH 12	//32Bits go up to 4GB + 1 Byte for \0

//Spare some program space by making a comment of all not used format flag lines
#define USE_LONG 	// %lx, %Lu and so on, else only 16 bit integer is allowed
//#define USE_OCTAL	// %o, %O Octal output. Who needs this ?
#define USE_STRING      // %s, %S Strings as parameters
#define USE_CHAR	// %c, %C Chars as parameters
#define USE_INTEGER	// %i, %I Remove this format flag. %d, %D does the same
#define USE_HEX		// %x, %X Hexadezimal output
#define USE_UPPERHEX	// %x, %X outputs A,B,C... else a,b,c...
#ifndef USE_HEX
 #undef USE_UPPERHEX    // ;)
#endif
#define USE_UPPER	// uncommenting this removes %C,%D,%I,%O,%S,%U,%X and %L..
                        // only lowercase format flags are used
#define PADDING         //SPACE and ZERO padding

void ssprintf(unsigned char *bf, char const *format, ...)
{
  unsigned char scratch[SCRATCH];
  unsigned char format_flag;
  unsigned short base;
  unsigned char *ptr;
  unsigned char issigned=0;
  va_list ap;

#ifdef USE_LONG
// #warning "use long"
  unsigned char islong=0;
  unsigned long u_val=0;
  long s_val=0;
#else
  unsigned int u_val=0;
  int s_val=0;
#endif

  unsigned char fill;
  unsigned char width;

 // if(*format==0);
  //else{

  va_start (ap, format);
  for (;;){
    while ((format_flag = *(format++)) != '%'){      // Until '%' or '\0'
      if (!format_flag){va_end (ap); *bf=0; return;}
	  *bf = format_flag;
	  bf++;
    }

    issigned=0; //default unsigned
    base = 10;

    format_flag = *format++; //get char after '%'

#ifdef PADDING
    width=0; //no formatting
    fill=0;  //no formatting
    if(format_flag=='0' || format_flag==' ') //SPACE or ZERO padding  ?
     {
      fill=format_flag;
      format_flag = *format++; //get char after padding char
      if(format_flag>='0' && format_flag<='9')
       {
        width=format_flag-'0';
        format_flag = *format++; //get char after width char
       }
     }
#endif

#ifdef USE_LONG
    islong=0; //default int value
#ifdef USE_UPPER
    if(format_flag=='l' || format_flag=='L') //Long value
#else
    if(format_flag=='l') //Long value
#endif
     {
      islong=1;
      format_flag = *format++; //get char after 'l' or 'L'
     }
#endif

    switch (format_flag)
    {
#ifdef USE_CHAR
    case 'c':
#ifdef USE_UPPER
    case 'C':
#endif
      format_flag = va_arg(ap,int);
      // no break -> run into default
#endif

    default:
	  *bf=format_flag; bf++;
      continue;

#ifdef USE_STRING
#ifdef USE_UPPER
    case 'S':
#endif
    case 's':
      ptr = (unsigned char*)va_arg(ap,char *);
      while(*ptr) { *bf=*ptr; bf++; ptr++; }
      continue;
#endif

#ifdef USE_OCTAL
    case 'o':
#ifdef USE_UPPER
    case 'O':
#endif
      base = 8;
	  *bf='0'; bf++;
      goto CONVERSION_LOOP1;
#endif

#ifdef USE_INTEGER //don't use %i, is same as %d
    case 'i':
#ifdef USE_UPPER
    case 'I':
#endif
#endif
    case 'd':
#ifdef USE_UPPER
    case 'D':
#endif
      issigned=1;
      // no break -> run into next case
    case 'u':
#ifdef USE_UPPER
    case 'U':
#endif

//don't insert some case below this if USE_HEX is undefined !
//or put       goto CONVERSION_LOOP;  before next case.
#ifdef USE_HEX
      goto CONVERSION_LOOP1;
    case 'x':
#ifdef USE_UPPER
    case 'X':
#endif
      base = 16;
#endif

    CONVERSION_LOOP1:

      if(issigned) //Signed types
       {
#ifdef USE_LONG
        if(islong) { s_val = va_arg(ap,long); }
        else { s_val = va_arg(ap,int); }
#else
        s_val = va_arg(ap,int);
#endif

        if(s_val < 0) //Value negativ ?
         {
          s_val = - s_val; //Make it positiv
          *bf='-'; bf++;    //Output sign
         }

        u_val = (unsigned long)s_val;
       }
      else //Unsigned types
       {
#ifdef USE_LONG
        if(islong) { u_val = va_arg(ap,unsigned long); }
        else { u_val = va_arg(ap,unsigned int); }
#else
        u_val = va_arg(ap,unsigned int);
#endif
       }

      ptr = scratch + SCRATCH;
      *--ptr = 0;
      do
       {
        char ch = u_val % base + '0';
#ifdef USE_HEX
        if (ch > '9')
         {
          ch += 'a' - '9' - 1;
#ifdef USE_UPPERHEX
          ch-=0x20;
#endif
         }
#endif
        *--ptr = ch;
        u_val /= base;

#ifdef PADDING
        if(width) width--; //calculate number of padding chars
#endif
      } while (u_val);

#ifdef PADDING
     while(width--) *--ptr = fill; //insert padding chars
#endif

      while(*ptr) { *bf=*ptr; bf++; ptr++; }
    }

	*bf=0;
  }//}
}
unsigned char hex_to_ASCII (unsigned char kkk)
{
    if(kkk==0x01) return '1';
    else if(kkk==0x02) return '2';
	else if(kkk==0x03) return '3';
	else if(kkk==0x04) return '4';
	else if(kkk==0x05) return '5';
	else if(kkk==0x06) return '6';
	else if(kkk==0x07) return '7';
	else if(kkk==0x08) return '8';
	else if(kkk==0x09) return '9';
	else if(kkk==0x00) return '0';
	else if(kkk==0x0A) return 'A';
	else if(kkk==0x0B) return 'B';
	else if(kkk==0x0C) return 'C';
	else if(kkk==0x0D) return 'D';
	else if(kkk==0x0E) return 'E';
	else if(kkk==0x0F) return 'F';
	return 0;
}
void debugPrintHexTable(unsigned short length, unsigned char *buffer, char *buf_p)
{
    int sx;
	unsigned char i;
	unsigned short j;
	unsigned char *buf;
	unsigned char s;

	buf = buffer;   sx=0;

	// print the low order address indicies and ASCII header
	ssprintf(buf_p,"     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  0123456789ABCDEF\r\n");       sx+=mini_strlen(buf_p);
	ssprintf(&buf_p[sx],"     -----------------------------------------------  ---- ASCII -----\r\n");  sx+=mini_strlen(&buf_p[sx]);

	// print the data
	for(j=0; j<((length+15)>>4); j++)
	{
		// print the high order address index for this line


	 buf_p[sx++] = hex_to_ASCII(((j<<4)>>12)&0x000F);
	 buf_p[sx++] = hex_to_ASCII(((j<<4)>>8)&0x000F);
	 buf_p[sx++] = hex_to_ASCII(((j<<4)>>4)&0x000F);
	 buf_p[sx++] = hex_to_ASCII((j<<4)&0x000F);
	 buf_p[sx++]=' ';


		// print the hex data
		for(i=0; i<0x10; i++)
		{
			// be nice and print only up to the exact end of the data
			if( ((j<<4)+i) < length)
			{
				// print hex byte


				buf_p[sx++] = hex_to_ASCII((buf[(j<<4)+i]>>4)&0x00F);
	            buf_p[sx++] = hex_to_ASCII(buf[(j<<4)+i]&0x0F);
				buf_p[sx++]=' ';
			}
			else
			{
				// we're past the end of the data's length
				// print spaces
				buf_p[sx++]=' ';
				buf_p[sx++]=' ';
				buf_p[sx++]=' ';
			}
		}

		// leave some space
		buf_p[sx++]=' ';

		// print the ascii data
		for(i=0; i<0x10; i++)
		{
			// be nice and print only up to the exact end of the data
			if( ((j<<4)+i) < length)
			{
				// get the character
				s = buf[(j<<4)+i];
				// make sure character is printable
				if(s >= 0x20)
					buf_p[sx++]=s;
				else
					buf_p[sx++]='.';
			}
			else
			{
				// we're past the end of the data's length
				// print a space
				//rprintf("\r\n");
			}
		}
		buf_p[sx++]='\r';buf_p[sx++]='\n';
	}

	buf_p[sx++]='\r';buf_p[sx++]='\n';
	buf_p[sx]=0;

}
