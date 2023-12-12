//
// Created by penciljiang on 2022/1/10.
//
#include <stdio.h>
#include <stdlib.h>
#include "router_server.h"
int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Fail to start router server, you should input valid param port\n");
    return -1;
  }
  int port = atoi(argv[1]);
  if (port <= 1024 || port > 65534) {
    printf("Fail to start router server with invalid port[%d], you should input valid param port\n",
           port);
    return -1;
  }
  printf("===============Begin StartRouter Port:%d=============\n", port);
  startRouterListen(port);
  printf("===============End StartRouter=============\n");
  //  printf("hello start router");
  return 0;
}