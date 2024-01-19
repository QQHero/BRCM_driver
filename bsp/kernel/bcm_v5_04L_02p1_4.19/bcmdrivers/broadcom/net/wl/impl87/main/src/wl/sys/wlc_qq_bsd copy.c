#include <bcmendian.h>
#include <wlioctl.h>
//#include <wlutils.h>
#include <wlc.h>
#include <wlc_types.h>

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
    if(wlc_info_qq_record_cur->bandtype < 0){        
        wlc_info_qq_record_cur->wlc = wlc;
        wlc_info_qq_record_cur->desired_BSSID = wlc->desired_BSSID;
        wlc_info_qq_record_cur->chanspec = wlc->chanspec;
        printk("update_wlc_info_qq_record:OSL_SYSUPTIME()----------(%u)band(%d)",OSL_SYSUPTIME(),wlc_info_qq_record_cur->bandtype);
        return;
    }
    if(wlc_info_qq_record_cur->wlc->pub->_cnt->txframe<wlc->pub->_cnt->txframe){
        wlc_info_qq_record_cur->wlc = wlc;
        wlc_info_qq_record_cur->desired_BSSID = wlc->desired_BSSID;
        wlc_info_qq_record_cur->chanspec = wlc->chanspec;
        
        printk("update_wlc_info_qq_record:OSL_SYSUPTIME()----------(%u)band(%d)",OSL_SYSUPTIME(),wlc_info_qq_record_cur->bandtype);
    }
}





// 处理BSS Transition响应
void handle_bss_trans_resp(wl_action_frame_t *af, struct ether_addr *sta_mac) {
    // 解析响应帧
    dot11_bsstrans_resp_t *trans_resp = (dot11_bsstrans_resp_t *)af->data;

    // 检查响应状态
    if (trans_resp->status == DOT11_BSSTRANS_RESP_STATUS_ACCEPT) {
        printf("BSS Transition request accepted by STA "MACF"\n", ETHER_TO_MACF(*sta_mac));
        // 更新终端设备状态，例如将其从5GHz频段移除并添加到2.4GHz频段
    } else {
        printf("BSS Transition request rejected by STA "MACF"\n", ETHER_TO_MACF(*sta_mac));
        // 处理拒绝情况，例如尝试其他优化措施或记录失败原因
    }
}

// 发送BSS Transition请求帧
int send_bss_trans_request(const char *ifname, struct ether_addr sta_mac,
                           struct ether_addr target_bssid, uint16 target_chanspec) {
    int ret;
    wl_action_frame_t action_frame;
    dot11_bsstrans_req_t *trans_req;

    // 清空并填充BSS Transition请求帧
    memset(&action_frame, 0, sizeof(action_frame));
    memcpy(&action_frame.da, &sta_mac, sizeof(struct ether_addr));
    action_frame.len = DOT11_BSSTRANS_REQ_LEN;
    action_frame.packetId = (uint32)OSL_RAND();

    // 填充BSS Transition请求帧的内容
    trans_req = (dot11_bsstrans_req_t *)action_frame.data;
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

    // 发送BSS Transition请求帧
    ret = wl_actframe(ifname, &action_frame, -1, handle_bss_trans_resp, &sta_mac);
    if (ret < 0) {
        printf("Error sending BSS Transition request: %d\n", ret);
    }

    return ret;
}

/*
// 示例用法
int main() {
    const char *ifname = "eth1"; // 无线接口名称

    // 当需要切换终端设备时
    struct ether_addr sta_mac; // 终端设备的MAC地址
    struct ether_addr target_bssid; // 目标2.4GHz BSS的BSSID
    uint16 target_chanspec; // 目标2.4GHz BSS
    // 假设已经获取了sta_mac, target_bssid和target_chanspec的值

    // 发送BSS Transition请求帧
    send_bss_trans_request(ifname, sta_mac, target_bssid, target_chanspec);

    return 0;
}*/