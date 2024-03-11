#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>

#define DEBUG_MAX_CLASS 8
#define DEBUG_CLASS_MAX_FIELD 240

#define DEBUG_CLASS_PHY 0
#define DEBUG_PHY_RSSI 0



#define PRINT_pkt_info 1
#define PRINT_pkt_count_info 1
#define PRINT_phy_info 1
#define PRINT_frameid_info 1
#define PRINT_multiuser_info 0
#define PRINT_monitor_info 0
#define PRINT_rate_change_info 1




#define MAX_MCS_QQ 30
struct rates_counts_txs_qq {
	uint32_t tx_cnt[MAX_MCS_QQ];
	uint32_t txsucc_cnt[MAX_MCS_QQ];
	uint32_t txrts_cnt;
	uint32_t rxcts_cnt;
	uint32_t ncons;
	uint32_t nlost;
};
typedef uint32_t kernel_info_t;
/* dump_flag_qqdx */
//#if 0
#define CCASTATS_MAX    9
typedef struct pkt_qq {
    uint32_t tcp_seq;/* Starting sequence number */
    uint32_t ampdu_seq;/* preassigned seqnum for AMPDU */
    uint32_t packetid;/* 未知变量packetid */
    uint16_t FrameID;//每个数据帧生命周期不变的
    uint16_t pktSEQ;//也许每个数据包生命周期不变的
	uint16_t n_pkts;       /**< number of queued packets */
    uint32_t fifo;
    uint8_t tid;//tid
    uint8_t APnum;//AP数量
    uint32_t pkt_qq_chain_len_add_start;//记录是第几个送入硬件的包
    uint32_t pkt_qq_chain_len_add_end;//记录数据包被释放时有多少包被送入硬件
    uint32_t pktnum_to_send_start;//本包送入硬件时硬件待发送队列包量
    uint32_t pktnum_to_send_end;//数据包被释放时硬件待发送队列包量
    uint32_t pkt_added_in_wlc_tx_start;//本包送入硬件时wlc_tx文件中实际准备发送的数据包量（不仅仅start数据）
    uint32_t pkt_added_in_wlc_tx_end;//数据包被释放时wlc_tx文件中实际准备发送的数据包量
    struct rates_counts_txs_qq rates_counts_txs_qq_start;
    struct rates_counts_txs_qq rates_counts_txs_qq_end;
    uint32_t into_CFP_time;/*进入CFP的时间*/
    uint8_t into_CFP_time_record_loc;/*记录进入CFP的时间的位置*/
    uint32_t into_hw_time;/*进入硬件队列的时间*/
    uint32_t free_time;/*传输成功被释放的时间*/
    uint32_t into_hw_txop;/*进入硬件队列的txop*/
    uint32_t free_txop;/*传输成功被释放的txop*/
    uint32_t txop_in_fly;/*传输过程中的busy_time*/
    uint32_t busy_time;/*传输过程中的txop*/
    uint32_t drop_time;/*传输失败被丢弃的时间*/
    uint32_t droped_withoutACK_time;/*传输失败被丢弃的时间*/
    uint8_t failed_cnt;/*发射失败次数*/
    uint32_t ps_totaltime;/*该scb设备进入ps的总时间，为了这个统计，我在wl_mk中添加了WL_PS_STATS = 1，但是失败了，路由器崩溃
    所以我在多处增加了#ifndef WL_PS_STATS #define WL_PS_STATS*/
    uint32_t ps_dur_trans;//传输过程中的PS统计
    uint32_t airtime_self;/*该数据包所在帧的airtime*/
    uint32_t airtime_all;/*该数据包进入硬件发送队列以后所有已发送帧的airtime之和*/
    uint32_t failed_time_list_qq[10];/*发射失败时间列表*/
    uint32_t retry_time_list_qq[10];/*发射失败重传时间列表*/
    uint32_t retry_time_list_index;/*发射失败重传时间列表当前index*/
    uint32_t ccastats_qq[CCASTATS_MAX];/*一些发送时间相关的变量*/
    uint32_t ccastats_qq_differ[CCASTATS_MAX];
    /*PPS相关变量*/
	uint32_t ps_pretend_probe;
	uint32_t ps_pretend_count;
	uint8_t  ps_pretend_succ_count;
	uint8_t  ps_pretend_failed_ack_count;
    uint32_t time_in_pretend_tot;
    uint32_t time_in_pretend_in_fly;
    /*总的进入PPS 时间
    该统计博通并未开启，通过BCMDBG宏来关闭相关统计，需要一个一个开启（将BCMDBG改为BCMDBG_PPS_qq并define），如下是所开启的相关部分：
    1.wlc_pspretend_scb_time_upd相关（wlc_pspretend.h，wlc_pspretend.c,wlc_app.c）
    2.wlc_pspretend.h中增加#ifndef BCMDBG_PPS_qq   #define BCMDBG_PPS_qq   #endif
    3.wlc_pspretend_supr_upd相关（wlc_pspretend.h，wlc_pspretend.c,wlc_app.c,wlc_ampdu.c,wlc_txs.c）
    4.scb_pps_info_t定义处（wlc_pspretend.c）
    5.本文件   PPS时间统计相关   部分
    */

    uint32_t qq_pkttag_pointer;
    struct pkt_qq *next;
    struct pkt_qq *prev;
    
}pkt_qq_t;

struct pkt_count_qq {
    uint32_t pkt_qq_chain_len_now;
    uint32_t pkt_qq_chain_len_add;
    uint32_t pkt_qq_chain_len_soft_retry;
    uint32_t pkt_qq_chain_len_acked;
    uint32_t pkt_qq_chain_len_unacked;
    uint32_t pkt_qq_chain_len_timeout;
    uint32_t pkt_qq_chain_len_outofrange;
    uint32_t pkt_qq_chain_len_notfound;
    uint32_t pkt_qq_chain_len_found;
    uint32_t pkt_phydelay_dict[30];    
}pkt_count_qq_t;




#define PKT_ERGODIC_MAX 150
struct pkt_ergodic {
    uint8_t print_loc;
    uint32_t pkt_len;
    uint32_t real_pkt_num;
    uint32_t cur_time;
    uint32_t into_CFP_time[PKT_ERGODIC_MAX];
    uint16_t pkt_FrameID[PKT_ERGODIC_MAX];
}pkt_ergodic_t;


static struct timespec pre_timestamp_class1;//上次打印的时间戳
static struct timespec pre_timestamp_class2;//上次打印的时间戳
static struct timespec pre_timestamp_class3;//上次打印的时间戳
static struct timespec pre_timestamp_class5;//上次打印的时间戳
static struct timespec pre_timestamp_class6;//上次打印的时间戳
static struct timespec pre_timestamp_class7;//上次打印的时间戳
static struct timespec pre_timestamp_class8;//上次打印的时间戳

typedef struct {
    struct timespec timestamp;  // for debugging
    //kernel_info_t info[sizeof(pkt_qq_t)];
    kernel_info_t info[DEBUG_CLASS_MAX_FIELD];
} info_class_t;



/*rssi的ring buffer*/
#define RSSI_RING_SIZE 40

typedef struct {
    int8_t RSSI;
    int8_t noiselevel;
    int32_t timestamp;
} DataPoint_qq;

#define RATESEL_MFBR_NUM      4
#define AC_COUNT		4	/* number of ACs */
struct phy_info_qq {
    uint8_t fix_rate;
    uint32_t mcs[RATESEL_MFBR_NUM];
    uint32_t nss[RATESEL_MFBR_NUM];
    uint32_t rate[RATESEL_MFBR_NUM];
    uint8_t BW[RATESEL_MFBR_NUM];
    uint32_t ISSGI[RATESEL_MFBR_NUM];
    int8_t SNR;
    int32_t RSSI;
    int16_t RSSI_loc;//0:wlc_cfp.c;1:wlc_qq.c;2:wlc_rx;3.wlc_lq_rssi_get
    int16_t RSSI_type;//数据包类型
    int16_t RSSI_subtype;//数据包类型
    int8_t noiselevel;
    uint8_t rssi_ring_buffer_index;
    DataPoint_qq rssi_ring_buffer[RSSI_RING_SIZE];
    uint8_t channel_index;
    uint8_t real_BW;
    
    /* PHY parameters */
	uint16_t    	chanspec;		/**< target operational channel */
	uint16_t		usr_fragthresh;		/**< user configured fragmentation threshold */
	uint16_t		fragthresh[AC_COUNT];	/**< per-AC fragmentation thresholds */
	uint16_t		RTSThresh;		/**< 802.11 dot11RTSThreshold */
	uint16_t		SRL;			/**< 802.11 dot11ShortRetryLimit */
	uint16_t		LRL;			/**< 802.11 dot11LongRetryLimit */
	uint16_t		SFBL;			/**< Short Frame Rate Fallback Limit */
	uint16_t		LFBL;			/**< Long Frame Rate Fallback Limit */

	/* network config */
	bool		shortslot;		/**< currently using 11g ShortSlot timing */
	int8_t		shortslot_override;	/**< 11g ShortSlot override */
	bool		ignore_bcns;		/**< override: ignore non shortslot bcns in a 11g */
	bool		interference_mode_crs;	/**< aphy crs state for interference mitigation */
	bool		legacy_probe;		/**< restricts probe requests to CCK rates */
}phy_info_qq_t;

//#endif





/* enum for different types Multi Users technologies */
typedef enum {
	WL_MU_NONE	= 0, /* No MU means SU */
	WL_MU_DLMMU	= 1, /* MU MIMO. Includes both VHT and HE */
	WL_MU_DLOFDMA	= 2, /* HE Downlink OFDMA */
	WL_MU_ULOFDMA	= 3, /* Uplink OFDMA */
	WL_MU_TWT	= 4, /* TWT */
	WL_MU_MAX	= 5
} mu_type_t;


typedef enum {
	D11AX_RU26_TYPE,
	D11AX_RU52_TYPE,
	D11AX_RU106_TYPE,
	D11AX_RU242_TYPE,
	D11AX_RU484_TYPE,
	D11AX_RU996_TYPE,
	D11AX_RU1992_TYPE,
	D11AX_RU_MAX_TYPE
} ru_type_t;


/* HE Timing related parameters (802.11ax D0.5 Table: 26.9 */
#define HE_T_LEG_STF			8
#define HE_T_LEG_LTF			8
#define HE_T_LEG_LSIG			4
#define HE_T_RL_SIG			4
#define HE_T_SIGA			8
#define HE_T_STF			8
#define HE_T_LEG_PREAMBLE		(HE_T_LEG_STF + HE_T_LEG_LTF + HE_T_LEG_LSIG)
#define HE_T_LEG_SYMB			4
#define HE_RU_26_TONE			26
#define HE_RU_52_TONE			52
#define HE_RU_106_TONE			106
#define HE_RU_242_TONE			242
#define HE_RU_484_TONE			484
#define HE_RU_996_TONE			996
#define HE_RU_2x996_TONE		(2*996)
#define HE_MAX_26_TONE_RU_INDX		36
#define HE_MAX_52_TONE_RU_INDX		52
#define HE_MAX_106_TONE_RU_INDX		60
#define HE_MAX_242_TONE_RU_INDX		64
#define HE_MAX_484_TONE_RU_INDX		66
#define HE_MAX_996_TONE_RU_INDX		67
#define HE_MAX_2x996_TONE_RU_INDX	68

/* musched dl policy definition */
#define MUSCHED_DL_POLICY_DISABLE		0
#define MUSCHED_DL_POLICY_FIXED			1
#define MUSCHED_DL_POLICY_TRIVIAL		2
#define MUSCHED_DL_POLICY_MAX			MUSCHED_DL_POLICY_TRIVIAL
#define MUSCHED_DL_POLICY_AUTO			(-1)

/* RU alloc mode */
#define MUSCHED_RUALLOC_AUTO		(-1)
#define MUSCHED_RUALLOC_RUCFG		0 /* badsed on rucfg table */
#define MUSCHED_RUALLOC_UCODERU		1 /* vasip based- ucode ru alloc */
#define MUSCHED_RUALLOC_MINRU		2 /* vasip based- minRU alloc */
#define MUSCHED_RUALLOC_MAX		MUSCHED_RUALLOC_MINRU

#define MUSCHED_RU_IDX_NUM		HE_MAX_2x996_TONE_RU_INDX /* 68 */
#define MUSCHED_RU_TYPE_NUM		D11AX_RU_MAX_TYPE
#define MUSCHED_RU_160MHZ		2
#define MUSCHED_RU_80MHZ		1
#define MUSCHED_RU_BMP_ROW_SZ		MUSCHED_RU_160MHZ
#define MUSCHED_RU_BMP_COL_SZ		((MUSCHED_RU_IDX_NUM - 1) / 8 + 1) /* 9 */


#define	ETHER_ADDR_LEN		6
struct	ether_addr {
	uint8_t octet[ETHER_ADDR_LEN];
};


/* forward declaration */
#define MUSCHED_RUCFG_ROW		16
#define MUSCHED_RUCFG_COL		16

enum d11_rev128_bw_enum {
	D11_REV128_BW_20MHZ = 0,
	D11_REV128_BW_40MHZ = 1,
	D11_REV128_BW_80MHZ = 2,
	D11_REV128_BW_160MHZ = 3,
	D11_REV128_BW_SZ = 4
};
/* for dl ofdma */
#define MUSCHED_DLOFDMA_MINUSER_SZ		2	/* 0: <=80 MHz 1: 160MHz */
#define MUSCHED_DLOFDMA_BW20_MAX_NUM_USERS	8	/* Default DL OFDMA BW20 max clients */

typedef struct musched_ru_stats {
	uint32_t	tx_cnt[MUSCHED_RU_TYPE_NUM]; /* total tx cnt per ru size */
	uint32_t	txsucc_cnt[MUSCHED_RU_TYPE_NUM]; /* succ tx cnt per ru size */
	uint8_t	ru_idx_use_bmap[MUSCHED_RU_BMP_ROW_SZ][MUSCHED_RU_BMP_COL_SZ];
} musched_ru_stats_t;
/* enum for different types Multi Users technologies */
/*musched info*/
struct scb_flagsinfo{
    bool wme;
    bool ampdu;
    bool amsdu;
    bool amsdu_in_ampdu;
    bool dtpc;
    bool ht;
    bool vht;
    bool isgf;
    bool nongf;
    bool coex;
    bool stbc;
    bool ht_ldpc;
    bool ht_prop_rates;
    bool oper_mode_notif;
    bool he;
    bool ibss_peer;
    bool pktc;
    bool qos;
    bool p2p;
    bool dwds;
    bool dwds_cap;
    bool map;
    bool map_p2;
    bool ecsa;
    bool legacy_wds;
    bool a4_data;
    bool a4_null_data;
    bool a4_8021x;
    bool mfp;
    bool sha256;
    bool qam_1024;
    bool vhtmu;
    bool hemmu;
    bool dlofdma;
    bool ulofdma;
    bool ulmmu;
    bool rrm;
    bool ftm;
    bool ftm_initiator;
    bool ftm_responder;
} scb_flagsinfo_t;
struct musched_info_qq {
    bool wlc_fifo_isMU;
    bool wlc_fifo_is_ulofdma;
    bool wlc_fifo_is_dlofdma;
    uint8_t mu_type;
    uint16_t mch;
    uint16_t mcl;
    uint16_t mch2;
    struct scb_flagsinfo scb_flags;
	uint16_t	flags;
	int	scbh;
	int16_t	dl_policy;
	int8_t	dl_schidx; /* decided by dl_policy; internal used by ucode */
	int8_t	rualloc; /* rualloc mode */
	int8_t	ack_policy;
	bool	mix_ackp; /* 1: allow mixed ackp0 and ackp1. 0: disallow */
	uint8_t	lowat[D11_REV128_BW_SZ];
	uint8_t	maxn[D11_REV128_BW_SZ];
	bool	rucfg_fixed; /* = TRUE: fixed/set by iovar */
	uint8_t	use_murts;
	bool	dyn_sigbmcs;
	uint16_t	tmout;
	int16_t	num_scb_stats;
	musched_ru_stats_t ru_stats;
	int16_t	num_dlofdma_users;
	uint16_t	min_dlofdma_users[MUSCHED_DLOFDMA_MINUSER_SZ]; /* min users to enable dlofdma */
	bool	mixbw;		/* TRUE: enabled; FALSE: disabled */
	bool	wfa20in80;	/* fixed RU alloc WAR for WFA test (HE-4.69.1) */
	bool	omutafwin;
	bool	aggx;		/* aggx feature */
	uint16_t	txdur_thresh_mu;	/* threshold to enforce dl ofdma */
	uint16_t	txdur_thresh_su;	/* threshold to fall back to SU */
    int8_t dl_schid; /*wlc_scbmusched_get_dlsch(wlc->musched, scb, &dl_schid, &dl_schpos)处的*/
    int8_t dl_schpos;/*wlc_scbmusched_get_dlsch(wlc->musched, scb, &dl_schid, &dl_schpos)处的*/
	struct ether_addr ea;
    uint32_t dlofdma_set_time;
}musched_info_qq_t;




struct wl_rxsts_qq {
    uint32_t   pkterror;       /* error flags per pkt */
    uint32_t   phytype;        /* 802.11 A/B/G /N  */
    uint16_t chanspec;        /* channel spec */
    uint16_t  datarate;       /* rate in 500kbps */
    uint8_t   mcs;            /* MCS for HT frame */
    uint8_t   htflags;        /* HT modulation flags */
    uint32_t   antenna;        /* antenna pkts received on */
    uint32_t   pktlength;      /* pkt length minus bcm phy hdr */
    uint32_t  mactime;        /* time stamp from mac, count per 1us */
    uint32_t   sq;         /* signal quality */
    int32_t   signal;         /* in dBm */
    int32_t   noise;          /* in dBm */
    uint32_t   preamble;       /* Unknown, short, long */
    uint32_t   encoding;       /* Unknown, CCK, PBCC, OFDM, HT, VHT */
    uint32_t   nfrmtype;       /* special 802.11 frames(AMPDU, AMSDU) or addition PPDU fromat */
    uint8_t   nss;            /* Number of spatial streams for VHT frame */
    uint8_t   coding;
    uint16_t  aid;            /* Partial AID for VHT frame */
    uint8_t   gid;            /* Group ID for VHT frame */
    uint8_t   bw;         /* Bandwidth for VHT frame */
    uint16_t  vhtflags;       /* VHT modulation flags */
    uint16_t	bw_nonht;       /* non-HT bw advertised in rts/cts */
    uint32_t  ampdu_counter;      /* AMPDU counter for sniffer */
    uint32_t  sig_a1;			/* HE  SIG-A1 field */
    uint32_t  sig_a2;			/* HE  SIG-A2 field */
} wl_mon_rxsts_t;

#define FC_PVER_MASK		0x3	/* PVER mask */
#define FC_PVER_SHIFT		0	/* PVER shift */
#define FC_TYPE_MASK		0xC	/* type mask */
#define FC_TYPE_SHIFT		2	/* type shift */
#define FC_SUBTYPE_MASK		0xF0	/* subtype mask */
#define FC_SUBTYPE_SHIFT	4	/* subtype shift */
#define FC_KIND_MASK		(FC_TYPE_MASK | FC_SUBTYPE_MASK)	/* FC kind mask */
struct dot11_header {
	uint16_t			fc;		/* frame control */
	uint16_t			durid;		/* duration/ID */
	struct ether_addr	a1;		/* address 1 */
	struct ether_addr	a2;		/* address 2 */
	struct ether_addr	a3;		/* address 3 */
	uint16_t			seq;		/* sequence control */
	struct ether_addr	a4;		/* address 4 */
};

struct monitor_info_qq {
    struct wl_rxsts_qq wl_mon_rxsts;
    uint32_t ru_type;
    uint16_t ruidx;
    //struct dot11_header h;
    struct dot11_header h1;
    uint16_t monitor_loc;
    bool wlc_pub_promisc;
    bool wlc_clk;
    uint32_t wlc_hw_maccontrol;
}monitor_info_qq_t;

#include <stdint.h>
#include <endian.h>

uint16_t ltoh16(uint16_t little_endian_value) {
    return le16toh(little_endian_value);
}




typedef uint32_t	ratespec_t;
//rate change info
struct rate_change_info_qq {
    uint8_t fix_rate;
    uint8_t change_mode;//上调还是下调还是别的，0是下调，1是上调,2是改变了nss
    uint8_t cur_rateid;
    uint8_t next_rateid;
    ratespec_t cur_rspec;
    ratespec_t next_rspec;
    uint8_t cur_mcs;
    uint8_t next_mcs;
    uint8_t cur_nss;
    uint8_t next_nss;
    uint8_t up_rateid;
    uint8_t down_rateid;
    uint32_t psr_fbr;
    uint32_t psr_cur;    
    uint32_t psr_dnp;    
    uint32_t psr_upp;
    uint32_t prate_cur;
    uint32_t prate_up;
    uint32_t prate_dn;
    uint32_t prate_fbr;
    uint32_t prate_next;
}rate_change_info_qq_t;



void hexdump(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
        if ((i + 1) % 16 == 0 || i == size - 1) {
            for (size_t j = 0; j < 15 - (i % 16); j++) {
                printf("   ");
            }
            printf("| ");
            for (size_t j = i - (i % 16); j <= i; j++) {
                char c = bytes[j];
                putchar((c >= 32 && c <= 126) ? c : '.');
            }
            putchar('\n');
        }
    }
}

void file_io(void) {
    FILE *file = NULL;
    info_class_t pkt_info;
    info_class_t pkt_count_info;
    info_class_t phy_info;
    info_class_t frameid_info;
    info_class_t multiuser_info;
    info_class_t monitor_info;
    info_class_t rate_change_info;
    ssize_t bytes_read;

    // Open the debugfs file for reading
    // DO NOT use fopen, fread, fseek ... as they maintain internal buffers
    int fd1 = open("/sys/kernel/debug/kernel_info/class1", O_RDONLY);
    int fd2 = open("/sys/kernel/debug/kernel_info/class2", O_RDONLY);
    int fd3 = open("/sys/kernel/debug/kernel_info/class3", O_RDONLY);
    int fd5 = open("/sys/kernel/debug/kernel_info/class5", O_RDONLY);
    int fd6 = open("/sys/kernel/debug/kernel_info/class6", O_RDONLY);
    int fd7 = open("/sys/kernel/debug/kernel_info/class7", O_RDONLY);
    int fd8 = open("/sys/kernel/debug/kernel_info/class8", O_RDONLY);

    if (fd1 < 0) {
        perror("Error opening debugfs file1");
        return;
    }
    if (fd2 < 0) {
        perror("Error opening debugfs file2");
        return;
    }
    if (fd3 < 0) {
        perror("Error opening debugfs file3");
        return;
    }

    if (fd5 < 0) {
        perror("Error opening debugfs file5");
        return;
    }
    if (fd6 < 0) {
        perror("Error opening debugfs file6");
        return;
    }
    if (fd7 < 0) {
        perror("Error opening debugfs file7");
        return;
    }
    if (fd8 < 0) {
        perror("Error opening debugfs file8");
        return;
    }
    int32_t loop_num=0;

    uint32_t cur_into_hw_time;
    uint32_t cur_free_time;
    uint16_t cur_FrameID;
    uint16_t pre_FrameID = 0;
    while (1) {

// Read the content of the debugfs file 1 into the buffer
        bytes_read = read(fd1, (uint8_t *)&pkt_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd1);
            return;
        }


        lseek(fd1, 0, SEEK_SET);
        // hexdump(&pkt_info, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &pkt_info.timestamp);
        struct pkt_qq *pkt_qq_cur;
        pkt_qq_cur = (struct pkt_qq *) malloc(sizeof(pkt_qq_t));
        //memcpy(pkt_qq_cur, pkt_info.info, sizeof(*pkt_qq_cur));
        /*
        for(int i = 0; i<DEBUG_CLASS_MAX_FIELD;i++){
        fprintf(stdout, "pkt_info.info: %ld,%ld\n ", pkt_info.info[i],i);

        }*/
        memcpy(pkt_qq_cur, pkt_info.info, sizeof(pkt_qq_t));
        if(pre_timestamp_class1.tv_nsec!= pkt_info.timestamp.tv_nsec){
                
            cur_into_hw_time = pkt_qq_cur->into_hw_time;
            cur_free_time = pkt_qq_cur->free_time;
            cur_FrameID = pkt_qq_cur->FrameID;
            struct rates_counts_txs_qq rates_counts_txs_qq_differ;
            rates_counts_txs_qq_differ.ncons = pkt_qq_cur->rates_counts_txs_qq_end.ncons - pkt_qq_cur->rates_counts_txs_qq_start.ncons;
            rates_counts_txs_qq_differ.nlost = pkt_qq_cur->rates_counts_txs_qq_end.nlost - pkt_qq_cur->rates_counts_txs_qq_start.nlost;
            rates_counts_txs_qq_differ.txrts_cnt = pkt_qq_cur->rates_counts_txs_qq_end.txrts_cnt - pkt_qq_cur->rates_counts_txs_qq_start.txrts_cnt;
            rates_counts_txs_qq_differ.rxcts_cnt = pkt_qq_cur->rates_counts_txs_qq_end.rxcts_cnt - pkt_qq_cur->rates_counts_txs_qq_start.rxcts_cnt;
            for(int8_t i = 0;i<MAX_MCS_QQ;i++){
                rates_counts_txs_qq_differ.tx_cnt[i] = pkt_qq_cur->rates_counts_txs_qq_end.tx_cnt[i] - pkt_qq_cur->rates_counts_txs_qq_start.tx_cnt[i];
                rates_counts_txs_qq_differ.txsucc_cnt[i] = pkt_qq_cur->rates_counts_txs_qq_end.txsucc_cnt[i] - pkt_qq_cur->rates_counts_txs_qq_start.txsucc_cnt[i];
            }
            fprintf(stdout,"rates_counts_txs_qq_differ:ncons(%u)nlost(%u)txrts_cnt(%u)rxcts_cnt(%u)\n"\
                ,rates_counts_txs_qq_differ.ncons,rates_counts_txs_qq_differ.nlost
                ,rates_counts_txs_qq_differ.txrts_cnt,rates_counts_txs_qq_differ.rxcts_cnt);

            fprintf(stdout,"rates_counts_txs_qq_differ.tx_cnt:");
            for(int8_t i = 0;i<MAX_MCS_QQ;i++){
                if(rates_counts_txs_qq_differ.tx_cnt[i]){
                    fprintf(stdout,"(%u:%u)",i,rates_counts_txs_qq_differ.tx_cnt[i]);
                }
            }
            fprintf(stdout,"\n");
            fprintf(stdout,"rates_counts_txs_qq_differ.txsucc_cnt:");
            for(int8_t i = 0;i<MAX_MCS_QQ;i++){
                if(rates_counts_txs_qq_differ.txsucc_cnt[i]>0){
                    fprintf(stdout,"(%u:%u)",i,rates_counts_txs_qq_differ.txsucc_cnt[i]);
                }
            }
            fprintf(stdout,"\n");

            fprintf(stdout,"pkt_qq_cur->rates_counts_txs_qq_end.tx_cnt:");
            for(int8_t i = 0;i<MAX_MCS_QQ;i++){
                if(pkt_qq_cur->rates_counts_txs_qq_end.tx_cnt[i]>0){
                    fprintf(stdout,"(%u:%u)",i,pkt_qq_cur->rates_counts_txs_qq_end.tx_cnt[i]);
                }
            }
            fprintf(stdout,"\n");
            fprintf(stdout,"pkt_qq_cur->rates_counts_txs_qq_start.tx_cnt:");
            for(int8_t i = 0;i<MAX_MCS_QQ;i++){
                if(pkt_qq_cur->rates_counts_txs_qq_start.tx_cnt[i]>0){
                    fprintf(stdout,"(%u:%u)",i,pkt_qq_cur->rates_counts_txs_qq_start.tx_cnt[i]);
                }
            }
            fprintf(stdout,"\n");



            // hexdump(&amp;pkt_info, sizeof(info_class_t));
            // clock_gettime(CLOCK_REALTIME, &amp;pkt_info.timestamp);
            if(PRINT_pkt_info){
                
                char pkt_info_type[30] = "";
                if(pkt_qq_cur->free_time>0){
                    strcpy(pkt_info_type,"timeout acked_pktinfo");
                }
                else{
                    strcpy(pkt_info_type,"timeout unacked_pktinfo");
                }
                if(pkt_qq_cur->pktSEQ>0){
                    fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,pkt_info.timestamp.tv_sec,pkt_info.timestamp.tv_nsec / 1000);

                    fprintf(stdout,"%s: FrameID(%u) pktSEQ(%u) n_pkts(%u) ampdu_seq(%u) APnum(%u) fifo(%u)"
                        "pkt_qq_chain_len_add(%u:%u:%u) "
                        "pkt_added_in_wlc_tx(%u:%u:%u) "
                        "pktnum_to_send(%u:%u:%d) tid(%u) failed_cnt(%u) PHYdelay(%u) cfp+PHYdelay(%u) ps_dur_trans(%u) " 
                        "free_time(%u) into_hw_time(%u)  into_CFP_time(%u:%u) droped_withoutACK_time(%u) airtime_self(%u) txop_in_fly(%u) busy_time(%u) "
                        "ps_pretend_probe(%u) ps_pretend_count(%u) ps_pretend_succ_count(%u) ps_pretend_failed_ack_count(%u) " 
                        "time_in_pretend_in_fly(%u) ccastats_qq_differ:TXDUR(%u)INBSS(%u)OBSS(%u)NOCTG(%u)NOPKT(%u)DOZE(%u)TXOP(%u)GDTXDUR(%u)BDTXDUR(%u)\n"
                        ,pkt_info_type, pkt_qq_cur->FrameID, pkt_qq_cur->pktSEQ, pkt_qq_cur->n_pkts, pkt_qq_cur->ampdu_seq, pkt_qq_cur->APnum, pkt_qq_cur->fifo\
                        ,pkt_qq_cur->pkt_qq_chain_len_add_start,pkt_qq_cur->pkt_qq_chain_len_add_end,pkt_qq_cur->pkt_qq_chain_len_add_end-pkt_qq_cur->pkt_qq_chain_len_add_start\
                        ,pkt_qq_cur->pkt_added_in_wlc_tx_start,pkt_qq_cur->pkt_added_in_wlc_tx_end,pkt_qq_cur->pkt_added_in_wlc_tx_end-pkt_qq_cur->pkt_added_in_wlc_tx_start\
                        ,pkt_qq_cur->pktnum_to_send_start, pkt_qq_cur->pktnum_to_send_end, (pkt_qq_cur->pktnum_to_send_end-pkt_qq_cur->pktnum_to_send_start), pkt_qq_cur->tid\
                        , pkt_qq_cur->failed_cnt,pkt_qq_cur->free_time-pkt_qq_cur->into_hw_time,pkt_qq_cur->free_time-pkt_qq_cur->into_CFP_time,pkt_qq_cur->ps_dur_trans\
                        ,pkt_qq_cur->free_time,pkt_qq_cur->into_hw_time,pkt_qq_cur->into_CFP_time_record_loc,pkt_qq_cur->into_CFP_time,pkt_qq_cur->droped_withoutACK_time,pkt_qq_cur->airtime_self,pkt_qq_cur->txop_in_fly,pkt_qq_cur->busy_time\
                        ,pkt_qq_cur->ps_pretend_probe, pkt_qq_cur->ps_pretend_count,pkt_qq_cur->ps_pretend_succ_count,pkt_qq_cur->ps_pretend_failed_ack_count\
                        ,pkt_qq_cur->time_in_pretend_in_fly,pkt_qq_cur->ccastats_qq_differ[0]\
                        ,pkt_qq_cur->ccastats_qq_differ[1],pkt_qq_cur->ccastats_qq_differ[2],pkt_qq_cur->ccastats_qq_differ[3]\
                        ,pkt_qq_cur->ccastats_qq_differ[4],pkt_qq_cur->ccastats_qq_differ[5],pkt_qq_cur->ccastats_qq_differ[6]\
                        ,pkt_qq_cur->ccastats_qq_differ[7],pkt_qq_cur->ccastats_qq_differ[8]);
                

                    /*fprintf(stdout,"%s: FrameID(%u) n_pkts(%u) APnum(%u) fifo(%u)"
                        "pkt_qq_chain_len_add(%u:%u:%u) "
                        "pktnum_to_send(%u:%u:%d) failed_cnt(%u) PHYdelay(%u) cfp+PHYdelay(%u) ps_dur_trans(%u) " 
                        "free_time(%u) into_hw_time(%u) into_CFP_time(%u:%u) airtime_self(%u) txop_in_fly(%u) busy_time(%u) "
                        "time_in_pretend_in_fly(%u) ccastats_qq_differ:TXDUR(%u)INBSS(%u)OBSS(%u)NOCTG(%u)NOPKT(%u)TXOP(%u)\n"
                        ,pkt_info_type, pkt_qq_cur->FrameID, pkt_qq_cur->n_pkts, pkt_qq_cur->APnum, pkt_qq_cur->fifo\
                        ,pkt_qq_cur->pkt_qq_chain_len_add_start,pkt_qq_cur->pkt_qq_chain_len_add_end,pkt_qq_cur->pkt_qq_chain_len_add_end-pkt_qq_cur->pkt_qq_chain_len_add_start\
                        ,pkt_qq_cur->pktnum_to_send_start, pkt_qq_cur->pktnum_to_send_end, (pkt_qq_cur->pktnum_to_send_end-pkt_qq_cur->pktnum_to_send_start), pkt_qq_cur->failed_cnt\
                        ,pkt_qq_cur->free_time-pkt_qq_cur->into_hw_time,pkt_qq_cur->free_time-pkt_qq_cur->into_CFP_time,pkt_qq_cur->ps_dur_trans\
                        ,pkt_qq_cur->free_time,pkt_qq_cur->into_hw_time,pkt_qq_cur->into_CFP_time_record_loc,pkt_qq_cur->into_CFP_time,pkt_qq_cur->airtime_self,pkt_qq_cur->txop_in_fly,pkt_qq_cur->busy_time\
                        ,pkt_qq_cur->time_in_pretend_in_fly,pkt_qq_cur->ccastats_qq_differ[0]\
                        ,pkt_qq_cur->ccastats_qq_differ[1],pkt_qq_cur->ccastats_qq_differ[2],pkt_qq_cur->ccastats_qq_differ[3]\
                        ,pkt_qq_cur->ccastats_qq_differ[4],pkt_qq_cur->ccastats_qq_differ[6]);*/
                    
                    if(pkt_qq_cur->failed_cnt>0){
                        fprintf(stdout,"failed_time_list_qq:0(%u)1(%u)2(%u)3(%u)4(%u)5(%u)6(%u)7(%u)8(%u)9(%u)\n",pkt_qq_cur->failed_time_list_qq[0]\
                        ,pkt_qq_cur->failed_time_list_qq[1],pkt_qq_cur->failed_time_list_qq[2],pkt_qq_cur->failed_time_list_qq[3]\
                        ,pkt_qq_cur->failed_time_list_qq[4],pkt_qq_cur->failed_time_list_qq[5],pkt_qq_cur->failed_time_list_qq[6]\
                        ,pkt_qq_cur->failed_time_list_qq[7],pkt_qq_cur->failed_time_list_qq[8],pkt_qq_cur->failed_time_list_qq[9]);
                    }
                    if(pkt_qq_cur->retry_time_list_index>0){
                        fprintf(stdout,"retry_time_list_qq:0(%u)1(%u)2(%u)3(%u)4(%u)5(%u)6(%u)7(%u)8(%u)9(%u)\n",pkt_qq_cur->retry_time_list_qq[0]\
                        ,pkt_qq_cur->retry_time_list_qq[1],pkt_qq_cur->retry_time_list_qq[2],pkt_qq_cur->retry_time_list_qq[3]\
                        ,pkt_qq_cur->retry_time_list_qq[4],pkt_qq_cur->retry_time_list_qq[5],pkt_qq_cur->retry_time_list_qq[6]\
                        ,pkt_qq_cur->retry_time_list_qq[7],pkt_qq_cur->retry_time_list_qq[8],pkt_qq_cur->retry_time_list_qq[9]);
                    }
                
                }
            }

            
            pre_timestamp_class1 = pkt_info.timestamp;
        }

// Read the content of the debugfs file 2 into the buffer
        bytes_read = read(fd2, (uint8_t *)&pkt_count_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd2);
            return;
        }

        lseek(fd2, 0, SEEK_SET);
        // hexdump(&pkt_count_info, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &pkt_count_info.timestamp);
        struct pkt_count_qq *pkt_count_qq_cur;
        pkt_count_qq_cur = (struct pkt_count_qq *) malloc(sizeof(pkt_count_qq_t));
        memcpy(pkt_count_qq_cur, pkt_count_info.info, sizeof(pkt_count_qq_t));
        if(pre_timestamp_class2.tv_nsec!= pkt_count_info.timestamp.tv_nsec){
            if(PRINT_pkt_count_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,pkt_count_info.timestamp.tv_sec,
                pkt_count_info.timestamp.tv_nsec / 1000);
                // hexdump(&amp;pkt_count_info, sizeof(info_class_t));
                // clock_gettime(CLOCK_REALTIME, &amp;pkt_count_info.timestamp);

                fprintf(stdout,"pkt count: pkt_qq_chain_len(%u) pkt_chain_add(%u) pkt_qq_chain_len_soft_retry(%u) "
                "pkt_chain_acked(%u) pkt_chain_unacked(%u) "
                "pkt_chain_timeout(%u) pkt_chain_outofrange(%u) "
                "pkt_chain_notfound(%u) pkt_chain_found(%u) "
                "pkt_phydelay_dict:10(%u)20(%u)30(%u)40(%u)50(%u)60(%u)70(%u)80(%u)90(%u)100(%u)"
                "110(%u)120(%u)130(%u)140(%u)150(%u)160(%u)170(%u)180(%u)190(%u)200(%u)"
                "210(%u)220(%u)230(%u)240(%u)250(%u)260(%u)270(%u)280(%u)290(%u)290up(%u)\n"\
                ,pkt_count_qq_cur->pkt_qq_chain_len_now,pkt_count_qq_cur->pkt_qq_chain_len_add,pkt_count_qq_cur->pkt_qq_chain_len_soft_retry\
                ,pkt_count_qq_cur->pkt_qq_chain_len_acked, pkt_count_qq_cur->pkt_qq_chain_len_unacked\
                ,pkt_count_qq_cur->pkt_qq_chain_len_timeout, pkt_count_qq_cur->pkt_qq_chain_len_outofrange\
                ,pkt_count_qq_cur->pkt_qq_chain_len_notfound,pkt_count_qq_cur->pkt_qq_chain_len_found\
                ,pkt_count_qq_cur->pkt_phydelay_dict[0]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[1],pkt_count_qq_cur->pkt_phydelay_dict[2],pkt_count_qq_cur->pkt_phydelay_dict[3]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[4],pkt_count_qq_cur->pkt_phydelay_dict[5],pkt_count_qq_cur->pkt_phydelay_dict[6]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[7],pkt_count_qq_cur->pkt_phydelay_dict[8],pkt_count_qq_cur->pkt_phydelay_dict[9],pkt_count_qq_cur->pkt_phydelay_dict[10]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[11],pkt_count_qq_cur->pkt_phydelay_dict[12],pkt_count_qq_cur->pkt_phydelay_dict[13]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[14],pkt_count_qq_cur->pkt_phydelay_dict[15],pkt_count_qq_cur->pkt_phydelay_dict[16]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[17],pkt_count_qq_cur->pkt_phydelay_dict[18],pkt_count_qq_cur->pkt_phydelay_dict[19],pkt_count_qq_cur->pkt_phydelay_dict[20]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[21],pkt_count_qq_cur->pkt_phydelay_dict[22],pkt_count_qq_cur->pkt_phydelay_dict[23]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[24],pkt_count_qq_cur->pkt_phydelay_dict[25],pkt_count_qq_cur->pkt_phydelay_dict[26]\
                ,pkt_count_qq_cur->pkt_phydelay_dict[27],pkt_count_qq_cur->pkt_phydelay_dict[28],pkt_count_qq_cur->pkt_phydelay_dict[29]);
                
            }

            pre_timestamp_class2 = pkt_count_info.timestamp;
        }



// Read the content of the debugfs file 3 into the buffer
        bytes_read = read(fd3, (uint8_t *)&phy_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd3);
            return;
        }

        lseek(fd3, 0, SEEK_SET);
        // hexdump(&phy_info_qq, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &phy_info_qq.timestamp);
        struct phy_info_qq *phy_info_qq_cur;
        phy_info_qq_cur = (struct phy_info_qq *) malloc(sizeof(phy_info_qq_t));
        memcpy(phy_info_qq_cur, phy_info.info, sizeof(phy_info_qq_t));
        if(pre_timestamp_class3.tv_nsec!= phy_info.timestamp.tv_nsec){
            if(PRINT_phy_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,phy_info.timestamp.tv_sec,
                phy_info.timestamp.tv_nsec / 1000);
                // hexdump(&amp;phy_info_qq, sizeof(info_class_t));
                // clock_gettime(CLOCK_REALTIME, &amp;phy_info_qq.timestamp);
                            //printk("----------[fyl] phy_info_qq_cur:mcs(%u):rate(%u):fix_rate(%u)----------",phy_info_qq_cur->mcs[0],phy_info_qq_cur->rate[0],phy_info_qq_cur->fix_rate);

                fprintf(stdout,"phy_info_qq_cur:RSSI_loc(%u) RSSI_type(%u) RSSI_subtype(%u) RSSI(%d) noiselevel(%d) channel_index(%u) real_BW(%u)\n"\
                ,phy_info_qq_cur->RSSI_loc,phy_info_qq_cur->RSSI_type,phy_info_qq_cur->RSSI_subtype,phy_info_qq_cur->RSSI,phy_info_qq_cur->noiselevel
                ,phy_info_qq_cur->channel_index,phy_info_qq_cur->real_BW);
            
            //}
            //if(PRINT_pkt_info){
                
                uint8_t rssi_ring_buffer_index_cur = (phy_info_qq_cur->rssi_ring_buffer_index- 1) % RSSI_RING_SIZE;
                if(pre_FrameID != cur_FrameID){

                    fprintf(stdout,"phy_info_qq_cur:fix_rate(%u) mcs(%u) rate(%u) nss(%u) BW(%u) ISSGI(%u) RSSI(%d) SNR(%d) noiselevel(%d)\n"\
                    ,phy_info_qq_cur->fix_rate,phy_info_qq_cur->mcs[0],phy_info_qq_cur->rate[0],\
                    phy_info_qq_cur->nss[0],phy_info_qq_cur->BW[0],phy_info_qq_cur->ISSGI[0],phy_info_qq_cur->RSSI,phy_info_qq_cur->SNR,phy_info_qq_cur->noiselevel);

                    fprintf(stdout,"time:rssi:noise");
                    for(int8_t i = 0;i<RSSI_RING_SIZE;i++){
                        //if(rates_counts_txs_qq_differ.txsucc_cnt[i]>0){
                            fprintf(stdout,"(%d:%d:%d)",pkt_qq_cur->free_time-phy_info_qq_cur->rssi_ring_buffer[rssi_ring_buffer_index_cur].timestamp\
                            ,phy_info_qq_cur->rssi_ring_buffer[rssi_ring_buffer_index_cur].RSSI,phy_info_qq_cur->rssi_ring_buffer[rssi_ring_buffer_index_cur].noiselevel);
                        //}
                        if(cur_into_hw_time>phy_info_qq_cur->rssi_ring_buffer[rssi_ring_buffer_index_cur].timestamp){
                            break;
                        }
                    rssi_ring_buffer_index_cur = (rssi_ring_buffer_index_cur - 1) % RSSI_RING_SIZE;
                    
                    }
                    fprintf(stdout,"\n");

                    pre_FrameID = cur_FrameID;
                }
            }
            pre_timestamp_class3 = phy_info.timestamp;
        }





// Read the content of the debugfs file 5 into the buffer
        bytes_read = read(fd5, (uint8_t *)&frameid_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd5);
            return;
        }

        lseek(fd5, 0, SEEK_SET);
        // hexdump(&phy_info_qq, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &phy_info_qq.timestamp);
        struct pkt_ergodic *pkt_ergodic_cur;
        pkt_ergodic_cur = (struct pkt_ergodic *) malloc(sizeof(pkt_ergodic_t));
        memcpy(pkt_ergodic_cur, frameid_info.info, sizeof(pkt_ergodic_t));
        if(pre_timestamp_class5.tv_nsec!= frameid_info.timestamp.tv_nsec){
            if(PRINT_frameid_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,frameid_info.timestamp.tv_sec,
                frameid_info.timestamp.tv_nsec / 1000);
                fprintf(stdout,"frameID:");
                
                fprintf(stdout,"frameID:(%u:%u:%u:%u)",pkt_ergodic_cur->print_loc,pkt_ergodic_cur->pkt_len,pkt_ergodic_cur->real_pkt_num,pkt_ergodic_cur->cur_time);
                for(int nn = 0;(nn<pkt_ergodic_cur->pkt_len)&&(nn<pkt_ergodic_cur->real_pkt_num);nn++){
                    fprintf(stdout,"(%u:%u:%u)",nn,pkt_ergodic_cur->pkt_FrameID[nn],pkt_ergodic_cur->into_CFP_time[nn]);
                }

                fprintf(stdout,"\n");
            }
            pre_timestamp_class5 = frameid_info.timestamp;
        }





// Read the content of the debugfs file 6 into the buffer
        bytes_read = read(fd6, (uint8_t *)&multiuser_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd6);
            return;
        }

        lseek(fd6, 0, SEEK_SET);
        // hexdump(&phy_info_qq, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &phy_info_qq.timestamp);
        struct musched_info_qq *musched_info_qq_cur;
        musched_info_qq_cur = (struct musched_info_qq *) malloc(sizeof(musched_info_qq_t));
        memcpy(musched_info_qq_cur, multiuser_info.info, sizeof(musched_info_qq_t));
        if(pre_timestamp_class6.tv_nsec!= multiuser_info.timestamp.tv_nsec){
            if(PRINT_multiuser_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,multiuser_info.timestamp.tv_sec,
                multiuser_info.timestamp.tv_nsec / 1000);
                fprintf(stdout,"multi-user info:");
                
                fprintf(stdout,"wlc_fifo_isMU(%d);wlc_fifo_is_ulofdma(%d);wlc_fifo_is_dlofdma(%d);dlofdma_set_time(%d);mu_type(%u);mch(%u);mcl(%u);mch2(%u);dl_schpos(%u)dl_schid(%u);MAC address(%02x:%02x:%02x:%02x:%02x:%02x)"\
                    ,musched_info_qq_cur->wlc_fifo_isMU,musched_info_qq_cur->wlc_fifo_is_ulofdma,musched_info_qq_cur->wlc_fifo_is_dlofdma,musched_info_qq_cur->dlofdma_set_time,musched_info_qq_cur->mu_type\
                    ,musched_info_qq_cur->mch,musched_info_qq_cur->mcl,musched_info_qq_cur->mch2,musched_info_qq_cur->dl_schpos,musched_info_qq_cur->dl_schid\
                    ,musched_info_qq_cur->ea.octet[0],musched_info_qq_cur->ea.octet[1],musched_info_qq_cur->ea.octet[2]\
                    ,musched_info_qq_cur->ea.octet[3],musched_info_qq_cur->ea.octet[4],musched_info_qq_cur->ea.octet[5]);
                
                fprintf(stdout,"scb_flagsinfo:WME: %d, AMPDU: %d, AMSDU: %d, AMSDU_IN_AMPDU: %d, DTPC: %d, HT: %d, VHT: %d, ISGF: %d, NONGF: %d, COEX: %d, STBC: %d, HT_LDPC: %d, HT_PROP_RATES: %d, OPER_MODE_NOTIF: %d, HE: %d, IBSS_PEER: %d, PKTC: %d, QOS: %d, P2P: %d, DWDS: %d, DWDS_CAP: %d, MAP: %d, MAP_P2: %d, ECSA: %d, LEGACY_WDS: %d, A4_DATA: %d, A4_NULL_DATA: %d, A4_8021X: %d, MFP: %d, SHA256: %d, QAM_1024: %d, VHTMU: %d, HEMMU: %d, DLOFDMA: %d, ULOFDMA: %d, ULMMU: %d, RRM: %d, FTM: %d, FTM_INITIATOR: %d, FTM_RESPONDER: %d"\
                    ,musched_info_qq_cur->scb_flags.wme, musched_info_qq_cur->scb_flags.ampdu, musched_info_qq_cur->scb_flags.amsdu, musched_info_qq_cur->scb_flags.amsdu_in_ampdu, musched_info_qq_cur->scb_flags.dtpc, musched_info_qq_cur->scb_flags.ht, musched_info_qq_cur->scb_flags.vht, musched_info_qq_cur->scb_flags.isgf, musched_info_qq_cur->scb_flags.nongf, musched_info_qq_cur->scb_flags.coex, musched_info_qq_cur->scb_flags.stbc, musched_info_qq_cur->scb_flags.ht_ldpc, musched_info_qq_cur->scb_flags.ht_prop_rates, musched_info_qq_cur->scb_flags.oper_mode_notif, musched_info_qq_cur->scb_flags.he, musched_info_qq_cur->scb_flags.ibss_peer, musched_info_qq_cur->scb_flags.pktc, musched_info_qq_cur->scb_flags.qos, musched_info_qq_cur->scb_flags.p2p, musched_info_qq_cur->scb_flags.dwds, musched_info_qq_cur->scb_flags.dwds_cap, musched_info_qq_cur->scb_flags.map, musched_info_qq_cur->scb_flags.map_p2, musched_info_qq_cur->scb_flags.ecsa, musched_info_qq_cur->scb_flags.legacy_wds, musched_info_qq_cur->scb_flags.a4_data, musched_info_qq_cur->scb_flags.a4_null_data, musched_info_qq_cur->scb_flags.a4_8021x, musched_info_qq_cur->scb_flags.mfp, musched_info_qq_cur->scb_flags.sha256, musched_info_qq_cur->scb_flags.qam_1024, musched_info_qq_cur->scb_flags.vhtmu, musched_info_qq_cur->scb_flags.hemmu, musched_info_qq_cur->scb_flags.dlofdma, musched_info_qq_cur->scb_flags.ulofdma, musched_info_qq_cur->scb_flags.ulmmu, musched_info_qq_cur->scb_flags.rrm, musched_info_qq_cur->scb_flags.ftm, musched_info_qq_cur->scb_flags.ftm_initiator, musched_info_qq_cur->scb_flags.ftm_responder);

                fprintf(stdout,"\n");
            }
            pre_timestamp_class6 = multiuser_info.timestamp;
        }


//void print_scb_flags(my_scb_t *flags) {
//    printf("WME: %d, AMPDU: %d, AMSDU: %d, AMSDU_IN_AMPDU: %d, DTPC: %d, HT: %d, VHT: %d, ISGF: %d, NONGF: %d, COEX: %d, STBC: %d, HT_LDPC: %d, HT_PROP_RATES: %d, OPER_MODE_NOTIF: %d, HE: %d, IBSS_PEER: %d, PKTC: %d, QOS: %d, P2P: %d, DWDS: %d, DWDS_CAP: %d, MAP: %d, MAP_P2: %d, ECSA: %d, LEGACY_WDS: %d, A4_DATA: %d, A4_NULL_DATA: %d, A4_8021X: %d, MFP: %d, SHA256: %d, QAM_1024: %d, VHTMU: %d, HEMMU: %d, DLOFDMA: %d, ULOFDMA: %d, ULMMU: %d, RRM: %d, FTM: %d, FTM_INITIATOR: %d, FTM_RESPONDER: %d\n",
//        flags->wme, flags->ampdu, flags->amsdu, flags->amsdu_in_ampdu, flags->dtpc, flags->ht, flags->vht, flags->isgf, flags->nongf, flags->coex, flags->stbc, flags->ht_ldpc, flags->ht_prop_rates, flags->oper_mode_notif, flags->he, flags->ibss_peer, flags->pktc, flags->qos, flags->p2p, flags->dwds, flags->dwds_cap, flags->map, flags->map_p2, flags->ecsa, flags->legacy_wds, flags->a4_data, flags->a4_null_data, flags->a4_8021x, flags->mfp, flags->sha256, flags->qam_1024, flags->vhtmu, flags->hemmu, flags->dlofdma, flags->ulofdma, flags->ulmmu, flags->rrm, flags->ftm, flags->ftm_initiator, flags->ftm_responder);
//}

// Read the content of the debugfs file 7 into the buffer
        bytes_read = read(fd7, (uint8_t *)&monitor_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd7);
            return;
        }

        lseek(fd7, 0, SEEK_SET);
        // hexdump(&phy_info_qq, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &phy_info_qq.timestamp);
        struct monitor_info_qq *monitor_info_qq_cur;
        monitor_info_qq_cur = (struct monitor_info_qq *) malloc(sizeof(monitor_info_qq_t));
        memcpy(monitor_info_qq_cur, monitor_info.info, sizeof(monitor_info_qq_t));
        if(pre_timestamp_class7.tv_nsec!= monitor_info.timestamp.tv_nsec){
            if(PRINT_monitor_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,monitor_info.timestamp.tv_sec,
                monitor_info.timestamp.tv_nsec / 1000);
                fprintf(stdout,"monitor info:");
                fprintf(stdout,"size:,sizeof(struct wl_rxsts_qq),sizeof(struct dot11_header),sizeof(struct monitor_info_qq)(%u:%u:%u)"\
                ,sizeof(struct wl_rxsts_qq),sizeof(struct dot11_header),sizeof(struct monitor_info_qq));
                
                fprintf(stdout,"monitor_loc(%u);wlc_pub_promisc(%d);wlc_clk(%d);wlc_hw_maccontrol(%u);ru_type(%u);ruidx(%u);bw(%u);mcs(%u);chanspec(0x%04x);sig_a1(%u);sig_a2(%u);type(%u);subtype(%u);MAC address h1.a1(%02x:%02x:%02x:%02x:%02x:%02x)"\
                    ";MAC address h1.a2(%02x:%02x:%02x:%02x:%02x:%02x);MAC address h1.a3(%02x:%02x:%02x:%02x:%02x:%02x)"\
                    ,monitor_info_qq_cur->monitor_loc,monitor_info_qq_cur->wlc_pub_promisc,monitor_info_qq_cur->wlc_clk,monitor_info_qq_cur->wlc_hw_maccontrol,monitor_info_qq_cur->ru_type,monitor_info_qq_cur->ruidx,monitor_info_qq_cur->wl_mon_rxsts.bw,monitor_info_qq_cur->wl_mon_rxsts.mcs,\
                    monitor_info_qq_cur->wl_mon_rxsts.chanspec,monitor_info_qq_cur->wl_mon_rxsts.sig_a1,monitor_info_qq_cur->wl_mon_rxsts.sig_a2,(ltoh16(monitor_info_qq_cur->h1.fc) & FC_TYPE_MASK)>> FC_TYPE_SHIFT,(ltoh16(monitor_info_qq_cur->h1.fc) & FC_SUBTYPE_MASK)>> FC_SUBTYPE_SHIFT,\
                                monitor_info_qq_cur->h1.a1.octet[0],monitor_info_qq_cur->h1.a1.octet[1],monitor_info_qq_cur->h1.a1.octet[2],\
                                monitor_info_qq_cur->h1.a1.octet[3],monitor_info_qq_cur->h1.a1.octet[4],monitor_info_qq_cur->h1.a1.octet[5],\
                                monitor_info_qq_cur->h1.a2.octet[0],monitor_info_qq_cur->h1.a2.octet[1],monitor_info_qq_cur->h1.a2.octet[2],\
                                monitor_info_qq_cur->h1.a2.octet[3],monitor_info_qq_cur->h1.a2.octet[4],monitor_info_qq_cur->h1.a2.octet[5],\
                                monitor_info_qq_cur->h1.a3.octet[0],monitor_info_qq_cur->h1.a3.octet[1],monitor_info_qq_cur->h1.a3.octet[2],\
                                monitor_info_qq_cur->h1.a3.octet[3],monitor_info_qq_cur->h1.a3.octet[4],monitor_info_qq_cur->h1.a3.octet[5]);

                fprintf(stdout,"\n");
            }
            pre_timestamp_class7 = monitor_info.timestamp;
        }




// Read the content of the debugfs file 8 into the buffer
        bytes_read = read(fd8, (uint8_t *)&rate_change_info, sizeof(info_class_t));
        if (bytes_read != sizeof(info_class_t)) {
            fprintf(stderr, "wrong bytes read: %d, shoud be %d\n", bytes_read,
                    sizeof(info_class_t));
            close(fd8);
            return;
        }

        lseek(fd8, 0, SEEK_SET);
        // hexdump(&phy_info_qq, sizeof(info_class_t));
        // clock_gettime(CLOCK_REALTIME, &phy_info_qq.timestamp);
        struct rate_change_info_qq *rate_change_info_qq_cur;
        rate_change_info_qq_cur = (struct rate_change_info_qq *) malloc(sizeof(struct rate_change_info_qq));
        memcpy(rate_change_info_qq_cur, rate_change_info.info, sizeof(struct rate_change_info_qq));
        if(pre_timestamp_class8.tv_nsec!= rate_change_info.timestamp.tv_nsec){
            if(PRINT_rate_change_info){
                
                fprintf(stdout, "loop_num(%d)#time: %ld:%ld \n ", loop_num,rate_change_info.timestamp.tv_sec,
                rate_change_info.timestamp.tv_nsec / 1000);
                fprintf(stdout,"rate_change info:");
                fprintf(stdout,"size:,sizeof(struct wl_rxsts_qq),sizeof(struct dot11_header),sizeof(struct rate_change_info_qq)(%u:%u:%u)"\
                ,sizeof(struct wl_rxsts_qq),sizeof(struct dot11_header),sizeof(struct rate_change_info_qq));
                
                fprintf(stdout,"fix_rate(%u);change_mode(%u);cur_rateid(%u);"\
                    "next_rateid(%u);up_rateid(%u);down_rateid(%u);"\
                    "psr_fbr(%u);psr_cur(%u);psr_dnp(%u);psr_upp(%u);"\
                    "prate_cur(%u);prate_up(%u);prate_dn(%u);prate_fbr(%u);"\
                    "prate_next(%u);cur_rspec(%u);next_rspec(%u);cur_mcs(%u);"\
                    "next_mcs(%u);cur_nss(%u);next_nss(%u);"\
                    ,rate_change_info_qq_cur->fix_rate, rate_change_info_qq_cur->change_mode,rate_change_info_qq_cur->cur_rateid
                    ,rate_change_info_qq_cur->next_rateid, rate_change_info_qq_cur->up_rateid, rate_change_info_qq_cur->down_rateid
                    ,rate_change_info_qq_cur->psr_fbr, rate_change_info_qq_cur->psr_cur, rate_change_info_qq_cur->psr_dnp, rate_change_info_qq_cur->psr_upp
                    ,rate_change_info_qq_cur->prate_cur, rate_change_info_qq_cur->prate_up, rate_change_info_qq_cur->prate_dn, rate_change_info_qq_cur->prate_fbr
                    ,rate_change_info_qq_cur->prate_next,rate_change_info_qq_cur->cur_rspec,rate_change_info_qq_cur->next_rspec,rate_change_info_qq_cur->cur_mcs
                    ,rate_change_info_qq_cur->next_mcs,rate_change_info_qq_cur->cur_nss,rate_change_info_qq_cur->next_nss);
                fprintf(stdout,"\n");
            }
            pre_timestamp_class8= rate_change_info.timestamp;
        }





        //usleep(1 * 1000);
        free(pkt_qq_cur);
        free(pkt_count_qq_cur);
        free(phy_info_qq_cur);
        free(pkt_ergodic_cur);
        free(musched_info_qq_cur);
        free(monitor_info_qq_cur);
        free(rate_change_info_qq_cur);
        loop_num++;
    }

    // Close the file descriptor
    close(fd1);
    close(fd2);
    close(fd3);
    close(fd5);
    close(fd6);
    close(fd7);
    close(fd8);
}

#define GET_FIELD(ptr, x) (((info_class_t*)ptr)->info[x])

void mmap_io(void) {
    struct timespec ts;
    int fd1 = open("/sys/kernel/debug/kernel_info/class1", O_RDONLY);
    if (fd1 < 0) {
        fprintf(stderr, "fail to open debugfs file\n");
        return;
    }

    uint8_t *mem = mmap(NULL, sizeof(info_class_t), PROT_READ, MAP_SHARED, fd1, 0);
    if (mem == MAP_FAILED) {
        perror("fail to mmap file");
        return;
    }

    close(fd1);

    while(1) {
        clock_gettime(CLOCK_REALTIME, &ts);
        fprintf(stdout, "time: %ld:%ld value: %d\n", ts.tv_sec, ts.tv_nsec / 1000, GET_FIELD(mem, DEBUG_PHY_RSSI));
        usleep(1000 * 1000);
    }
}

int main() { 
    // mmap_io();
    file_io();
    return 0; 
}