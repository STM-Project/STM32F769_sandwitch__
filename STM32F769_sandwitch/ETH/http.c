/*
 * http.c
 *
 *  Created on: 17.07.2020
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include <string.h>

#include "api.h"
#include "netif.h"
#include "ethernetif.h"
#include "tcpip.h"
#include "http.h"
#include "SDCard.h"
#include "ssl_server.h"
#include "dbg.h"
#include "dhcp.h"
#include "mini_printf.h"
#include "sdram.h"
#include "tcp.h"

osThreadId defaultTaskHandle2;

#define MAX_DHCP_TRIES  250
__IO uint8_t DHCP_state;

int block=0;
#define USE_DHCP

#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   68

#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1

#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

#define WEBSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 4 )

struct netif gnetif;

static uint32_t len;
SDRAM char pLcd6[1000000];

osSemaphoreId Netif_LinkSemaphore = NULL;
struct link_str link_arg;

static void Netif_Config(void)
{
  struct ip4_addr ipaddr;
  struct ip4_addr netmask;
  struct ip4_addr gw;

  /* IP address setting */
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
  struct ip_addr *netmask, struct ip_addr *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))

  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/

  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernetif_update_config);

  /* create a binary semaphore used for informing ethernetif of frame reception */
  osSemaphoreDef(Netif_SEM);
  Netif_LinkSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM) , 1 );

  link_arg.netif = &gnetif;
  link_arg.semaphore = Netif_LinkSemaphore;
  /* Create the Ethernet link handler thread */
  osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(LinkThr), &link_arg);

}

void http_server_serve(struct netconn *conn)
{
  struct netbuf *inbuf;
  err_t recv_err;
  char* buf;
  u16_t buflen;



  //conn->pcb.tcp->remote_ip.addr;

  /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
  recv_err = netconn_recv(conn, &inbuf);

  if (recv_err == ERR_OK)  //http
  {
    if (netconn_err(conn) == ERR_OK)
    {
      netbuf_data(inbuf, (void**)&buf, &buflen);

      /* Is this an HTTP GET command? (only check the first 5 chars, since
      there are other formats for GET, and we're keeping it very simple )*/
      if(strstr(buf, "GET / "))   //http
      {
    	  dbg("\r\nHTTP...");

      	if (STARTUP_WaitForBits(0x0001))
      	{
      		dbg(" Clear ");
    		      STARTUP_ClaerBits(0x0002);

                   SDCardFileOpen(0,"aaa.htm",FA_READ);
          	      SDCardFileRead(0, pLcd6, 500000, &len);
          		 SDCardFileClose(0);

      		STARTUP_SetBits(0x0002);   dbg(" Set ");

      		netconn_write(conn, pLcd6,len, NETCONN_NOCOPY);
      	}
      	else
      		netconn_write(conn, "1234567890",10, NETCONN_NOCOPY);



      }
      else
      {
    	  dbg("\r\n111...");
    	  netconn_write(conn, "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",strlen("111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"), NETCONN_NOCOPY);
      }
    }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);


  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

static void http_server_netconn_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err, accept_err;
dbg("sssss");
  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);

  if (conn!= NULL)
  {dbg("AAAAAAA");
    /* Bind to port 80 (HTTP) with default IP address */
    err = netconn_bind(conn, NULL, 80);

    if (err == ERR_OK)
    { dbg("dddddddddddddddddd");
      /* Put the connection into LISTEN state */
      netconn_listen(conn);

      while(1)
      {
        /* accept any icoming connection */
        accept_err = netconn_accept(conn, &newconn);
        if(accept_err == ERR_OK)
        {
          /* serve connection */
          http_server_serve(newconn);

          /* delete connection */
          netconn_delete(newconn);
        }
      }
    }
  }
}

void http_server_netconn_init()
{
  //sys_thread_new("HTTP", ssl_server, NULL, 3*DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
	sys_thread_new("HTTP", http_server_netconn_thread, NULL, 512, osPriorityRealtime);
}

void DHCP_thread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  struct ip4_addr ipaddr;
  struct ip4_addr netmask;
  struct ip4_addr gw;
  uint32_t IPaddress;
  dbg("\r\nDHCP ");
  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        netif->ip_addr.addr = 0;
        netif->netmask.addr = 0;
        netif->gw.addr = 0;
        IPaddress = 0;
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
        dbg("\r\nWAIT.... ");
      }
      break;

    case DHCP_WAIT_ADDRESS:
      {
        /* Read the new IP address */
        IPaddress = netif->ip_addr.addr;

        if (IPaddress!=0)
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;    dbg("\r\nDHCP  OK  ");
          char buf_p[50];
          mini_snprintf(buf_p,49,"\r\nIP: %d.%d.%d.%d",(IPaddress>>24)&0x000000FF,(IPaddress>>16)&0x000000FF,(IPaddress>>8)&0x000000FF,(IPaddress>>0)&0x000000FF); dbg(buf_p);

          /* Stop DHCP */
          dhcp_stop(netif);
          STARTUP_SetBits(0x0001);

        }
        else
        {
          /* DHCP timeout */
          if (netif->dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;


            dhcp_stop(netif);


            IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(netif, &ipaddr , &netmask, &gw);

            dbg("\r\nDHCP_TIMEOUT ");
            STARTUP_SetBits(0x0001);

          }
        }
      }
      break;

    default: break;
    }

    /* wait 250 ms */
    //osDelay(250);
  }
}

void User_notification(struct netif *netif)
{dbg("111");
  if (netif_is_up(netif))
  {dbg("222");
#ifdef USE_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_START;
#endif /* USE_DHCP */
  }
  else
  {dbg("333");
#ifdef USE_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */
  }
}

void MyThread_2(void const * argument)
{ dbg("11");
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);
  dbg("22");
  /* Initialize the LwIP stack */
  Netif_Config();
  dbg("33");

  /* Initialize webserver demo */
  http_server_netconn_init();

  dbg("KKKKKK");


  /* Start SSL Server task */
   //osThreadDef(Server, ssl_server, osPriorityAboveNormal, 0,  20*configMINIMAL_STACK_SIZE);
  // osThreadCreate (osThread(Server), NULL);


  /* Notify user about the network interface config */
  User_notification(&gnetif);

  dbg("\r\nhhhhhhKKKK");

#ifdef USE_DHCP
  /* Start DHCPClient */
#if defined(__GNUC__)
  osThreadDef(DHCP, DHCP_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 2);
#else
  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
#endif

  osThreadCreate (osThread(DHCP), &gnetif);
#endif

  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(NULL);
  }
}

void MyThread(void const * argument)
{
	dbg("\r\nStart ");
	//wybor_bitowy&=~0x01;  //Watek HTTP OFF
	//wybor_bitowy&=~0x02;  //Watek SMTPS OFF

	  osThreadDef(Server, SSL_Server, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 10);
	  defaultTaskHandle2 = osThreadCreate (osThread(Server), NULL);

	// http_server_netconn_init();

	  for( ;; )
	  {
	    // Delete the start Thread
		  dbg("f");
	        osThreadTerminate(NULL);
	  }
}

void Create_ETH_DHCP_HTTP_Thread(void)
{
	  osThreadDef(defaultTask2, MyThread_2, osPriorityLow, 0, 128);
	  defaultTaskHandle2 = osThreadCreate(osThread(defaultTask2), NULL);

}

void Create_ETH_DHCP_HTTPS_Thread(void)
{
	 osThreadDef(StartMyThread, MyThread, osPriorityLow, 0, configMINIMAL_STACK_SIZE*2);
	 osThreadCreate(osThread(StartMyThread), NULL);

}
