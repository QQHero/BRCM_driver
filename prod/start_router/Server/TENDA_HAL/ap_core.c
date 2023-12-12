#include <signal.h>
#include "TencentWiFi.h"
#include "../mp_util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>



#define BUFSIZE 256
#define DATARATEMBIT (1000 * 8 / 1024 / 1024)

uint32_t mp_version_num = 675501011;

void init_hal()
{

}

void clear_hal_info()
{


}

int get_hal_version()
{
	return HAL_VERSION_AX3;

}

static int exec_prog(char **argv)
{
    pid_t   my_pid;
    int     status, timeout /* unused ifdef WAIT_FOR_COMPLETION */;

    if (0 == (my_pid = fork())) {
            if (-1 == execve(argv[0], (char **)argv , NULL)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }

#ifdef WAIT_FOR_COMPLETION
    timeout = 1000;

    while (0 == waitpid(my_pid , &status , WNOHANG)) {
            if ( --timeout < 0 ) {
                    perror("timeout");
                    return -1;
            }
            sleep(1);
    }

    printf("%s WEXITSTATUS %d WIFEXITED %d [status %d]\n",
            argv[0], WEXITSTATUS(status), WIFEXITED(status), status);

    if (1 != WIFEXITED(status) || 0 != WEXITSTATUS(status)) {
            perror("%s failed, halt system");
            return -1;
    }

#endif
    return 0;
}

int set_wmm_traffic(int wlan_interface,char *server_ip_address,int net_mask_length,int port,int ac_queue_tos )
{
    char net_mask_string[80];
    sprintf(net_mask_string,"%d",net_mask_length);

    char port_string[80];
    sprintf(port_string,"%d",port);

    char ac_queue_tos_string[80];
    sprintf(ac_queue_tos_string,"%d",ac_queue_tos);

    char* args[] = {"/bin/wifibase","-s","set_wme_ac_ip","wlan1",server_ip_address,net_mask_string,port_string,ac_queue_tos_string, NULL};
    exec_prog((char **)args);
}

int clear_wmm_traffic()  //TODO: the tos queue id should be maintain
{   
    int ac_queue_tos = 7; 
    char ac_queue_tos_string[80];
    sprintf(ac_queue_tos_string,"%d",ac_queue_tos);

    char* args[] = {"/bin/wifibase","-s","set_wme_ac_ip","wlan1","0.0.0.0","0","0",ac_queue_tos_string, NULL};
    exec_prog((char **)args);
}

int set_wmm_parameter(char* ac_queue_name,char* parameter_name,char* parameter_number) 
{   
    char* args[] = {"/bin/wl","-i","wl1","wme_ac","ap",ac_queue_name,parameter_name,parameter_number,NULL};
    exec_prog((char **)args);
}

int disable_wlan0() 
{   
    char* args[] = {"/bin/wl","-i","wl0","down",NULL};
    exec_prog((char **)args);
}


int ip_to_mac_address(char* ip_addr, char* mac_address) 
{
    char cmd[BUFSIZE];	 
    char buf[BUFSIZE];
    FILE *fp;
	int found = 0;
	char dest[BUFSIZE];
    memset(dest, 0, BUFSIZE);
    int num = 0;
	char *tempstr = NULL;

	sprintf(cmd, "arp %s",ip_addr); 

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }

	
	
    
    if(fgets(buf, BUFSIZE, fp) != NULL) {
		tempstr = strstr(buf, "at");
		printf("tempstr-%s\n",tempstr);
		num =strstr(tempstr,"[") - tempstr ;
		printf("tempstr num-%d\n",num);
		//strncpy(dest,tempstr+3,num-3);
		strncpy(dest,tempstr+3,num-4);
		strcpy(mac_address, dest);
		pclose(fp);
		return 0;
    }

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        return -1;
    }
	printf("Mac addr not found\n");
    return -1;
}

int split(char dst[][BUFSIZE], char* str, const char* spl)
{

    int n = 0;
    char *result = NULL;
	char a;
	int i=1;
	a = str[0];
	while (a!=NULL) {
		a = str[i];
		++i;
	}
    result = strtok(str, spl);
    while( result != NULL )
    {
        strcpy(dst[n++], result);
        result = strtok(NULL, spl);
    }
    return n;
}


ch_info* get_channel_info(session_node* node)
{
	char *cmd = NULL;
	char buf[BUFSIZE];
	FILE *fp;
	char dest[BUFSIZE];
	memset(dest, 0, BUFSIZE);
	int num = 0;
	char *tempstr = NULL;

	if(node->freg_band == FREQUENCY_BAND_2GHZ)
		cmd = "wl -i wl0 chanim_stats";	  

	if(node->freg_band == FREQUENCY_BAND_5GHZ)
		cmd = "wl -i wl1 chanim_stats";	 

	if ((fp = popen(cmd, "r")) == NULL) {
			printf("Error opening pipe!\n");
			return NULL;
	}

	//skip two lines
	for (int i=0; i<2 ;i++){
		if(fgets(buf, BUFSIZE, fp)== NULL){
			printf("No return for channel_info!\n");
			pclose(fp);
			return NULL;
		}
	}

	if(fgets(buf, BUFSIZE, fp)== NULL){
		printf("No return for channel_info!\n");
		pclose(fp);
		return NULL;
	}

	char dst[20][BUFSIZE];
	char spl[3];
	spl[0] = 0x09; //TAB saperate
	spl[1] = ' ';
	spl[2] = '\0';
    int cnt = split(dst, buf, spl);
	
    ch_info* temp_info;
    temp_info = (ch_info *) malloc(sizeof(ch_info));
	memset(temp_info, 0, sizeof(ch_info));
	temp_info->txop = (uint8_t)atoi(dst[7]);
	temp_info->brokensignal_count = (uint32_t)atoi(dst[10]);
	temp_info->noise_level = (int16_t)atoi(dst[12]);
	pclose(fp);
	return temp_info;

}

ap_info* get_bssap_info(session_node* node)
{
	char *cmd = NULL;  
	char buf[BUFSIZE];
	FILE *fp;
	char dest[BUFSIZE];
	memset(dest, 0, BUFSIZE);
	int num = 0;
	char *tempstr = NULL;
	int sta_status = -1;


	if(node->freg_band == FREQUENCY_BAND_2GHZ)
		cmd = "wl -i wl0 bs_data";	  

	if(node->freg_band == FREQUENCY_BAND_5GHZ)
		cmd = "wl -i wl1 bs_data";	 


	if ((fp = popen(cmd, "r")) == NULL) {
			printf("Error opening pipe!\n");
			return NULL;
	}
	if(fgets(buf, BUFSIZE, fp) == NULL){
		printf("No return for bssap_info!\n");
		pclose(fp);
		return NULL;
	}

    uint8_t sta_count = 0;
	int overall_datarate = 0;
	int air_time = 0;
	while (fgets(buf, BUFSIZE, fp) != NULL) {
		char dst[20][BUFSIZE];
		char spl[3];
		spl[0] = 0x09;   //TAB saperate
		spl[1] = ' ';
		spl[2] = '\0';
    	int cnt = split(dst, buf, spl);
		if(cnt > 0){
			if(strcmp(dst[0],"(overall)")!= 0){
				sta_count++;
				string_lower_to_upper(node->sta_mac_addr);
				if(strcmp(dst[0],node->sta_mac_addr) == 0){
					sta_status = 0;
					char temp_string[10];
					strncpy(temp_string, dst[3], strlen(dst[3])- 1);
					air_time = atoi(temp_string);
					//debug_print("airtime:%d\n", air_time);
				}
			}
			else{
				overall_datarate = atoi(dst[2]);
				//debug_print("overall datarate:%d\n", overall_datarate);

			}
		}
	}


	if(sta_status == -1){
		if(node->freg_band == FREQUENCY_BAND_2GHZ)
			cmd = "wl -i wl1 bs_data";	
		if(node->freg_band == FREQUENCY_BAND_5GHZ)
			cmd = "wl -i wl0 bs_data";	 

		if ((fp = popen(cmd, "r")) == NULL) {
			printf("Error opening pipe!\n");
			return NULL;
		}
		if(fgets(buf, BUFSIZE, fp) == NULL){
			printf("No return for bssap_info!\n");
			pclose(fp);
			return NULL;
		}  

		while (fgets(buf, BUFSIZE, fp) != NULL) {
			char dst[20][BUFSIZE];
			char spl[3];
			spl[0] = 0x09;   //TAB saperate
			spl[1] = ' ';
			spl[2] = '\0';
    		int cnt = split(dst, buf, spl);
			if(cnt > 0){
				if(strcmp(dst[0],node->sta_mac_addr) == 0){
					sta_status = SESSION_STOPREASON_SWITCH_BAND;
					break;
				}
			}
		}
		if(sta_status == -1){	
			sta_status = SESSION_STOPREASON_DEASSOC;
		}
	}


	ap_info* temp_info;
    temp_info = (ap_info *) malloc(sizeof(ap_info));
	memset(temp_info, 0, sizeof(ap_info));
	temp_info->sta_count = sta_count;
	temp_info->overall_datarate = overall_datarate;
	temp_info->air_time = air_time;
	temp_info->sta_status = sta_status;
	pclose(fp);
	return temp_info;

}

sta_info* get_station_info(session_node* node)
{
	char cmd[BUFSIZE];	  
	char buf[BUFSIZE];
	FILE *fp;
	char dest[BUFSIZE];
	memset(dest, 0, BUFSIZE);
	int num = 0;
	char *tempstr = NULL;


	if(node->freg_band == FREQUENCY_BAND_2GHZ)
		sprintf(cmd, "wl -i wl0 sta_info %s",node->sta_mac_addr); 

	if(node->freg_band == FREQUENCY_BAND_5GHZ)
		sprintf(cmd, "wl -i wl1 sta_info %s",node->sta_mac_addr); 


	if ((fp = popen(cmd, "r")) == NULL) {
			printf("Error opening pipe!\n");
			return NULL;
	}

	sta_info* temp_info;
    temp_info = (sta_info *) malloc(sizeof(sta_info));
	memset(temp_info, 0, sizeof(sta_info));
	uint32_t tx_failure_delta = 0;
	uint32_t tx_retry_delta = 0;
	uint32_t tx_total_pkts_delta = 0;
	uint32_t rx_data_pkt_delta = 0;
	uint32_t rx_failure_delta = 0;

	node->sample_number += 1;

	if(fgets(buf, BUFSIZE, fp) == NULL){
		printf("No return for sta_info!\n");
		return NULL;
	}
	else{
	
    	while (fgets(buf, BUFSIZE, fp) != NULL) {

		//printf("console string:%s\n",buf);
		char dst[20][BUFSIZE];
    	int cnt = split(dst, buf, " ");

  

			if((!strcmp(dst[1],"tx")) && (!strcmp(dst[2],"total")) && (!strcmp(dst[3],"bytes:"))){
				//printf("tx total bytes string:%s\n",dst[4]);

	   			uint64_t bytes_now = (uint64_t)string_to_umax(dst[4],NULL,0);

				//printf("tx total bytes now:%llu\n",bytes_now);
				//printf("tx total bytes last time:%llu\n",node->tx_total_bytes);
	

				uint64_t bytes_delta = bytes_now - node->tx_total_bytes;
				//printf("tx total bytes delta:%llu\n",bytes_delta);
				if(bytes_delta < 0){
					bytes_delta += ULLONG_MAX; //add uint64_t maximum value
				}
				node->tx_total_bytes = bytes_now;
				if(bytes_delta != 0 && node->sample_number > 1){
				temp_info->tx_data_rate = (bytes_delta / node->timer_ms) * 0.00762939453125; //nomalize to Mbit unit
			//temp_info->tx_data_rate = (bytes_delta / node->timer_ms) * DATARATEMBIT; //nomalize to Mbit unit

		    
			//printf("tx_data_rate:%f\n", temp_info->tx_data_rate);
				}
			}	

			if((!strcmp(dst[1],"tx")) && (!strcmp(dst[2],"failures:"))){
		
        		//printf("tx failures:%s\n",dst[3]);
				uint32_t failure_now = (uint32_t)strtoul(dst[3],NULL,0);
				tx_failure_delta = failure_now - node->tx_failure_pkts;
				if(tx_failure_delta < 0){
					tx_failure_delta += ULONG_MAX; //add uint32_t maximum value
				}
				node->tx_failure_pkts = failure_now;

			//printf("tx failure delta:%u\n", tx_failure_delta);
		
			}

			if((!strcmp(dst[1],"tx")) && (!strcmp(dst[2],"pkts")) && (!strcmp(dst[3],"retries:"))){
        		//printf("tx pkts retries string:%s\n",dst[4]);
				uint32_t retry_now = (uint32_t)strtoul(dst[4],NULL,0);
				//printf("tx pkts retries now:%u\n", retry_now);
				tx_retry_delta = retry_now - node->tx_retry_pkts;
				//printf("tx pkts retries last time:%u\n", node->tx_retry_pkts);
				if(tx_retry_delta < 0){
					tx_retry_delta += ULONG_MAX; //add uint32_t maximum value
				}
				node->tx_retry_pkts = retry_now;

				//printf("tx pkts retries delta:%u\n", tx_retry_delta);
			}

			if((!strcmp(dst[1],"rx")) && (!strcmp(dst[2],"data")) && (!strcmp(dst[3],"pkts:"))){
        		//printf("rx data pkts:%s\n",dst[4]);
				uint32_t rxpkt_now = (uint32_t)strtoul(dst[4],NULL,0);
				rx_data_pkt_delta = rxpkt_now - node->rx_total_pkts;
				if(rx_data_pkt_delta < 0){
					rx_data_pkt_delta += ULONG_MAX; //add uint32_t maximum value
				}
				node->rx_total_pkts = rxpkt_now;

				//printf("rx data pkt delta:%u\n",rx_data_pkt_delta );
			}

			if((!strcmp(dst[1],"tx")) && (!strcmp(dst[2],"total")) && (!strcmp(dst[3],"pkts:"))){
				//printf("tx total pkts:%s\n"	,dst[4]);
				uint32_t txpkt_now = (uint32_t)strtoul(dst[4],NULL,0);
				//printf("tx total pkts int:%u\n",txpkt_now);
				//printf("tx total pkts int before:%u\n",node->tx_total_pkts);
				tx_total_pkts_delta = txpkt_now - node->tx_total_pkts;
				if(tx_total_pkts_delta < 0){
					tx_total_pkts_delta += ULONG_MAX; //add uint32_t maximum value
				}
				node->tx_total_pkts = txpkt_now;

				//printf("tx total pkt delta:%u\n",tx_total_pkts_delta  );
			}

			if((!strcmp(dst[1],"rx")) && (!strcmp(dst[2],"decrypt")) && (!strcmp(dst[3],"failures:"))){
				//printf("rx decrypt failures:%s\n"	,dst[4]);
				uint32_t rx_failure_now = (uint32_t)strtoul(dst[4],NULL,0);
				rx_failure_delta = rx_failure_now - node->rx_decrypt_fail_pkts;
				if(rx_failure_delta < 0){
					rx_failure_delta += ULONG_MAX; //add uint32_t maximum value
				}
				node->rx_decrypt_fail_pkts = rx_failure_now;

				//printf("rx failure delta:%u\n",rx_failure_delta  );
				
			}

			if((!strcmp(dst[0],"smoothed")) && (!strcmp(dst[1],"rssi:"))){
				//printf("rssi:%s\n"	,dst[2]);
				temp_info->rssi = atoi(dst[2]);
			}

			if(!strcmp(dst[1],"chanspec")) {
				char dst2[10][BUFSIZE];
				//printf("chanspec:%s\n"	,dst[2]);
				//printf("chanspec:%s\n"	,dst[3]);
				int cnt = split(dst2, dst[2], "/");
				temp_info->channel_num = atoi(dst2[0]);
				if(node->freg_band == FREQUENCY_BAND_5GHZ){
					temp_info->channel_bw = atoi(dst2[1]);
					temp_info->frequency_band = FREQUENCY_BAND_5GHZ;
				}
				else{
					temp_info->channel_bw = 20;    //for 2.4GHz band, bw is 20
					temp_info->frequency_band = FREQUENCY_BAND_2GHZ;
				}	
			}
			
			if((!strcmp(dst[1],"rate")) && (!strcmp(dst[2],"of")) && (!strcmp(dst[3],"last")) && (!strcmp(dst[4],"tx"))
				&& (!strcmp(dst[5],"pkt:"))){
				//printf("phy rate:%s\n",dst[6]);
				//printf("fb phy rate:%s\n",dst[9]);
				temp_info->phy_rate = atoi(dst[6])/1000;
				temp_info->fb_phy_rate = atoi(dst[9])/1000;
			}

			if((!strcmp(dst[1],"per")) && (!strcmp(dst[2],"antenna")) && (!strcmp(dst[3],"noise")) && (!strcmp(dst[4],"floor:"))){
				int antenna_count = 0;
				for(int i=5; i< 9 ; i++){
					int j = atoi(dst[i]); 
					if(j != 0)
						antenna_count++;		
				}
				temp_info->antenna_count = antenna_count;
				//printf("antenna count:%d\n",antenna_count);
			}
    	}
	}

	if(tx_total_pkts_delta != 0 && node->sample_number > 1){
		temp_info->tx_failure_count = tx_failure_delta;
		temp_info->tx_retry_count = tx_retry_delta;
		temp_info->tx_pkt_count = tx_total_pkts_delta;
		temp_info->tx_attempt_count = (float)tx_retry_delta  /(float) tx_total_pkts_delta;
		temp_info->tx_attempt_count += 1;
	}
	else{
		temp_info->tx_failure_count = 0;
		temp_info->tx_retry_count = 0;
		temp_info->tx_pkt_count = 0;
		temp_info->tx_attempt_count = 1;  //actually it is a average value, default is 1
	}
	if(rx_data_pkt_delta != 0 && node->sample_number > 1){
		temp_info->rx_decrypt_failure_count = rx_failure_delta;
	}
	else{
		temp_info->rx_decrypt_failure_count = 0;
	}
	
		
    pclose(fp);
	return temp_info;

}

//This function needs to be re-written for each wifi HW platform
wifi_info* get_wifi_info(session_node* cb_node)
{
	char message[UDP_BUFSIZE];
	int mem_index;



	
	ch_info* new_chinfo = get_channel_info(cb_node);
	if(new_chinfo == NULL){
		debug_print("get channel info failed\n");
		memset(message, 0, UDP_BUFSIZE);
		mem_index = compose_session_error_report(message,cb_node->session_id, SESSION_ERROR_ABNORMAL_CHANNEL_DATA);
		if(udp_send(cb_node->sock_udp_fd,&cb_node->addr, message, mem_index) == -1){
			debug_print("session id:%s udp send error\n",cb_node->session_id);
		}
		return;
	}	
	else{
		/*
		debug_print("session id:%s, idle rate:%d\n",cb_node->session_id, new_chinfo->idle_rate);
		debug_print("broken count:%d\n",new_chinfo->brokensignal_count);
		debug_print("noise level:%d\n",new_chinfo->noise_level);
		*/
	}


	ap_info* new_apinfo = get_bssap_info(cb_node);
	if(new_apinfo == NULL){
		debug_print("get bssap info failed\n");
		memset(message, 0, UDP_BUFSIZE);
		mem_index = compose_session_error_report(message,cb_node->session_id, SESSION_ERROR_ABNORMAL_BSS_DATA);
		if(udp_send(cb_node->sock_udp_fd,&cb_node->addr, message, mem_index) == -1){
			debug_print("session id:%s udp send error\n",cb_node->session_id);
		}
		free(new_chinfo);
		return;
	}
	else{
		if(new_apinfo->sta_status == SESSION_STOPREASON_SWITCH_BAND){
			debug_print("session id:%s STA already swtich band, stop session\n",cb_node->session_id);
			mp_stop_session(cb_node->session_id, SESSION_STOPREASON_SWITCH_BAND);
			free(new_chinfo);
			return;
		}
		if(new_apinfo->sta_status == SESSION_STOPREASON_DEASSOC){
			debug_print("session id:%s STA deassociated, stop session\n",cb_node->session_id);
			if(cb_node->session_status == SESSION_STATUS_ONGOING && cb_node->last_rssi > -70){
				mp_stop_session(cb_node->session_id, SESSION_STOPREASON_DEASSOC_HIGH_RSSI);
			}
			else{
				mp_stop_session(cb_node->session_id, SESSION_STOPREASON_DEASSOC);
			}
			free(new_chinfo);
			return;
		}
	}

	sta_info* new_stainfo = get_station_info(cb_node);
	if(new_stainfo == NULL){
		debug_print("get station info failed\n");
		memset(message, 0, UDP_BUFSIZE);
		mem_index = compose_session_error_report(message,cb_node->session_id, SESSION_ERROR_ABNORMAL_STA_DATA);
		if(udp_send(cb_node->sock_udp_fd,&cb_node->addr, message, mem_index) == -1){
			debug_print("session id:%s udp send error\n",cb_node->session_id);
		}
		free(new_chinfo);
		free(new_apinfo);
		return;
	}
	else{	
		
	}
	
	int idle_rate_total = new_chinfo->txop + new_apinfo->air_time;
	if(idle_rate_total > 100)
		idle_rate_total = 100;


	wifi_info* temp_info;
    temp_info = (wifi_info *) malloc(sizeof(wifi_info));
	memset(temp_info, 0, sizeof(wifi_info));

	temp_info->rssi = new_stainfo->rssi;
	temp_info->phy_rate = new_stainfo->phy_rate;
	temp_info->sta_count = new_apinfo->sta_count;
	temp_info->idle_rate = idle_rate_total;
	temp_info->txop = new_chinfo->txop ;
	temp_info->tx_attempt_avg = new_stainfo->tx_attempt_count;
	temp_info->tx_fail_cnt = new_stainfo->tx_failure_count;
	temp_info->tx_data_rate = new_stainfo->tx_data_rate;
	temp_info->broken_signal_cnt = new_chinfo->brokensignal_count;
	temp_info->noise_level = new_chinfo->noise_level;
	temp_info->channel_bw = new_stainfo->channel_bw;     
	temp_info->freq_band = cb_node->freg_band;      //0-2.4G  1-5G
	temp_info->overall_datarate = new_apinfo->overall_datarate;
	temp_info->air_time = new_apinfo->air_time;
	//no below value in 6755 platform
	temp_info->lost_ac_pkts = NA_VALUE_INT;
	temp_info->total_ac_pkts = NA_VALUE_INT;
	temp_info->avg_mac_delay = NA_VALUE_INT;
	temp_info->max_mac_delay = NA_VALUE_INT;
	temp_info->mac_delay_dist1_count = NA_VALUE_INT;
	temp_info->mac_delay_dist2_count = NA_VALUE_INT;
	temp_info->mac_delay_dist3_count = NA_VALUE_INT;
	temp_info->mac_delay_dist4_count = NA_VALUE_INT;
	temp_info->mac_delay_dist5_count = NA_VALUE_INT;
	temp_info->pktq_pkt_dropped = NA_VALUE_INT;
	temp_info->pktq_queue_length = NA_VALUE_INT;	

	free(new_chinfo);
	free(new_apinfo);
	free(new_stainfo);
	return temp_info;

}


int get_pktq_stats(int ac_queue_id)
{
	char *cmd = "wl -i wl1 pktq_stats";	  
	char buf[BUFSIZE];
	FILE *fp;
	char dest[BUFSIZE];
	memset(dest, 0, BUFSIZE);
	int num = 0;
	char *tempstr = NULL;

	//usleep(10000); //10ms

	if ((fp = popen(cmd, "r")) == NULL) {
			printf("Error opening pipe!\n");
			return -1;
	}
	if(fgets(buf, BUFSIZE, fp) == NULL){
		printf("No return for pktq_stats!\n");
		pclose(fp);
			return -1;
	}
	while (fgets(buf, BUFSIZE, fp) != NULL) {
		char dst[20][BUFSIZE];
		char* spl = ":";
    	int cnt = split(dst, buf, spl);
		if(cnt > 0){
			//if(strstr(dst[0],"12")!= NULL){       //12 is the first VO queue
			char str[5];
			int num = ac_queue_id * 2;
			sprintf(str, "%d",num);
			if(strstr(dst[0],str)!= NULL){ 
				char dst2[20][BUFSIZE];
				char* spl = ",";
				int cnt2 = split(dst2, dst[1], spl);
					int len = strlen(dst2[9]);   //9 is the element to store the Tx data rate
					int i;
					char temp[20];
					strcpy(temp, dst2[9]);
					for(i =0; i<len; i++){
						if(temp[i] != 0x20)
							break;
					}
					char temp2[20];
					strcpy(temp2, temp+i-1);
					return atoi(temp2);							
			}
		}
	}
	return -1;
}

int get_sta_freq_band(session_node* node)
{
	char cmd[BUFSIZE];	  
	sprintf(cmd, "wl -i wl1 sta_info %s",node->sta_mac_addr);

	char buf[BUFSIZE];
	FILE *fp;
	char dest[BUFSIZE];
	memset(dest, 0, BUFSIZE);
	int num = 0;
	char *tempstr = NULL;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	if(fgets(buf, BUFSIZE, fp) == NULL){
		printf("No return for sta_info!\n");
		node->freg_band = FREQUENCY_BAND_2GHZ;
	}
	else{
		node->freg_band = FREQUENCY_BAND_5GHZ;
	}
	pclose(fp);
	return 0;
	
}

int turn_off_rts()
{
	return -2; //not supported in this platform
}

int turn_on_rts()
{	
	return -2; //not supported in this platform
}