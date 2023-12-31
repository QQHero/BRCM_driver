/*
 * WBD Shared code between master and slave
 *
 * Copyright 2022 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wbd_shared.c 808794 2022-02-28 05:52:07Z $
 */

#include <ctype.h>
#include <fcntl.h>

#include "wbd.h"
#include "wbd_shared.h"
#include "wbd_json_utility.h"
#include "wbd_sock_utility.h"
#include "wlif_utils.h"

#include "ieee1905_tlv.h"

typedef struct wbd_cmd {
	char* cmd_name;
	wbd_cmd_type_t cmd_id;
} wbd_cmd_t;

/* All the commands */
static wbd_cmd_t g_wbd_cmds[] = {
	{"WEAK_CLIENT_BSD",		WBD_CMD_WEAK_CLIENT_BSD_REQ},
	{"WEAK_CANCEL_BSD",		WBD_CMD_WEAK_CANCEL_BSD_REQ},
	{"BLOCK_CLIENT_RESP",		WBD_CMD_BLOCK_CLIENT_RESP},
	{"STA_STATUS_BSD",		WBD_CMD_STA_STATUS_BSD_REQ},
	{"BLOCK_CLIENT_BSD",		WBD_CMD_BLOCK_CLIENT_BSD_REQ},
	{"STRONG_CLIENT_BSD",		WBD_CMD_STRONG_CLIENT_BSD_REQ},
	{"STRONG_CANCEL_BSD",		WBD_CMD_STRONG_CANCEL_BSD_REQ},
};

typedef struct wbd_cli_cmd {
	char* cmd_name;
	wbd_cli_cmd_type_t cmd_id;
} wbd_cli_cmd_t;

/* All the CLI commands */
static wbd_cli_cmd_t g_wbd_cli_cmds[] = {
	{"version",		WBD_CMD_CLI_VERSION},
	{"slavelist",		WBD_CMD_CLI_SLAVELIST},
	{"steer",		WBD_CMD_CLI_STEER},
	{"info",		WBD_CMD_CLI_INFO},
	{"clientlist",		WBD_CMD_CLI_CLIENTLIST},
	{"monitorlist",		WBD_CMD_CLI_MONITORLIST},
	{"monitoradd",		WBD_CMD_CLI_MONITORADD},
	{"monitordel",		WBD_CMD_CLI_MONITORDEL},
	{"logs",		WBD_CMD_CLI_LOGS},
	{"msglevel",		WBD_CMD_CLI_MSGLEVEL},
	{"bhopt",		WBD_CMD_CLI_BHOPT},
	{"exit",		WBD_CMD_CLI_EXIT},
	{"reload",		WBD_CMD_CLI_RELOAD},
};

/* Get Random Bytes */
extern void RAND_bytes(unsigned char *buf, int num);

/* Gets the command ID from the command name */
wbd_cmd_type_t
wbd_get_command_id(const char *cmd)
{
	int i;
	wbd_cmd_type_t ret_id = -1;
	WBD_ENTER();

	for (i = 0; i < ARRAYSIZE(g_wbd_cmds); i++) {
		if (strcasecmp(cmd, g_wbd_cmds[i].cmd_name) == 0) {
			ret_id = g_wbd_cmds[i].cmd_id;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return ret_id;
}

/* Gets the command name from the command ID */
const char*
wbd_get_command_name(const wbd_cmd_type_t id)
{
	if (id < ARRAYSIZE(g_wbd_cmds))
		return g_wbd_cmds[id].cmd_name;

	return "";
}

/* Gets the CLI command ID from the command name */
wbd_cli_cmd_type_t
wbd_get_cli_command_id(const char *cmd)
{
	int i;
	wbd_cli_cmd_type_t ret_id = -1;
	WBD_ENTER();

	for (i = 0; i < ARRAYSIZE(g_wbd_cli_cmds); i++) {
		if (strcasecmp(cmd, g_wbd_cli_cmds[i].cmd_name) == 0) {
			ret_id = g_wbd_cli_cmds[i].cmd_id;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return ret_id;
}

/* Gets the CLI command name from the command ID */
const char*
wbd_get_cli_command_name(const wbd_cli_cmd_type_t id)
{
	if (id < ARRAYSIZE(g_wbd_cli_cmds))
		return g_wbd_cli_cmds[id].cmd_name;

	return "";
}

/* Try to open the Event FD's till it succeeds */
int
wbd_try_open_event_fd(int portno, int* error)
{
	int event_fd = INVALID_SOCKET;
	WBD_ENTER();

	event_fd = wbd_open_eventfd(portno);

	if (event_fd != INVALID_SOCKET) {
		WBD_INFO("Successfully Opened EVENT EAPD Port : %d\n", portno);
	}

	if (error != NULL) {
		if (event_fd == INVALID_SOCKET) {
			*error = WBDE_SOCK_ERROR;
		} else {
			*error = WBDE_OK;
		}
	}

	WBD_EXIT();
	return event_fd;
}

/* Try to open the server FD's till it succeeds */
int
wbd_try_open_server_fd(int portno, int* error)
{
	int sfd = INVALID_SOCKET;
	WBD_ENTER();

	while (1) {
		sfd = wbd_open_server_fd(portno);
		if (sfd == INVALID_SOCKET) {
			sleep(WBD_SLEEP_SERVER_FAIL);
			continue;
		}
		break;
	}
	WBD_INFO("Successfully Opened Server Port : %d\n", portno);

	if (error != NULL) {
		if (sfd == INVALID_SOCKET) {
			*error = WBDE_SOCK_ERROR;
		} else {
			*error = WBDE_OK;
		}
	}

	WBD_EXIT();
	return sfd;
}

/* Generic memory Allocation function for WBD app */
void*
wbd_malloc(unsigned int len, int *error)
{
	int ret = WBDE_MALLOC_FL;
	void* pbuffer = NULL;
	WBD_ENTER();

	if (len <= 0) {
		goto end;
	}

	pbuffer = calloc(1, len);
	if (pbuffer == NULL) {
		goto end;
	} else {
		ret = WBDE_OK;
	}

end:
	if (ret != WBDE_OK) {
		WBD_ERROR("len[%d] %s\n", len, wbderrorstr(WBDE_MALLOC_FL));
	}
	if (error) {
		*error = ret;
	}

	WBD_EXIT();
	return pbuffer;
}

/* Generic memory ReAllocation function for WBD app.
 * It will memset the newly allocated memory.
*/
void*
wbd_realloc(void *buf, unsigned int old_len, unsigned int new_len, int *error)
{
	int ret = WBDE_MALLOC_FL;
	void* pbuffer = NULL, *tmp = NULL;
	WBD_ENTER();

	if ((new_len <= 0) || (buf == NULL)) {
		goto end;
	}

	pbuffer = realloc(buf, new_len);
	if (pbuffer == NULL) {
		goto end;
	} else {
		ret = WBDE_OK;
	}

	/* Memset the newly allocated Memory */
	tmp = (uint8 *) pbuffer + old_len;
	if (new_len > old_len) {
		memset(tmp, 0, (new_len - old_len));
	}
end:
	if (ret != WBDE_OK) {
		WBD_ERROR("len[%d] %s\n", new_len, wbderrorstr(WBDE_MALLOC_FL));
	}
	if (error) {
		*error = ret;
	}

	WBD_EXIT();
	return pbuffer;
}

/* Common cli usage printing fn for master and slave apps */
static void
wbd_print_cli_usage(int argc, char **argv)
{
	printf("\n %s command line options:\n", ((argc > 1) ? argv[0] : ""));
	printf("-f | -h | -t\n");
}

/* Returns master's pid. */
static pid_t
wbd_get_master_pid()
{
	pid_t pid = -1;

	if ((pid = get_pid_by_name(WBD_MASTER_PROCESS_NAME1)) <= 0) {
		if ((pid = get_pid_by_name(WBD_MASTER_PROCESS_NAME2)) <= 0) {
			if ((pid = get_pid_by_name(WBD_MASTER_PROCESS)) <= 0) {
				WBD_ERROR("%s %s\n", WBD_MASTER_PROCESS,
					wbderrorstr(WBDE_PROC_NOT_RUNNING));
			}
		}

	}

	return pid;
}

/* Returns slave's pid. */
static pid_t
wbd_get_slave_pid()
{
	pid_t pid = -1;

	if ((pid = get_pid_by_name(WBD_SLAVE_PROCESS_NAME1)) <= 0) {
		if ((pid = get_pid_by_name(WBD_SLAVE_PROCESS_NAME2)) <= 0) {
			if ((pid = get_pid_by_name(WBD_SLAVE_PROCESS)) <= 0) {
				WBD_ERROR("%s %s\n", WBD_SLAVE_PROCESS,
					wbderrorstr(WBDE_PROC_NOT_RUNNING));
			}
		}

	}

	return pid;
}

/* Display tty info. */
static void
wbd_show_tty_info(char *name)
{
	pid_t pid = -1;
	FILE *fp = NULL;
	char *file = NULL;

	WBD_ENTER();

	/* Based of master / slave set tty file path.  */
	if (!strcmp(name, WBD_MASTER_PROCESS)) {
		if ((pid = wbd_get_master_pid()) <= 0) {
			goto end;
		}
		file = WBD_MASTER_FILE_TTY;
	} else if (!strcmp(name, WBD_SLAVE_PROCESS)) {
		if ((pid = wbd_get_slave_pid()) <= 0) {
			goto end;
		}
		file = WBD_SLAVE_FILE_TTY;
	} else {
		goto end;
	}

	fp = fopen(file, "w+");

	if (!fp) {
		WBD_ERROR("%s %s\n", wbderrorstr(WBDE_FILE_OPEN_FAILED), file);
		goto end;
	}

	fprintf(fp, "%s", ttyname(0));
	fclose(fp);
	kill(pid, SIGPWR);

end:
	WBD_EXIT();
	return;
}

/* Parse common cli arguments for master and slave apps */
void
wbd_parse_cli_args(int argc, char *argv[])
{
	char c;
	bool foreground = FALSE;

	if (argc > 1) {
		if ((c = getopt(argc, argv, "hHfFtT")) != -1) {
			switch (c) {
				case 'f':
				case 'F':
					foreground = TRUE;
					break;
				case 'h':
				case 'H':
					wbd_print_cli_usage(argc, argv);
					break;
				case 't':
				case 'T':
					wbd_show_tty_info(argv[0]);
					break;
				default:
					WBD_WARNING("%s invalid option\n", argv[0]);
			}
		}
		if (foreground == FALSE)
			exit(0);
	}

	if (foreground == FALSE) {
		if (daemon(1, 1) == -1) {
			perror("daemon");
			exit(errno);
		}
	}
}

/* get nvram setting to enable/disable log in syslogd */
static int
init_log_in_syslog(wbd_info_t *info)
{
	WBD_ENTER();

	/* WBD logs in syslogd is enabled or not */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_LOG_IN_SYSLOG, WBD_DEF_LOG_IN_SYSLOG)) {
		/* open syslog. explicit opening is optional; helps add ident,
		 * pid prefixes and set flags
		 */
		openlog((const char *)g_wbd_process_name, LOG_ODELAY, LOG_USER);

		info->flags |= WBD_INFO_FLAGS_LOG_IN_SYSLOG;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_LOG_IN_SYSLOG;
	}

	WBD_EXIT();
	return WBDE_OK;
}

/* Allocate & Initialize the info structure */
wbd_info_t*
wbd_info_init(int *error, wbd_info_t *g_info, bool reload)
{
	int ret = WBDE_OK;
	wbd_info_t *info = NULL;
	WBD_ENTER();

	/* If just reload do not allocate memory just update the global pointer */
	if (reload && g_info) {
		info = g_info;
		goto skipalloc;
	}
	/* Allocate the info structure */
	info = (wbd_info_t*)wbd_malloc(sizeof(*info), &ret);
	WBD_ASSERT_MSG("WBD Info alloc failed... Aborting...\n");

	info->version = WBD_VERSION;
	info->map_mode = MAP_MODE_FLAG_DISABLED;

	/* get nvram settings to enable/disable file operation */
	init_blanket_info(&info->wbd_bkt_info);

	/* get nvram settings to enable/disable log in syslogd */
	init_log_in_syslog(info);

	info->hdl = bcm_usched_init();
	if (info->hdl == NULL) {
		WBD_ERROR("Failed to create usched handle\n");
		ret = WBDE_USCHED_ERROR;
		goto end;
	}

	/* Initialize FBT Information of all BSSes On this Device */
	memset(&info->wbd_fbt_config, 0, sizeof(info->wbd_fbt_config));
	wbd_ds_glist_init(&(info->wbd_fbt_config.entry_list));

	snprintf(info->server_ip, sizeof(info->server_ip), "%s", WBD_LOOPBACK_IP);

	info->event_fd = INVALID_SOCKET;
	info->server_fd = INVALID_SOCKET;
	info->cli_server_fd = INVALID_SOCKET;
	wbd_ds_glist_init(&info->beacon_reports);

	WBD_INFO("Info init done\n");

skipalloc:
	wbd_retrieve_common_config(info);

	/* If disabled just exit */
	if (MAP_IS_DISABLED(info->map_mode)) {
		WBD_ERROR("Multi-AP is disabled. map_mode: %d\n", info->map_mode);
		ret = WBDE_INV_MODE;
		goto end;
	}

end:
	if (error) {
		*error = ret;
	}
	WBD_EXIT();
	return info;
}

/* Cleanup all the sockets and allocated memory */
void
wbd_info_cleanup(wbd_info_t *info)
{
	WBD_ENTER();

	/* Remove all FBT Information data items */
	wbd_ds_glist_cleanup(&(info->wbd_fbt_config.entry_list));

	/* Stop the scheduler and deinit */
	if (info->hdl) {
		bcm_usched_stop(info->hdl);
		bcm_usched_deinit(info->hdl);
	}

	wbd_close_socket(&info->event_fd);
	wbd_close_socket(&info->cli_server_fd);
	wbd_close_socket(&info->server_fd);
	wbd_ds_cleanup_beacon_reports(info);
	/* cleanup probe STA table */
	wbd_ds_cleanup_prb_sta_table(info);

	if (info) {
		free(info);
		info = NULL;
	}

	/* close the current syslog connection */
	closelog();

	WBD_INFO("Info Cleanup Done\n");
	WBD_EXIT();
}

/* Main loop which keeps on checking for the timers and fd's */
void
wbd_run(bcm_usched_handle *hdl)
{
	BCM_USCHED_STATUS status = BCM_USCHEDE_OK;

	WBD_INFO("Scheduler going to run\n");
	status = bcm_usched_run(hdl);
	WBD_WARNING("Return Code %d and Message : %s\n", status, bcm_usched_strerror(status));
}

/* Add FD to micro scheduler */
int
wbd_add_fd_to_schedule(bcm_usched_handle *hdl, int fd, void *arg, bcm_usched_fdscbfn *cbfn)
{
	int ret = WBDE_OK;
	int fdbits = 0;
	BCM_USCHED_STATUS status = 0;
	WBD_ENTER();

	if (fd != INVALID_SOCKET) {
		BCM_USCHED_SETFDMASK(fdbits, BCM_USCHED_MASK_READFD);
		status = bcm_usched_add_fd_schedule(hdl, fd, fdbits, cbfn, arg);
		if (status != BCM_USCHEDE_OK) {
			WBD_WARNING("Failed to add FD[%d]. Error : %s\n", fd,
				bcm_usched_strerror(status));
			ret = WBDE_USCHED_ERROR;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return ret;
}

/* Add timers to micro scheduler */
int
wbd_add_timers(bcm_usched_handle *hdl, void *arg, unsigned long long timeout,
	bcm_usched_timerscbfn *cbfn,
	int repeat_flag)
{
	int ret = WBDE_OK;
	BCM_USCHED_STATUS status = 0;
	WBD_ENTER();

	status = bcm_usched_add_timer(hdl, timeout, repeat_flag, cbfn, arg);
	if (status != BCM_USCHEDE_OK) {
		WBD_WARNING("Timeout[%llu]Msec arg[%p] cbfn[%p] Failed to add Timer. Error : %s\n",
			timeout, arg, cbfn, bcm_usched_strerror(status));
		if (status == BCM_USCHEDE_TIMER_EXISTS) {
			ret = WBDE_USCHED_TIMER_EXIST;
		} else {
			ret = WBDE_USCHED_ERROR;
		}
		goto end;
	}

end:
	WBD_EXIT();
	return ret;
}

/* Remove timers fm micro scheduler */
int
wbd_remove_timers(bcm_usched_handle *hdl, bcm_usched_timerscbfn *cbfn, void *arg)
{
	int ret = WBDE_OK;
	BCM_USCHED_STATUS status = 0;
	WBD_ENTER();

	status = bcm_usched_remove_timer(hdl, cbfn, arg);
	if (status != BCM_USCHEDE_OK) {
		WBD_WARNING("arg[%p] cbfn[%p] Failed to Remove Timer. Error : %s\n", arg, cbfn,
			bcm_usched_strerror(status));
		ret = WBDE_USCHED_ERROR;
		goto end;
	}

end:
	WBD_EXIT();
	return ret;
}

/* Gets common config settings for mastre and slave */
int
wbd_retrieve_common_config(wbd_info_t *info)
{
	WBD_ENTER();
	char *str;
	int num = 0;

	/* Get NVRAM : Debug Message Level */
	wbd_msglevel = blanket_get_config_val_uint(NULL,
		WBD_NVRAM_MSGLVL, WBD_DEBUG_DEFAULT);
	/* Get NVRAM : Wi-Fi Blanket Application Mode */
	info->map_mode = blanket_get_config_val_int(NULL,
		WBD_NVRAM_MULTIAP_MODE, MAP_MODE_FLAG_DISABLED);
	/* Get NVRAM : Timeout for Slave to Unblock a Steering STA */
	info->max.tm_blk_sta = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_BLK_STA, WBD_TM_SLV_BLOCK_STA);
	/* Get NVRAM : Timeout for Slave to run Weak Client Watchdog */
	info->max.tm_wd_weakclient = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_WEAK_CLIENT, WBD_TM_SLV_WD_WEAK_CLIENT);
	/* Get NVRAM : Timeout for Slave to run Target BSS Watchdog */
	info->max.tm_wd_tbss = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_TARGET_BSS, WBD_TM_SLV_WD_TARGET_BSS);
	/* Get NVRAM : Timeout for RSSI received in beacon report */
	info->max.tm_bcn_rssi = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_BCN_RSSI, WBD_TM_SLV_WD_BCN_RSSI);
	/* Get NVRAM : Timeout to read monitored RSSI for MAP */
	info->max.tm_map_monitor_read = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_MAP_MONITOR_READ, WBD_TM_MAP_MONITOR_READ);
	/* Get NVRAM : Timeout to read monitored RSSI for MAP */
	info->max.tm_map_send_bcn_report = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_MAP_SEND_BCN_REPORT, WBD_TM_MAP_SEND_BCN_REPORT);
	/* Get NVRAM : Timeout to autoconfiguration renew */
	info->max.tm_autoconfig_renew = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_AUTOCONFIG_RENEW, WBD_TM_AUTOCONFIG_RENEW);
	/* Get NVRAM : Timeout to send channel preference query */
	info->max.tm_channel_select = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_CHANNEL_SELECT, WBD_TM_CHANNEL_SELECT);
	/* Get NVRAM : Tiemout for slave process, after all AP configured */
	info->max.tm_process_ap_configure = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_PROCESS_AP_CONFIGURE, WBD_TM_PROCESS_AP_CONFIGURE);
#if defined(MULTIAPR2)
	/* Get NVRAM : Timeout to send OnBoot Channel Scan Request by Master */
	info->max.tm_onboot_chscan_req = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_MTR_ONBOOT_CHSCAN_REQ, WBD_TM_MTR_ONBOOT_CHSCAN_REQ);
	/* Get NVRAM : Timeout for slave to do onboot scan */
	info->max.tm_do_onboot_scan = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_DO_ONBOOT_SCAN, WBD_TM_DO_ONBOOT_SCAN);
#endif /* MULTIAPR2 */
	/* Get NVRAM : ACTION fram intervval for home and off-channel weak STA */
	str = blanket_nvram_prefix_safe_get(NULL, WBD_NVRAM_TM_SLV_ACT_FRAME_INTERVAL);
	if (str) {
		num = sscanf(str, "%u %u %u", &info->max.offchan_af_period,
			&info->max.offchan_af_interval,
			&info->max.tm_bcn_req_frame);
	}
	if (!str || num != 3) {
		info->max.offchan_af_period = WBD_TM_SLV_OFFCHAN_AF_PERIOD;
		info->max.offchan_af_interval = WBD_TM_SLV_OFFCHAN_AF_INTERVAL;
		info->max.tm_bcn_req_frame = WBD_TM_SLV_BCN_REQ_INTERVAL;
	}
	blanket_log_default_nvram("%s=%u %u %u\n", WBD_NVRAM_TM_SLV_ACT_FRAME_INTERVAL,
		info->max.offchan_af_period, info->max.offchan_af_interval,
		info->max.tm_bcn_req_frame);
	/* Get NVRAM : Timeout to remove stale client entry from slave */
	info->max.tm_remove_client = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_MTR_REMOVE_CLIENT_INTERVAL, WBD_TM_MTR_RM_CLIENT_INTERVAL);
	/* Get NVRAM : Remove client */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_REMOVE_CLIENT,
		WBD_DEF_REMOVE_CLIENT)) {
		info->flags |= WBD_INFO_FLAGS_REMOVE_CLIENT;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_REMOVE_CLIENT;
	}
	/* Get NVRAM : Timeout to retry remove stale client entry from slave */
	info->max.tm_retry_remove_client = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_MTR_RTRY_REMOVE_CLIENT, WBD_TM_MTR_RTRY_RM_CLIENT);
	/* Get NVRAM : Timeout for Both to do rc restart gracefully */
	info->max.tm_rc_restart = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_RC_RESTART, WBD_TM_RC_RESTART);
	/* Get NVRAM : Multi channel Support */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_MCHAN_SLAVE,
		WBD_DEF_MCHAN_SLAVE)) {
		info->flags |= WBD_INFO_FLAGS_MCHAN_SLAVE;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_MCHAN_SLAVE;
	}
	/* Get NVRAM : Single channel backhaul operation enabled or not */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_ENAB_SINGLE_CHAN_BH,
		WBD_DEF_SCHAN_BH)) {
		info->flags |= WBD_INFO_FLAGS_SCHAN_BH_ENAB;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_SCHAN_BH_ENAB;
	}
	/* Get NVRAM: Enable/disable smart conversion of beacon report rcpi */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_SMART_CONV_RCPI,
		WBD_DEF_SMART_CONV_RCPI)) {
		info->flags |= WBD_INFO_FLAGS_SMART_CONV_RCPI;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_SMART_CONV_RCPI;
	}
	/* Get NVRAM : valid rssi range */
	str = blanket_nvram_prefix_safe_get(NULL, WBD_NVRAM_VALID_RSSI_RANGE);
	if (str) {
		int max, min;
		num = sscanf(str, "%d %d", &max, &min);
		info->rssi_max = (int8)max;
		info->rssi_min = (int8)min;
	}
	if (!str || num != 2) {
		info->rssi_max = WBD_DEF_RSSI_MAX;
		info->rssi_min = WBD_DEF_RSSI_MIN;
	}
	blanket_log_default_nvram("%s=%d %d\n", WBD_NVRAM_VALID_RSSI_RANGE, info->rssi_max,
			info->rssi_min);
	/* Get NVRAM : Real Free space path loss between 2G and 5G */
	info->crossband_rssi_est = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_CROSSBAND_RSSI_EST, WBD_DEF_CROSSBAND_RSSI_EST);
	/* Get STEER Retry configs */
	str = blanket_nvram_safe_get(WBD_NVRAM_STEER_RETRY_CONFIG);
	if (str) {
		num = sscanf(str, "%d %d",
			&info->steer_retry_config.tm_gap,
			&info->steer_retry_config.retry_count);
	}
	if (!str || num != 2) {
		info->steer_retry_config.tm_gap = WBD_TM_STEER_RETRY_INTRVL;
		info->steer_retry_config.retry_count = WBD_STEER_RETRY_COUNT;
		WBD_WARNING("%s : %s = %s\n", wbderrorstr(WBDE_INV_NVVAL),
			WBD_NVRAM_STEER_RETRY_CONFIG, str);
	}
	/* Get NVRAM : Timeout to remove probe STA entry */
	info->max.tm_probe = blanket_get_config_val_int(NULL, WBD_NVRAM_TM_PROBE, WBD_TM_PROBE);

	/* Local steering allowed or not */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_BAND_STEER, WBD_DEF_BAND_STEER)) {
		info->flags |= WBD_INFO_FLAGS_BAND_STEER;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_BAND_STEER;
	}

	/* Upstream AP or not */
	if (blanket_get_config_val_int(NULL, WBD_NVRAM_UAP, WBD_DEF_UAP)) {
		info->flags |= WBD_INFO_FLAGS_UAP;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_UAP;
	}

	/* Get NVRAM : Timeout to run backhaul STA weak client watchdog */
	info->max.tm_wd_bh_weakclient = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_BH_WD_WEAK_CLIENT, WBD_TM_BH_WD_WEAK_CLIENT);

	/* Get NVRAM : Timeout for Master to run Target BSS Watchdog for Backhaul STAs */
	info->max.tm_wd_bh_tbss = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_BH_TARGET_BSS, WBD_TM_SLV_WD_BH_TARGET_BSS);

	/* Get NVRAM : Timeout for Master to run Target BSS Watchdog for Backhaul Optimization */
	info->max.tm_wd_bh_opt_tbss = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_BH_OPT_TARGET_BSS, WBD_TM_SLV_WD_BH_OPT_TARGET_BSS);

	/* Hostapd is enabled or not */
	if (blanket_get_config_val_int(NULL, NVRAM_HAPD_ENABLED, 0)) {
		info->flags |= WBD_INFO_FLAGS_IS_HAPD_ENABLED;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_IS_HAPD_ENABLED;
	}

	/* MAP certification is enabled or not */
	if (blanket_get_config_val_int(NULL, NVRAM_MAP_CERT, WBD_DEF_MAP_CERT)) {
		info->flags |= WBD_INFO_FLAGS_MAP_CERT;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_MAP_CERT;
	}

	/* Onboot Channel Scan is Forced on RootAP or not */
	if (blanket_get_config_val_int(NULL, NVRAM_MAP_CHSCAN_FORCED_ONBOOT,
		DEF_MAP_CHSCAN_FORCED_ONBOOT)) {
		info->flags |= WBD_INFO_FLAGS_FORCED_ONBOOT;
	} else {
		info->flags &= ~WBD_INFO_FLAGS_FORCED_ONBOOT;
	}

	/* Get NVRAM : MultiAP Profile */
	info->map_profile = (uint8)blanket_get_config_val_uint(NULL,
		NVRAM_MAP_PROFILE, WBD_DEF_MAP_PROFILE);

	/* Get NVRAM : 2G RSSI for backhaul STA above which band steering can be considered */
	info->t_2g_bs_bsta_rssi = blanket_get_config_val_int(NULL,
		NVRAM_MAP_2G_BSTA_BS_RSSI_THLD, WBD_DEF_2G_BSTA_BS_RSSI_THLD);

	info->max.tm_wd_fbt_resp = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_SLV_WD_FBT_RESP, WBD_TM_SLV_WD_FBT_RESP);

	info->max.tm_per_chan_bcn_req = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TM_PER_CHAN_BCN_REQ, WBD_MIN_BCN_METRIC_QUERY_DELAY);

	WBD_DEBUG("%s=%d\n %s=%d\n"
		" %s=%d\n %s=%d\n %s=%d\n"
		" %s=%d\n %s=%d\n %s=%d\n %s=%d\n"
		" %s=%d %d %d\n %s=STEER Fail Tm[%d] STEER Fail Repeat Cnt[%d]\n"
		" %s=%d\n %s=%d\n %s=%d\n %s=%d\n %s=%d\n %s=%d\n %s=%d\n %s=%d\n FLAGS=0x%x\n",
		WBD_NVRAM_MSGLVL, wbd_msglevel,
		WBD_NVRAM_MULTIAP_MODE, info->map_mode,
		WBD_NVRAM_TM_BLK_STA, info->max.tm_blk_sta,
		WBD_NVRAM_TM_SLV_WD_WEAK_CLIENT, info->max.tm_wd_weakclient,
		WBD_NVRAM_TM_SLV_WD_TARGET_BSS, info->max.tm_wd_tbss,
		WBD_NVRAM_TM_MTR_REMOVE_CLIENT_INTERVAL, info->max.tm_remove_client,
		WBD_NVRAM_TM_MTR_RTRY_REMOVE_CLIENT, info->max.tm_retry_remove_client,
		WBD_NVRAM_TM_RC_RESTART, info->max.tm_rc_restart,
		WBD_NVRAM_CROSSBAND_RSSI_EST, info->crossband_rssi_est,
		WBD_NVRAM_TM_SLV_ACT_FRAME_INTERVAL, info->max.offchan_af_period,
		info->max.offchan_af_interval, info->max.tm_bcn_req_frame,
		WBD_NVRAM_STEER_RETRY_CONFIG, info->steer_retry_config.tm_gap,
		info->steer_retry_config.retry_count,
		WBD_NVRAM_TM_PROBE, info->max.tm_probe,
		WBD_NVRAM_TM_BH_WD_WEAK_CLIENT, info->max.tm_wd_bh_weakclient,
		WBD_NVRAM_TM_SLV_WD_BH_TARGET_BSS, info->max.tm_wd_bh_tbss,
		WBD_NVRAM_TM_SLV_WD_BH_OPT_TARGET_BSS, info->max.tm_wd_bh_opt_tbss,
		WBD_NVRAM_TM_SLV_WD_FBT_RESP, info->max.tm_wd_fbt_resp,
		WBD_NVRAM_TM_PER_CHAN_BCN_REQ, info->max.tm_per_chan_bcn_req,
		NVRAM_MAP_PROFILE, info->map_profile,
		NVRAM_MAP_2G_BSTA_BS_RSSI_THLD, info->t_2g_bs_bsta_rssi,
		info->flags);

	WBD_EXIT();
	return WBDE_OK;
}

/* Gets list of mac list from NVRAM and store it in glist */
int
wbd_retrieve_maclist_from_nvram(const char *nvramval, wbd_glist_t *list)
{
	int ret = WBDE_OK;
	char *tmpmaclist = NULL, *strmaclist = NULL;
	char *mac = NULL;
	const char token[] = " ";
	struct ether_addr ether_mac;
	WBD_ENTER();

	/* Get maclist from NVRAM, separated by " " (space) */
	tmpmaclist = blanket_nvram_safe_get(nvramval);
	WBD_ASSERT_ARG(tmpmaclist, WBDE_OK);

	strmaclist = strdup(tmpmaclist);
	WBD_ASSERT_ARG(strmaclist, WBDE_MALLOC_FL);

	/* Each MAC is separated by space */
	mac = strtok(strmaclist, token);
	while (mac != NULL) {
		/* Convert string MAC to ether_addr */
		if (wbd_ether_atoe(mac, &ether_mac)) {
			/* Add to list */
			wbd_ds_add_mac_to_maclist(list, &ether_mac, NULL);
		} else {
			WBD_WARNING("%s\n", wbderrorstr(WBDE_INV_MAC));
		}
		mac = strtok(NULL, token);
	}
	free(strmaclist);

end:
	WBD_EXIT();
	return ret;
}

/* To get the digit of Bridge in which Interface belongs */
int
wbd_get_ifname_bridge(char* ifname, int *out_bridge)
{
	int ret = WBDE_OK;

	/* Validate arg */
	WBD_ASSERT_ARG(ifname, WBDE_INV_ARG);
	WBD_ASSERT_ARG(out_bridge, WBDE_INV_ARG);
	*out_bridge = -1;

	/* Find ifname in LAN Network and Guest Network by bridge name store in lanX_ifname. */
	for (int i = 0; i < WLIFU_MAX_NO_BRIDGE; i++) {
		char lan_ifnames[NVRAM_MAX_PARAM_LEN] = {0};

		if (i == 0) {
			sprintf(lan_ifnames, "lan_ifnames");
		} else {
			sprintf(lan_ifnames, "lan%d_ifnames", i);
		}

		if (find_in_list(nvram_safe_get(lan_ifnames), ifname)) {
			*out_bridge = i;
			goto end;
		}
	}

	/* Control reached here, means ifname is not valid */
	ret = WBDE_INV_IFNAME;

end:
	return ret;
}

/* Gets WBD Band Enumeration from ifname's Chanspec & Bridge Type */
int
wbd_identify_wbd_band_type(chanspec_t ifr_chanspec, int *out_band)
{
	int ret = WBDE_OK, channel = 0;
	WBD_ENTER();

	WBD_ASSERT_ARG(out_band, WBDE_INV_ARG);
	*out_band = WBD_BAND_LAN_INVALID;

	/* Fetch Current Channel form Chanspec */
	channel = CHSPEC_CHANNEL(ifr_chanspec);

	/* If Current Channel is in Band 2G */
	if (CHSPEC_IS2G(ifr_chanspec)) {
		/* WBD band type is LAN_2G */
		*out_band = WBD_BAND_LAN_2G;

		/* If Current Channel is in Band 5G */
	} else if (CHSPEC_IS5G(ifr_chanspec)) {

		if (channel < 100) {
			/* WBD band type is LAN_5GL */
			*out_band = WBD_BAND_LAN_5GL;
		} else {
			/* WBD band type is LAN_5GH */
			*out_band = WBD_BAND_LAN_5GH;
		}
	} else if (CHSPEC_IS6G(ifr_chanspec)) {
		*out_band = WBD_BAND_LAN_6G;
	}

end:
	WBD_EXIT();
	return ret;
}

/* Processes the Version CLI command */
void
wbd_process_version_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	char tmpdata[WBD_MAX_BUF_256];
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	WBD_ENTER();

	snprintf(tmpdata, sizeof(tmpdata), "Current Version is : %d\n", WBD_VERSION);
	if (wbd_com_send_cmd(hndl, childfd, tmpdata, NULL) != WBDE_OK) {
		WBD_WARNING("Failed to send CLI version response\n");
	}

	if (clidata)
		free(clidata);

	WBD_EXIT();
	return;
}

/* Processes the Set Message Level command */
void
wbd_process_set_msglevel_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	char tmpdata[WBD_MAX_BUF_256];
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	WBD_ENTER();

	wbd_msglevel = clidata->msglevel;
	blanket_set_msglevel(wbd_msglevel);
	snprintf(tmpdata, sizeof(tmpdata), "Set %d\n", clidata->msglevel);
	if (wbd_com_send_cmd(hndl, childfd, tmpdata, NULL) != WBDE_OK) {
		WBD_WARNING("Failed to send set Message Level CLI response\n");
	}

	if (clidata)
		free(clidata);

	WBD_EXIT();
	return;
}

/* Formats the SSID */
int
wbd_wl_format_ssid(char* ssid_buf, uint8* ssid, int ssid_len)
{
	int i, c;
	char *p = ssid_buf;
	WBD_ENTER();

	if (ssid_len > 32) ssid_len = 32;

	for (i = 0; i < ssid_len; i++) {
		c = (int)ssid[i];
		if (c == '\\') {
			*p++ = '\\';
			*p++ = '\\';
		} else if (isprint((uchar)c)) {
			*p++ = (char)c;
		} else {
			p += sprintf(p, "\\x%02X", c);
		}
	}
	*p = '\0';

	WBD_EXIT();
	return p - ssid_buf;
}

static char wbd_undeferrstr[32];
static const char *wbderrorstrtable[] = WBDERRSTRINGTABLE;

/* Convert the error codes into related error strings  */
const char *
wbderrorstr(int wbderror)
{
	/* check if someone added a wbderror code but forgot to add errorstring */
	assert(ABS(WBDE_LAST) == (ARRAYSIZE(wbderrorstrtable) - 1));

	/* check if wbderror is valid */
	if (wbderror > 0 || wbderror < WBDE_LAST) {
		snprintf(wbd_undeferrstr, sizeof(wbd_undeferrstr), "Undefined error %d", wbderror);
		return wbd_undeferrstr;
	}

	/* check if someone added a errorstring longer than allowed */
	assert(strlen(wbderrorstrtable[-wbderror]) < WBDE_STRLEN);

	return wbderrorstrtable[-wbderror];
}

/* strcat function with relloc buffer functionality in-built */
int
wbd_strcat_with_realloc_buffer(char **outdata, int* outlen,
	int *totlen, int len_extend, char *tmpline)
{
	int ret = WBDE_OK;
	char *tmp = NULL;
	WBD_ENTER();

	if ((*totlen + strlen(tmpline) + 1) > *outlen) {
		*outlen += len_extend;
		tmp = (char*)realloc(*outdata, sizeof(char) * (*outlen));
		if (tmp == NULL) {
			WBD_ERROR("%s of %d\n", wbderrorstr(WBDE_REALLOC_FL), (*outlen));
			ret = WBDE_REALLOC_FL;
			goto end;
		}
		*outdata = tmp;
	}
	strncat(*outdata, tmpline, ((*outlen) - (*totlen) - 1));
	*totlen += strlen(tmpline);

end:
	WBD_EXIT();
	return ret;
}

/* Get output buffer for Monitored STA Item */
int
wbd_snprintf_monitor_sta_info(wbd_monitor_sta_item_t *sta_item, char* format, char **outdata,
	int* outlen, int *totlen, int len_extend, int index)
{
	int ret = WBDE_OK, rssi = 0;
	char tmpline[WBD_MAX_BUF_512] = {0};
	WBD_ENTER();

	/* Check if valid 802.11k Beacon Report RSSI is present, show it, else show STAMON RSSI */
	rssi = (WBD_RSSI_VALID(sta_item->bcn_rpt_rssi)) ? sta_item->bcn_rpt_rssi : sta_item->rssi;
	snprintf(tmpline, sizeof(tmpline), format, index, ETHER_TO_MACF(sta_item->sta_mac), rssi);

	ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen, len_extend, tmpline);

	WBD_EXIT();
	return ret;
}

/* Get output buffer for i5_ifr Item */
int
wbd_snprintf_i5_device(i5_dm_device_type *device, char *format, char **outdata, int *outlen,
	int *totlen, int len_extend)
{
	int ret = WBDE_OK;
	char tmpline[WBD_MAX_BUF_512] = {0};

	snprintf(tmpline, sizeof(tmpline), format,
		MAC2STRDBG(device->DeviceId),
		I5_IS_MULTIAP_CONTROLLER(device->flags) ? "Controller " : "",
		I5_IS_MULTIAP_AGENT(device->flags) ? "Agent " : "",
		I5_IS_REGISTRAR(device->flags) ? "Registrar " : "");

	ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen, len_extend, tmpline);

	WBD_EXIT();

	return ret;
}

/* Get output buffer for i5_clients Item */
int
wbd_snprintf_i5_clients(i5_dm_bss_type *bss, char **outdata, int *outlen, int *totlen,
	int len_extend, bool is_bssid_req)
{
	int ret = WBDE_OK, idx = 0;
	char tmp[WBD_MAX_BUF_512] = {0};
	i5_dm_clients_type *client = NULL;
	wbd_assoc_sta_item_t *assoc_sta = NULL;
	WBD_ENTER();

	if (bss->ClientsNumberOfEntries > 0) {
		if (is_bssid_req) {
			snprintf(tmp, sizeof(tmp), CLI_CMD_I5_ASSOCLIST_BSSID_FMT,
				MAC2STRDBG(bss->BSSID), bss->ClientsNumberOfEntries);
		}
		ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen, len_extend, tmp);

		foreach_i5glist_item(client, i5_dm_clients_type, bss->client_list) {
			char buf[WBD_MAX_BUF_128] = {0};
			char *status = NULL;

			assoc_sta = (wbd_assoc_sta_item_t*)client->vndr_data;
			if (assoc_sta == NULL) {
				continue;
			}

			status = (assoc_sta->status == WBD_STA_STATUS_WEAK) ? "Weak" :
				(assoc_sta->status == WBD_STA_STATUS_STRONG) ? "STRONG" : "Normal";

			if ((assoc_sta->status == WBD_STA_STATUS_WEAK) ||
				(assoc_sta->status == WBD_STA_STATUS_STRONG) ||
				(WBD_ASSOC_ITEM_IS_BH_OPT(assoc_sta->flags))) {
				snprintf(buf, sizeof(buf), "  Rssi: %d  TxRate: %d Mbps",
					WBD_RCPI_TO_RSSI(client->link_metric.rcpi),
					client->link_metric.downlink_rate);
			}

			snprintf(tmp, sizeof(tmp), CLI_CMD_I5_ASSOCLIST_DTL_FMT,
				++idx, MAC2STRDBG(client->mac), status, buf[0] != '\0' ? buf : "");

			ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen,
				len_extend, tmp);
		}
	}

	WBD_EXIT();

	return ret;
}

/* Get output buffer for i5_client Item */
int
wbd_snprintf_i5_client(i5_dm_clients_type *client, char *format, char **outdata, int *outlen,
	int *totlen, int len_extend, int index)
{

	int ret = WBDE_OK;
	char tmpline[WBD_MAX_BUF_1024] = {0}, *status = NULL, *unit = NULL;
	wbd_assoc_sta_item_t *assoc_sta = NULL;
	i5_dm_device_type *device;
	i5_dm_interface_type *ifr;
	i5_dm_bss_type *bss;
	unsigned char byte_cntr_unit;

	bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(client);
	ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(bss);
	device = (i5_dm_device_type*)WBD_I5LL_PARENT(ifr);
	byte_cntr_unit = device->p2ApCap.byte_cntr_unit;

	assoc_sta = (wbd_assoc_sta_item_t*)client->vndr_data;
	if (assoc_sta == NULL) {
		return -1;
	}

	unit = (I5_P2APCAP_COUNTER_UNITS_GET(byte_cntr_unit)
		== I5_P2APCAP_COUNTER_UNITS_KBYTES) ? "(KiB)" :
		(I5_P2APCAP_COUNTER_UNITS_GET(byte_cntr_unit)
		== I5_P2APCAP_COUNTER_UNITS_MBYTES) ? "(MiB)" : "";

	status = (assoc_sta->status == WBD_STA_STATUS_WEAK) ? "WEAK" :
		(assoc_sta->status == WBD_STA_STATUS_STRONG) ? "STRONG" : "NORMAL";

	snprintf(tmpline, sizeof(tmpline), format,
		index, MAC2STRDBG(client->mac), status,
		WBD_RCPI_TO_RSSI(client->link_metric.rcpi),
		client->link_metric.downlink_rate,
		client->link_metric.uplink_rate,
		client->link_metric.last_data_downlink_rate,
		client->link_metric.last_data_uplink_rate,
		unit, client->traffic_stats.bytes_sent,
		unit, client->traffic_stats.bytes_recv,
		client->traffic_stats.packets_sent,
		client->traffic_stats.packets_recv,
		client->traffic_stats.tx_packet_err,
		client->traffic_stats.rx_packet_err,
		client->traffic_stats.retransmission_count);

	ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen, len_extend, tmpline);

	WBD_EXIT();

	return ret;
}

/* Get output buffer for i5_bss Item */
int
wbd_snprintf_i5_bss(i5_dm_bss_type *bss, char *format, char **outdata, int *outlen,
	int *totlen, int len_extend, bool include_clients, bool include_monitored_stas)
{
	int ret = WBDE_OK, bw = 0;
	char tmpline[WBD_MAX_BUF_512] = {0};
	char tmp[WBD_MAX_BUF_32] = {0}, map[WBD_MAX_BUF_128] = {0};
	i5_dm_interface_type *ifr = NULL;
	WBD_ENTER();

	ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(bss);

	if (wf_chspec_valid(ifr->chanspec)) {
		if (CHSPEC_IS20(ifr->chanspec)) {
			bw = 20;
		} else if (CHSPEC_IS40(ifr->chanspec)) {
			bw = 40;
		} else if (CHSPEC_IS80(ifr->chanspec)) {
			bw = 80;
		} else if (CHSPEC_IS160(ifr->chanspec)) {
			bw = 160;
		} else {
			bw = 10;
		}
		snprintf(tmp, sizeof(tmp), "%s, %d/%d", CHSPEC_IS2G(ifr->chanspec) ? "2G" :
			(CHSPEC_IS6G(ifr->chanspec) ? "6G" :"5G"),
			wf_chspec_ctlchan(ifr->chanspec), bw);
	} else {
		snprintf(tmp, sizeof(tmp), "Invalid chanspec 0x%x", ifr->chanspec);
	}

	if (bss->mapFlags > 0) {
		snprintf(map, sizeof(map), "0x%02X(%s%s%s%s)", bss->mapFlags,
			I5_IS_BSS_FRONTHAUL(bss->mapFlags) ? "Fronthaul" : "",
			((I5_IS_BSS_FRONTHAUL(bss->mapFlags) && I5_IS_BSS_BACKHAUL(bss->mapFlags)) ?
			" " : ""), I5_IS_BSS_BACKHAUL(bss->mapFlags) ? "Backhaul" : "",
			I5_IS_BSS_GUEST(bss->mapFlags) ? " Guest" : "");
	}

	snprintf(tmpline, sizeof(tmpline), format, MAC2STRDBG(bss->BSSID), bss->ssid.SSID,
		tmp, map);
	ret = wbd_strcat_with_realloc_buffer(outdata, outlen, totlen, len_extend, tmpline);

	if (include_clients) {
		ret = wbd_snprintf_i5_clients(bss, outdata, outlen, totlen, len_extend, FALSE);
	}

	/* Fetch monitored sta in the bss */
	if (include_monitored_stas) {
		int iter_sta = 0;
		wbd_bss_item_t *bss_data = (wbd_bss_item_t*)bss->vndr_data;
		wbd_monitor_sta_item_t *sta_item = NULL;
		dll_t *sta_item_p;

		if (bss_data == NULL) {
			goto end;
		}

		foreach_glist_item(sta_item_p, bss_data->monitor_sta_list) {
			iter_sta++;
			sta_item = (wbd_monitor_sta_item_t*)sta_item_p;
			ret = wbd_snprintf_monitor_sta_info(sta_item, CLI_CMD_CLILST_MONITOR_INFO,
				outdata, outlen, totlen, WBD_MAX_BUF_8192, iter_sta);

		}
	}

end:
	WBD_EXIT();

	return ret;
}

/* Get Bridge MAC */
int
wbd_get_bridge_mac(struct ether_addr *addr)
{
	int ret = WBDE_OK;
	char *nvval = NULL;
	WBD_ENTER();

	nvval = blanket_nvram_prefix_safe_get(NULL, NVRAM_LAN_HWADDR);
	WBD_GET_VALID_MAC(nvval, addr, "", WBDE_INV_MAC);

end:
	WBD_EXIT();
	return ret;
}

/* Replace all occurrences of a character "old" with "new" */
void
wbd_strrep(char *list, char c_old, char c_new)
{
	int iter = 0;

	for (iter = 0; (list[iter] != '\0'); iter++) {
		if (list[iter] == c_old) {
			list[iter] = c_new;
		}
	}
}

/* In the space-separated/null-terminated list(haystack), find total number of "needle" s */
int
wbd_count_needles(char *haystack)
{
	int iter = 0, state = 0, needles = 0;

	for (iter = 0; (haystack[iter] != '\0'); iter++) {

		/* check if current char is white space or new line or tab */
		if ((haystack[iter] == ' ') ||
			(haystack[iter] == '\n') || (haystack[iter] == '\t')) {
			state = 0;
		} else if (state == 0) {
			state = 1;
			needles++;
		}
	}
	return needles;
}

/* Concatinate 2 space-separated/null-terminated lists(haystack) by removing duplicates */
void
wbd_concat_list(char *list1, char *list2, char *out_list, int out_sz)
{
	char *next;
	char field[WBD_MAX_BUF_256];

	foreach(field, list1, next) {
		add_to_list(field, out_list, out_sz);
	}

	foreach(field, list2, next) {
		add_to_list(field, out_list, out_sz);
	}
}

/* Function to display tty output. */
void
wbd_tty_hdlr(char *file)
{
	char ttybuf[WBD_MAX_BUF_32] = "";
	FILE *fp;
	int fd;

	WBD_ENTER();

	if ((fp = fopen(file, "r")) == NULL) {
		WBD_ERROR("%s %s\n", wbderrorstr(WBDE_FILE_OPEN_FAILED), file);
		goto end;
	}

	/* Read from fp and store in ttybuf. */
	(void)fgets(ttybuf, sizeof(ttybuf), fp);
	(void)fclose(fp);

	if (ttybuf[0] == '\0') {
		WBD_ERROR("%s %s\n", file,  wbderrorstr(WBDE_FILE_IS_EMPTY));
		goto end;
	}

	/* Open in read and write mode. */
	if ((fd = open(ttybuf, O_RDWR, 0644)) == -1) {
		WBD_ERROR("%s %s!\n", wbderrorstr(WBDE_FILE_OPEN_FAILED), ttybuf);
		goto end;
	}

	/* Duplicate the fd. */
	if (dup2(fd, STDOUT_FILENO) == -1) {
		WBD_ERROR("%s %s.\n", wbderrorstr(WBDE_STDOUT_SWITCH_FAIL), ttybuf);
	}

	/* Close fd. */
	close(fd);

end:
	WBD_EXIT();
	return;
}

/* Checks whether FBT is enabled or not from NVRAMs */
int
wbd_is_fbt_nvram_enabled(char *prefix)
{
	int fbt = WBD_FBT_DEF_FBT_DISABLED;
	char *nvval;
	WBD_ENTER();

	/* Check if the akm contains psk2ft/saeft or not */
	nvval = blanket_nvram_prefix_safe_get(prefix, NVRAM_AKM);

	/* If the security is psk2, then psk2ft should be present */
	if (find_in_list(nvval, "psk2")) {
		if (find_in_list(nvval, "psk2ft") == NULL) {
			WBD_INFO("%s%s[%s]. Not psk2ft\n", prefix, NVRAM_AKM, nvval);
			goto end;
		}
	}

	/* If the security is sae, then saeft should be present */
	if (find_in_list(nvval, "sae")) {
		if (find_in_list(nvval, "saeft") == NULL) {
			WBD_INFO("%s%s[%s]. Not saeft\n", prefix, NVRAM_AKM, nvval);
			goto end;
		}
	}

	fbt = blanket_get_config_val_int(prefix, WBD_NVRAM_FBT, WBD_FBT_NOT_DEFINED);

end:
	WBD_EXIT();
	return fbt;
}

#ifdef WLHOSTFBT
/* Add a FBT BSS item in a Slave's FBT Request List */
int
wbd_add_item_in_fbt_cmdlist(wbd_fbt_bss_entry_t *in_fbt_bss,
	wbd_cmd_fbt_config_t *fbt_config, wbd_fbt_bss_entry_t **out_fbt_bss,
	bool prepend)
{
	int ret = WBDE_OK;
	wbd_fbt_bss_entry_t *new_fbt_bss = NULL;
	WBD_ENTER();

	/* Validate Args */
	WBD_ASSERT_ARG(fbt_config, WBDE_INV_ARG);

	/* Create a FBT Request List item */
	new_fbt_bss = (wbd_fbt_bss_entry_t*)wbd_malloc(sizeof(*new_fbt_bss), &ret);
	WBD_ASSERT();

	/* Initialize FBT info for FBT_CONFIG_RESP */
	memset(&(new_fbt_bss->fbt_info), 0, sizeof(new_fbt_bss->fbt_info));

	/* Assign FBT info for FBT_CONFIG_RESP */
	new_fbt_bss->fbt_info.mdid = in_fbt_bss->fbt_info.mdid;
	new_fbt_bss->fbt_info.ft_cap_policy = in_fbt_bss->fbt_info.ft_cap_policy;
	new_fbt_bss->fbt_info.tie_reassoc_interval = in_fbt_bss->fbt_info.tie_reassoc_interval;

	/* BSSID of the BSS */
	memcpy(new_fbt_bss->bssid, in_fbt_bss->bssid, sizeof(new_fbt_bss->bssid));

	/* Per BSS Bridge MAC */
	memcpy(new_fbt_bss->bss_br_mac, in_fbt_bss->bss_br_mac, sizeof(new_fbt_bss->bss_br_mac));

	/* Blanket ID */
	new_fbt_bss->blanket_id = in_fbt_bss->blanket_id;

	/* DeviceID AL MAC */
	memcpy(new_fbt_bss->al_mac, in_fbt_bss->al_mac, sizeof(new_fbt_bss->al_mac));

	/* R0KH_ID length */
	new_fbt_bss->len_r0kh_id = in_fbt_bss->len_r0kh_id;

	/* R0KH_ID */
	memcpy(new_fbt_bss->r0kh_id, in_fbt_bss->r0kh_id, sizeof(new_fbt_bss->r0kh_id));

	/* R0KH_KEY length */
	new_fbt_bss->len_r0kh_key = in_fbt_bss->len_r0kh_key;

	/* R0KH_KEY */
	memcpy(new_fbt_bss->r0kh_key, in_fbt_bss->r0kh_key, sizeof(new_fbt_bss->r0kh_key));

	/* Add this new FBT Request List item to FBT Request List */
	if (prepend) {
		wbd_ds_glist_prepend(&(fbt_config->entry_list), (dll_t *) new_fbt_bss);
	} else {
		wbd_ds_glist_append(&(fbt_config->entry_list), (dll_t *) new_fbt_bss);
	}

	if (out_fbt_bss) {
		*out_fbt_bss = new_fbt_bss;
	}
	WBD_INFO("New FBT_CONFIG_XXX item Added in FBTList : "
		"Blanket[BR%d] Device["MACDBG"] "
		"BRDG["MACDBG"] BSS["MACDBG"] "
		"R0KH_ID[%s] LEN_R0KH_ID[%d] "
		"R0KH_Key[%s] LEN_R0KH_Key[%d] "
		"MDID[%d] FT_CAP[%d] FT_REASSOC[%d]\n",
		new_fbt_bss->blanket_id,
		MAC2STRDBG(new_fbt_bss->al_mac),
		MAC2STRDBG(new_fbt_bss->bss_br_mac),
		MAC2STRDBG(new_fbt_bss->bssid),
		new_fbt_bss->r0kh_id, new_fbt_bss->len_r0kh_id,
		new_fbt_bss->r0kh_key, new_fbt_bss->len_r0kh_key,
		new_fbt_bss->fbt_info.mdid,
		new_fbt_bss->fbt_info.ft_cap_policy,
		new_fbt_bss->fbt_info.tie_reassoc_interval);
end:
	WBD_EXIT();
	return ret;
}

/* Find matching fbt_info for this bss */
wbd_fbt_bss_entry_t *
wbd_find_fbt_bss_item_for_bssid(unsigned char *bssid, wbd_cmd_fbt_config_t *fbt_config)
{
	dll_t *fbt_item_p = NULL;
	wbd_fbt_bss_entry_t *fbt_data = NULL;
	wbd_fbt_bss_entry_t *matching_fbt_bss = NULL;
	WBD_ENTER();

	/* Travese FBT Config Response List items */
	foreach_glist_item(fbt_item_p, fbt_config->entry_list) {

		fbt_data = (wbd_fbt_bss_entry_t *)fbt_item_p;

		/* Compare Self BSSID with FBT Data ITER_BSSID */
		if (memcmp(bssid, fbt_data->bssid, MAC_ADDR_LEN) == 0) {
			matching_fbt_bss = fbt_data;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return matching_fbt_bss;
}
#endif /* WLHOSTFBT */

/* Get IEEE1905_MAP_FLAG_XXX flag value for a interface */
unsigned char
wbd_get_map_flags(char *prefix)
{
	return blanket_get_config_val_uint(prefix, NVRAM_MAP, 0);
}

/* Add a Metric Policy item in a Metric Policy List */
int
wbd_add_item_in_metric_policylist(wbd_metric_policy_ifr_entry_t *in_policy_ifr,
	wbd_cmd_vndr_metric_policy_config_t *policy_config,
	wbd_metric_policy_ifr_entry_t **out_policy_ifr)
{
	int ret = WBDE_OK;
	wbd_metric_policy_ifr_entry_t *new_policy_ifr = NULL;
	WBD_ENTER();

	/* Validate Args */
	WBD_ASSERT_ARG(policy_config, WBDE_INV_ARG);

	/* Create a metric_policy item */
	new_policy_ifr = (wbd_metric_policy_ifr_entry_t*)wbd_malloc(sizeof(*new_policy_ifr), &ret);
	WBD_ASSERT();

	/* MAC of Interface */
	memcpy(new_policy_ifr->ifr_mac, in_policy_ifr->ifr_mac, sizeof(new_policy_ifr->ifr_mac));

	/* Copy Vendor Metric Policy for this Radio, in Vendor Metric Policy List */
	memcpy(&(new_policy_ifr->vndr_policy), &(in_policy_ifr->vndr_policy),
		sizeof(new_policy_ifr->vndr_policy));

	/* In the end, Add this new metric_policy item to metric_policylist */
	wbd_ds_glist_append(&(policy_config->entry_list), (dll_t *) new_policy_ifr);

	if (out_policy_ifr) {
		*out_policy_ifr = new_policy_ifr;
	}
	WBD_INFO("New METRIC_POLICY item Added in PolicyList : "
		"IFR_MAC["MACDBG"] Idle_rate_Threshold[%d] "
		"Tx_rate_Threshold[%d] Tx_failures_Threshold[%d] Flags[0x%X]\n",
		MAC2STRDBG(new_policy_ifr->ifr_mac),
		new_policy_ifr->vndr_policy.t_idle_rate, new_policy_ifr->vndr_policy.t_tx_rate,
		new_policy_ifr->vndr_policy.t_tx_failures, new_policy_ifr->vndr_policy.flags);
end:
	WBD_EXIT();
	return ret;
}

/* Find matching Metric Policy for this radio */
wbd_metric_policy_ifr_entry_t *
wbd_find_vndr_metric_policy_for_radio(unsigned char *in_ifr_mac,
	wbd_cmd_vndr_metric_policy_config_t *policy_config)
{
	dll_t *policy_item_p = NULL;
	wbd_metric_policy_ifr_entry_t *policy_data = NULL, *matching_policy_data = NULL;
	WBD_ENTER();

	/* Travese Vendor Metric Policy Config List items */
	foreach_glist_item(policy_item_p, policy_config->entry_list) {

		policy_data = (wbd_metric_policy_ifr_entry_t *)policy_item_p;

		/* Compare Radio MAC with Vendor Metric Policy item's Radio MAC */
		if (memcmp(in_ifr_mac, policy_data->ifr_mac, MAC_ADDR_LEN) == 0) {
			matching_policy_data = policy_data;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return matching_policy_data;
}

/* Convert WBD error code to weak client response reason code */
wbd_wc_resp_reason_code_t wbd_error_to_wc_resp_reason_code(int error_code)
{
	switch (error_code) {
		case WBDE_OK:
			return WBD_WC_RESP_REASON_SUCCESS;
		case WBDE_IGNORE_STA:
			return WBD_WC_RESP_REASON_IGNORE_STA;
		case WBDE_DS_BOUNCING_STA:
			return WBD_WC_RESP_REASON_BOUNCINGSTA;
		default:
			return WBD_WC_RESP_REASON_UNSPECIFIED;
	}
}

/* Convert weak client response reason code to WBD error code */
int wbd_wc_resp_reason_code_to_error(wbd_wc_resp_reason_code_t error_code)
{
	switch (error_code) {
		case WBD_WC_RESP_REASON_SUCCESS:
			return WBDE_OK;
		case WBD_WC_RESP_REASON_IGNORE_STA:
			return WBDE_IGNORE_STA;
		case WBD_WC_RESP_REASON_BOUNCINGSTA:
			return WBDE_DS_BOUNCING_STA;
		default:
			return WBDE_FAIL_XX;
	}
}

/* Get IEEE1905 message level values from NVRAM */
static void
blanket_get_ieee1905_msglevel(ieee1905_msglevel_t *msglevel)
{
	int ret = WBDE_OK, ncount = 0, idx = 0;
	char *module_list, module[WBD_MAX_BUF_16], *next_module;
	WBD_ENTER();

	memset(msglevel, 0, sizeof(*msglevel));

	msglevel->level = blanket_get_config_val_int(NULL, WBD_NVRAM_MULTIAP_MSGLEVEL, 0);

	/* Get modules list */
	module_list = blanket_nvram_safe_get(WBD_NVRAM_MULTIAP_MSG_MODULE);

	/* First count the modules in the list */
	foreach(module, module_list, next_module) {
		ncount++;
	}

	if (ncount <= 0) {
		goto end;
	}

	/* Allocate memory for modules */
	msglevel->module = (int*)wbd_malloc(ncount * sizeof(int), &ret);
	WBD_ASSERT();

	/* Store the modules */
	foreach(module, module_list, next_module) {
		msglevel->module[idx++] = (int)strtoul(module, NULL, 0);
	}
	WBD_INFO("module_list[%s] count[%d]\n", module_list, ncount);
	msglevel->module_count = ncount;

end:
	WBD_EXIT();
}

/* Read the basic common VLAN configurations required for both controller and agent */
void
wbd_get_basic_common_vlan_config(ieee1905_config *config, uint16 *sec_vlan_id)
{
	/* Get NVRAM : Traffic Separation supported or not */
	if (blanket_get_config_val_int(NULL, NVRAM_MAP_TS_SUPPORTED,
		WBD_DEF_TS_SUPPORTED)) {
		config->flags |= I5_INIT_FLAG_TS_SUPPORTED;
	}

	/* Get NVRAM : Traffic Separation is iptables or not */
	if (blanket_get_config_val_int(NULL, NVRAM_MAP_TS_IPTABLES, WBD_DEF_TS_IPTABLES)) {
		config->flags |= I5_INIT_FLAG_TS_USING_IPTABLE;
	}

	/* Get primary VLAN ID */
	config->prim_vlan_id = (unsigned short)blanket_get_config_val_uint(NULL,
		NVRAM_MAP_TS_PRIM_VLAN_ID, WBD_DEF_PRIM_VLAN_ID);

	/* Get secondary VLAN ID */
	*sec_vlan_id = (uint16)blanket_get_config_val_uint(NULL, NVRAM_MAP_TS_SEC_VLAN_ID,
		WBD_DEF_SEC_VLAN_ID);

	/* Get default PCP */
	config->default_pcp = (unsigned char)blanket_get_config_val_uint(NULL,
		NVRAM_MAP_TS_DEF_PCP, 0);

	/* This may be required if the primary VLAN ID is enabled at run time on repeater */
	config->vlan_ether_type = (unsigned short)blanket_get_config_val_uint(NULL,
		NVRAM_MAP_VLAN_ETHER_TYPE, WBD_DEF_VLAN_ETHER_TYPE);

	WBD_INFO("Primary VLAN ID[%d] Secondary VLAN ID[%d] Default PCP[%d] Flags[%x]\n\n",
		config->prim_vlan_id, *sec_vlan_id, config->default_pcp, config->flags);
}

/* Initialize the IEEE1905 module */
int
ieee1905_module_init(wbd_info_t *info, unsigned int supServiceFlag, int isRegistrar,
	ieee1905_call_bks_t *cbs, ieee1905_config *config)
{
	int ret = WBDE_OK;
	ieee1905_msglevel_t msglevel;	/* Multi AP Message Level */

	blanket_get_ieee1905_msglevel(&msglevel);

	config->map_profile = info->map_profile;

	/* Get Basic capabilities */
	(void)blanket_get_basic_cap(&config->basic_caps);

	/* Set MCHAN Flag */
	if (WBD_MCHAN_ENAB(info->flags)) {
		config->flags |= I5_INIT_FLAG_MCHAN;
	}

	config->cbs = cbs;

	ret = ieee1905_init(info->hdl, supServiceFlag, isRegistrar, &msglevel, config);
	if (msglevel.module) {
		free(msglevel.module);
	}

	return ret;
}

/* De-Initialize the blanket */
void
blanket_deinit()
{
	ieee1905_deinit();
}

/* Start MultiAP Messaging. */
void
blanket_start_multiap_messaging()
{
	ieee1905_start();
}

/* Gets NVRAM settings for backhaul */
int
wbd_retrieve_backhaul_nvram_config(wbd_weak_sta_policy_t *metric_policy)
{
	int ret = WBDE_OK, num = 0;
	wbd_weak_sta_policy_t wbd_metric;
	char *nvval;
	WBD_ENTER();

	metric_policy->t_idle_rate = WBD_BH_METRICS_REPORTING_IDLE_RATE_THLD;
	metric_policy->t_rssi = WBD_BH_METRICS_REPORTING_RSSI_THLD;
	metric_policy->t_hysterisis = WBD_BH_METRICS_REPORTING_RSSI_HYSTERISIS_MARGIN;
	metric_policy->t_tx_rate = WBD_BH_METRICS_REPORTING_TX_RATE_THLD;
	metric_policy->t_tx_failures = WBD_BH_METRICS_REPORTING_TX_FAIL_THLD;
	metric_policy->flags = WBD_BH_METRICS_REPORTING_FLAGS;

	memset(&wbd_metric, 0, sizeof(wbd_metric));
	nvval = blanket_nvram_safe_get(WBD_NVRAM_WEAK_STA_CFG_BH);
	num = sscanf(nvval, "%d %d %d %d %d %x", &wbd_metric.t_idle_rate, &wbd_metric.t_rssi,
		&wbd_metric.t_hysterisis, &wbd_metric.t_tx_rate,
		&wbd_metric.t_tx_failures, &wbd_metric.flags);

	blanket_log_default_nvram("%s=%d %d %d %d %d %x\n", WBD_NVRAM_WEAK_STA_CFG_BH,
		wbd_metric.t_idle_rate, wbd_metric.t_rssi, wbd_metric.t_hysterisis,
		wbd_metric.t_tx_rate, wbd_metric.t_tx_failures, wbd_metric.flags);

	if (num == 6) {
		memcpy(metric_policy, &wbd_metric, sizeof(*metric_policy));
	}

	WBD_INFO("NVRAM[%s=%s] IDLE_RATE[%d] RSSI[%d] HYSTERISIS[%d] "
		"PHY_RATE[%d] TX_FAILURES[%d] FLAGS[0x%X]\n",
		WBD_NVRAM_WEAK_STA_CFG_BH, nvval,
		metric_policy->t_idle_rate, metric_policy->t_rssi,
		metric_policy->t_hysterisis, metric_policy->t_tx_rate,
		metric_policy->t_tx_failures, metric_policy->flags);

	WBD_EXIT();
	return ret;
}

/* Free memory allocated for NVRAM set vendor specific command */
void
wbd_free_nvram_sets(wbd_cmd_vndr_nvram_set_t *cmd_nvram_set)
{
	int ret = WBDE_OK, idx;
	WBD_ENTER();

	/* Validate Args */
	WBD_ASSERT_ARG(cmd_nvram_set, WBDE_INV_ARG);

	/* Free common NVRAMs */
	if (cmd_nvram_set->common_nvrams) {
		free(cmd_nvram_set->common_nvrams);
		cmd_nvram_set->common_nvrams = NULL;
	}

	/* Free Radio NVRAMs */
	if (cmd_nvram_set->radio_nvrams) {
		for (idx = 0; idx < cmd_nvram_set->n_radios; idx++) {
			if (cmd_nvram_set->radio_nvrams[idx].nvrams) {
				free(cmd_nvram_set->radio_nvrams[idx].nvrams);
				cmd_nvram_set->radio_nvrams[idx].nvrams = NULL;
			}
		}
		free(cmd_nvram_set->radio_nvrams);
		cmd_nvram_set->radio_nvrams = NULL;
	}

	/* Free BSS NVRAMs */
	if (cmd_nvram_set->bss_nvrams) {
		for (idx = 0; idx < cmd_nvram_set->n_bsss; idx++) {
			if (cmd_nvram_set->bss_nvrams[idx].nvrams) {
				free(cmd_nvram_set->bss_nvrams[idx].nvrams);
				cmd_nvram_set->bss_nvrams[idx].nvrams = NULL;
			}
		}
		free(cmd_nvram_set->bss_nvrams);
		cmd_nvram_set->bss_nvrams = NULL;
	}

	/* Free SSID NVRAMs */
	if (cmd_nvram_set->ssid_nvrams) {
		for (idx = 0; idx < cmd_nvram_set->n_ssid; idx++) {
			if (cmd_nvram_set->ssid_nvrams[idx].nvrams) {
				free(cmd_nvram_set->ssid_nvrams[idx].nvrams);
				cmd_nvram_set->ssid_nvrams[idx].nvrams = NULL;
			}
		}
		free(cmd_nvram_set->ssid_nvrams);
		cmd_nvram_set->ssid_nvrams = NULL;
	}

end:
	WBD_EXIT();
	return;
}

/* check for overlap between the passed channel arguments */
bool
wbd_check_chanspec_for_overlap(chanspec_t cur_chspec, chanspec_t candi_chspec)
{
	uint8 channel1, channel2;

	FOREACH_20_SB(candi_chspec, channel1) {
		FOREACH_20_SB(cur_chspec, channel2) {
			if (channel1 == channel2) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

/* Check whether the current channel from chan info is good channel to consider or not
 */
bool
wbd_check_chan_good_candidate(int sub_channel, wbd_dfs_chan_info_t* chan_info)
{
	int j = 0;
	int channel = 0x00;

	WBD_ENTER();

	for (j = 0; j < chan_info->count; j++) {
		channel = chan_info->channel[j];

		if (sub_channel != channel) {
			continue;
		}
		return TRUE;
	}

	WBD_EXIT();
	return FALSE;
}

/* For Debug only, not required at check-in time */
/* pretty hex print a contiguous buffer */
void
prhex(const char *msg, const uchar *buf, uint nbytes)
{
	char line[128], *p;
	int len = sizeof(line);
	int nchar;
	uint i;

	if (msg && (msg[0] != '\0'))
		printf("%s:\n", msg);

	p = line;
	for (i = 0; i < nbytes; i++) {
		if (i % 16 == 0) {
			nchar = snprintf(p, len, "  %04x: ", i);	/* line prefix */
			p += nchar;
			len -= nchar;
		}
		if (len > 0) {
			nchar = snprintf(p, len, "%02x ", buf[i]);
			p += nchar;
			len -= nchar;
		}

		if (i % 16 == 15) {
			printf("%s\n", line);		/* flush line */
			p = line;
			len = sizeof(line);
		}
	}

	/* flush last partial line */
	if (p != line)
		printf("%s\n", line);
}

/* Find ie for given id and type */
bcm_tlv_t *
wbd_wl_find_ie(uint8 id, void *tlvs, int tlvs_len, const char *voui, uint8 *type, int type_len)
{
	bcm_tlv_t *ie;
	uint8 ie_len;

	ie = (bcm_tlv_t*)tlvs;

	/* make sure we are looking at a valid IE */
	if (ie == NULL || !bcm_valid_tlv(ie, tlvs_len)) {
		return NULL;
	}

	/* Walk through the IEs looking for an OUI match */
	do {
		ie_len = ie->len;
		if ((ie->id == id) && (ie_len >= (DOT11_OUI_LEN + type_len)) &&
			!bcmp(ie->data, voui, DOT11_OUI_LEN)) {
			/* compare optional type */
			if (type_len == 0 || !bcmp(&ie->data[DOT11_OUI_LEN], type, type_len)) {
				return (ie);		/* a match */
			}
		}
	} while ((ie = bcm_next_tlv(ie, &tlvs_len)) != NULL);

	return NULL;
}

/* Add a Metric Policy item in a Metric Policy List */
int
wbd_add_item_in_strong_sta_policylist(wbd_strong_sta_policy_ifr_entry_t *in_policy_ifr,
	wbd_cmd_vndr_strong_sta_policy_config_t *policy_config,
	wbd_strong_sta_policy_ifr_entry_t **out_policy_ifr)
{
	int ret = WBDE_OK;
	wbd_strong_sta_policy_ifr_entry_t *new_policy_ifr = NULL;
	WBD_ENTER();

	/* Validate Args */
	WBD_ASSERT_ARG(policy_config, WBDE_INV_ARG);

	/* Create a metric_policy item */
	new_policy_ifr = (wbd_strong_sta_policy_ifr_entry_t*)wbd_malloc(sizeof(*new_policy_ifr),
			&ret);
	WBD_ASSERT();

	/* MAC of Interface */
	memcpy(new_policy_ifr->ifr_mac, in_policy_ifr->ifr_mac, sizeof(new_policy_ifr->ifr_mac));

	/* Copy Vendor Metric Policy for this Radio, in Vendor Metric Policy List */
	memcpy(&(new_policy_ifr->vndr_policy), &(in_policy_ifr->vndr_policy),
		sizeof(new_policy_ifr->vndr_policy));

	/* In the end, Add this new metric_policy item to metric_policylist */
	wbd_ds_glist_append(&(policy_config->entry_list), (dll_t *) new_policy_ifr);

	if (out_policy_ifr) {
		*out_policy_ifr = new_policy_ifr;
	}
	WBD_INFO("New STRONG_STA_POLICY item Added in PolicyList : "
		"IFR_MAC["MACDBG"] Idle_rate[%d] RSSI[%d] Hysterisis[%d] Flags[0x%x] \n",
		MAC2STRDBG(new_policy_ifr->ifr_mac),
		new_policy_ifr->vndr_policy.t_idle_rate, new_policy_ifr->vndr_policy.t_rssi,
		new_policy_ifr->vndr_policy.t_hysterisis, new_policy_ifr->vndr_policy.flags);
end:
	WBD_EXIT();
	return ret;
}

/* Find matching Strong Sta Policy for this radio */
wbd_strong_sta_policy_ifr_entry_t *
wbd_find_vndr_strong_sta_policy_for_radio(unsigned char *in_ifr_mac,
	wbd_cmd_vndr_strong_sta_policy_config_t *policy_config)
{
	dll_t *policy_item_p = NULL;
	wbd_strong_sta_policy_ifr_entry_t *policy_data = NULL, *matching_policy_data = NULL;
	WBD_ENTER();

	/* Travese Vendor Metric Policy Config List items */
	foreach_glist_item(policy_item_p, policy_config->entry_list) {

		policy_data = (wbd_strong_sta_policy_ifr_entry_t *)policy_item_p;

		/* Compare Radio MAC with Vendor Strong Sta Policy item's Radio MAC */
		if (memcmp(in_ifr_mac, policy_data->ifr_mac, MAC_ADDR_LEN) == 0) {
			matching_policy_data = policy_data;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return matching_policy_data;
}

/* API to Convert Chanspec List to String of Hex Chanspecs separated by space */
int
wbd_convert_chspeclist_to_string(char *s_chlist, unsigned int s_chlist_size,
	ieee1905_glist_t *p_chlist)
{
	i5_chspec_item_t *new_item = NULL;
	char s_chspec[10] = {0};

	/* NULL list means, No channels are set. Return without modifying the vector */
	if (!s_chlist || !s_chlist_size || !p_chlist || !p_chlist->count) {
		return 0;
	}

	/* Non-NULL list means, Create Channel List String: <chan>,<chan>,...<chan> */
	/* Clear it first */
	foreach_iglist_item(new_item, i5_chspec_item_t, (*p_chlist)) {
		if (!new_item->chspec) {
			continue;
		}
		if (s_chlist_size < 7) {
			WBD_ERROR("Chanspec list buffer size not enough\n");
			break;
		}
		snprintf(s_chspec, sizeof(s_chspec), "0x%04x ", new_item->chspec);
		strcat(s_chlist, s_chspec);
		s_chlist_size -= 7;
	}

	return strlen(s_chlist);
}
