/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/tcp-cJSON>              *
 *                                                                          *
 * This file was heavily adapted from the source code in:                   *
 *   "TCP/IP Sockets in C: Practical Guide for Programmers"                 *
 *   by Michael J. Donahoo and Kenneth L. Calvert                           *
 *   <http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html>       *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file client.c
  * @author Jordan396
  * @date 19 April 2019
  * @brief client.c sends a JSON payload to a server.
  *
  * This file is to be compiled and executed on the client side.
  * 
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
  */

#include "client.h"
#include "../TencentWiFi.h"

/* Function prototypes */

int send_reset_WMM_AC__VO_TXOP_json(int sock)
{
  cJSON *jobjToSend = cJSON_CreateObject();

  cJSON_AddItemToObject(jobjToSend, "WLAN_CMD", cJSON_CreateNumber(SET_WMM_AC_PARAMETER));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_NAME", cJSON_CreateString("vo"));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_PARAMETER", cJSON_CreateString("txop"));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_PARAMETER_VALUE", cJSON_CreateString("0x2f"));

  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);
}


int send_set_WMM_AC__VO_TXOP_json(int sock)
{
  cJSON *jobjToSend = cJSON_CreateObject();

  cJSON_AddItemToObject(jobjToSend, "WLAN_CMD", cJSON_CreateNumber(SET_WMM_AC_PARAMETER));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_NAME", cJSON_CreateString("vo"));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_PARAMETER", cJSON_CreateString("txop"));
  cJSON_AddItemToObject(jobjToSend, "AC_QUEUE_PARAMETER_VALUE", cJSON_CreateString("0x5e"));

  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);
}

int send_set_WMM_traffic_json(int sock)
{
  cJSON *jobjToSend = cJSON_CreateObject();

  char server_ip[32] = "14.0.0.0";
  int wlan = 1;
  int port = 2006;
  int tos = 7;
  int net_mask_length = 0;

  cJSON_AddItemToObject(jobjToSend, "WLAN_CMD", cJSON_CreateNumber(SET_WMM_TRAFFIC));
  cJSON_AddItemToObject(jobjToSend, "Server_IP", cJSON_CreateString(server_ip));
  cJSON_AddItemToObject(jobjToSend, "WLAN_Interface", cJSON_CreateNumber(wlan));
  cJSON_AddItemToObject(jobjToSend, "Net_Mask_Length", cJSON_CreateNumber(net_mask_length));
  cJSON_AddItemToObject(jobjToSend, "Server_Port", cJSON_CreateNumber(port));
  cJSON_AddItemToObject(jobjToSend, "AC_Queue_TOS", cJSON_CreateNumber(tos));

  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);
}

int send_quit_json(int sock)
{
  cJSON *jobjToSend = cJSON_CreateObject();
  cJSON_AddItemToObject(jobjToSend, "WLAN_CMD", cJSON_CreateNumber(WLAN_QUIT));
  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);  
}

int send_clear_WMM_traffic_json(int sock)
{
  cJSON *jobjToSend = cJSON_CreateObject();
  cJSON_AddItemToObject(jobjToSend, "WLAN_CMD", cJSON_CreateNumber(CLEAR_WMM_TRAFFIC));
  send_payload(sock, jobjToSend);
  cJSON_Delete(jobjToSend);  
}

int main(int argc, char *argv[])
{
  /* Socket variables */
  int sock;                       /* Socket descriptor */
  struct sockaddr_in servAddress; /* ttweet server address */
  unsigned short servPort;        /* ttweet server port */
  char *servIP;                   /* Server IP address (dotted quad) */

  /* User variables */
  int userNumber;
  char *userMessage;

  /* Variables to handle transfer of data over TCP */
  cJSON *jobjToSend; /* JSON payload to be sent */

  if (argc != 3) /* Test for correct number of arguments */
  {
    die_with_error("Command not recognized!\nUsage: $./client <ServerIP> <ServerPort>");
  }

  servIP = argv[1];         /* Server IP address (dotted quad) */
  servPort = atoi(argv[2]); /* Use given port, if any */
  //userNumber = atoi(argv[3]);     /* Parse number */
  //userMessage = argv[4];    /* Parse message */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_with_error("socket() failed");

  /* Construct the server address structure */
  memset(&servAddress, 0, sizeof(servAddress));    /* Zero out structure */
  servAddress.sin_family = AF_INET;                /* Internet address family */
  servAddress.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  servAddress.sin_port = htons(servPort);          /* Server port */

  /* Establish the connection to the ttweet server */
  if (connect(sock, (struct sockaddr *)&servAddress, sizeof(servAddress)) < 0)
    die_with_error("connect() failed");
  else
    printf("Server Connected Success\n");

  int command;
  while(1) {
    printf("0. Quit\n");
    printf("1. Set WMM traffic\n");
    printf("2. Clear WMM Traffic\n");
    printf("3. Set WMM Txop\n");
    printf("4. Reset WMM Txop\n");
    printf("Please input command: ");
    scanf("%d",&command);

    switch(command) {
      case 0:
        send_quit_json(sock);
        exit(1);
        break;
      case 1:
        send_set_WMM_traffic_json(sock);
        break;
      case 2:
        send_clear_WMM_traffic_json(sock);
        break;
      case 3:
        send_set_WMM_AC__VO_TXOP_json(sock);
        break;
      case 4:
        send_reset_WMM_AC__VO_TXOP_json(sock);
        break;
      default:
        exit(1);
    }
    printf("Payload sent!\n");
  }

  
  exit(1);
}