#include <bcmendian.h>
#include <wlioctl.h>
//#include <wlutils.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <wlc.h>
#include <wlc_types.h>
#include <wl_gas.h>

struct wlc_info_qq_record{
    wlc_info_t *wlc;
    int    bandtype;        /**< WLC_BAND_2G, WLC_BAND_5G, .. */
    struct ether_addr    desired_BSSID;    /**< allow this station */
    chanspec_t    chanspec;        /**< target operational channel */
};

struct wlc_info_qq_record wlc_info_qq_record_2G = {.bandtype = -1};
struct wlc_info_qq_record wlc_info_qq_record_5G = {.bandtype = -1};
void update_wlc_info_qq_record(wlc_info_t *wlc){
    struct wlc_info_qq_record *wlc_info_qq_record_cur;
    if(wlc->band->bandtype == WLC_BAND_2G){
        wlc_info_qq_record_cur = &wlc_info_qq_record_2G;
    }else if(wlc->band->bandtype == WLC_BAND_5G){
        
        wlc_info_qq_record_cur = &wlc_info_qq_record_5G;
    }else{
        return;
    }
    if(wlc_info_qq_record_cur->wlc == NULL ||!wlc_info_qq_record_cur->wlc->pub->up|| wlc_info_qq_record_cur->bandtype < 0){        
        wlc_info_qq_record_cur->wlc = wlc;
        wlc_info_qq_record_cur->bandtype = wlc->band->bandtype;
        wlc_info_qq_record_cur->desired_BSSID = wlc->desired_BSSID;
        wlc_info_qq_record_cur->chanspec = wlc->chanspec;
        printk("update_wlc_info_qq_record:OSL_SYSUPTIME()-(%u)band(%d)MAC address (hdr): %02x:%02x:%02x:%02x:%02x:%02x"
            ,OSL_SYSUPTIME(),wlc_info_qq_record_cur->bandtype,
								wlc_info_qq_record_cur->desired_BSSID.octet[0],
								wlc_info_qq_record_cur->desired_BSSID.octet[1],
								wlc_info_qq_record_cur->desired_BSSID.octet[2],
								wlc_info_qq_record_cur->desired_BSSID.octet[3],
								wlc_info_qq_record_cur->desired_BSSID.octet[4],
								wlc_info_qq_record_cur->desired_BSSID.octet[5]);
        return;
    }
    if(wlc_info_qq_record_cur->wlc->pub->_cnt->txframe<wlc->pub->_cnt->txframe){
        wlc_info_qq_record_cur->wlc = wlc;
        wlc_info_qq_record_cur->desired_BSSID = wlc->desired_BSSID;
        wlc_info_qq_record_cur->chanspec = wlc->chanspec;
        
        printk("update_wlc_info_qq_record:OSL_SYSUPTIME()-(%u)band(%d)MAC address (hdr): %02x:%02x:%02x:%02x:%02x:%02x"
            ,OSL_SYSUPTIME(),wlc_info_qq_record_cur->bandtype,
								wlc_info_qq_record_cur->desired_BSSID.octet[0],
								wlc_info_qq_record_cur->desired_BSSID.octet[1],
								wlc_info_qq_record_cur->desired_BSSID.octet[2],
								wlc_info_qq_record_cur->desired_BSSID.octet[3],
								wlc_info_qq_record_cur->desired_BSSID.octet[4],
								wlc_info_qq_record_cur->desired_BSSID.octet[5]);
    }
}



// 生成BSS Transition请求帧的数据部分
int create_bss_trans_req_data(struct ether_addr sta_mac, struct ether_addr target_bssid,
                              uint16 target_chanspec, uint8 *data, uint16 *len) {
    dot11_bsstrans_req_t *trans_req;

    // 清空并填充BSS Transition请求帧的内容
    *len = DOT11_BSSTRANS_REQ_LEN;
    trans_req = (dot11_bsstrans_req_t *)data;
    trans_req->category = DOT11_ACTION_CAT_WNM;
    trans_req->action = DOT11_WNM_ACTION_BSSTRANS_REQ;
    trans_req->token = (uint8)OSL_RAND();
    trans_req->reqmode = DOT11_BSSTRANS_REQMODE_PREF_LIST_INCL;
    trans_req->disassoc_tmr = 0; // 不强制切换
    trans_req->validity_intrvl = 0;

    // 填充目标BSS信息
    dot11_neighbor_rep_ie_t *nbr_rep_ie = (dot11_neighbor_rep_ie_t *)trans_req->data;
    nbr_rep_ie->id = DOT11_MNG_NEIGHBOR_REP_ID;
    nbr_rep_ie->len = DOT11_NEIGHBOR_REP_IE_FIXED_LEN;
    memcpy(&nbr_rep_ie->bssid, &target_bssid, sizeof(struct ether_addr));
    nbr_rep_ie->bssid_info = 0; // 可根据需要设置合适的值
    nbr_rep_ie->reg = 0; // 可根据需要设置合适的值
    nbr_rep_ie->channel = wf_chspec_ctlchan(target_chanspec);
    nbr_rep_ie->phytype = 0; // 可根据需要设置合适的值

    return 0;
}

// 示例用法
int btm_qq_send(wlc_info_t *wlc, struct ether_addr sta_mac, int bandtype){
    // 初始化部分
    struct wlc_info_qq_record *wlc_info_qq_record_cur;
    if(bandtype == WLC_BAND_2G){
        wlc_info_qq_record_cur = &wlc_info_qq_record_2G;
    }else if(bandtype == WLC_BAND_5G){
        
        wlc_info_qq_record_cur = &wlc_info_qq_record_5G;
    }else{
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

    return ret;
}