/*
 * png_dec.c
 *
 *  Created on: 05.08.2020
 *      Author: RafalMar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "sdram.h"
#include "upng.h"
#include "png_dec.h"
#include "dbg.h"
#include "SDCard.h"


#define UPNG_FLIP_Y

SDRAM static uint8_t bmp_buffer[1152000];
SDRAM static uint8_t png_buffer[1152000];

void encodeToBMP(uint8_t *bmp_header, uint32_t xsize, uint32_t ysize)
{
	uint8_t header[54] =
	{
    	0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    uint32_t file_size = (uint32_t)xsize * (uint32_t)ysize * 3 + 54;
    header[2] = (unsigned char)(file_size &0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;

    uint32_t width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;

    uint32_t height = ysize;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;

	memmove(bmp_header, header, sizeof(header));
}

uint8_t writeToBMP(const uint8_t *data, uint32_t data_len, uint32_t item_id)
{

    uint8_t res = SDCardFileOpen(1,"BMPPPPP.bmp",FA_CREATE_ALWAYS|FA_WRITE);
    if(res)
    {
        dbg("\r\nunable creat file! ");
        return 0;
    }
    else
    {
    	//bmp
        SDCardFileWrite(1, data, data_len);  dbg("\r\nWRITE !!!");
    }

   // DbgSprintf(40,"\r\nwrite file succ, file name = %s, len = %d ", filename_buf, pic_file.fsize);
    SDCardFileClose(1);
}

/*
 * flip the image
 * Set to flip the image data along the horizontal axis and vertical axis
 * in_buffer:  ptr to the in data
 * width:      iamge width
 * height:     image height
 * out_buffer: output data buffer
 */
void raw_data_convert(uint8_t *in_buffer, uint32_t width, uint32_t height, uint8_t *out_buffer)
{
	uint32_t pixel_num = width * height;
	uint32_t color_idx = 0;

	uint32_t depth = 3;
	int32_t y;

//#ifndef UPNG_FLIP_Y
//	for (y = 0; y != height; ++y){
//#else
	for (y = 0; y != height; ++y){   //for (y = height-1; y >= 0; --y){
//#endif
		for (uint32_t x = 0; x < width; ++x)
		{
			out_buffer[color_idx++] = in_buffer[(height - y - 1) * width * depth + x * depth + (depth - 0 - 1)];
			out_buffer[color_idx++] = in_buffer[(height - y - 1) * width * depth + x * depth + (depth - 1 - 1)];
			out_buffer[color_idx++] = in_buffer[(height - y - 1) * width * depth + x * depth + (depth - 2 - 1)];
			if(x==62)
				asm("nop");
		}
		if(y==1)
			asm("nop");
	}

	if(color_idx != pixel_num*3)
	{
		dbg("\r\nconvert err! ");
	}
}

/*
 * convert png to bmp
 * note: read png data from file by fatfs and decode to a malloc buffer
 * png_path: png file path
 */
int dec_png(const char * png_path)
{
	upng_t* upng;
	unsigned width, height, depth;
	unsigned x, y, d;

	upng = upng_new_from_file(png_path);
	if(upng != NULL)
	{
		upng_decode(upng);
		if (upng_get_error(upng) != UPNG_EOK)
		{
			DbgSprintf(50,"\r\nError: %d %d", upng_get_error(upng), upng_get_error_line(upng));
			upng_free(upng);
			return 0;
		}
	}
	else
	{
		dbg("\r\nOpen file err!");
		return 0;
	}

	width = upng_get_width(upng);
	height = upng_get_height(upng);
	depth = upng_get_bpp(upng) / 8;

	DbgSprintf(50,"\r\nw:%d h:%d bpp:%d size:%d ", width, height, upng_get_bpp(upng), upng_get_size(upng));
//	printf("format:	%d\n", upng_get_format(upng));

	if (upng_get_format(upng) == UPNG_RGB8)
	{
		uint8_t *header = bmp_buffer;
		encodeToBMP(header, width, height);

		raw_data_convert((uint8_t *)upng_get_buffer(upng), width, height, &bmp_buffer[54]);

		//write to a file
		uint32_t length = width * height * depth;
		static uint32_t item_id = 0;
		writeToBMP(bmp_buffer, length + 54, item_id++);
	}
	else
	{
		dbg("\r\n[PNG] unsupport color format. ");
	}

	upng_free(upng);
	return 0;
}

/*
 * convert png to bmp
 * note: read png data from buffer and decode to a preallocated buffer
 * png_buffer:  ptr to png source data
 * png_size:    png source length
 * bmp_buffer:  ptr to bmp out data buffer
 * bmp_buf_len: out buffer length
 */
int dec_png_buf(uint8_t *png_buffer, uint32_t png_size, uint8_t *bmp_buffer, uint32_t bmp_buf_len)
{
	upng_t* upng;
	unsigned width, height, depth;

	upng = upng_new_from_bytes(png_buffer, png_size);
	if(upng != NULL)
	{
		upng_decode_to_buffer(upng, bmp_buffer, bmp_buf_len);
		if (upng_get_error(upng) != UPNG_EOK)
		{
			DbgSprintf(50,"\r\nerror: %d %d ", upng_get_error(upng), upng_get_error_line(upng));
			upng_free(upng);
			return 0;
		}
	}
	else
	{
		dbg("\r\nopen file err! ");
		return 0;
	}

	width = upng_get_width(upng);
	height = upng_get_height(upng);
	depth = upng_get_bpp(upng) / 8;

	DbgSprintf(50,"\r\nw:%d h:%d bpp:%d size:%d", width, height, upng_get_bpp(upng), upng_get_size(upng));
//	printf("format:	%d\n", upng_get_format(upng));

	if (upng_get_format(upng) == UPNG_RGB8)
	{
		uint8_t *header = bmp_buffer;
		encodeToBMP(header, width, height);

		uint8_t *ppp=upng_get_buffer(upng);
		//raw_data_convert((uint8_t *)upng_get_buffer(upng), width, height, &header[54]);
		raw_data_convert(ppp, width, height, &header[54]);
		//write to a bmp file
		uint32_t length = width * height * depth;
		static uint32_t item_id = 0;
		writeToBMP(bmp_buffer, length + 54, item_id++);
	}
	else
	{
		dbg("\r\n[PNG] unsupport color format.");

		uint8_t *header = bmp_buffer;
		encodeToBMP(header, width, height);

		uint8_t *ppp=upng_get_buffer(upng);
		//raw_data_convert((uint8_t *)upng_get_buffer(upng), width, height, &header[54]);
		raw_data_convert(ppp, width, height, &header[54]);
		//write to a bmp file
		uint32_t length = width * height * depth;
		static uint32_t item_id = 0;
		writeToBMP(bmp_buffer, length + 54, item_id++);
	}

	upng_free(upng);
	return 0;
}


/*
 * test main()
 */
void png_dec_test()
{

	//uint8_t i = 2;
	//for(i=1; i<=10; i++)
	//{
		//sprintf(png_path, PNG_PATH, i);
		//dec_png("test.png"/*, sdram_buffer_2, sizeof(sdram_buffer_2)*/);

	//}
  int png_size;

	SDCardFileOpen(0,"test.png",FA_READ);
	SDCardFileRead(0,png_buffer,1000000,&png_size);
	SDCardFileClose(0);

	dec_png_buf(png_buffer, png_size, bmp_buffer, 1000000);

}
