#include "TencentWiFi.h"



#define DECA_SUPPORTED 0


int hal_deca_supported(){
   return DECA_SUPPORTED;
}


void deca_init(){

}

int deca_policy_refresh(wifi_info* info, int deca_enabled){

   return 0;

}

int deca_start_probe(int freq_band){

   return 0;

}

int deca_stop_probe(int freq_band){

   return 0;

}


int deca_probe_failed_check(wifi_info* new_wifi_info){


	return 0;

}



