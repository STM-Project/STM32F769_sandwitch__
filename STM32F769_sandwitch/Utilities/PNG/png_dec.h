/*
 * png_dec.h
 *
 *  Created on: 05.08.2020
 *      Author: RafalMar
 */

#ifndef PNG_PNG_DEC_H_
#define PNG_PNG_DEC_H_

#include <stdint.h>

extern int dec_png(const char * png_path);
extern int dec_png_buf(uint8_t *png_buffer, uint32_t png_size, uint8_t *bmp_buffer, uint32_t bmp_buf_len);
extern void png_dec_test(void);

#endif /* PNG_PNG_DEC_H_ */
