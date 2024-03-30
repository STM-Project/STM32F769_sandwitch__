/**
 *
 *  Portions COPYRIGHT 2016 STMicroelectronics
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *
 ******************************************************************************
 * @file    ssl_server.c
 * @author  MCD Application Team
 * @brief   SSL server application
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include "mbedtls_config.h"
#endif

#include "sdram.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "dbg.h"
#include "SDCard.h"
#include "StartUp.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define dbg     printf
#endif

#include <stdlib.h>
#include <string.h>
//https://github.com/ARMmbed/mbedtls/blob/development/programs/ssl/ssl_pthread_server.c   MULTI CONNECT !!!!
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

static int lenA;
extern SDRAM char pLcd6[1000000];

int page;
extern osThreadId defaultTaskHandle2;
extern char wybor_bitowy;

static mbedtls_net_context listen_fd, client_fd;
SRAM static char buf[1400]; // __attribute__ ((section(".data")));
static const uint8_t *pers = (uint8_t*) "ssl_server";
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;

mbedtls_x509_crt srvcert;
mbedtls_pk_context pkey;

#if defined(MBEDTLS_SSL_CACHE_C)
mbedtls_ssl_cache_context cache;
#endif

void SSL_Server(void const *argument)
{
	int ret, len, o, m;

#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
	mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));
#endif
	mbedtls_net_init(&listen_fd);
	mbedtls_net_init(&client_fd);

#ifdef _HttpServer
	goto exit;
#endif

	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_init(&cache);
#endif
	mbedtls_x509_crt_init(&srvcert);
	mbedtls_pk_init(&pkey);
	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

#if defined(MBEDTLS_DEBUG_C)
	mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

	/*
	 * 1. Load the certificates and private RSA key
	 */

	dbg("\r\n  . Loading the server cert. and key...");

	/*
	 * This demonstration program uses embedded test certificates.
	 * Using mbedtls_x509_crt_parse_file() to read the server and CA certificates
	 * resuires the implmentation of the File I/O API using the FatFs, that is
	 * not implemented yet.
	 */

	ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_srv_crt, mbedtls_test_srv_crt_len);
	if (ret != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_x509_crt_parse returned %d ", ret);
		goto exit;
	}

	ret = mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
	if (ret != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_x509_crt_parse returned %d ", ret);
		goto exit;
	}

	ret = mbedtls_pk_parse_key(&pkey, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0);

	if (ret != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_pk_parse_key returned %d ", ret);
		goto exit;
	}

	dbg("\r\nok ");

	wybor_bitowy |= 0x02;    //Watek SMTPS ON

	/*
	 * 2. Setup the listening TCP socket
	 */
	dbg("\r\n  . Bind on https://localhost:443/ ...");

	if ((ret = mbedtls_net_bind(&listen_fd, NULL, "443", MBEDTLS_NET_PROTO_TCP)) != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_net_bind returned %d ", ret);
		goto exit;
	}

	dbg("\r\nok ");

	/*
	 * 3. Seed the RNG
	 */
	dbg("\r\n  . Seeding the random number generator...");

	if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen((char *) pers))) != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ctr_drbg_seed returned %d ", ret);
		goto exit;
	}

	dbg("\r\nok ");

	/*
	 * 4. Setup stuff
	 */
	dbg("\r\n  . Setting up the SSL data....");

	if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_config_defaults returned %d ", ret);
		goto exit;
	}

	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_conf_session_cache(&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
#endif

	mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
	if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey)) != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_conf_own_cert returned %d ", ret);
		goto exit;
	}

	if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_setup returned %d ", ret);
		goto exit;
	}

	dbg("\r\nok ");

	reset: STARTUP_SetBits(0x0002);
	dbg(" Set ");
#ifdef MBEDTLS_ERROR_C
	if (ret != 0)
	{
		uint8_t error_buf[100];
		mbedtls_strerror(ret, (char *) error_buf, 100);
		DbgSprintf(250, "\r\nLast error was: %d - %s ", ret, error_buf);
	}
#endif

	mbedtls_net_free(&client_fd);

	mbedtls_ssl_session_reset(&ssl);

	/*
	 * 5. Wait until a client connects
	 */
	dbg("\r\n  . Waiting for a remote connection ...");

	if ((ret = mbedtls_net_accept(&listen_fd, &client_fd, NULL, 0, NULL)) != 0)  //To TU OCZEKUJE !!!
	{
		DbgSprintf(250, "\r\nFAILD!!!  mbedtls_net_accept returned %d ", ret);
		goto exit;
	}

	if (STARTUP_WaitForBits(0x0001))
	{
		STARTUP_ClaerBits(0x0002);
		dbg(" Set ");

		mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

		dbg("\r\nok ");

		/*
		 * 6. Handshake
		 */
		dbg("\r\n . Performing the SSL/TLS handshake...");

		while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
		{
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
			{
				DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_handshake returned %d ", ret);
				goto reset;
			}
		}
		if (STARTUP_WaitForBits(0x0001))
		{
			STARTUP_ClaerBits(0x0002);
			dbg(" Set ");
		}

		dbg("\r\nok ");

		/*
		 * 7. Read the HTTP Request
		 */
		dbg("\r\n  < Read from client:");

		do
		{
			len = sizeof(buf) - 1;
			memset(buf, 0, sizeof(buf));
			ret = mbedtls_ssl_read(&ssl, (unsigned char*) buf, len); /*buf[30]=0;*/

			dbg(" Clear ");
			STARTUP_ClaerBits(0x0002);

			dbg(".");

			if ((len >= 5) && (strncmp(buf, "GET /2", 5 + 1) == 0))
			{
				SDCardFileOpen(0, "aaa.htm", FA_READ);
				SDCardFileRead(0, pLcd6, 500000, &lenA);
				SDCardFileClose(0);
				m = 0;
				o = 0;
				// AspCallback(pLcd5,pLcd6);

			}
			else if (strncmp(buf, "GET /lpc.cgi", 12) == 0)
			{
				osDelay(200);
				dbg_roz(buf);
				//CGI_callback(buf);
				len = sprintf(pLcd6, "HTTP/1.0 200 OK\r\nElektronika RM\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><head><meta http-equiv=\"refresh\" content=\"0;url=../%d\"><link rel=\"shortcut icon\" href=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAABGdBTUEAALGPC/xhBQAAACRJREFUOE9jZGj4z0AaAGogCZGmGuQckowf1UBkcI0GKxEBBQA55GigBl4jdAAAAABJRU5ErkJggg==\"></head><body bgcolor=\"000000\"></body></html>", page);

				while ((ret = mbedtls_ssl_write(&ssl, (unsigned char*) pLcd6, len)) <= 0)
				{
					if (ret == MBEDTLS_ERR_NET_CONN_RESET)
					{
						dbg("\r\n failed  ! peer closed the connection ");
						goto reset;
					}

					if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
					{
						DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_write returned %d ", ret);
						goto exit;
					}
				}
				goto endddddd;

			}
			else if (strncmp(buf, "GET /TME.txt", 12) == 0)
			{
				len = sprintf(pLcd6, "19:22:05 &nbsp;19/06/13--0000000000000 111111111111111111  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0  --.-0 --a0       7.9       7.7   + 244    1+ 389    1+ 356    1+ 356    1+ 718    1+ 820    1+ 483    1+ 447    1   ---   0   ---   0+ 460    1   ---   0   ---   0   ---   0 V     V              0.001   KWh            0.001  0          0.001  0          0.001  0          0.001  0          0.001   KWh            0.001  0          0.001  0          0.001  0          0.001  0          0.001   KWh            0.001  0          0.001  0          0.001  0          0.001  0          0.001   KWh            0.001  0          0.001  0          0.001  0          0.001  0 ");

				while ((ret = mbedtls_ssl_write(&ssl, (unsigned char*) pLcd6, len)) <= 0)
				{
					if (ret == MBEDTLS_ERR_NET_CONN_RESET)
					{
						dbg("\r\n failed ! peer closed the connection ");
						goto reset;
					}

					if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
					{
						DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_write returned %d ", ret);
						goto exit;
					}
				}
				goto endddddd;

			}
			else //if(strncmp(buf, "GET / ", 6) == 0)
			{

				if (strlen(buf) == 0)
					goto endddddd;

				SDCardFileOpen(0, "aaa.htm", FA_READ);
				SDCardFileRead(0, pLcd6, 500000, &lenA);
				SDCardFileClose(0);
				m = 0;
				o = 0;
				// AspCallback(pLcd5,pLcd6);
			}

// osThreadList(buff);  dbg(buff);

			if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
			{
				continue;
			}
			if (ret <= 0)
			{
				switch (ret)
				{
				case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
					dbg("\r\n connection was closed gracefully ");
					break;

				case MBEDTLS_ERR_NET_CONN_RESET:
					dbg("\r\nconnection was reset by peer ");
					break;

				default:
					DbgSprintf(250, "\r\nmbedtls_ssl_read returned -0x%x ", -ret);
					break;
				}

				break;
			}

			len = ret;
			DbgSprintf(250, "\r\n%d bytes read\n\n%s ", len, (char *) buf);

			if (ret > 0)
			{
				break;
			}
		} while (1);

		/*
		 * 8. Write the 200 Response
		 */
		dbg("\r\n  > Write to client:");

		while (1)
		{
			if (lenA < 1024)
				o = lenA;
			else
				o = 1024;

			while ((ret = mbedtls_ssl_write(&ssl, (unsigned char*) pLcd6 + m, o)) <= 0)
			{
				dbg("*");
				if (ret == MBEDTLS_ERR_NET_CONN_RESET)
				{
					dbg("\r\nFAILD!!! peer closed the connection ");
					goto reset;
				}

				if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
				{
					DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_write returned %d ", ret);
					goto exit;
				}
			}
			len = ret;
			dbg(".");

			m += o;
			lenA -= o;
			if (lenA == 0)
				break;

		}

		endddddd:

		dbg("\r\n  . Closing the connection...");

		while ((ret = mbedtls_ssl_close_notify(&ssl)) < 0)
		{
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
			{
				DbgSprintf(250, "\r\nFAILD!!!  mbedtls_ssl_close_notify returned %d ", ret);
				goto reset;
			}
		}

		STARTUP_SetBits(0x0002);
		dbg(" Set ");
	}

	dbg("\r\nok ");

	ret = 0;
	goto reset;

	exit:

	mbedtls_net_free(&client_fd);
	mbedtls_net_free(&listen_fd);

	mbedtls_x509_crt_free(&srvcert);
	mbedtls_pk_free(&pkey);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_free(&cache);
#endif
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

#ifdef _HttpServer
	wybor_bitowy|=0x01;    //Watek HTTP ON
	wybor_bitowy|=0x02;//Watek SMTPS ON
#endif

	osThreadTerminate(defaultTaskHandle2);

	wybor_bitowy |= 0x08;

	STARTUP_SetBits(0x0002);
	dbg(" Set ");

}

void Koniec_SSL_Server_Watek(void)
{
	mbedtls_net_free(&client_fd);
	mbedtls_net_free(&listen_fd);

	mbedtls_x509_crt_free(&srvcert);
	mbedtls_pk_free(&pkey);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
#if defined(MBEDTLS_SSL_CACHE_C)
	mbedtls_ssl_cache_free(&cache);
#endif
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

#ifdef _HttpServer
	wybor_bitowy|=0x01;    //Watek HTTP ON
	wybor_bitowy|=0x02;//Watek SMTPS ON
#endif

	osThreadTerminate(defaultTaskHandle2);
}
