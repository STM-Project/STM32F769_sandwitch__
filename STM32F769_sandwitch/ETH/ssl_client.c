 /**
  *
  *  Portions COPYRIGHT 2016 STMicroelectronics
  *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
  *
  ******************************************************************************
  * @file    ssl_client.c
  * @author  MCD Application Team
  * @brief   SSL client application
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

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t
#define mbedtls_fprintf    fprintf
#define dbg     printf
#endif


#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||  \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||         \
    !defined(MBEDTLS_CERTS_C) || !defined(MBEDTLS_PEM_PARSE_C) || \
    !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)
#endif
#if 0
int main( void )
{

    dbg("MBEDTLS_BIGNUM_C and/or MBEDTLS_ENTROPY_C and/or "
           "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
           "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
           "MBEDTLS_CTR_DRBG_C and/or MBEDTLS_X509_CRT_PARSE_C "
           "not defined.\n");

    return( 0 );
}
#else

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/memory_buffer_alloc.h"

#include "main.h"
#include "cmsis_os.h"

#include <string.h>
#include "sdram.h"

extern osThreadId defaultTaskHandle3;
static mbedtls_net_context server_fd2;
static uint32_t flags2;
SRAM static uint8_t buf2[1024];// __attribute__ ((section(".data")));
static const uint8_t *pers2 = (uint8_t *)("ssl_client");
static uint8_t vrfy_buf[512]={0};// __attribute__ ((section(".data")));

static int ret2;
extern int moj_wsk;

mbedtls_entropy_context entropy2;
mbedtls_ctr_drbg_context ctr_drbg2;
mbedtls_ssl_context ssl2;
mbedtls_ssl_config conf2;
mbedtls_x509_crt cacert2;

extern osThreadId defaultTaskHandle2;

/* use static allocation to keep the heap size as low as possible */
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
uint8_t memory_buf[MAX_MEM_SIZE];
#endif

int SSL_Send_Data(mbedtls_ssl_context *ssl2, char *req )
{
	 int len,ret2;
	  dbg( "  > Write to server:" );

	  len = strlen((char *) req);

	  while( ( ret2 = mbedtls_ssl_write( ssl2, req, len ) ) <= 0 )
	  {
	    if( ret2 != MBEDTLS_ERR_SSL_WANT_READ && ret2 != MBEDTLS_ERR_SSL_WANT_WRITE )
	    {
	      dbg( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret2 );
	      return 0;
	    }
	  }
	  return 1;
}

int SSL_Reciev_Data(mbedtls_ssl_context *ssl2, char *req, int ilosc)
{
   int len,ret2;
	dbg( "  < Read from server:" );
  do
  {
    len = sizeof( buf2 ) - 1;
    memset( buf2, 0, sizeof( buf2 ) );  dbg("\r\nA");
    ret2 = mbedtls_ssl_read( ssl2, buf2, ilosc );   dbg("B ");


    if( ret2 == MBEDTLS_ERR_SSL_WANT_READ || ret2 == MBEDTLS_ERR_SSL_WANT_WRITE )
    {
         dbg("AAAAA "); continue;
    }

    if( ret2 == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
    {
    	dbg("BBBB");//break;
    	//return 2;
    }

    if( ret2 < 0 )
    {
      dbg( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret2 );
      dbg("CCCCC");   //break;
  	   return 0;
    }

    if( ret2 == 0 )
    {
      dbg( "\n\nEOF\n\n" );
       dbg("DDDDD");   // break;
    }

    len = ret2;
    dbg( " %d bytes read\n\n%s", len, (char *) buf2 );
    buf2[len]=0;  dbg(buf2);

    if(ilosc==0)        return 1;
    if(strstr(buf2,req)) return 1;
    else                return 0;

  }
  while( 1 );

}


void SSL_Client(void const *argument)
{
  int len;

  /*
   * 0. Initialize the RNG and the session data
   */
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
  mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));
#endif
 // mbedtls_net_init(NULL);

  start:
  mbedtls_ssl_init(&ssl2);
  mbedtls_ssl_config_init(&conf2);
  mbedtls_x509_crt_init(&cacert2);
  mbedtls_ctr_drbg_init(&ctr_drbg2);

  dbg( "\n  . Seeding the random number generator..." );

  mbedtls_entropy_init( &entropy2 );
  len = strlen((char *)pers2);
  if( ( ret2 = mbedtls_ctr_drbg_seed( &ctr_drbg2, mbedtls_entropy_func, &entropy2,
                             (const unsigned char *) pers2, len ) ) != 0 )
  {
    dbg( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret2 );
    goto exit;
  }

  dbg( " ok\n" );

  /*
   * 0. Initialize certificates
   */
  dbg( "  . Loading the CA root certificate ..." );

  ret2 = mbedtls_x509_crt_parse( &cacert2, (const unsigned char *) mbedtls_test_cas_pem,
                        mbedtls_test_cas_pem_len );
  if( ret2 < 0 )
  {
    dbg( " failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret2 );
    goto exit;
  }

  dbg( " ok (%d skipped)\n", ret2 );



 // osThreadResume(defaultTaskHandle2);

  do{
 	 osDelay(1000);   if(moj_wsk==1) break;
  }while(1);

 // osThreadSuspend(defaultTaskHandle2);

  if (STARTUP_WaitForBits(0x0001)){
	  STARTUP_ClaerBits(0x0004);   dbg(" Set ");



  /*
   * 1. Start the connection
   */
  dbg( "  . Connecting to tcp/%s/%s...", "#########", "465" );

 // if( ( ret2 = mbedtls_net_connect( &server_fd2, "213.180.147.145", "465", MBEDTLS_NET_PROTO_TCP ) ) != 0 )   //smtp.poczta.onet.pl
 if( ( ret2 = mbedtls_net_connect( &server_fd2, "173.194.222.109", "465", MBEDTLS_NET_PROTO_TCP ) ) != 0 )  //smtp.gmail.com
  {
    dbg( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret2 );
    goto exit;
  }

  dbg( " ok\n" );

  /*
   * 2. Setup stuff
   */
  dbg( "  . Setting up the SSL/TLS structure..." );

  if( ( ret2 = mbedtls_ssl_config_defaults( &conf2,
                  MBEDTLS_SSL_IS_CLIENT,
                  MBEDTLS_SSL_TRANSPORT_STREAM,
                  MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
  {
    dbg( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret2 );
    goto exit;
  }

  dbg( " ok\n" );

  /* OPTIONAL is not optimal for security,
   * but makes interop easier in this simplified example */
  mbedtls_ssl_conf_authmode( &conf2, MBEDTLS_SSL_VERIFY_OPTIONAL );
  mbedtls_ssl_conf_ca_chain( &conf2, &cacert2, NULL );
  mbedtls_ssl_conf_rng( &conf2, mbedtls_ctr_drbg_random, &ctr_drbg2 );

  if( ( ret2 = mbedtls_ssl_setup( &ssl2, &conf2 ) ) != 0 )
  {
    dbg( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret2 );
    goto exit;
  }

  if( ( ret2 = mbedtls_ssl_set_hostname( &ssl2, "mbed TLS Server 1" ) ) != 0 )
  {
    dbg( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret2 );
    goto exit;
  }

  mbedtls_ssl_set_bio( &ssl2, &server_fd2, mbedtls_net_send, mbedtls_net_recv, NULL );

  /*
   * 4. Handshake
   */
  dbg( "  . Performing the SSL/TLS handshake..." );

  while( ( ret2 = mbedtls_ssl_handshake( &ssl2 ) ) != 0 )
  {
    if( ret2 != MBEDTLS_ERR_SSL_WANT_READ && ret2 != MBEDTLS_ERR_SSL_WANT_WRITE )
    {
      dbg( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret2 );
      goto exit;
    }
  }

  dbg( " ok\n" );

  /*
   * 5. Verify the server certificate
   */
  dbg( "  . Verifying peer X.509 certificate..." );

  if( ( flags2 = mbedtls_ssl_get_verify_result( &ssl2 ) ) != 0 )
  {

    dbg( " failed\n" );
    mbedtls_x509_crt_verify_info( (char *)vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags2 );

    dbg( "%s\n", vrfy_buf );
  }
  else
  {
    dbg( " ok\n" );
  }

  /*
   * Prawdopodobnie w gmailu masz w³¹czone zabezpieczenie które nie pozwala korzystaæ z "mniej zabezpieczonego" rodzaju oprogramowania, wejdŸ tu i zobacz czy masz w³¹czone czy wy³¹czone: https://www.google.com/settings/security/lesssecureapps
   */
//  mariusz.szczygielek@op.pl    //smtp.poczta.onet.pl
//  bWFyaXVzei5zemN6eWdpZWxla0BvcC5wbA==
//  Szczygielek6666
//  U3pjenlnaWVsZWs2NjY2

  //  mariusz.szczygielek@gmail.com       //smtp.gmail.com
  //  bWFyaXVzei5zemN6eWdpZWxla0BnbWFpbC5jb20=
  //  Szczygielek6666
  //  U3pjenlnaWVsZWs2NjY2




  //AUTH LOGIN PLAIN XOAUTH2 PLAIN-CLIENTTOKEN OAUTHBEARER XOAUTH   GMAIL
  if(SSL_Reciev_Data(&ssl2,"220 ",200)==0)  goto exit;    SSL_Send_Data(&ssl2,"EHLO smtp.gmail.com\r\n");
  if(SSL_Reciev_Data(&ssl2,"250 SMTPUTF8",1500)==0)  goto exit;               SSL_Send_Data(&ssl2,"AUTH LOGIN\r\n");
  if(SSL_Reciev_Data(&ssl2,"334 VXNlcm5hbWU6",200)==0)  goto exit;            SSL_Send_Data(&ssl2,"bWFyaXVzei5zemN6eWdpZWxla0BnbWFpbC5jb20=\r\n");
  if(SSL_Reciev_Data(&ssl2,"334 UGFzc3dvcmQ6",200)==0)  goto exit;             SSL_Send_Data(&ssl2,"U3pjenlnaWVsZWs2NjY2\r\n");
  if(SSL_Reciev_Data(&ssl2,"235 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"MAIL From:<mariusz.szczygielek@gmial.com>\r\n");
  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"RCPT To:<marafal@interia.pl>\r\n");
  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"DATA\r\n");
  if(SSL_Reciev_Data(&ssl2,"354 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"From: mariusz.szczygielek@gmial.com\r\n");
  HAL_Delay(1);       SSL_Send_Data(&ssl2,"To: marafal@interia.pl\r\n");
  HAL_Delay(1);       SSL_Send_Data(&ssl2,"Subject: Info\r\n");
  HAL_Delay(1);       SSL_Send_Data(&ssl2,"\r\n");
  HAL_Delay(1);       SSL_Send_Data(&ssl2,"TRESC wiadomosic\r\n");
  HAL_Delay(1);       SSL_Send_Data(&ssl2,"\r\n.\r\n");
  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"quit\r\n");
  if(SSL_Reciev_Data(&ssl2,"221 ",200)==0)  goto exit;



//  if(SSL_Reciev_Data(&ssl2,"220 ",200)==0)  goto exit;    SSL_Send_Data(&ssl2,"EHLO sdfsdfdsfvdfv\r\n");
//  HAL_Delay(400);if(SSL_Reciev_Data(&ssl2,"250 8BITMIME",266)==0)  goto exit;                SSL_Send_Data(&ssl2,"AUTH LOGIN\r\n");
//  if(SSL_Reciev_Data(&ssl2,"334 VXNlcm5hbWU6",200)==0)  goto exit;            SSL_Send_Data(&ssl2,"bWFyaXVzei5zemN6eWdpZWxla0BvcC5wbA==\r\n");
//  if(SSL_Reciev_Data(&ssl2,"334 UGFzc3dvcmQ6",200)==0)  goto exit;            SSL_Send_Data(&ssl2,"U3pjenlnaWVsZWs2NjY2\r\n");
//  if(SSL_Reciev_Data(&ssl2,"235 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"MAIL From:<mariusz.szczygielek@op.pl>\r\n");
//  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"RCPT To:<marafal@interia.pl>\r\n");
//  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"DATA\r\n");
//  if(SSL_Reciev_Data(&ssl2,"354 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"From: mariusz.szczygielek@op.pl\r\n");
//  HAL_Delay(1);       SSL_Send_Data(&ssl2,"To: marafal@interia.pl\r\n");
//  HAL_Delay(1);       SSL_Send_Data(&ssl2,"Subject: Info\r\n");
//  HAL_Delay(1);       SSL_Send_Data(&ssl2,"\r\n");
//  HAL_Delay(1);       SSL_Send_Data(&ssl2,"TRESC wiadomosic\r\n");
//  HAL_Delay(1);       SSL_Send_Data(&ssl2,"\r\n.\r\n");
//  if(SSL_Reciev_Data(&ssl2,"250 ",200)==0)  goto exit;       SSL_Send_Data(&ssl2,"quit\r\n");
//  if(SSL_Reciev_Data(&ssl2,"221 ",200)==0)  goto exit;





  dbg("\r\nKONIEC ");




  mbedtls_ssl_close_notify( &ssl2 );

exit:
dbg("\r\nEXIT");
  mbedtls_net_free( &server_fd2 );

  mbedtls_x509_crt_free( &cacert2 );
  mbedtls_ssl_free( &ssl2 );
  mbedtls_ssl_config_free( &conf2 );
  mbedtls_ctr_drbg_free( &ctr_drbg2 );
  mbedtls_entropy_free( &entropy2 );

  if ((ret2 < 0) && (ret2 != MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY))
  {
    Error_Handler();
  }
  else
  {
    //Success_Handler();
	  dbg("\r\nSuccess_Handler  ");
  }
  moj_wsk=0;

  STARTUP_SetBits(0x0004);   dbg(" Set ");
  }


  goto start;
}

void Koniec_SSL_client_Watek(void)
{
	dbg("\r\nEXIT");
	  mbedtls_net_free( &server_fd2 );

	  mbedtls_x509_crt_free( &cacert2 );
	  mbedtls_ssl_free( &ssl2 );
	  mbedtls_ssl_config_free( &conf2 );
	  mbedtls_ctr_drbg_free( &ctr_drbg2 );
	  mbedtls_entropy_free( &entropy2 );

	  osThreadTerminate(defaultTaskHandle3);
}

#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_ENTROPY_C && MBEDTLS_SSL_TLS_C &&
          MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
          MBEDTLS_CERTS_C && MBEDTLS_PEM_PARSE_C && MBEDTLS_CTR_DRBG_C &&
          MBEDTLS_X509_CRT_PARSE_C */
