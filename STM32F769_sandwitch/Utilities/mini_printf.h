/*
 * mini_printf.h
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */

#ifndef MINI_PRINTF_H_
#define MINI_PRINTF_H_

unsigned int mini_strlen(const char *s);
void StrBuffCopy(char *dest, char *src);
void StrBuffCopylimit(char *dest, char *src, int lenSrc);
int mini_vsnprintf(char* buffer, unsigned int buffer_len, const char *fmt, va_list va);
int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

#endif /* MINI_PRINTF_H_ */
