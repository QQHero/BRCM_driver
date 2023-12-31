/*
 * WBD Communication Related Declarations for Master
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
 * $Id: wbd_master_com_hdlr.h 808791 2022-02-28 05:27:19Z $
 */

#ifndef _WBD_MASTER_COM_HDLR_H_
#define _WBD_MASTER_COM_HDLR_H_

#include "wbd.h"
#include "wbd_shared.h"
#include "ieee1905_datamodel.h"

/* Bit fields for flags in wbd_master_info */
#define WBD_FLAGS_MASTER_FBT_ENABLED		0x0001	/* FBT enabled in Master */
#define WBD_FLAGS_MASTER_BH_OPT_TIMER		0x0002	/* Backhaul Optimization Timer is running */
#define WBD_FLAGS_MASTER_BH_OPT_RUNNING		0x0004	/* Backhaul Optimization is Running */
#define WBD_FLAGS_MASTER_BH_OPT_PENDING		0x0008	/* Backhaul Optimization is pending */

/* Helper macros for flags on wbd_master_info */
#define WBD_MINFO_IS_BH_OPT_TIMER(flags)	((flags) & WBD_FLAGS_MASTER_BH_OPT_TIMER)
#define WBD_MINFO_IS_BH_OPT_RUNNING(flags)	((flags) & WBD_FLAGS_MASTER_BH_OPT_RUNNING)
#define WBD_MINFO_IS_BH_OPT_PENDING(flags)	((flags) & WBD_FLAGS_MASTER_BH_OPT_PENDING)

/* Bit fields for flags in wbd_blanket_master */
#define WBD_FLAGS_BKT_BH_OPT_ENABLED		0x00000001	/* Backhaul Optimization feature
								 * is enabled
								 */
#define WBD_FLAGS_BKT_DONT_SEND_BCN_QRY	0x00000002	/* Don't Send Beacon Query */
#define WBD_FLAGS_BKT_DONT_USE_BCN_RPT	0x00000004	/* Don't use Beacon report */
#define WBD_FLAGS_BKT_INDEP_CHSCAN_ENAB	0x00000008	/* Independent ChScan Reporting enab ? */
#define WBD_FLAGS_BKT_DONT_UPDATE_MEDIA_INFO  0x00000010	/* Don't update media info
								 * in Topoloy response
								 */

/* Helper macros for flags on wbd_blanket_master */
#define WBD_BKT_BH_OPT_ENAB(flags)		((flags) & (WBD_FLAGS_BKT_BH_OPT_ENABLED))
#define WBD_BKT_DONT_SEND_BCN_QRY(flags)	((flags) & (WBD_FLAGS_BKT_DONT_SEND_BCN_QRY))
#define WBD_BKT_DONT_USE_BCN_RPT(flags)		((flags) & (WBD_FLAGS_BKT_DONT_USE_BCN_RPT))
#define WBD_BKT_INDEP_CHSCAN_ENAB(flags)	((flags) & (WBD_FLAGS_BKT_INDEP_CHSCAN_ENAB))
#define WBD_BKT_DONT_UPDATE_MEDIA_INFO(flags)	((flags) & (WBD_FLAGS_BKT_DONT_UPDATE_MEDIA_INFO))

/* Bit fields for wbd_master_flags NVRAM */
#define WBD_NVRAM_FLAGS_MASTER_DONT_SEND_BCN_QRY	0x0001	/* Don't Send Beacon Query */
#define WBD_NVRAM_FLAGS_MASTER_DONT_USE_BCN_RPT		0x0002	/* Don't Use Beacon report */
#define WBD_NVRAM_FLAGS_MASTER_DONT_UPDATE_MEDIA_INFO	0x0004	/* Don't update Media info */

/* stats expiry time */
#define WBD_METRIC_EXPIRY_TIME		10

/* Initialize the communication module for master */
extern int wbd_init_master_com_handle(wbd_info_t *info);

/* Process 1905 Vendor Specific Messages at WBD Application Layer */
extern int wbd_master_process_vendor_specific_msg(ieee1905_vendor_data *msg_data);

/* Load configurations for each master info */
extern void wbd_master_load_all_master_info_configurations(wbd_info_t *info);

/* Creates the blanket master for the blanket ID */
extern void wbd_master_create_master_info(wbd_info_t *info, uint8 bkt_id,
	char *bkt_name);

/* Got Associated STA link metric response */
extern void wbd_master_process_assoc_sta_metric_resp(wbd_info_t *info, unsigned char *al_mac,
	unsigned char *bssid, unsigned char *sta_mac, ieee1905_sta_link_metric *metric);

/* Got UnAssociated STA link metric response */
extern void wbd_master_unassoc_sta_metric_resp(unsigned char *al_mac,
	ieee1905_unassoc_sta_link_metric *metric);

/* Got Beacon Metrics metric response */
extern void wbd_master_beacon_metric_resp(unsigned char *al_mac, ieee1905_beacon_report *report);

/* Set AP configured flag */
extern void wbd_master_set_ap_configured(unsigned char *al_mac, unsigned char *radio_mac,
	int if_band);

/* Send channel selection request */
void wbd_master_send_channel_selection_request(wbd_info_t *info, unsigned char *al_mac,
	unsigned char *radio_mac);

/* Process Non-operable Chspec Update of an IFR, & Send Consolidated Exclude list to ACSD */
int wbd_master_process_nonoperable_channel_update(i5_dm_interface_type *in_ifr, int dynamic);

/* Add the metric policy for a radio */
void wbd_master_add_metric_report_policy(wbd_info_t *info,
	unsigned char* radio_mac, int if_band);

#if defined(MULTIAPR2)
/* Add the Channel Scan policy for a radio */
void wbd_master_add_chscan_report_policy();
#endif /* MULTIAPR2 */

/* Send Policy Configuration for a radio */
void wbd_master_send_policy_config(wbd_info_t *info, unsigned char *al_mac,
	unsigned char* radio_mac, int if_band);

/* Get Vendor Specific TLV  for Metric Reporting Policy from Master */
void wbd_master_get_vndr_tlv_metric_policy(ieee1905_vendor_data *out_vendor_tlv);

/* Update the AP channel report */
void wbd_master_update_ap_chan_report(wbd_glist_t *ap_chan_report, i5_dm_interface_type *i5_ifr);

int wbd_master_send_link_metric_requests(wbd_master_info_t *master, i5_dm_clients_type *sta);

/* Get interface info */
int wbd_master_get_interface_info_cb(char *ifname, ieee1905_ifr_info *info);

/* Handle master restart; Send renew if the agent didn't AP Auto configuration */
int wbd_master_create_ap_autoconfig_renew_timer(wbd_info_t *info);

/* Create channel prefernce query timer */
int wbd_master_create_channel_select_timer(wbd_info_t *info, unsigned char *al_mac);

/* Timer callback to send channel selection request to a device */
void wbd_master_send_channel_select_cb(bcm_usched_handle *hdl, void *arg);

/* Timer callback to send channel preference query to a device */
void wbd_master_send_channel_preference_query_cb(bcm_usched_handle *hdl, void *arg);

/* Callback from IEEE 1905 module when the controller gets Channel Selection Response */
void wbd_master_process_chan_selection_response_cb(unsigned char *al_mac,
	unsigned char *interface_mac, t_I5_CHAN_SEL_RESP_CODE chan_sel_resp_code);

/* Handle operating channel report */
void wbd_master_process_operating_chan_report(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_operating_chan_report *chan_report);

/* Update the lowest Tx Power of all BSS */
void wbd_master_update_lowest_tx_pwr(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_operating_chan_report *chan_report);

/* Store beacon metric response */
void
wbd_master_store_beacon_metric_resp(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_beacon_report *report);

/* Send BSS capability query message */
extern int wbd_master_send_bss_capability_query(wbd_info_t *info, unsigned char *al_mac,
	unsigned char* radio_mac);

/* Called when some STA joins the slave */
extern int wbd_controller_refresh_blanket_on_sta_assoc(struct ether_addr *sta_mac,
	struct ether_addr *parent_bssid, wbd_wl_sta_stats_t *sta_stats);

/* Send BSS metrics query message */
extern int wbd_master_send_bss_metrics_query(unsigned char *al_mac);

/* Send 1905 Vendor Specific backhaul STA mertric policy command, from Controller to Agent */
extern int wbd_master_send_backhaul_sta_metric_policy_vndr_cmd(wbd_info_t *info,
	unsigned char *neighbor_al_mac, unsigned char *radio_mac);

/* Send zwdfs message to all agents */
extern int wbd_master_broadcast_vendor_msg_zwdfs(uint8 *src_al_mac,
	wbd_cmd_vndr_zwdfs_msg_t *msg, uint8 band);

/* Update the multiap mode from controller to agent */
extern void wbd_master_update_multiap_mode_to_agent();

#if defined(MULTIAPR2)
/* Process tunnel message based on msg type in ieee1905_tunnel_msg control block */
extern void wbd_master_process_tunneled_msg(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_tunnel_msg_t *msg);
/* Callback fn to send OnBoot Channel Scan Request from Controller to Agent */
void wbd_master_send_onboot_channel_scan_req_cb(bcm_usched_handle *hdl, void *arg);
/* Create OnBoot Channel Scan Request timer to Get Channel Scan Report from this Agent */
extern int wbd_master_create_onboot_channel_scan_req_timer(wbd_info_t *info,
	unsigned char *al_mac);
/* Processes Channel Scan Request by Controller */
extern void wbd_master_process_channel_scan_rpt_cb(unsigned char *src_al_mac, time_t ts_chscan_rpt,
	ieee1905_chscan_report_msg *chscan_rpt);
/* Average Stats of All Existing Repeaters for same 20 MHz Channel, and Send it to ACSD */
extern int wbd_master_average_and_send_pry_info_to_acsd(unsigned char *src_al_mac,
	i5_dm_device_type *leaving_dev, ieee1905_chscan_report_msg *chscan_rpt);
/* Send CAC termination to an agent */
extern void wbd_master_send_cac_termination_message(uint8 *al_mac, uint8 *radio_mac,
	uint8 cntrl_chan, uint8 opclass);
/* Prepare and send commonf dfs channel info to all the agents */
void wbd_master_prepare_and_send_dfs_chan_info(i5_dm_device_type *src_device);
/* Set Radar detected channels in other agent as inactive in controller also */
void wbd_master_set_dfs_channel_inactive(i5_dm_device_type *src_device);
#endif /* MULTIAPR2 */

#endif /* _WBD_MASTER_COM_HDLR_H_ */
