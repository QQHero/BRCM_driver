#include <bcmendian.h>
#include <wlioctl.h>
//#include <wlutils.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <wlc.h>
#include <wlc_types.h>
#include <wl_gas.h>
#include <wlc_hw_priv.h>

#include <bcm_steering.h>

#include <wlc_qq_struct.h>






extern struct wlc_info_qq_record wlc_info_qq_record_2G;
extern struct wlc_info_qq_record wlc_info_qq_record_5G;


/* Steer STA */
void bsd_steer_sta_qq(bsd_info_t *info, bsd_sta_info_t *sta, bsd_bssinfo_t *to_bss)
{
	int ret = BSD_OK;
	bsd_bssinfo_t *bssinfo;
	bsd_intf_info_t *intf_info;
	bsd_bssinfo_t *steer_bssinfo;
	bsd_maclist_t *sta_ptr;
	wl_wlif_bss_trans_data_t ioctl_data;
	bsd_steerlib_callback_data_t *callback_data = NULL;

	BSD_ENTER();

	BSD_STEER("Steering sta[%p], to_bss[%p]\n", sta, to_bss);

	if (sta == NULL) {
		/* no action */
		return;
	}

	bssinfo = sta->bssinfo;
	if (bssinfo == NULL) {
		/* no action */
		return;
	}

	intf_info = bssinfo->intf_info;

	/* get this interface's bssinfo */
	steer_bssinfo = (to_bss != NULL) ? to_bss : bssinfo->steer_bssinfo;

	/* adding STA to maclist and set mode to deny */
	/* deauth STA */

	/* avoid flip-flop if sta under steering from target interface */
	sta_ptr = bsd_maclist_by_addr(steer_bssinfo, &(sta->addr));
	if (sta_ptr && (sta_ptr->steer_state == BSD_STA_STEERING)) {
		BSD_STEER("Skip STA:"MACF" under steering\n", ETHER_TO_MACF(sta->addr));
		return;
	}

	/* Fill bsd callback data to steer lib. */
	callback_data = (bsd_steerlib_callback_data_t*)calloc(1, sizeof(*callback_data));
	if (!callback_data) {
		BSD_STEER("Calloc failed for steer lib callback data.\n");
		return;
	}

	callback_data->info = info;
	callback_data->bssinfo = bssinfo;
	callback_data->steer_bssinfo = steer_bssinfo;
	memcpy(&callback_data->sta, sta, sizeof(callback_data->sta));

	/* removing from steer-ed (target) intf maclist */
	bsd_remove_maclist(steer_bssinfo, &(sta->addr));
	wl_wlif_unblock_mac(steer_bssinfo->wlif_hdl, sta->addr, 0);
#ifdef CMWIFI_RDKB
	snprintf(log_buf, sizeof(log_buf), "Remove STA "MACF" from blacklist on %s when sending BTM req\n",
		ETHER_TO_MACF(sta->addr), steer_bssinfo->ifnames);
	WLAN_APPS_LOGS("BSD", log_buf);
#endif

	/* Fill bss-trans action frame data and invoke bss-trans API */
	memset(&ioctl_data, 0, sizeof(ioctl_data));
	ioctl_data.rclass = steer_bssinfo->rclass;
	ioctl_data.chanspec = steer_bssinfo->chanspec;
	eacopy(&(steer_bssinfo->bssid), &(ioctl_data.bssid));
	eacopy(&(sta->addr), &(ioctl_data.addr));
	ioctl_data.timeout = (intf_info->band != BSD_BAND_2G) ?
		info->block_sta_timeout : info->block_sta_2g_timeout;
	ioctl_data.bssid_info = steer_bssinfo->bssid_info;
	ioctl_data.phytype = steer_bssinfo->phytype;
	ioctl_data.resp_hndlr = bsd_process_bss_trans_resp;
	ioctl_data.resp_hndlr_data = callback_data;
	ioctl_data.flags |= WLIFU_BSS_TRANS_FLAGS_BTM_ABRIDGED;

	BSD_STEER("BSD Sending BTM REQ for Steering sta:"MACF" from "
		"%s[%d][%d]["MACF"[ to %s[%d][%d]["MACF"] RClass[%d]"
		"Chanspec[0x%x] Timeout[%d] BSSID_Info[0x%x] PHY_Type[0x%x] BTM_flags[0x%x]\n",
		ETHER_TO_MACF(sta->addr), bssinfo->prefix, intf_info->idx, bssinfo->idx,
		ETHER_TO_MACF(bssinfo->bssid),
		bssinfo->steer_prefix, (steer_bssinfo->intf_info)->idx, steer_bssinfo->idx,
		ETHER_TO_MACF(steer_bssinfo->bssid), steer_bssinfo->rclass,
		steer_bssinfo->chanspec, ioctl_data.timeout,
		steer_bssinfo->bssid_info, steer_bssinfo->phytype, ioctl_data.flags);

#ifdef CMWIFI_RDKB
	snprintf(log_buf, sizeof(log_buf), "Add STA "MACF" to blacklist on %s when sending BTM req\n",
		ETHER_TO_MACF(sta->addr), bssinfo->ifnames);
	WLAN_APPS_LOGS("BSD", log_buf);
#endif
	ret = wl_wlif_do_bss_trans(sta->bssinfo->wlif_hdl, &ioctl_data);
	BSD_STEER("BSD %sSent BTM REQ for Steering sta:"MACF" from "
		"%s[%d][%d]["MACF"[ to %s[%d][%d]["MACF"] ret[%d]\n", ((ret != 0) ? "Not " : ""),
		ETHER_TO_MACF(sta->addr), bssinfo->prefix, intf_info->idx, bssinfo->idx,
		ETHER_TO_MACF(bssinfo->bssid),
		bssinfo->steer_prefix, (steer_bssinfo->intf_info)->idx, steer_bssinfo->idx,
		ETHER_TO_MACF(steer_bssinfo->bssid), ret);
	/* If the BTM request is not sent free the callback data */
	if (ret && callback_data) {
		free(callback_data);
		callback_data = NULL;
	}

	BSD_EXIT();
	return;
}

// 示例用法
int btm_qq_send(wlc_info_t *wlc, struct ether_addr sta_mac, int bandtype){
    // 初始化部分
    struct wlc_info_qq_record *wlc_info_qq_record_cur;
    if(bandtype == WLC_BAND_2G){
        wlc_info_qq_record_cur = &wlc_info_qq_record_2G;
        struct ether_addr G2_ether_addr = {
            .octet = {0xB8, 0x3A, 0x08, 0xB6, 0x83, 0xF2}
        };
        wlc_info_qq_record_cur->desired_BSSID = G2_ether_addr;
    }else if(bandtype == WLC_BAND_5G){
        
        wlc_info_qq_record_cur = &wlc_info_qq_record_5G;
    }else{
        printk("in btm_qq_send1:OSL_SYSUPTIME()-(%u)band(%d)MAC address (hdr): %02x:%02x:%02x:%02x:%02x:%02x"
            ,OSL_SYSUPTIME(),wlc->band->bandtype,
                            wlc->hw->etheraddr.octet[0],
                            wlc->hw->etheraddr.octet[1],
                            wlc->hw->etheraddr.octet[2],
                            wlc->hw->etheraddr.octet[3],
                            wlc->hw->etheraddr.octet[4],
                            wlc->hw->etheraddr.octet[5]);
        return 0;
    }
    // void *w; // 无线控制器指针
    int bsscfg_idx = 0; // BSS配置索引
    //chanspec_t chspec20; // 当前20MHz信道
    int32 dwell_time = 0; // 停留时间（毫秒）
    //struct ether_addr sta_mac; // 终端设备的MAC地址
    //struct ether_addr target_bssid; // 目标2.4GHz BSS的BSSID
    //uint16 target_chanspec; // 目标2.4GHz BSS的信道信息

    // 假设已经获取了w, bsscfg_idx, chspec20, dwell_time, sta_mac, target_bssid和target_chanspec的值

    // 生成BSS Transition请求帧的数据部分
    uint8 data[ACTION_FRAME_SIZE];
    uint16 len;
    create_bss_trans_req_data(sta_mac, wlc_info_qq_record_cur->desired_BSSID, wlc_info_qq_record_cur->chanspec, data, &len);

    // 发送BSS Transition请求帧
    int ret;
    ret = wl_gas_tx_actframe(wlc, bsscfg_idx, (uint32)OSL_RAND(), wlc->chanspec, dwell_time, NULL, &sta_mac, len, data);
    // 在这里添加处理响应的逻辑，例如根据响应更新终端设备状态、处理拒绝情况等
    printk("in btm_qq_send2:OSL_SYSUPTIME()-(%u);band(%d);ret(%d);MAC address (hdr): %02x:%02x:%02x:%02x:%02x:%02x"
        ,OSL_SYSUPTIME(), wlc_info_qq_record_cur->bandtype, ret,
                            wlc_info_qq_record_cur->desired_BSSID.octet[0],
                            wlc_info_qq_record_cur->desired_BSSID.octet[1],
                            wlc_info_qq_record_cur->desired_BSSID.octet[2],
                            wlc_info_qq_record_cur->desired_BSSID.octet[3],
                            wlc_info_qq_record_cur->desired_BSSID.octet[4],
                            wlc_info_qq_record_cur->desired_BSSID.octet[5]);

    return ret;
}