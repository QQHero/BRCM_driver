/*
 * Broadcom 20xx Digital Radios
 * Chip-specific hardware definitions.
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
 * $Id: wlc_phy_radio.h 689894 2017-03-13 23:32:18Z $
 */

#ifndef	_BCM20XX_H
#define	_BCM20XX_H

/* generic radio id ragister */
#define	RADIO_IDCODE			0x01

#define RADIO_DEFAULT_CORE		0

/* Bits in RX_CTL0: */
#define	RXC0_RSSI_RST			0x80
#define	RXC0_MODE_RSSI			0x40
#define	RXC0_MODE_OFF			0x20
#define	RXC0_MODE_CM			0x10
#define	RXC0_LAN_LOAD			0x08
#define	RXC0_OFF_ADJ_MASK		0x07

/* Bits in TX_CTL0: */
#define	TXC0_MODE_TXLPF			0x04
#define	TXC0_PA_TSSI_EN			0x02
#define	TXC0_TSSI_EN			0x01

/* Bits in TX_CTL1: */
#define	TXC1_PA_GAIN_MASK		0x60
#define	TXC1_PA_GAIN_3DB		0x40
#define	TXC1_PA_GAIN_2DB		0x20
#define	TXC1_TX_MIX_GAIN		0x10
#define	TXC1_OFF_I_MASK			0x0c
#define	TXC1_OFF_Q_MASK			0x03

/* radio read offsets */
#define	RADIO_2057_READ_OFF		0x200	/* Read offset for all registers but idcode */

/* 2055 pin control */
#define RADIO_MIMO_CORESEL_OFF		0x0
#define RADIO_MIMO_CORESEL_CORE1	0x1
#define RADIO_MIMO_CORESEL_CORE2	0x2
#define RADIO_MIMO_CORESEL_CORE3	0x3
#define RADIO_MIMO_CORESEL_CORE4	0x4
#define RADIO_MIMO_CORESEL_ALLRX	0x5
#define RADIO_MIMO_CORESEL_ALLTX	0x6
#define RADIO_MIMO_CORESEL_ALLRXTX	0x7

/* Auto-generated 2057 register definitions */
#define RADIO_2057_DACBUF_VINCM_CORE0            0x0 /* not for rev9 */
#define RADIO_2057_IDCODE                        0x1 /* not for rev9 */
#define RADIO_2057_RCCAL_MASTER                  0x2 /* not for rev9 */
#define RADIO_2057_RCCAL_CAP_SIZE                0x3
#define RADIO_2057_RCAL_CONFIG                   0x4
#define RADIO_2057_GPAIO_CONFIG                  0x5
#define RADIO_2057_GPAIO_SEL1                    0x6
#define RADIO_2057_GPAIO_SEL0                    0x7
#define RADIO_2057_CLPO_CONFIG                   0x8
#define RADIO_2057_BANDGAP_CONFIG                0x9
#define RADIO_2057_BANDGAP_RCAL_TRIM             0xa
#define RADIO_2057_AFEREG_CONFIG                 0xb
#define RADIO_2057_TEMPSENSE_CONFIG              0xc
#define RADIO_2057_XTAL_CONFIG1                  0xd
#define RADIO_2057_XTAL_ICORE_SIZE               0xe
#define RADIO_2057_XTAL_BUF_SIZE                 0xf
#define RADIO_2057_XTAL_PULLCAP_SIZE             0x10
#define RADIO_2057_RFPLL_MASTER                  0x11
#define RADIO_2057_VCOMONITOR_VTH_L              0x12
#define RADIO_2057_VCOMONITOR_VTH_H              0x13
#define RADIO_2057_VCOCAL_BIASRESET_RFPLLREG_VOUT 0x14
#define RADIO_2057_VCO_VARCSIZE_IDAC             0x15
#define RADIO_2057_VCOCAL_COUNTVAL0              0x16
#define RADIO_2057_VCOCAL_COUNTVAL1              0x17
#define RADIO_2057_VCOCAL_INTCLK_COUNT           0x18
#define RADIO_2057_VCOCAL_MASTER                 0x19
#define RADIO_2057_VCOCAL_NUMCAPCHANGE           0x1a
#define RADIO_2057_VCOCAL_WINSIZE                0x1b
#define RADIO_2057_VCOCAL_DELAY_AFTER_REFRESH    0x1c
#define RADIO_2057_VCOCAL_DELAY_AFTER_CLOSELOOP  0x1d
#define RADIO_2057_VCOCAL_DELAY_AFTER_OPENLOOP   0x1e
#define RADIO_2057_VCOCAL_DELAY_BEFORE_OPENLOOP  0x1f
#define RADIO_2057_VCO_FORCECAPEN_FORCECAP1      0x20
#define RADIO_2057_VCO_FORCECAP0                 0x21
#define RADIO_2057_RFPLL_REFMASTER_SPAREXTALSIZE 0x22
#define RADIO_2057_RFPLL_PFD_RESET_PW            0x23
#define RADIO_2057_RFPLL_LOOPFILTER_R2           0x24
#define RADIO_2057_RFPLL_LOOPFILTER_R1           0x25
#define RADIO_2057_RFPLL_LOOPFILTER_C3           0x26
#define RADIO_2057_RFPLL_LOOPFILTER_C2           0x27
#define RADIO_2057_RFPLL_LOOPFILTER_C1           0x28
#define RADIO_2057_CP_KPD_IDAC                   0x29
#define RADIO_2057_RFPLL_IDACS                   0x2a
#define RADIO_2057_RFPLL_MISC_EN                 0x2b
#define RADIO_2057_RFPLL_MMD0                    0x2c
#define RADIO_2057_RFPLL_MMD1                    0x2d
#define RADIO_2057_RFPLL_MISC_CAL_RESETN         0x2e
#define RADIO_2057_JTAGXTAL_SIZE_CPBIAS_FILTRES  0x2f
#define RADIO_2057_VCO_ALCREF_BBPLLXTAL_SIZE     0x30
#define RADIO_2057_VCOCAL_READCAP0               0x31
#define RADIO_2057_VCOCAL_READCAP1               0x32
#define RADIO_2057_VCOCAL_STATUS                 0x33
#define RADIO_2057_LOGEN_PUS                     0x34
#define RADIO_2057_LOGEN_PTAT_RESETS             0x35
#define RADIO_2057_VCOBUF_IDACS                  0x36
#define RADIO_2057_VCOBUF_TUNE                   0x37
#define RADIO_2057_CMOSBUF_TX2GQ_IDACS           0x38
#define RADIO_2057_CMOSBUF_TX2GI_IDACS           0x39
#define RADIO_2057_CMOSBUF_TX5GQ_IDACS           0x3a
#define RADIO_2057_CMOSBUF_TX5GI_IDACS           0x3b
#define RADIO_2057_CMOSBUF_RX2GQ_IDACS           0x3c
#define RADIO_2057_CMOSBUF_RX2GI_IDACS           0x3d
#define RADIO_2057_CMOSBUF_RX5GQ_IDACS           0x3e
#define RADIO_2057_CMOSBUF_RX5GI_IDACS           0x3f
#define RADIO_2057_LOGEN_MX2G_IDACS              0x40
#define RADIO_2057_LOGEN_MX2G_TUNE               0x41
#define RADIO_2057_LOGEN_MX5G_IDACS              0x42
#define RADIO_2057_LOGEN_MX5G_TUNE               0x43
#define RADIO_2057_LOGEN_MX5G_RCCR               0x44
#define RADIO_2057_LOGEN_INDBUF2G_IDAC           0x45
#define RADIO_2057_LOGEN_INDBUF2G_IBOOST         0x46
#define RADIO_2057_LOGEN_INDBUF2G_TUNE           0x47
#define RADIO_2057_LOGEN_INDBUF5G_IDAC           0x48
#define RADIO_2057_LOGEN_INDBUF5G_IBOOST         0x49
#define RADIO_2057_LOGEN_INDBUF5G_TUNE           0x4a
#define RADIO_2057_CMOSBUF_TX_RCCR               0x4b
#define RADIO_2057_CMOSBUF_RX_RCCR               0x4c
#define RADIO_2057_LOGEN_SEL_PKDET               0x4d
#define RADIO_2057_CMOSBUF_SHAREIQ_PTAT          0x4e
#define RADIO_2057_RXTXBIAS_CONFIG_CORE0         0x4f
#define RADIO_2057_TXGM_TXRF_PUS_CORE0           0x50
#define RADIO_2057_TXGM_IDAC_BLEED_CORE0         0x51
#define RADIO_2057_TXGM_GAIN_CORE0               0x56
#define RADIO_2057_TXGM2G_PKDET_PUS_CORE0        0x57
#define RADIO_2057_PAD2G_PTATS_CORE0             0x58
#define RADIO_2057_PAD2G_IDACS_CORE0             0x59
#define RADIO_2057_PAD2G_BOOST_PU_CORE0          0x5a
#define RADIO_2057_PAD2G_CASCV_GAIN_CORE0        0x5b
#define RADIO_2057_TXMIX2G_TUNE_BOOST_PU_CORE0   0x5c
#define RADIO_2057_TXMIX2G_LODC_CORE0            0x5d
#define RADIO_2057_PAD2G_TUNE_PUS_CORE0          0x5e
#define RADIO_2057_IPA2G_GAIN_CORE0              0x5f
#define RADIO_2057_TSSI2G_SPARE1_CORE0           0x60
#define RADIO_2057_TSSI2G_SPARE2_CORE0           0x61
#define RADIO_2057_IPA2G_TUNEV_CASCV_PTAT_CORE0  0x62
#define RADIO_2057_IPA2G_IMAIN_CORE0             0x63
#define RADIO_2057_IPA2G_CASCONV_CORE0           0x64
#define RADIO_2057_IPA2G_CASCOFFV_CORE0          0x65
#define RADIO_2057_IPA2G_BIAS_FILTER_CORE0       0x66
#define RADIO_2057_TX5G_PKDET_CORE0              0x69
#define RADIO_2057_PGA_PTAT_TXGM5G_PU_CORE0      0x6a
#define RADIO_2057_PAD5G_PTATS1_CORE0            0x6b
#define RADIO_2057_PAD5G_CLASS_PTATS2_CORE0      0x6c
#define RADIO_2057_PGA_BOOSTPTAT_IMAIN_CORE0     0x6d
#define RADIO_2057_PAD5G_CASCV_IMAIN_CORE0       0x6e
#define RADIO_2057_TXMIX5G_IBOOST_PAD_IAUX_CORE0 0x6f
#define RADIO_2057_PGA_BOOST_TUNE_CORE0          0x70
#define RADIO_2057_PGA_GAIN_CORE0                0x71
#define RADIO_2057_PAD5G_CASCOFFV_GAIN_PUS_CORE0 0x72
#define RADIO_2057_TXMIX5G_BOOST_TUNE_CORE0      0x73
#define RADIO_2057_PAD5G_TUNE_MISC_PUS_CORE0     0x74
#define RADIO_2057_IPA5G_IAUX_CORE0              0x75
#define RADIO_2057_IPA5G_GAIN_CORE0              0x76
#define RADIO_2057_TSSI5G_SPARE1_CORE0           0x77
#define RADIO_2057_TSSI5G_SPARE2_CORE0           0x78
#define RADIO_2057_IPA5G_CASCOFFV_PU_CORE0       0x79
#define RADIO_2057_IPA5G_PTAT_CORE0              0x7a
#define RADIO_2057_IPA5G_IMAIN_CORE0             0x7b
#define RADIO_2057_IPA5G_CASCONV_CORE0           0x7c
#define RADIO_2057_IPA5G_BIAS_FILTER_CORE0       0x7d
#define RADIO_2057_PAD_BIAS_FILTER_BWS_CORE0     0x80
#define RADIO_2057_TR2G_CONFIG1_CORE0_NU         0x81
#define RADIO_2057_TR2G_CONFIG2_CORE0_NU         0x82
#define RADIO_2057_LNA5G_RFEN_CORE0              0x83
#define RADIO_2057_TR5G_CONFIG2_CORE0_NU         0x84
#define RADIO_2057_RXRFBIAS_IBOOST_PU_CORE0      0x85
#define RADIO_2057_RXRF_IABAND_RXGM_IMAIN_PTAT_CORE0 0x86
#define RADIO_2057_RXGM_CMFBITAIL_AUXPTAT_CORE0  0x87
#define RADIO_2057_RXMIX_ICORE_RXGM_IAUX_CORE0   0x88
#define RADIO_2057_RXMIX_CMFBITAIL_PU_CORE0      0x89
#define RADIO_2057_LNA2_IMAIN_PTAT_PU_CORE0      0x8a
#define RADIO_2057_LNA2_IAUX_PTAT_CORE0          0x8b
#define RADIO_2057_LNA1_IMAIN_PTAT_PU_CORE0      0x8c
#define RADIO_2057_LNA15G_INPUT_MATCH_TUNE_CORE0 0x8d
#define RADIO_2057_RXRFBIAS_BANDSEL_CORE0        0x8e
#define RADIO_2057_TIA_CONFIG_CORE0              0x8f
#define RADIO_2057_TIA_IQGAIN_CORE0              0x90
#define RADIO_2057_TIA_IBIAS2_CORE0              0x91
#define RADIO_2057_TIA_IBIAS1_CORE0              0x92
#define RADIO_2057_TIA_SPARE_Q_CORE0             0x93
#define RADIO_2057_TIA_SPARE_I_CORE0             0x94
#define RADIO_2057_RXMIX2G_PUS_CORE0             0x95
#define RADIO_2057_RXMIX2G_VCMREFS_CORE0         0x96
#define RADIO_2057_RXMIX2G_LODC_QI_CORE0         0x97
#define RADIO_2057_W12G_BW_LNA2G_PUS_CORE0       0x98
#define RADIO_2057_LNA2G_GAIN_CORE0              0x99
#define RADIO_2057_LNA2G_TUNE_CORE0              0x9a
#define RADIO_2057_RXMIX5G_PUS_CORE0             0x9b
#define RADIO_2057_RXMIX5G_VCMREFS_CORE0         0x9c
#define RADIO_2057_RXMIX5G_LODC_QI_CORE0         0x9d
#define RADIO_2057_W15G_BW_LNA5G_PUS_CORE0       0x9e
#define RADIO_2057_LNA5G_GAIN_CORE0              0x9f
#define RADIO_2057_LNA5G_TUNE_CORE0              0xa0
#define RADIO_2057_LPFSEL_TXRX_RXBB_PUS_CORE0    0xa1
#define RADIO_2057_RXBB_BIAS_MASTER_CORE0        0xa2
#define RADIO_2057_RXBB_VGABUF_IDACS_CORE0       0xa3
#define RADIO_2057_LPF_VCMREF_TXBUF_VCMREF_CORE0 0xa4
#define RADIO_2057_TXBUF_VINCM_CORE0             0xa5
#define RADIO_2057_TXBUF_IDACS_CORE0             0xa6
#define RADIO_2057_LPF_RESP_RXBUF_BW_CORE0       0xa7
#define RADIO_2057_RXBB_CC_CORE0                 0xa8
#define RADIO_2057_RXBB_SPARE3_CORE0             0xa9
#define RADIO_2057_RXBB_RCCAL_HPC_CORE0          0xaa
#define RADIO_2057_LPF_IDACS_CORE0               0xab
#define RADIO_2057_LPFBYP_DCLOOP_BYP_IDAC_CORE0  0xac
#define RADIO_2057_TXBUF_GAIN_CORE0              0xad
#define RADIO_2057_AFELOOPBACK_AACI_RESP_CORE0   0xae
#define RADIO_2057_RXBUF_DEGEN_CORE0             0xaf
#define RADIO_2057_RXBB_SPARE2_CORE0             0xb0
#define RADIO_2057_RXBB_SPARE1_CORE0             0xb1
#define RADIO_2057_RSSI_MASTER_CORE0             0xb2
#define RADIO_2057_W2_MASTER_CORE0               0xb3
#define RADIO_2057_NB_MASTER_CORE0               0xb4
#define RADIO_2057_W2_IDACS0_Q_CORE0             0xb5
#define RADIO_2057_W2_IDACS1_Q_CORE0             0xb6
#define RADIO_2057_W2_IDACS0_I_CORE0             0xb7
#define RADIO_2057_W2_IDACS1_I_CORE0             0xb8
#define RADIO_2057_RSSI_GPAIOSEL_W1_IDACS_CORE0  0xb9
#define RADIO_2057_NB_IDACS_Q_CORE0              0xba
#define RADIO_2057_NB_IDACS_I_CORE0              0xbb
#define RADIO_2057_BACKUP4_CORE0                 0xc1
#define RADIO_2057_BACKUP3_CORE0                 0xc2
#define RADIO_2057_BACKUP2_CORE0                 0xc3
#define RADIO_2057_BACKUP1_CORE0                 0xc4
#define RADIO_2057_SPARE16_CORE0                 0xc5
#define RADIO_2057_SPARE15_CORE0                 0xc6
#define RADIO_2057_SPARE14_CORE0                 0xc7
#define RADIO_2057_SPARE13_CORE0                 0xc8
#define RADIO_2057_SPARE12_CORE0                 0xc9
#define RADIO_2057_SPARE11_CORE0                 0xca
#define RADIO_2057_TX2G_BIAS_RESETS_CORE0        0xcb
#define RADIO_2057_TX5G_BIAS_RESETS_CORE0        0xcc
#define RADIO_2057_IQTEST_SEL_PU                 0xcd
#define RADIO_2057_XTAL_CONFIG2                  0xce
#define RADIO_2057_BUFS_MISC_LPFBW_CORE0         0xcf
#define RADIO_2057_TXLPF_RCCAL_CORE0             0xd0
#define RADIO_2057_RXBB_GPAIOSEL_RXLPF_RCCAL_CORE0 0xd1
#define RADIO_2057_LPF_GAIN_CORE0                0xd2
#define RADIO_2057_DACBUF_IDACS_BW_CORE0         0xd3
#define RADIO_2057_RXTXBIAS_CONFIG_CORE1         0xd4
#define RADIO_2057_TXGM_TXRF_PUS_CORE1           0xd5
#define RADIO_2057_TXGM_IDAC_BLEED_CORE1         0xd6
#define RADIO_2057_TXGM_GAIN_CORE1               0xdb
#define RADIO_2057_TXGM2G_PKDET_PUS_CORE1        0xdc
#define RADIO_2057_PAD2G_PTATS_CORE1             0xdd
#define RADIO_2057_PAD2G_IDACS_CORE1             0xde
#define RADIO_2057_PAD2G_BOOST_PU_CORE1          0xdf
#define RADIO_2057_PAD2G_CASCV_GAIN_CORE1        0xe0
#define RADIO_2057_TXMIX2G_TUNE_BOOST_PU_CORE1   0xe1
#define RADIO_2057_TXMIX2G_LODC_CORE1            0xe2
#define RADIO_2057_PAD2G_TUNE_PUS_CORE1          0xe3
#define RADIO_2057_IPA2G_GAIN_CORE1              0xe4
#define RADIO_2057_TSSI2G_SPARE1_CORE1           0xe5
#define RADIO_2057_TSSI2G_SPARE2_CORE1           0xe6
#define RADIO_2057_IPA2G_TUNEV_CASCV_PTAT_CORE1  0xe7
#define RADIO_2057_IPA2G_IMAIN_CORE1             0xe8
#define RADIO_2057_IPA2G_CASCONV_CORE1           0xe9
#define RADIO_2057_IPA2G_CASCOFFV_CORE1          0xea
#define RADIO_2057_IPA2G_BIAS_FILTER_CORE1       0xeb
#define RADIO_2057_TX5G_PKDET_CORE1              0xee
#define RADIO_2057_PGA_PTAT_TXGM5G_PU_CORE1      0xef
#define RADIO_2057_PAD5G_PTATS1_CORE1            0xf0
#define RADIO_2057_PAD5G_CLASS_PTATS2_CORE1      0xf1
#define RADIO_2057_PGA_BOOSTPTAT_IMAIN_CORE1     0xf2
#define RADIO_2057_PAD5G_CASCV_IMAIN_CORE1       0xf3
#define RADIO_2057_TXMIX5G_IBOOST_PAD_IAUX_CORE1 0xf4
#define RADIO_2057_PGA_BOOST_TUNE_CORE1          0xf5
#define RADIO_2057_PGA_GAIN_CORE1                0xf6
#define RADIO_2057_PAD5G_CASCOFFV_GAIN_PUS_CORE1 0xf7
#define RADIO_2057_TXMIX5G_BOOST_TUNE_CORE1      0xf8
#define RADIO_2057_PAD5G_TUNE_MISC_PUS_CORE1     0xf9
#define RADIO_2057_IPA5G_IAUX_CORE1              0xfa
#define RADIO_2057_IPA5G_GAIN_CORE1              0xfb
#define RADIO_2057_TSSI5G_SPARE1_CORE1           0xfc
#define RADIO_2057_TSSI5G_SPARE2_CORE1           0xfd
#define RADIO_2057_IPA5G_CASCOFFV_PU_CORE1       0xfe
#define RADIO_2057_IPA5G_PTAT_CORE1              0xff
#define RADIO_2057_IPA5G_IMAIN_CORE1             0x100
#define RADIO_2057_IPA5G_CASCONV_CORE1           0x101
#define RADIO_2057_IPA5G_BIAS_FILTER_CORE1       0x102
#define RADIO_2057_PAD_BIAS_FILTER_BWS_CORE1     0x105
#define RADIO_2057_TR2G_CONFIG1_CORE1_NU         0x106
#define RADIO_2057_TR2G_CONFIG2_CORE1_NU         0x107
#define RADIO_2057_LNA5G_RFEN_CORE1              0x108
#define RADIO_2057_TR5G_CONFIG2_CORE1_NU         0x109
#define RADIO_2057_RXRFBIAS_IBOOST_PU_CORE1      0x10a
#define RADIO_2057_RXRF_IABAND_RXGM_IMAIN_PTAT_CORE1 0x10b
#define RADIO_2057_RXGM_CMFBITAIL_AUXPTAT_CORE1  0x10c
#define RADIO_2057_RXMIX_ICORE_RXGM_IAUX_CORE1   0x10d
#define RADIO_2057_RXMIX_CMFBITAIL_PU_CORE1      0x10e
#define RADIO_2057_LNA2_IMAIN_PTAT_PU_CORE1      0x10f
#define RADIO_2057_LNA2_IAUX_PTAT_CORE1          0x110
#define RADIO_2057_LNA1_IMAIN_PTAT_PU_CORE1      0x111
#define RADIO_2057_LNA15G_INPUT_MATCH_TUNE_CORE1 0x112
#define RADIO_2057_RXRFBIAS_BANDSEL_CORE1        0x113
#define RADIO_2057_TIA_CONFIG_CORE1              0x114
#define RADIO_2057_TIA_IQGAIN_CORE1              0x115
#define RADIO_2057_TIA_IBIAS2_CORE1              0x116
#define RADIO_2057_TIA_IBIAS1_CORE1              0x117
#define RADIO_2057_TIA_SPARE_Q_CORE1             0x118
#define RADIO_2057_TIA_SPARE_I_CORE1             0x119
#define RADIO_2057_RXMIX2G_PUS_CORE1             0x11a
#define RADIO_2057_RXMIX2G_VCMREFS_CORE1         0x11b
#define RADIO_2057_RXMIX2G_LODC_QI_CORE1         0x11c
#define RADIO_2057_W12G_BW_LNA2G_PUS_CORE1       0x11d
#define RADIO_2057_LNA2G_GAIN_CORE1              0x11e
#define RADIO_2057_LNA2G_TUNE_CORE1              0x11f
#define RADIO_2057_RXMIX5G_PUS_CORE1             0x120
#define RADIO_2057_RXMIX5G_VCMREFS_CORE1         0x121
#define RADIO_2057_RXMIX5G_LODC_QI_CORE1         0x122
#define RADIO_2057_W15G_BW_LNA5G_PUS_CORE1       0x123
#define RADIO_2057_LNA5G_GAIN_CORE1              0x124
#define RADIO_2057_LNA5G_TUNE_CORE1              0x125
#define RADIO_2057_LPFSEL_TXRX_RXBB_PUS_CORE1    0x126
#define RADIO_2057_RXBB_BIAS_MASTER_CORE1        0x127
#define RADIO_2057_RXBB_VGABUF_IDACS_CORE1       0x128
#define RADIO_2057_LPF_VCMREF_TXBUF_VCMREF_CORE1 0x129
#define RADIO_2057_TXBUF_VINCM_CORE1             0x12a
#define RADIO_2057_TXBUF_IDACS_CORE1             0x12b
#define RADIO_2057_LPF_RESP_RXBUF_BW_CORE1       0x12c
#define RADIO_2057_RXBB_CC_CORE1                 0x12d
#define RADIO_2057_RXBB_SPARE3_CORE1             0x12e
#define RADIO_2057_RXBB_RCCAL_HPC_CORE1          0x12f
#define RADIO_2057_LPF_IDACS_CORE1               0x130
#define RADIO_2057_LPFBYP_DCLOOP_BYP_IDAC_CORE1  0x131
#define RADIO_2057_TXBUF_GAIN_CORE1              0x132
#define RADIO_2057_AFELOOPBACK_AACI_RESP_CORE1   0x133
#define RADIO_2057_RXBUF_DEGEN_CORE1             0x134
#define RADIO_2057_RXBB_SPARE2_CORE1             0x135
#define RADIO_2057_RXBB_SPARE1_CORE1             0x136
#define RADIO_2057_RSSI_MASTER_CORE1             0x137
#define RADIO_2057_W2_MASTER_CORE1               0x138
#define RADIO_2057_NB_MASTER_CORE1               0x139
#define RADIO_2057_W2_IDACS0_Q_CORE1             0x13a
#define RADIO_2057_W2_IDACS1_Q_CORE1             0x13b
#define RADIO_2057_W2_IDACS0_I_CORE1             0x13c
#define RADIO_2057_W2_IDACS1_I_CORE1             0x13d
#define RADIO_2057_RSSI_GPAIOSEL_W1_IDACS_CORE1  0x13e
#define RADIO_2057_NB_IDACS_Q_CORE1              0x13f
#define RADIO_2057_NB_IDACS_I_CORE1              0x140
#define RADIO_2057_BACKUP4_CORE1                 0x146
#define RADIO_2057_BACKUP3_CORE1                 0x147
#define RADIO_2057_BACKUP2_CORE1                 0x148
#define RADIO_2057_BACKUP1_CORE1                 0x149
#define RADIO_2057_SPARE16_CORE1                 0x14a
#define RADIO_2057_SPARE15_CORE1                 0x14b
#define RADIO_2057_SPARE14_CORE1                 0x14c
#define RADIO_2057_SPARE13_CORE1                 0x14d
#define RADIO_2057_SPARE12_CORE1                 0x14e
#define RADIO_2057_SPARE11_CORE1                 0x14f
#define RADIO_2057_TX2G_BIAS_RESETS_CORE1        0x150
#define RADIO_2057_TX5G_BIAS_RESETS_CORE1        0x151
#define RADIO_2057_SPARE8_CORE1                  0x152
#define RADIO_2057_SPARE7_CORE1                  0x153
#define RADIO_2057_BUFS_MISC_LPFBW_CORE1         0x154
#define RADIO_2057_TXLPF_RCCAL_CORE1             0x155
#define RADIO_2057_RXBB_GPAIOSEL_RXLPF_RCCAL_CORE1 0x156
#define RADIO_2057_LPF_GAIN_CORE1                0x157
#define RADIO_2057_DACBUF_IDACS_BW_CORE1         0x158
#define RADIO_2057_DACBUF_VINCM_CORE1            0x159
#define RADIO_2057_RCCAL_START_R1_Q1_P1          0x15a
#define RADIO_2057_RCCAL_X1                      0x15b
#define RADIO_2057_RCCAL_TRC0                    0x15c
#define RADIO_2057_RCCAL_TRC1                    0x15d
#define RADIO_2057_RCCAL_DONE_OSCCAP             0x15e
#define RADIO_2057_RCCAL_N0_0                    0x15f
#define RADIO_2057_RCCAL_N0_1                    0x160
#define RADIO_2057_RCCAL_N1_0                    0x161
#define RADIO_2057_RCCAL_N1_1                    0x162
#define RADIO_2057_RCAL_STATUS                   0x163
#define RADIO_2057_XTALPUOVR_PINCTRL             0x164
#define RADIO_2057_OVR_REG0                      0x165
#define RADIO_2057_OVR_REG1                      0x166
#define RADIO_2057_OVR_REG2                      0x167
#define RADIO_2057_OVR_REG3                      0x168
#define RADIO_2057_OVR_REG4                      0x169
#define RADIO_2057_RCCAL_SCAP_VAL                0x16a
#define RADIO_2057_RCCAL_BCAP_VAL                0x16b
#define RADIO_2057_RCCAL_HPC_VAL                 0x16c
#define RADIO_2057_RCCAL_OVERRIDES               0x16d
#define RADIO_2057_TX0_IQCAL_GAIN_BW             0x170
#define RADIO_2057_TX0_LOFT_FINE_I               0x171
#define RADIO_2057_TX0_LOFT_FINE_Q               0x172
#define RADIO_2057_TX0_LOFT_COARSE_I             0x173
#define RADIO_2057_TX0_LOFT_COARSE_Q             0x174
#define RADIO_2057_TX0_TX_SSI_MASTER             0x175
#define RADIO_2057_TX0_IQCAL_VCM_HG              0x176
#define RADIO_2057_TX0_IQCAL_IDAC                0x177
#define RADIO_2057_TX0_TSSI_VCM                  0x178
#define RADIO_2057_TX0_TX_SSI_MUX                0x179
#define RADIO_2057_TX0_TSSIA                     0x17a
#define RADIO_2057_TX0_TSSIG                     0x17b
#define RADIO_2057_TX0_TSSI_MISC1                0x17c
#define RADIO_2057_TX0_TXRXCOUPLE_2G_ATTEN       0x17d
#define RADIO_2057_TX0_TXRXCOUPLE_2G_PWRUP       0x17e
#define RADIO_2057_TX0_TXRXCOUPLE_5G_ATTEN       0x17f
#define RADIO_2057_TX0_TXRXCOUPLE_5G_PWRUP       0x180
#define RADIO_2057_TX1_IQCAL_GAIN_BW             0x190
#define RADIO_2057_TX1_LOFT_FINE_I               0x191
#define RADIO_2057_TX1_LOFT_FINE_Q               0x192
#define RADIO_2057_TX1_LOFT_COARSE_I             0x193
#define RADIO_2057_TX1_LOFT_COARSE_Q             0x194
#define RADIO_2057_TX1_TX_SSI_MASTER             0x195
#define RADIO_2057_TX1_IQCAL_VCM_HG              0x196
#define RADIO_2057_TX1_IQCAL_IDAC                0x197
#define RADIO_2057_TX1_TSSI_VCM                  0x198
#define RADIO_2057_TX1_TX_SSI_MUX                0x199
#define RADIO_2057_TX1_TSSIA                     0x19a
#define RADIO_2057_TX1_TSSIG                     0x19b
#define RADIO_2057_TX1_TSSI_MISC1                0x19c
#define RADIO_2057_TX1_TXRXCOUPLE_2G_ATTEN       0x19d
#define RADIO_2057_TX1_TXRXCOUPLE_2G_PWRUP       0x19e
#define RADIO_2057_TX1_TXRXCOUPLE_5G_ATTEN       0x19f
#define RADIO_2057_TX1_TXRXCOUPLE_5G_PWRUP       0x1a0
#define RADIO_2057_AFE_VCM_CAL_MASTER_CORE0      0x1a1
#define RADIO_2057_AFE_SET_VCM_I_CORE0           0x1a2
#define RADIO_2057_AFE_SET_VCM_Q_CORE0           0x1a3
#define RADIO_2057_AFE_STATUS_VCM_IQADC_CORE0    0x1a4
#define RADIO_2057_AFE_STATUS_VCM_I_CORE0        0x1a5
#define RADIO_2057_AFE_STATUS_VCM_Q_CORE0        0x1a6
#define RADIO_2057_AFE_VCM_CAL_MASTER_CORE1      0x1a7
#define RADIO_2057_AFE_SET_VCM_I_CORE1           0x1a8
#define RADIO_2057_AFE_SET_VCM_Q_CORE1           0x1a9
#define RADIO_2057_AFE_STATUS_VCM_IQADC_CORE1    0x1aa
#define RADIO_2057_AFE_STATUS_VCM_I_CORE1        0x1ab
#define RADIO_2057_AFE_STATUS_VCM_Q_CORE1        0x1ac
#define	RADIO_2057_OVR_REG10			 0x1b7
#define	RADIO_2057_OVR_REG20			 0x1c1
#define	RADIO_2057_OVR_REG21			 0x1c2
#define	RADIO_2057_OVR_REG29			 0x1cc
#define RADIO_2057_TR2G_CONFIG4_CORE0		 0x1cd
#define RADIO_2057_TR2G_CONFIG4_CORE1		 0x1d1
/* New REGs for 2057 rev7 and rev10 */
#define RADIO_2057v7_DACBUF_VINCM_CORE0          0x1ad /* not for rev9 */
#define RADIO_2057v7_RCCAL_MASTER                0x1ae /* not for rev9 */
#define RADIO_2057v7_TR2G_CONFIG3_CORE0_NU       0x1af /* not for rev9 */
#define RADIO_2057v7_TR2G_CONFIG3_CORE1_NU       0x1b0 /* not for rev9 */
#define RADIO_2057v7_LOGEN_PUS1                  0x1b1 /* not for rev9 */
#define RADIO_2057v7_OVR_REG5                    0x1b2 /* not for rev9 */
#define RADIO_2057v7_OVR_REG6                    0x1b3 /* not for rev9 */
#define RADIO_2057v7_OVR_REG7                    0x1b4 /* not for rev9 */
#define RADIO_2057v7_OVR_REG8                    0x1b5 /* not for rev9 */
#define RADIO_2057v7_OVR_REG9                    0x1b6 /* not for rev9 */
#define RADIO_2057v7_OVR_REG10                   0x1b7 /* not for rev9 */
#define RADIO_2057v7_OVR_REG11                   0x1b8 /* not for rev9 */
#define RADIO_2057v7_OVR_REG12                   0x1b9 /* not for rev9 */
#define RADIO_2057v7_OVR_REG13                   0x1ba /* not for rev9 */
#define RADIO_2057v7_OVR_REG14                   0x1bb /* not for rev9 */
#define RADIO_2057v7_OVR_REG15                   0x1bc /* not for rev9 */
#define RADIO_2057v7_OVR_REG16                   0x1bd /* not for rev9 */
#define RADIO_2057v7_OVR_REG17                   0x1be /* not for rev9 */
#define RADIO_2057v7_OVR_REG18                   0x1bf /* not for rev9 */
#define RADIO_2057v7_OVR_REG19                   0x1c0 /* not for rev9 */
#define RADIO_2057v7_OVR_REG20                   0x1c1 /* not for rev9 */
#define RADIO_2057v7_OVR_REG21                   0x1c2 /* not for rev9 */
#define RADIO_2057v7_OVR_REG22                   0x1c3 /* not for rev9 */
#define RADIO_2057v7_OVR_REG23                   0x1c4 /* not for rev9 */
#define RADIO_2057v7_OVR_REG24                   0x1c5 /* not for rev9 */
#define RADIO_2057v7_OVR_REG25                   0x1c6 /* not for rev9 */
#define RADIO_2057v7_OVR_REG26                   0x1c7 /* not for rev9 */
#define RADIO_2057v7_OVR_REG27                   0x1c8 /* not for rev9 */
#define RADIO_2057v7_OVR_REG28                   0x1c9 /* not for rev9 */
#define RADIO_2057v7_IQTEST_SEL_PU2              0x1ca /* not for rev9 */

/* New REGs for 2057 rev9 */
#define RADIO_2057v9_REV_ID                        0x0
#define RADIO_2057v9_DEV_ID0                       0x1
#define RADIO_2057v9_DEV_ID1                       0x2
#define RADIO_2057v9_DACBUF_VINCM_CORE0            0x1ad
#define RADIO_2057v9_RCCAL_MASTER                  0x1ae
#define RADIO_2057v9_TR2G_CONFIG3_CORE0_NU         0x1af
#define RADIO_2057v9_TR2G_CONFIG3_CORE1_NU         0x1b0
#define RADIO_2057v9_LOGEN_PUS1                    0x1b1
#define RADIO_2057v9_OVR_REG5                      0x1b2
#define RADIO_2057v9_OVR_REG6                      0x1b3
#define RADIO_2057v9_OVR_REG7                      0x1b4
#define RADIO_2057v9_OVR_REG8                      0x1b5
#define RADIO_2057v9_OVR_REG9                      0x1b6
#define RADIO_2057v9_OVR_REG10                     0x1b7
#define RADIO_2057v9_OVR_REG11                     0x1b8
#define RADIO_2057v9_OVR_REG12                     0x1b9
#define RADIO_2057v9_OVR_REG13                     0x1ba
#define RADIO_2057v9_OVR_REG14                     0x1bb
#define RADIO_2057v9_OVR_REG15                     0x1bc
#define RADIO_2057v9_OVR_REG16                     0x1bd
#define RADIO_2057v9_OVR_REG17                     0x1be
#define RADIO_2057v9_OVR_REG18                     0x1bf
#define RADIO_2057v9_OVR_REG19                     0x1c0
#define RADIO_2057v9_OVR_REG20                     0x1c1
#define RADIO_2057v9_OVR_REG21                     0x1c2
#define RADIO_2057v9_OVR_REG22                     0x1c3
#define RADIO_2057v9_OVR_REG23                     0x1c4
#define RADIO_2057v9_OVR_REG24                     0x1c5
#define RADIO_2057v9_OVR_REG25                     0x1c6
#define RADIO_2057v9_OVR_REG26                     0x1c7
#define RADIO_2057v9_OVR_REG27                     0x1c8
#define RADIO_2057v9_OVR_REG28                     0x1c9
#define RADIO_2057v9_IQTEST_SEL_PU2                0x1ca
#define RADIO_2057v9_VBAT_CONFIG                   0x1cb

#define RADIO_2057_VCM_MASK			 0x7

/* 2069 defines */
#define JTAG_2069_SHIFT                          9

/* REV0 specific defines */

/* 2069 JTAG core selects */
#define JTAG_2069_REV0_CR0                            (0x0 << JTAG_2069_SHIFT)
#define JTAG_2069_REV0_CR1                            (0x1 << JTAG_2069_SHIFT)
#define JTAG_2069_REV0_CR2                            (0x2 << JTAG_2069_SHIFT)
#define JTAG_2069_REV0_ALL                            (0x3 << JTAG_2069_SHIFT)
#define JTAG_2069_REV0_PLL                            (0x4 << JTAG_2069_SHIFT)
#define JTAG_2069_REV0_MASK                           (0x7 << JTAG_2069_SHIFT)

/* address defines */
#define RF0_2069_REV0_REV_ID                          (0x0 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_DEV_ID                          (0x1 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TEMPSENSE_CFG                   (0xe | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_VBAT_CFG                        (0xf | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TESTBUF_CFG1                    (0x17 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_GPAIO_SEL0                      (0x18 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_GPAIO_SEL1                      (0x19 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_IQCAL_CFG1                      (0x1a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_IQCAL_CFG2                      (0x1b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_IQCAL_CFG3                      (0x1c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_IQCAL_IDAC                      (0x1d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_TSSI                       (0x1e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX5G_TSSI                       (0x1f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXRX2G_CAL_TX                   (0x20 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXRX5G_CAL_TX                   (0x21 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXRX2G_CAL_RX                   (0x22 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXRX5G_CAL_RX                   (0x23 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_AUXPGA_CFG1                     (0x24 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_AUXPGA_VMID                     (0x25 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_CORE_BIAS                       (0x26 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_CFG1                      (0x27 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_CFG2                      (0x28 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_TUNE                      (0x29 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_IDAC1                     (0x2a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_IDAC2                     (0x2b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_RSSI                      (0x2c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA2G_RSSI_ANA                  (0x2d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_CFG1                      (0x2e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_CFG2                      (0x2f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_TUNE                      (0x30 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_IDAC1                     (0x31 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_IDAC2                     (0x32 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_RSSI                      (0x33 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LNA5G_RSSI_ANA                  (0x34 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXMIX2G_CFG1                    (0x35 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXMIX2G_IDAC                    (0x36 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXMIX5G_CFG1                    (0x37 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXMIX5G_IDAC                    (0x38 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF2G_CFG1                     (0x39 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF2G_CFG2                     (0x3a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF2G_SPARE                    (0x3b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF5G_CFG1                     (0x3c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF5G_CFG2                     (0x3d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RX_REG_BACKUP_1                 (0x3e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RX_REG_BACKUP_2                 (0x3f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RXRF5G_SPARE                    (0x40 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TIA_CFG1                        (0x41 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TIA_CFG2                        (0x42 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TIA_CFG3                        (0x43 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TIA_IDAC1                       (0x44 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_NBRSSI_CONFG                    (0x45 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_NBRSSI_BIAS                     (0x46 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_NBRSSI_IB                       (0x47 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_NBRSSI_TEST                     (0x48 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_WRSSI3_CONFG                    (0x49 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_WRSSI3_BIAS                     (0x4a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_WRSSI3_TEST                     (0x4b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_MUX_SWITCHES                (0x4c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_ENABLES                     (0x4d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_MAIN_CONTROLS               (0x4e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_CORNER_FREQUENCY_TUNING     (0x4f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_DC_LOOP_AND_MISC            (0x50 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_BIAS_LEVELS_LOW             (0x51 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_BIAS_LEVELS_MID             (0x52 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_BIAS_LEVELS_HIGH            (0x53 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x54 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LPF_COMMON_MODE_REFERENCE_LEVELS (0x55 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX2G_CFG1                       (0x56 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX2G_CFG2                       (0x57 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX5G_CFG1                       (0x58 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_CFG1                       (0x59 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXGM_CFG2                       (0x5a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXMIX2G_CFG1                    (0x5b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TXMIX5G_CFG1                    (0x5c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA2G_CFG1                      (0x5d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA2G_CFG2                      (0x5e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA2G_IDAC                      (0x5f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA2G_INCAP                     (0x60 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA5G_CFG1                      (0x61 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA5G_CFG2                      (0x62 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA5G_IDAC                      (0x63 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PGA5G_INCAP                     (0x64 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD2G_CFG1                      (0x65 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD2G_IDAC                      (0x66 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD2G_SLOPE                     (0x67 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD2G_TUNE                      (0x68 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD2G_INCAP                     (0x69 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD5G_CFG1                      (0x6a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD5G_IDAC                      (0x6b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD5G_SLOPE                     (0x6c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD5G_TUNE                      (0x6d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PAD5G_INCAP                     (0x6e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_CFG1                       (0x6f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_CFG2                       (0x70 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_CFG3                       (0x71 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_IDAC1                      (0x72 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_IDAC2                      (0x73 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA2G_INCAP                      (0x74 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_CFG1                       (0x75 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_CFG2                       (0x76 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_CFG3                       (0x77 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_IDAC1                      (0x78 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_IDAC2                      (0x79 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_PA5G_INCAP                      (0x7a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW2G_CFG1                     (0x7b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW2G_CFG2                     (0x7c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW2G_CFG3                     (0x7d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW5G_CFG1                     (0x7e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW5G_CFG2                     (0x7f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TRSW5G_CFG3                     (0x80 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX_REG_BACKUP_1                 (0x81 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX_REG_BACKUP_2                 (0x82 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX_REG_BACKUP_3                 (0x83 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_TX_REG_BACKUP_4                 (0x84 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_CFG1                    (0x10d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_CFG2                    (0x10e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_IDAC2                   (0x10f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_IDAC3                   (0x110 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_RCCR                    (0x111 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN2G_TUNE                    (0x112 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_CFG1                    (0x113 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_CFG2                    (0x114 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_IDAC1                   (0x115 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_IDAC2                   (0x116 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_IDAC3                   (0x117 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_IDAC4                   (0x118 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_RCCR                    (0x119 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_TUNE1                   (0x11a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_LOGEN5G_TUNE2                   (0x11b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_DAC_CFG1                        (0x11c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_DAC_CFG2                        (0x11d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_DAC_BIAS                        (0x11e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CFG1                        (0x11f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CFG2                        (0x120 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CFG3                        (0x121 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CFG4                        (0x122 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CFG5                        (0x123 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_BIAS1                       (0x124 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_BIAS2                       (0x125 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_RC1                         (0x126 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_RC2                         (0x127 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE1                    (0x128 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE2                    (0x129 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE3                    (0x12a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE4                    (0x12b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE5                    (0x12c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE6                    (0x12d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE7                    (0x12e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE8                    (0x12f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE9                    (0x130 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE10                   (0x131 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE11                   (0x132 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE12                   (0x133 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE13                   (0x134 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE14                   (0x135 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE15                   (0x136 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE16                   (0x137 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE17                   (0x138 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE18                   (0x139 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE19                   (0x13a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE20                   (0x13b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE21                   (0x13c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE22                   (0x13d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE23                   (0x13e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE24                   (0x13f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE25                   (0x140 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE26                   (0x141 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE27                   (0x142 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_CALCODE28                   (0x143 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_ADC_STATUS                      (0x144 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_AFEDIV1                         (0x145 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_AFEDIV2                         (0x146 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_GPAIO_CFG1                      (0x147 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RADIO_SPARE1                    (0x157 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RADIO_SPARE2                    (0x158 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RADIO_SPARE3                    (0x159 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_RADIO_SPARE4                    (0x15a | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_CLB_REG_0                       (0x15b | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_CLB_REG_1                       (0x15c | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR1                            (0x15d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR3                            (0x15f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR4                            (0x160 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR5                            (0x161 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR6                            (0x162 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR7                            (0x163 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR8                            (0x164 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR9                            (0x165 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR10                           (0x166 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR11                           (0x167 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR12                           (0x168 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR13                           (0x169 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR17                           (0x16d | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR18                           (0x16e | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR19                           (0x16f | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR20                           (0x170 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR21                           (0x171 | JTAG_2069_REV0_CR0)
#define RF0_2069_REV0_OVR23                           (0x173 | JTAG_2069_REV0_CR0)
#define RF1_2069_REV0_REV_ID                          (0x0 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_DEV_ID                          (0x1 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TEMPSENSE_CFG                   (0xe | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_VBAT_CFG                        (0xf | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TESTBUF_CFG1                    (0x17 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_GPAIO_SEL0                      (0x18 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_GPAIO_SEL1                      (0x19 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_IQCAL_CFG1                      (0x1a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_IQCAL_CFG2                      (0x1b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_IQCAL_CFG3                      (0x1c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_IQCAL_IDAC                      (0x1d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_TSSI                       (0x1e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX5G_TSSI                       (0x1f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXRX2G_CAL_TX                   (0x20 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXRX5G_CAL_TX                   (0x21 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXRX2G_CAL_RX                   (0x22 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXRX5G_CAL_RX                   (0x23 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_AUXPGA_CFG1                     (0x24 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_AUXPGA_VMID                     (0x25 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_CORE_BIAS                       (0x26 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_CFG1                      (0x27 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_CFG2                      (0x28 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_TUNE                      (0x29 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_IDAC1                     (0x2a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_IDAC2                     (0x2b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_RSSI                      (0x2c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA2G_RSSI_ANA                  (0x2d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_CFG1                      (0x2e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_CFG2                      (0x2f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_TUNE                      (0x30 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_IDAC1                     (0x31 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_IDAC2                     (0x32 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_RSSI                      (0x33 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LNA5G_RSSI_ANA                  (0x34 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXMIX2G_CFG1                    (0x35 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXMIX2G_IDAC                    (0x36 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXMIX5G_CFG1                    (0x37 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXMIX5G_IDAC                    (0x38 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF2G_CFG1                     (0x39 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF2G_CFG2                     (0x3a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF2G_SPARE                    (0x3b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF5G_CFG1                     (0x3c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF5G_CFG2                     (0x3d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RX_REG_BACKUP_1                 (0x3e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RX_REG_BACKUP_2                 (0x3f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RXRF5G_SPARE                    (0x40 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TIA_CFG1                        (0x41 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TIA_CFG2                        (0x42 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TIA_CFG3                        (0x43 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TIA_IDAC1                       (0x44 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_NBRSSI_CONFG                    (0x45 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_NBRSSI_BIAS                     (0x46 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_NBRSSI_IB                       (0x47 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_NBRSSI_TEST                     (0x48 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_WRSSI3_CONFG                    (0x49 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_WRSSI3_BIAS                     (0x4a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_WRSSI3_TEST                     (0x4b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_MUX_SWITCHES                (0x4c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_ENABLES                     (0x4d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_MAIN_CONTROLS               (0x4e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_CORNER_FREQUENCY_TUNING     (0x4f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_DC_LOOP_AND_MISC            (0x50 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_BIAS_LEVELS_LOW             (0x51 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_BIAS_LEVELS_MID             (0x52 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_BIAS_LEVELS_HIGH            (0x53 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x54 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LPF_COMMON_MODE_REFERENCE_LEVELS (0x55 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX2G_CFG1                       (0x56 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX2G_CFG2                       (0x57 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX5G_CFG1                       (0x58 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_CFG1                       (0x59 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXGM_CFG2                       (0x5a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXMIX2G_CFG1                    (0x5b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TXMIX5G_CFG1                    (0x5c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA2G_CFG1                      (0x5d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA2G_CFG2                      (0x5e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA2G_IDAC                      (0x5f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA2G_INCAP                     (0x60 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA5G_CFG1                      (0x61 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA5G_CFG2                      (0x62 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA5G_IDAC                      (0x63 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PGA5G_INCAP                     (0x64 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD2G_CFG1                      (0x65 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD2G_IDAC                      (0x66 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD2G_SLOPE                     (0x67 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD2G_TUNE                      (0x68 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD2G_INCAP                     (0x69 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD5G_CFG1                      (0x6a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD5G_IDAC                      (0x6b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD5G_SLOPE                     (0x6c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD5G_TUNE                      (0x6d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PAD5G_INCAP                     (0x6e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_CFG1                       (0x6f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_CFG2                       (0x70 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_CFG3                       (0x71 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_IDAC1                      (0x72 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_IDAC2                      (0x73 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA2G_INCAP                      (0x74 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_CFG1                       (0x75 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_CFG2                       (0x76 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_CFG3                       (0x77 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_IDAC1                      (0x78 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_IDAC2                      (0x79 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_PA5G_INCAP                      (0x7a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW2G_CFG1                     (0x7b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW2G_CFG2                     (0x7c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW2G_CFG3                     (0x7d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW5G_CFG1                     (0x7e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW5G_CFG2                     (0x7f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TRSW5G_CFG3                     (0x80 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX_REG_BACKUP_1                 (0x81 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX_REG_BACKUP_2                 (0x82 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX_REG_BACKUP_3                 (0x83 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_TX_REG_BACKUP_4                 (0x84 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN2G_CFG2                    (0x10e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN2G_IDAC3                   (0x110 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN2G_RCCR                    (0x111 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN5G_CFG2                    (0x114 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN5G_IDAC4                   (0x118 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_LOGEN5G_RCCR                    (0x119 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_DAC_CFG1                        (0x11c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_DAC_CFG2                        (0x11d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_DAC_BIAS                        (0x11e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CFG1                        (0x11f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CFG2                        (0x120 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CFG3                        (0x121 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CFG4                        (0x122 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CFG5                        (0x123 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_BIAS1                       (0x124 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_BIAS2                       (0x125 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_RC1                         (0x126 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_RC2                         (0x127 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE1                    (0x128 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE2                    (0x129 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE3                    (0x12a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE4                    (0x12b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE5                    (0x12c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE6                    (0x12d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE7                    (0x12e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE8                    (0x12f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE9                    (0x130 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE10                   (0x131 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE11                   (0x132 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE12                   (0x133 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE13                   (0x134 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE14                   (0x135 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE15                   (0x136 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE16                   (0x137 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE17                   (0x138 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE18                   (0x139 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE19                   (0x13a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE20                   (0x13b | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE21                   (0x13c | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE22                   (0x13d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE23                   (0x13e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE24                   (0x13f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE25                   (0x140 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE26                   (0x141 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE27                   (0x142 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_CALCODE28                   (0x143 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_ADC_STATUS                      (0x144 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_GPAIO_CFG1                      (0x147 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RADIO_SPARE1                    (0x157 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RADIO_SPARE2                    (0x158 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RADIO_SPARE3                    (0x159 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_RADIO_SPARE4                    (0x15a | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR1                            (0x15d | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR3                            (0x15f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR4                            (0x160 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR5                            (0x161 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR6                            (0x162 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR7                            (0x163 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR9                            (0x165 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR10                           (0x166 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR11                           (0x167 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR12                           (0x168 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR13                           (0x169 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR18                           (0x16e | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR19                           (0x16f | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR20                           (0x170 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR21                           (0x171 | JTAG_2069_REV0_CR1)
#define RF1_2069_REV0_OVR23                           (0x173 | JTAG_2069_REV0_CR1)
#define RF2_2069_REV0_REV_ID                          (0x0 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_DEV_ID                          (0x1 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_BG_CFG1                         (0x7 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_BG_CFG2                         (0x8 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_BG_TRIM1                        (0x9 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_BG_TRIM2                        (0xa | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCAL_CFG                        (0xb | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_VREG_CFG                        (0xc | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPO_CFG                         (0xd | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TEMPSENSE_CFG                   (0xe | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_VBAT_CFG                        (0xf | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_CFG                       (0x10 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_LOGIC1                    (0x11 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_TRC                       (0x12 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_LOGIC2                    (0x13 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_LOGIC3                    (0x14 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_LOGIC4                    (0x15 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RCCAL_LOGIC5                    (0x16 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TESTBUF_CFG1                    (0x17 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_GPAIO_SEL0                      (0x18 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_GPAIO_SEL1                      (0x19 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_IQCAL_CFG1                      (0x1a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_IQCAL_CFG2                      (0x1b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_IQCAL_CFG3                      (0x1c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_IQCAL_IDAC                      (0x1d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_TSSI                       (0x1e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX5G_TSSI                       (0x1f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXRX2G_CAL_TX                   (0x20 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXRX5G_CAL_TX                   (0x21 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXRX2G_CAL_RX                   (0x22 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXRX5G_CAL_RX                   (0x23 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_AUXPGA_CFG1                     (0x24 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_AUXPGA_VMID                     (0x25 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CORE_BIAS                       (0x26 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_CFG1                      (0x27 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_CFG2                      (0x28 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_TUNE                      (0x29 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_IDAC1                     (0x2a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_IDAC2                     (0x2b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_RSSI                      (0x2c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA2G_RSSI_ANA                  (0x2d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_CFG1                      (0x2e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_CFG2                      (0x2f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_TUNE                      (0x30 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_IDAC1                     (0x31 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_IDAC2                     (0x32 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_RSSI                      (0x33 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LNA5G_RSSI_ANA                  (0x34 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXMIX2G_CFG1                    (0x35 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXMIX2G_IDAC                    (0x36 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXMIX5G_CFG1                    (0x37 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXMIX5G_IDAC                    (0x38 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF2G_CFG1                     (0x39 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF2G_CFG2                     (0x3a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF2G_SPARE                    (0x3b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF5G_CFG1                     (0x3c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF5G_CFG2                     (0x3d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RX_REG_BACKUP_1                 (0x3e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RX_REG_BACKUP_2                 (0x3f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RXRF5G_SPARE                    (0x40 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TIA_CFG1                        (0x41 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TIA_CFG2                        (0x42 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TIA_CFG3                        (0x43 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TIA_IDAC1                       (0x44 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_NBRSSI_CONFG                    (0x45 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_NBRSSI_BIAS                     (0x46 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_NBRSSI_IB                       (0x47 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_NBRSSI_TEST                     (0x48 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_WRSSI3_CONFG                    (0x49 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_WRSSI3_BIAS                     (0x4a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_WRSSI3_TEST                     (0x4b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_MUX_SWITCHES                (0x4c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_ENABLES                     (0x4d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_MAIN_CONTROLS               (0x4e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_CORNER_FREQUENCY_TUNING     (0x4f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_DC_LOOP_AND_MISC            (0x50 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_BIAS_LEVELS_LOW             (0x51 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_BIAS_LEVELS_MID             (0x52 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_BIAS_LEVELS_HIGH            (0x53 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x54 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LPF_COMMON_MODE_REFERENCE_LEVELS (0x55 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX2G_CFG1                       (0x56 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX2G_CFG2                       (0x57 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX5G_CFG1                       (0x58 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_CFG1                       (0x59 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXGM_CFG2                       (0x5a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXMIX2G_CFG1                    (0x5b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TXMIX5G_CFG1                    (0x5c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA2G_CFG1                      (0x5d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA2G_CFG2                      (0x5e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA2G_IDAC                      (0x5f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA2G_INCAP                     (0x60 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA5G_CFG1                      (0x61 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA5G_CFG2                      (0x62 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA5G_IDAC                      (0x63 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PGA5G_INCAP                     (0x64 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD2G_CFG1                      (0x65 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD2G_IDAC                      (0x66 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD2G_SLOPE                     (0x67 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD2G_TUNE                      (0x68 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD2G_INCAP                     (0x69 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD5G_CFG1                      (0x6a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD5G_IDAC                      (0x6b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD5G_SLOPE                     (0x6c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD5G_TUNE                      (0x6d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PAD5G_INCAP                     (0x6e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_CFG1                       (0x6f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_CFG2                       (0x70 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_CFG3                       (0x71 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_IDAC1                      (0x72 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_IDAC2                      (0x73 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA2G_INCAP                      (0x74 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_CFG1                       (0x75 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_CFG2                       (0x76 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_CFG3                       (0x77 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_IDAC1                      (0x78 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_IDAC2                      (0x79 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_PA5G_INCAP                      (0x7a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW2G_CFG1                     (0x7b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW2G_CFG2                     (0x7c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW2G_CFG3                     (0x7d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW5G_CFG1                     (0x7e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW5G_CFG2                     (0x7f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TRSW5G_CFG3                     (0x80 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX_REG_BACKUP_1                 (0x81 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX_REG_BACKUP_2                 (0x82 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX_REG_BACKUP_3                 (0x83 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_TX_REG_BACKUP_4                 (0x84 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN2G_CFG2                    (0x10e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN2G_IDAC3                   (0x110 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN2G_RCCR                    (0x111 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN5G_CFG2                    (0x114 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN5G_IDAC4                   (0x118 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_LOGEN5G_RCCR                    (0x119 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_DAC_CFG1                        (0x11c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_DAC_CFG2                        (0x11d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_DAC_BIAS                        (0x11e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CFG1                        (0x11f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CFG2                        (0x120 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CFG3                        (0x121 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CFG4                        (0x122 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CFG5                        (0x123 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_BIAS1                       (0x124 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_BIAS2                       (0x125 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_RC1                         (0x126 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_RC2                         (0x127 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE1                    (0x128 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE2                    (0x129 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE3                    (0x12a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE4                    (0x12b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE5                    (0x12c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE6                    (0x12d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE7                    (0x12e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE8                    (0x12f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE9                    (0x130 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE10                   (0x131 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE11                   (0x132 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE12                   (0x133 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE13                   (0x134 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE14                   (0x135 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE15                   (0x136 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE16                   (0x137 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE17                   (0x138 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE18                   (0x139 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE19                   (0x13a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE20                   (0x13b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE21                   (0x13c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE22                   (0x13d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE23                   (0x13e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE24                   (0x13f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE25                   (0x140 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE26                   (0x141 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE27                   (0x142 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_CALCODE28                   (0x143 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_ADC_STATUS                      (0x144 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_GPAIO_CFG1                      (0x147 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CGPAIO_CFG1                     (0x148 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CGPAIO_CFG2                     (0x149 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CGPAIO_CFG3                     (0x14a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CGPAIO_CFG4                     (0x14b | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_CGPAIO_CFG5                     (0x14c | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RADIO_SPARE1                    (0x157 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RADIO_SPARE2                    (0x158 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RADIO_SPARE3                    (0x159 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_RADIO_SPARE4                    (0x15a | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR1                            (0x15d | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR2                            (0x15e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR3                            (0x15f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR4                            (0x160 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR5                            (0x161 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR6                            (0x162 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR7                            (0x163 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR9                            (0x165 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR10                           (0x166 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR11                           (0x167 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR12                           (0x168 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR13                           (0x169 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR18                           (0x16e | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR19                           (0x16f | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR20                           (0x170 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR21                           (0x171 | JTAG_2069_REV0_CR2)
#define RF2_2069_REV0_OVR23                           (0x173 | JTAG_2069_REV0_CR2)
#define RFP_2069_REV0_REV_ID                          (0x0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_DEV_ID                          (0x1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTALLDO1                    (0x85 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_HVLDO1                      (0x86 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_HVLDO2                      (0x87 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_HVLDO3                      (0x88 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC1                        (0x89 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC2                        (0x8a | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC3                        (0x8b | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC4                        (0x8c | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC5                        (0x8d | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_ADC6                        (0x8e | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CP1                         (0x8f | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CP2                         (0x90 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CP3                         (0x91 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CP4                         (0x92 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CP5                         (0x93 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP1                        (0x94 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP2                        (0x95 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP3                        (0x96 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP4                        (0x97 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP5                        (0x98 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP6                        (0x99 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP7                        (0x9a | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP8                        (0x9b | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP9                        (0x9c | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP10                       (0x9d | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP11                       (0x9e | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP12                       (0x9f | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP13                       (0xa0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP14                       (0xa1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP15                       (0xa2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP16                       (0xa3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP17                       (0xa4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP18                       (0xa5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP19                       (0xa6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP20                       (0xa7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP21                       (0xa8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP22                       (0xa9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP23                       (0xaa | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP24                       (0xab | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP25                       (0xac | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP26                       (0xad | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP27                       (0xae | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP28                       (0xaf | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP29                       (0xb0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP30                       (0xb1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP31                       (0xb2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP32                       (0xb3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP33                       (0xb4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP34                       (0xb5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP35                       (0xb6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP36                       (0xb7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP37                       (0xb8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP38                       (0xb9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP39                       (0xba | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP40                       (0xbb | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP41                       (0xbc | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP42                       (0xbd | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP43                       (0xbe | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP44                       (0xbf | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP45                       (0xc0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP46                       (0xc1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSP47                       (0xc2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_FRCT1                       (0xc3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_FRCT2                       (0xc4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_FRCT3                       (0xc5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF1                         (0xc6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF2                         (0xc7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF3                         (0xc8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF4                         (0xc9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF5                         (0xca | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF6                         (0xcb | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_LF7                         (0xcc | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_MMD1                        (0xcd | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_MONITOR1                    (0xce | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG1                        (0xcf | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG2                        (0xd0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG3                        (0xd1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG4                        (0xd2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG5                        (0xd3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFG6                        (0xd4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO1                        (0xd5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO2                        (0xd6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO3                        (0xd7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO4                        (0xd8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO5                        (0xd9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO6                        (0xda | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCO8                        (0xdb | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL1                     (0xdc | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL2                     (0xdd | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL3                     (0xde | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL4                     (0xdf | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL5                     (0xe0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL6                     (0xe1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL7                     (0xe2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL8                     (0xe3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL9                     (0xe4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL10                    (0xe5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL11                    (0xe6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL12                    (0xe7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL13                    (0xe8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTAL1                       (0xe9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTAL2                       (0xea | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTAL3                       (0xeb | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTAL4                       (0xec | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_XTAL5                       (0xed | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR1                       (0xee | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR2                       (0xef | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR3                       (0xf0 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR4                       (0xf1 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR5                       (0xf2 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR6                       (0xf3 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR7                       (0xf4 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR8                       (0xf5 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR9                       (0xf6 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR10                      (0xf7 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR11                      (0xf8 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR12                      (0xf9 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR13                      (0xfa | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR14                      (0xfb | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR15                      (0xfc | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR16                      (0xfd | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR17                      (0xfe | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR18                      (0xff | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR19                      (0x100 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR20                      (0x101 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR21                      (0x102 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR22                      (0x103 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR23                      (0x104 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR24                      (0x105 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR25                      (0x106 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR26                      (0x107 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_DSPR27                      (0x108 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFGR1                       (0x109 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_CFGR2                       (0x10a | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL14                    (0x10b | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_PLL_VCOCAL15                    (0x10c | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE1                      (0x14d | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE2                      (0x14e | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE3                      (0x14f | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE4                      (0x150 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE5                      (0x151 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE6                      (0x152 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE7                      (0x153 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE8                      (0x154 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE9                      (0x155 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_TOP_SPARE10                     (0x156 | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_OVR1                            (0x15d | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_OVR14                           (0x16a | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_OVR15                           (0x16b | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_OVR16                           (0x16c | JTAG_2069_REV0_PLL)
#define RFP_2069_REV0_OVR22                           (0x172 | JTAG_2069_REV0_PLL)
/* 2069 broadcast registers */
#define RFX_2069_REV0_REV_ID                          (0x0 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_DEV_ID                          (0x1 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TEMPSENSE_CFG                   (0xe | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_VBAT_CFG                        (0xf | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TESTBUF_CFG1                    (0x17 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_GPAIO_SEL0                      (0x18 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_GPAIO_SEL1                      (0x19 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_IQCAL_CFG1                      (0x1a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_IQCAL_CFG2                      (0x1b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_IQCAL_CFG3                      (0x1c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_IQCAL_IDAC                      (0x1d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_TSSI                       (0x1e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX5G_TSSI                       (0x1f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXRX2G_CAL_TX                   (0x20 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXRX5G_CAL_TX                   (0x21 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXRX2G_CAL_RX                   (0x22 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXRX5G_CAL_RX                   (0x23 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_AUXPGA_CFG1                     (0x24 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_AUXPGA_VMID                     (0x25 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_CORE_BIAS                       (0x26 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_CFG1                      (0x27 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_CFG2                      (0x28 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_TUNE                      (0x29 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_IDAC1                     (0x2a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_IDAC2                     (0x2b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_RSSI                      (0x2c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA2G_RSSI_ANA                  (0x2d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_CFG1                      (0x2e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_CFG2                      (0x2f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_TUNE                      (0x30 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_IDAC1                     (0x31 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_IDAC2                     (0x32 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_RSSI                      (0x33 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LNA5G_RSSI_ANA                  (0x34 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXMIX2G_CFG1                    (0x35 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXMIX2G_IDAC                    (0x36 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXMIX5G_CFG1                    (0x37 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXMIX5G_IDAC                    (0x38 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF2G_CFG1                     (0x39 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF2G_CFG2                     (0x3a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF2G_SPARE                    (0x3b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF5G_CFG1                     (0x3c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF5G_CFG2                     (0x3d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RX_REG_BACKUP_1                 (0x3e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RX_REG_BACKUP_2                 (0x3f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RXRF5G_SPARE                    (0x40 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TIA_CFG1                        (0x41 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TIA_CFG2                        (0x42 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TIA_CFG3                        (0x43 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TIA_IDAC1                       (0x44 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_NBRSSI_CONFG                    (0x45 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_NBRSSI_BIAS                     (0x46 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_NBRSSI_IB                       (0x47 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_NBRSSI_TEST                     (0x48 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_WRSSI3_CONFG                    (0x49 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_WRSSI3_BIAS                     (0x4a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_WRSSI3_TEST                     (0x4b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_MUX_SWITCHES                (0x4c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_ENABLES                     (0x4d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_MAIN_CONTROLS               (0x4e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_CORNER_FREQUENCY_TUNING     (0x4f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_DC_LOOP_AND_MISC            (0x50 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_BIAS_LEVELS_LOW             (0x51 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_BIAS_LEVELS_MID             (0x52 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_BIAS_LEVELS_HIGH            (0x53 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x54 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LPF_COMMON_MODE_REFERENCE_LEVELS (0x55 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX2G_CFG1                       (0x56 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX2G_CFG2                       (0x57 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX5G_CFG1                       (0x58 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_CFG1                       (0x59 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXGM_CFG2                       (0x5a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXMIX2G_CFG1                    (0x5b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TXMIX5G_CFG1                    (0x5c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA2G_CFG1                      (0x5d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA2G_CFG2                      (0x5e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA2G_IDAC                      (0x5f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA2G_INCAP                     (0x60 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA5G_CFG1                      (0x61 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA5G_CFG2                      (0x62 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA5G_IDAC                      (0x63 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PGA5G_INCAP                     (0x64 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD2G_CFG1                      (0x65 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD2G_IDAC                      (0x66 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD2G_SLOPE                     (0x67 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD2G_TUNE                      (0x68 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD2G_INCAP                     (0x69 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD5G_CFG1                      (0x6a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD5G_IDAC                      (0x6b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD5G_SLOPE                     (0x6c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD5G_TUNE                      (0x6d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PAD5G_INCAP                     (0x6e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_CFG1                       (0x6f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_CFG2                       (0x70 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_CFG3                       (0x71 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_IDAC1                      (0x72 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_IDAC2                      (0x73 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA2G_INCAP                      (0x74 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_CFG1                       (0x75 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_CFG2                       (0x76 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_CFG3                       (0x77 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_IDAC1                      (0x78 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_IDAC2                      (0x79 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_PA5G_INCAP                      (0x7a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW2G_CFG1                     (0x7b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW2G_CFG2                     (0x7c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW2G_CFG3                     (0x7d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW5G_CFG1                     (0x7e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW5G_CFG2                     (0x7f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TRSW5G_CFG3                     (0x80 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX_REG_BACKUP_1                 (0x81 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX_REG_BACKUP_2                 (0x82 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX_REG_BACKUP_3                 (0x83 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_TX_REG_BACKUP_4                 (0x84 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN2G_CFG2                    (0x10e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN2G_IDAC3                   (0x110 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN2G_RCCR                    (0x111 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN5G_CFG2                    (0x114 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN5G_IDAC4                   (0x118 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_LOGEN5G_RCCR                    (0x119 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_DAC_CFG1                        (0x11c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_DAC_CFG2                        (0x11d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_DAC_BIAS                        (0x11e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CFG1                        (0x11f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CFG2                        (0x120 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CFG3                        (0x121 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CFG4                        (0x122 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CFG5                        (0x123 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_BIAS1                       (0x124 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_BIAS2                       (0x125 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_RC1                         (0x126 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_RC2                         (0x127 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE1                    (0x128 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE2                    (0x129 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE3                    (0x12a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE4                    (0x12b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE5                    (0x12c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE6                    (0x12d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE7                    (0x12e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE8                    (0x12f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE9                    (0x130 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE10                   (0x131 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE11                   (0x132 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE12                   (0x133 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE13                   (0x134 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE14                   (0x135 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE15                   (0x136 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE16                   (0x137 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE17                   (0x138 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE18                   (0x139 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE19                   (0x13a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE20                   (0x13b | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE21                   (0x13c | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE22                   (0x13d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE23                   (0x13e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE24                   (0x13f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE25                   (0x140 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE26                   (0x141 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE27                   (0x142 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_CALCODE28                   (0x143 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_ADC_STATUS                      (0x144 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_GPAIO_CFG1                      (0x147 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RADIO_SPARE1                    (0x157 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RADIO_SPARE2                    (0x158 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RADIO_SPARE3                    (0x159 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_RADIO_SPARE4                    (0x15a | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR1                            (0x15d | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR3                            (0x15f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR4                            (0x160 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR5                            (0x161 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR6                            (0x162 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR7                            (0x163 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR9                            (0x165 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR10                           (0x166 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR11                           (0x167 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR12                           (0x168 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR13                           (0x169 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR18                           (0x16e | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR19                           (0x16f | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR20                           (0x170 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR21                           (0x171 | JTAG_2069_REV0_ALL)
#define RFX_2069_REV0_OVR23                           (0x173 | JTAG_2069_REV0_ALL)

/* Common defines */
/* 2069 JTAG core selects */
#define JTAG_2069_CR0                            (0x0 << JTAG_2069_SHIFT)
#define JTAG_2069_CR1                            (0x1 << JTAG_2069_SHIFT)
#define JTAG_2069_CR2                            (0x2 << JTAG_2069_SHIFT)
#define JTAG_2069_ALL                            (0x3 << JTAG_2069_SHIFT)
#define JTAG_2069_PLL                            (0x4 << JTAG_2069_SHIFT)
#define JTAG_2069_MASK                           (0x7 << JTAG_2069_SHIFT)

#define RF0_2069_REV_ID		(((ACREV0) ? 0x0 : 0x0) | JTAG_2069_CR0)
#define RF0_2069_DEV_ID		(((ACREV0) ? 0x1 : 0x1) | JTAG_2069_CR0)
#define RF0_2069_TXGM_LOFT_FINE_I		(((ACREV0) ? 0x2 : 0x2) | JTAG_2069_CR0)
#define RF0_2069_TXGM_LOFT_FINE_Q		(((ACREV0) ? 0x3 : 0x3) | JTAG_2069_CR0)
#define RF0_2069_TXGM_LOFT_COARSE_I		(((ACREV0) ? 0x4 : 0x4) | JTAG_2069_CR0)
#define RF0_2069_TXGM_LOFT_COARSE_Q		(((ACREV0) ? 0x5 : 0x5) | JTAG_2069_CR0)
#define RF0_2069_TXGM_LOFT_SCALE		(((ACREV0) ? 0x6 : 0x6) | JTAG_2069_CR0)
#define RF0_2069_TEMPSENSE_CFG		(((ACREV0) ? 0xe : 0x13) | JTAG_2069_CR0)
#define RF0_2069_VBAT_CFG		(((ACREV0) ? 0xf : 0x14) | JTAG_2069_CR0)
#define RF0_2069_TESTBUF_CFG1		(((ACREV0) ? 0x17 : 0x1c) | JTAG_2069_CR0)
#define RF0_2069_GPAIO_SEL0		(((ACREV0) ? 0x18 : 0x1d) | JTAG_2069_CR0)
#define RF0_2069_GPAIO_SEL1		(((ACREV0) ? 0x19 : 0x1e) | JTAG_2069_CR0)
#define RF0_2069_IQCAL_CFG1		(((ACREV0) ? 0x1a : 0x1f) | JTAG_2069_CR0)
#define RF0_2069_IQCAL_CFG2		(((ACREV0) ? 0x1b : 0x20) | JTAG_2069_CR0)
#define RF0_2069_IQCAL_CFG3		(((ACREV0) ? 0x1c : 0x21) | JTAG_2069_CR0)
#define RF0_2069_IQCAL_IDAC		(((ACREV0) ? 0x1d : 0x22) | JTAG_2069_CR0)
#define RF0_2069_PA2G_TSSI		(((ACREV0) ? 0x1e : 0x23) | JTAG_2069_CR0)
#define RF0_2069_TX5G_TSSI		(((ACREV0) ? 0x1f : 0x24) | JTAG_2069_CR0)
#define RF0_2069_TXRX2G_CAL_TX		(((ACREV0) ? 0x20 : 0x25) | JTAG_2069_CR0)
#define RF0_2069_TXRX5G_CAL_TX		(((ACREV0) ? 0x21 : 0x26) | JTAG_2069_CR0)
#define RF0_2069_TXRX2G_CAL_RX		(((ACREV0) ? 0x22 : 0x27) | JTAG_2069_CR0)
#define RF0_2069_TXRX5G_CAL_RX		(((ACREV0) ? 0x23 : 0x28) | JTAG_2069_CR0)
#define RF0_2069_AUXPGA_CFG1		(((ACREV0) ? 0x24 : 0x29) | JTAG_2069_CR0)
#define RF0_2069_AUXPGA_VMID		(((ACREV0) ? 0x25 : 0x2a) | JTAG_2069_CR0)
#define RF0_2069_CORE_BIAS		(((ACREV0) ? 0x26 : 0x2b) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_CFG1		(((ACREV0) ? 0x27 : 0x2c) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_CFG2		(((ACREV0) ? 0x28 : 0x2d) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_TUNE		(((ACREV0) ? 0x29 : 0x2f) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_IDAC1		(((ACREV0) ? 0x2a : 0x30) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_IDAC2		(((ACREV0) ? 0x2b : 0x31) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_RSSI		(((ACREV0) ? 0x2c : 0x32) | JTAG_2069_CR0)
#define RF0_2069_LNA2G_RSSI_ANA		(((ACREV0) ? 0x2d : 0x33) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_CFG1		(((ACREV0) ? 0x2e : 0x34) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_CFG2		(((ACREV0) ? 0x2f : 0x35) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_TUNE		(((ACREV0) ? 0x30 : 0x37) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_IDAC1		(((ACREV0) ? 0x31 : 0x38) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_IDAC2		(((ACREV0) ? 0x32 : 0x39) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_RSSI		(((ACREV0) ? 0x33 : 0x3a) | JTAG_2069_CR0)
#define RF0_2069_LNA5G_RSSI_ANA		(((ACREV0) ? 0x34 : 0x3b) | JTAG_2069_CR0)
#define RF0_2069_RXMIX2G_CFG1		(((ACREV0) ? 0x35 : 0x3c) | JTAG_2069_CR0)
#define RF0_2069_RXMIX2G_IDAC		(((ACREV0) ? 0x36 : 0x3d) | JTAG_2069_CR0)
#define RF0_2069_RXMIX5G_CFG1		(((ACREV0) ? 0x37 : 0x3e) | JTAG_2069_CR0)
#define RF0_2069_RXMIX5G_IDAC		(((ACREV0) ? 0x38 : 0x3f) | JTAG_2069_CR0)
#define RF0_2069_RXRF2G_CFG1		(((ACREV0) ? 0x39 : 0x40) | JTAG_2069_CR0)
#define RF0_2069_RXRF2G_CFG2		(((ACREV0) ? 0x3a : 0x41) | JTAG_2069_CR0)
#define RF0_2069_RXRF2G_SPARE		(((ACREV0) ? 0x3b : 0x42) | JTAG_2069_CR0)
#define RF0_2069_RXRF5G_CFG1		(((ACREV0) ? 0x3c : 0x43) | JTAG_2069_CR0)
#define RF0_2069_RXRF5G_CFG2		(((ACREV0) ? 0x3d : 0x44) | JTAG_2069_CR0)
#define RF0_2069_RX_REG_BACKUP_1		(((ACREV0) ? 0x3e : 0x45) | JTAG_2069_CR0)
#define RF0_2069_RX_REG_BACKUP_2		(((ACREV0) ? 0x3f : 0x46) | JTAG_2069_CR0)
#define RF0_2069_RXRF5G_SPARE		(((ACREV0) ? 0x40 : 0x47) | JTAG_2069_CR0)
#define RF0_2069_TIA_CFG1		(((ACREV0) ? 0x41 : 0x48) | JTAG_2069_CR0)
#define RF0_2069_TIA_CFG2		(((ACREV0) ? 0x42 : 0x49) | JTAG_2069_CR0)
#define RF0_2069_TIA_CFG3		(((ACREV0) ? 0x43 : 0x4a) | JTAG_2069_CR0)
#define RF0_2069_TIA_IDAC1		(((ACREV0) ? 0x44 : 0x4b) | JTAG_2069_CR0)
#define RF0_2069_NBRSSI_CONFG		(((ACREV0) ? 0x45 : 0x4c) | JTAG_2069_CR0)
#define RF0_2069_NBRSSI_BIAS		(((ACREV0) ? 0x46 : 0x4d) | JTAG_2069_CR0)
#define RF0_2069_NBRSSI_IB		(((ACREV0) ? 0x47 : 0x4e) | JTAG_2069_CR0)
#define RF0_2069_NBRSSI_TEST		(((ACREV0) ? 0x48 : 0x4f) | JTAG_2069_CR0)
#define RF0_2069_WRSSI3_CONFG		(((ACREV0) ? 0x49 : 0x50) | JTAG_2069_CR0)
#define RF0_2069_WRSSI3_BIAS		(((ACREV0) ? 0x4a : 0x51) | JTAG_2069_CR0)
#define RF0_2069_WRSSI3_TEST		(((ACREV0) ? 0x4b : 0x52) | JTAG_2069_CR0)
#define RF0_2069_LPF_MUX_SWITCHES		(((ACREV0) ? 0x4c : 0x53) | JTAG_2069_CR0)
#define RF0_2069_LPF_ENABLES		(((ACREV0) ? 0x4d : 0x54) | JTAG_2069_CR0)
#define RF0_2069_LPF_MAIN_CONTROLS		(((ACREV0) ? 0x4e : 0x55) | JTAG_2069_CR0)
#define RF0_2069_LPF_CORNER_FREQUENCY_TUNING		(((ACREV0) ? 0x4f : 0x56) | JTAG_2069_CR0)
#define RF0_2069_LPF_DC_LOOP_AND_MISC		(((ACREV0) ? 0x50 : 0x57) | JTAG_2069_CR0)
#define RF0_2069_LPF_BIAS_LEVELS_LOW		(((ACREV0) ? 0x51 : 0x58) | JTAG_2069_CR0)
#define RF0_2069_LPF_BIAS_LEVELS_MID		(((ACREV0) ? 0x52 : 0x59) | JTAG_2069_CR0)
#define RF0_2069_LPF_BIAS_LEVELS_HIGH		(((ACREV0) ? 0x53 : 0x5a) | JTAG_2069_CR0)
#define RF0_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS		\
	(((ACREV0) ? 0x54 : 0x5b) | JTAG_2069_CR0)
#define RF0_2069_LPF_COMMON_MODE_REFERENCE_LEVELS		\
	(((ACREV0) ? 0x55 : 0x5c) | JTAG_2069_CR0)
#define RF0_2069_TX2G_CFG1		(((ACREV0) ? 0x56 : 0x5d) | JTAG_2069_CR0)
#define RF0_2069_TX2G_CFG2		(((ACREV0) ? 0x57 : 0x5e) | JTAG_2069_CR0)
#define RF0_2069_TX5G_CFG1		(((ACREV0) ? 0x58 : 0x5f) | JTAG_2069_CR0)
#define RF0_2069_TXGM_CFG1		(((ACREV0) ? 0x59 : 0x60) | JTAG_2069_CR0)
#define RF0_2069_TXGM_CFG2		(((ACREV0) ? 0x5a : 0x61) | JTAG_2069_CR0)
#define RF0_2069_TXMIX2G_CFG1		(((ACREV0) ? 0x5b : 0x62) | JTAG_2069_CR0)
#define RF0_2069_TXMIX5G_CFG1		(((ACREV0) ? 0x5c : 0x63) | JTAG_2069_CR0)
#define RF0_2069_PGA2G_CFG1		(((ACREV0) ? 0x5d : 0x64) | JTAG_2069_CR0)
#define RF0_2069_PGA2G_CFG2		(((ACREV0) ? 0x5e : 0x65) | JTAG_2069_CR0)
#define RF0_2069_PGA2G_IDAC		(((ACREV0) ? 0x5f : 0x66) | JTAG_2069_CR0)
#define RF0_2069_PGA2G_INCAP		(((ACREV0) ? 0x60 : 0x67) | JTAG_2069_CR0)
#define RF0_2069_PGA5G_CFG1		(((ACREV0) ? 0x61 : 0x68) | JTAG_2069_CR0)
#define RF0_2069_PGA5G_CFG2		(((ACREV0) ? 0x62 : 0x69) | JTAG_2069_CR0)
#define RF0_2069_PGA5G_IDAC		(((ACREV0) ? 0x63 : 0x6a) | JTAG_2069_CR0)
#define RF0_2069_PGA5G_INCAP		(((ACREV0) ? 0x64 : 0x6b) | JTAG_2069_CR0)
#define RF0_2069_PAD2G_CFG1		(((ACREV0) ? 0x65 : 0x6c) | JTAG_2069_CR0)
#define RF0_2069_PAD2G_IDAC		(((ACREV0) ? 0x66 : 0x6d) | JTAG_2069_CR0)
#define RF0_2069_PAD2G_SLOPE		(((ACREV0) ? 0x67 : 0x6e) | JTAG_2069_CR0)
#define RF0_2069_PAD2G_TUNE		(((ACREV0) ? 0x68 : 0x6f) | JTAG_2069_CR0)
#define RF0_2069_PAD2G_INCAP		(((ACREV0) ? 0x69 : 0x70) | JTAG_2069_CR0)
#define RF0_2069_PAD5G_CFG1		(((ACREV0) ? 0x6a : 0x71) | JTAG_2069_CR0)
#define RF0_2069_PAD5G_IDAC		(((ACREV0) ? 0x6b : 0x72) | JTAG_2069_CR0)
#define RF0_2069_PAD5G_SLOPE		(((ACREV0) ? 0x6c : 0x73) | JTAG_2069_CR0)
#define RF0_2069_PAD5G_TUNE		(((ACREV0) ? 0x6d : 0x74) | JTAG_2069_CR0)
#define RF0_2069_PAD5G_INCAP		(((ACREV0) ? 0x6e : 0x75) | JTAG_2069_CR0)
#define RF0_2069_PA2G_CFG1		(((ACREV0) ? 0x6f : 0x76) | JTAG_2069_CR0)
#define RF0_2069_PA2G_CFG2		(((ACREV0) ? 0x70 : 0x77) | JTAG_2069_CR0)
#define RF0_2069_PA2G_CFG3		(((ACREV0) ? 0x71 : 0x78) | JTAG_2069_CR0)
#define RF0_2069_PA2G_IDAC1		(((ACREV0) ? 0x72 : 0x79) | JTAG_2069_CR0)
#define RF0_2069_PA2G_IDAC2		(((ACREV0) ? 0x73 : 0x7a) | JTAG_2069_CR0)
#define RF0_2069_PA2G_INCAP		(((ACREV0) ? 0x74 : 0x7b) | JTAG_2069_CR0)
#define RF0_2069_PA5G_CFG1		(((ACREV0) ? 0x75 : 0x7c) | JTAG_2069_CR0)
#define RF0_2069_PA5G_CFG2		(((ACREV0) ? 0x76 : 0x7d) | JTAG_2069_CR0)
#define RF0_2069_PA5G_CFG3		(((ACREV0) ? 0x77 : 0x7e) | JTAG_2069_CR0)
#define RF0_2069_PA5G_IDAC1		(((ACREV0) ? 0x78 : 0x7f) | JTAG_2069_CR0)
#define RF0_2069_PA5G_IDAC2		(((ACREV0) ? 0x79 : 0x80) | JTAG_2069_CR0)
#define RF0_2069_PA5G_INCAP		(((ACREV0) ? 0x7a : 0x81) | JTAG_2069_CR0)
#define RF0_2069_TRSW2G_CFG1		(((ACREV0) ? 0x7b : 0x82) | JTAG_2069_CR0)
#define RF0_2069_TRSW2G_CFG2		(((ACREV0) ? 0x7c : 0x83) | JTAG_2069_CR0)
#define RF0_2069_TRSW2G_CFG3		(((ACREV0) ? 0x7d : 0x84) | JTAG_2069_CR0)
#define RF0_2069_TRSW5G_CFG1		(((ACREV0) ? 0x7e : 0x85) | JTAG_2069_CR0)
#define RF0_2069_TRSW5G_CFG2		(((ACREV0) ? 0x7f : 0x86) | JTAG_2069_CR0)
#define RF0_2069_TRSW5G_CFG3		(((ACREV0) ? 0x80 : 0x87) | JTAG_2069_CR0)
#define RF0_2069_TX_REG_BACKUP_1		(((ACREV0) ? 0x81 : 0x88) | JTAG_2069_CR0)
#define RF0_2069_TX_REG_BACKUP_2		(((ACREV0) ? 0x82 : 0x89) | JTAG_2069_CR0)
#define RF0_2069_TX_REG_BACKUP_3		(((ACREV0) ? 0x83 : 0x8a) | JTAG_2069_CR0)
#define RF0_2069_TX_REG_BACKUP_4		(((ACREV0) ? 0x84 : 0x8b) | JTAG_2069_CR0)
#define RF0_2069_LOGEN2G_CFG1		\
	(((ACREV0) ? 0x10d : 0x11f) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN2G_CFG2		(((ACREV0) ? 0x10e : 0x120) | JTAG_2069_CR0)
#define RF0_2069_LOGEN2G_IDAC2		\
	(((ACREV0) ? 0x10f : 0x121) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN2G_IDAC3		(((ACREV0) ? 0x110 : 0x122) | JTAG_2069_CR0)
#define RF0_2069_LOGEN2G_RCCR		(((ACREV0) ? 0x111 : 0x123) | JTAG_2069_CR0)
#define RF0_2069_LOGEN2G_TUNE		\
	(((ACREV0) ? 0x112 : 0x124) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_CFG1		\
	(((ACREV0) ? 0x113 : 0x125) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_CFG2		(((ACREV0) ? 0x114 : 0x126) | JTAG_2069_CR0)
#define RF0_2069_LOGEN5G_IDAC1		\
	(((ACREV0) ? 0x115 : 0x127) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_IDAC2		\
	(((ACREV0) ? 0x116 : 0x128) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_IDAC3		\
	(((ACREV0) ? 0x117 : 0x129) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_IDAC4		(((ACREV0) ? 0x118 : 0x12a) | JTAG_2069_CR0)
#define RF0_2069_LOGEN5G_RCCR		(((ACREV0) ? 0x119 : 0x12b) | JTAG_2069_CR0)
#define RF0_2069_LOGEN5G_TUNE1		\
	(((ACREV0) ? 0x11a : 0x12c) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_LOGEN5G_TUNE2		\
	(((ACREV0) ? 0x11b : 0x12d) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_DAC_CFG1		(((ACREV0) ? 0x11c : 0x12e) | JTAG_2069_CR0)
#define RF0_2069_DAC_CFG2		(((ACREV0) ? 0x11d : 0x12f) | JTAG_2069_CR0)
#define RF0_2069_DAC_BIAS		(((ACREV0) ? 0x11e : 0x130) | JTAG_2069_CR0)
#define RF0_2069_ADC_CFG1		(((ACREV0) ? 0x11f : 0x131) | JTAG_2069_CR0)
#define RF0_2069_ADC_CFG2		(((ACREV0) ? 0x120 : 0x132) | JTAG_2069_CR0)
#define RF0_2069_ADC_CFG3		(((ACREV0) ? 0x121 : 0x133) | JTAG_2069_CR0)
#define RF0_2069_ADC_CFG4		(((ACREV0) ? 0x122 : 0x134) | JTAG_2069_CR0)
#define RF0_2069_ADC_CFG5		(((ACREV0) ? 0x123 : 0x135) | JTAG_2069_CR0)
#define RF0_2069_ADC_BIAS1		(((ACREV0) ? 0x124 : 0x136) | JTAG_2069_CR0)
#define RF0_2069_ADC_BIAS2		(((ACREV0) ? 0x125 : 0x137) | JTAG_2069_CR0)
#define RF0_2069_ADC_RC1		(((ACREV0) ? 0x126 : 0x138) | JTAG_2069_CR0)
#define RF0_2069_ADC_RC2		(((ACREV0) ? 0x127 : 0x139) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE1		(((ACREV0) ? 0x128 : 0x13a) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE2		(((ACREV0) ? 0x129 : 0x13b) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE3		(((ACREV0) ? 0x12a : 0x13c) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE4		(((ACREV0) ? 0x12b : 0x13d) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE5		(((ACREV0) ? 0x12c : 0x13e) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE6		(((ACREV0) ? 0x12d : 0x13f) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE7		(((ACREV0) ? 0x12e : 0x140) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE8		(((ACREV0) ? 0x12f : 0x141) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE9		(((ACREV0) ? 0x130 : 0x142) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE10		(((ACREV0) ? 0x131 : 0x143) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE11		(((ACREV0) ? 0x132 : 0x144) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE12		(((ACREV0) ? 0x133 : 0x145) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE13		(((ACREV0) ? 0x134 : 0x146) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE14		(((ACREV0) ? 0x135 : 0x147) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE15		(((ACREV0) ? 0x136 : 0x148) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE16		(((ACREV0) ? 0x137 : 0x149) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE17		(((ACREV0) ? 0x138 : 0x14a) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE18		(((ACREV0) ? 0x139 : 0x14b) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE19		(((ACREV0) ? 0x13a : 0x14c) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE20		(((ACREV0) ? 0x13b : 0x14d) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE21		(((ACREV0) ? 0x13c : 0x14e) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE22		(((ACREV0) ? 0x13d : 0x14f) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE23		(((ACREV0) ? 0x13e : 0x150) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE24		(((ACREV0) ? 0x13f : 0x151) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE25		(((ACREV0) ? 0x140 : 0x152) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE26		(((ACREV0) ? 0x141 : 0x153) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE27		(((ACREV0) ? 0x142 : 0x154) | JTAG_2069_CR0)
#define RF0_2069_ADC_CALCODE28		(((ACREV0) ? 0x143 : 0x155) | JTAG_2069_CR0)
#define RF0_2069_ADC_STATUS		(((ACREV0) ? 0x144 : 0x156) | JTAG_2069_CR0)
#define RF0_2069_AFEDIV1		\
	(((ACREV0) ? 0x145 : 0x157) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_AFEDIV2		\
	(((ACREV0) ? 0x146 : 0x158) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_GPAIO_CFG1		(((ACREV0) ? 0x147 : 0x15a) | JTAG_2069_CR0)
#define RF0_2069_RADIO_SPARE1		(((ACREV0) ? 0x157 : 0x16a) | JTAG_2069_CR0)
#define RF0_2069_RADIO_SPARE2		(((ACREV0) ? 0x158 : 0x16b) | JTAG_2069_CR0)
#define RF0_2069_RADIO_SPARE3		(((ACREV0) ? 0x159 : 0x16c) | JTAG_2069_CR0)
#define RF0_2069_RADIO_SPARE4		(((ACREV0) ? 0x15a : 0x16d) | JTAG_2069_CR0)
#define RF0_2069_CLB_REG_0		(((ACREV0) ? 0x15b : 0x16e) | JTAG_2069_CR0)
#define RF0_2069_CLB_REG_1		(((ACREV0) ? 0x15c : 0x16f) | JTAG_2069_CR0)
#define RF0_2069_OVR1		\
	(((ACREV0) ? 0x15d : 0x170) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_OVR3		(((ACREV0) ? 0x15f : 0x172) | JTAG_2069_CR0)
#define RF0_2069_OVR4		(((ACREV0) ? 0x160 : 0x173) | JTAG_2069_CR0)
#define RF0_2069_OVR5		(((ACREV0) ? 0x161 : 0x174) | JTAG_2069_CR0)
#define RF0_2069_OVR6		(((ACREV0) ? 0x162 : 0x175) | JTAG_2069_CR0)
#define RF0_2069_OVR7		(((ACREV0) ? 0x163 : 0x176) | JTAG_2069_CR0)
#define RF0_2069_OVR8		\
	(((ACREV0) ? 0x164 : 0x177) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_OVR9		\
	(((ACREV0) ? 0x165 : 0x178) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_OVR10		(((ACREV0) ? 0x166 : 0x179) | JTAG_2069_CR0)
#define RF0_2069_OVR11		(((ACREV0) ? 0x167 : 0x17a) | JTAG_2069_CR0)
#define RF0_2069_OVR12		(((ACREV0) ? 0x168 : 0x17b) | JTAG_2069_CR0)
#define RF0_2069_OVR13		(((ACREV0) ? 0x169 : 0x17c) | JTAG_2069_CR0)
#define RFP_2069_OVR14		\
	(((ACREV0) ? 0x16a : 0x17d) | ((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RF0_2069_OVR17		\
	(((ACREV0) ? 0x16d : 0x180) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_OVR18		\
	(((ACREV0) ? 0x16e : 0x181) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_OVR19		(((ACREV0) ? 0x16f : 0x182) | JTAG_2069_CR0)
#define RF0_2069_OVR20		(((ACREV0) ? 0x170 : 0x183) | JTAG_2069_CR0)
#define RF0_2069_OVR21		(((ACREV0) ? 0x171 : 0x184) | JTAG_2069_CR0)
#define RFP_2069_OVR22		\
	(((ACREV0) ? 0x172 : 0x185) | ((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RF0_2069_OVR23		\
	(((ACREV0) ? 0x173 : 0x186) | ((ACREV0) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RF0_2069_DAC_CFG3                        (0x174 | JTAG_2069_CR0)
#define RF0_2069_AFEDIV3                         (0x176 | JTAG_2069_CR0)
#define RF0_2069_GP_REGISTER                     (0x178 | JTAG_2069_CR0)
#define RF1_2069_REV_ID		(((ACREV0) ? 0x0 : 0x0) | JTAG_2069_CR1)
#define RF1_2069_DEV_ID		(((ACREV0) ? 0x1 : 0x1) | JTAG_2069_CR1)
#define RF1_2069_TXGM_LOFT_FINE_I		(((ACREV0) ? 0x2 : 0x2) | JTAG_2069_CR1)
#define RF1_2069_TXGM_LOFT_FINE_Q		(((ACREV0) ? 0x3 : 0x3) | JTAG_2069_CR1)
#define RF1_2069_TXGM_LOFT_COARSE_I		(((ACREV0) ? 0x4 : 0x4) | JTAG_2069_CR1)
#define RF1_2069_TXGM_LOFT_COARSE_Q		(((ACREV0) ? 0x5 : 0x5) | JTAG_2069_CR1)
#define RF1_2069_TXGM_LOFT_SCALE		(((ACREV0) ? 0x6 : 0x6) | JTAG_2069_CR1)
#define RF1_2069_TEMPSENSE_CFG		(((ACREV0) ? 0xe : 0x13) | JTAG_2069_CR1)
#define RF1_2069_VBAT_CFG		(((ACREV0) ? 0xf : 0x14) | JTAG_2069_CR1)
#define RF1_2069_TESTBUF_CFG1		(((ACREV0) ? 0x17 : 0x1c) | JTAG_2069_CR1)
#define RF1_2069_GPAIO_SEL0		(((ACREV0) ? 0x18 : 0x1d) | JTAG_2069_CR1)
#define RF1_2069_GPAIO_SEL1		(((ACREV0) ? 0x19 : 0x1e) | JTAG_2069_CR1)
#define RF1_2069_IQCAL_CFG1		(((ACREV0) ? 0x1a : 0x1f) | JTAG_2069_CR1)
#define RF1_2069_IQCAL_CFG2		(((ACREV0) ? 0x1b : 0x20) | JTAG_2069_CR1)
#define RF1_2069_IQCAL_CFG3		(((ACREV0) ? 0x1c : 0x21) | JTAG_2069_CR1)
#define RF1_2069_IQCAL_IDAC		(((ACREV0) ? 0x1d : 0x22) | JTAG_2069_CR1)
#define RF1_2069_PA2G_TSSI		(((ACREV0) ? 0x1e : 0x23) | JTAG_2069_CR1)
#define RF1_2069_TX5G_TSSI		(((ACREV0) ? 0x1f : 0x24) | JTAG_2069_CR1)
#define RF1_2069_TXRX2G_CAL_TX		(((ACREV0) ? 0x20 : 0x25) | JTAG_2069_CR1)
#define RF1_2069_TXRX5G_CAL_TX		(((ACREV0) ? 0x21 : 0x26) | JTAG_2069_CR1)
#define RF1_2069_TXRX2G_CAL_RX		(((ACREV0) ? 0x22 : 0x27) | JTAG_2069_CR1)
#define RF1_2069_TXRX5G_CAL_RX		(((ACREV0) ? 0x23 : 0x28) | JTAG_2069_CR1)
#define RF1_2069_AUXPGA_CFG1		(((ACREV0) ? 0x24 : 0x29) | JTAG_2069_CR1)
#define RF1_2069_AUXPGA_VMID		(((ACREV0) ? 0x25 : 0x2a) | JTAG_2069_CR1)
#define RF1_2069_CORE_BIAS		(((ACREV0) ? 0x26 : 0x2b) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_CFG1		(((ACREV0) ? 0x27 : 0x2c) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_CFG2		(((ACREV0) ? 0x28 : 0x2d) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_TUNE		(((ACREV0) ? 0x29 : 0x2f) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_IDAC1		(((ACREV0) ? 0x2a : 0x30) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_IDAC2		(((ACREV0) ? 0x2b : 0x31) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_RSSI		(((ACREV0) ? 0x2c : 0x32) | JTAG_2069_CR1)
#define RF1_2069_LNA2G_RSSI_ANA		(((ACREV0) ? 0x2d : 0x33) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_CFG1		(((ACREV0) ? 0x2e : 0x34) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_CFG2		(((ACREV0) ? 0x2f : 0x35) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_TUNE		(((ACREV0) ? 0x30 : 0x37) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_IDAC1		(((ACREV0) ? 0x31 : 0x38) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_IDAC2		(((ACREV0) ? 0x32 : 0x39) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_RSSI		(((ACREV0) ? 0x33 : 0x3a) | JTAG_2069_CR1)
#define RF1_2069_LNA5G_RSSI_ANA		(((ACREV0) ? 0x34 : 0x3b) | JTAG_2069_CR1)
#define RF1_2069_RXMIX2G_CFG1		(((ACREV0) ? 0x35 : 0x3c) | JTAG_2069_CR1)
#define RF1_2069_RXMIX2G_IDAC		(((ACREV0) ? 0x36 : 0x3d) | JTAG_2069_CR1)
#define RF1_2069_RXMIX5G_CFG1		(((ACREV0) ? 0x37 : 0x3e) | JTAG_2069_CR1)
#define RF1_2069_RXMIX5G_IDAC		(((ACREV0) ? 0x38 : 0x3f) | JTAG_2069_CR1)
#define RF1_2069_RXRF2G_CFG1		(((ACREV0) ? 0x39 : 0x40) | JTAG_2069_CR1)
#define RF1_2069_RXRF2G_CFG2		(((ACREV0) ? 0x3a : 0x41) | JTAG_2069_CR1)
#define RF1_2069_RXRF2G_SPARE		(((ACREV0) ? 0x3b : 0x42) | JTAG_2069_CR1)
#define RF1_2069_RXRF5G_CFG1		(((ACREV0) ? 0x3c : 0x43) | JTAG_2069_CR1)
#define RF1_2069_RXRF5G_CFG2		(((ACREV0) ? 0x3d : 0x44) | JTAG_2069_CR1)
#define RF1_2069_RX_REG_BACKUP_1		(((ACREV0) ? 0x3e : 0x45) | JTAG_2069_CR1)
#define RF1_2069_RX_REG_BACKUP_2		(((ACREV0) ? 0x3f : 0x46) | JTAG_2069_CR1)
#define RF1_2069_RXRF5G_SPARE		(((ACREV0) ? 0x40 : 0x47) | JTAG_2069_CR1)
#define RF1_2069_TIA_CFG1		(((ACREV0) ? 0x41 : 0x48) | JTAG_2069_CR1)
#define RF1_2069_TIA_CFG2		(((ACREV0) ? 0x42 : 0x49) | JTAG_2069_CR1)
#define RF1_2069_TIA_CFG3		(((ACREV0) ? 0x43 : 0x4a) | JTAG_2069_CR1)
#define RF1_2069_TIA_IDAC1		(((ACREV0) ? 0x44 : 0x4b) | JTAG_2069_CR1)
#define RF1_2069_NBRSSI_CONFG		(((ACREV0) ? 0x45 : 0x4c) | JTAG_2069_CR1)
#define RF1_2069_NBRSSI_BIAS		(((ACREV0) ? 0x46 : 0x4d) | JTAG_2069_CR1)
#define RF1_2069_NBRSSI_IB		(((ACREV0) ? 0x47 : 0x4e) | JTAG_2069_CR1)
#define RF1_2069_NBRSSI_TEST		(((ACREV0) ? 0x48 : 0x4f) | JTAG_2069_CR1)
#define RF1_2069_WRSSI3_CONFG		(((ACREV0) ? 0x49 : 0x50) | JTAG_2069_CR1)
#define RF1_2069_WRSSI3_BIAS		(((ACREV0) ? 0x4a : 0x51) | JTAG_2069_CR1)
#define RF1_2069_WRSSI3_TEST		(((ACREV0) ? 0x4b : 0x52) | JTAG_2069_CR1)
#define RF1_2069_LPF_MUX_SWITCHES		(((ACREV0) ? 0x4c : 0x53) | JTAG_2069_CR1)
#define RF1_2069_LPF_ENABLES		(((ACREV0) ? 0x4d : 0x54) | JTAG_2069_CR1)
#define RF1_2069_LPF_MAIN_CONTROLS		(((ACREV0) ? 0x4e : 0x55) | JTAG_2069_CR1)
#define RF1_2069_LPF_CORNER_FREQUENCY_TUNING		(((ACREV0) ? 0x4f : 0x56) | JTAG_2069_CR1)
#define RF1_2069_LPF_DC_LOOP_AND_MISC		(((ACREV0) ? 0x50 : 0x57) | JTAG_2069_CR1)
#define RF1_2069_LPF_BIAS_LEVELS_LOW		(((ACREV0) ? 0x51 : 0x58) | JTAG_2069_CR1)
#define RF1_2069_LPF_BIAS_LEVELS_MID		(((ACREV0) ? 0x52 : 0x59) | JTAG_2069_CR1)
#define RF1_2069_LPF_BIAS_LEVELS_HIGH		(((ACREV0) ? 0x53 : 0x5a) | JTAG_2069_CR1)
#define RF1_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS		\
	(((ACREV0) ? 0x54 : 0x5b) | JTAG_2069_CR1)
#define RF1_2069_LPF_COMMON_MODE_REFERENCE_LEVELS		\
	(((ACREV0) ? 0x55 : 0x5c) | JTAG_2069_CR1)
#define RF1_2069_TX2G_CFG1		(((ACREV0) ? 0x56 : 0x5d) | JTAG_2069_CR1)
#define RF1_2069_TX2G_CFG2		(((ACREV0) ? 0x57 : 0x5e) | JTAG_2069_CR1)
#define RF1_2069_TX5G_CFG1		(((ACREV0) ? 0x58 : 0x5f) | JTAG_2069_CR1)
#define RF1_2069_TXGM_CFG1		(((ACREV0) ? 0x59 : 0x60) | JTAG_2069_CR1)
#define RF1_2069_TXGM_CFG2		(((ACREV0) ? 0x5a : 0x61) | JTAG_2069_CR1)
#define RF1_2069_TXMIX2G_CFG1		(((ACREV0) ? 0x5b : 0x62) | JTAG_2069_CR1)
#define RF1_2069_TXMIX5G_CFG1		(((ACREV0) ? 0x5c : 0x63) | JTAG_2069_CR1)
#define RF1_2069_PGA2G_CFG1		(((ACREV0) ? 0x5d : 0x64) | JTAG_2069_CR1)
#define RF1_2069_PGA2G_CFG2		(((ACREV0) ? 0x5e : 0x65) | JTAG_2069_CR1)
#define RF1_2069_PGA2G_IDAC		(((ACREV0) ? 0x5f : 0x66) | JTAG_2069_CR1)
#define RF1_2069_PGA2G_INCAP		(((ACREV0) ? 0x60 : 0x67) | JTAG_2069_CR1)
#define RF1_2069_PGA5G_CFG1		(((ACREV0) ? 0x61 : 0x68) | JTAG_2069_CR1)
#define RF1_2069_PGA5G_CFG2		(((ACREV0) ? 0x62 : 0x69) | JTAG_2069_CR1)
#define RF1_2069_PGA5G_IDAC		(((ACREV0) ? 0x63 : 0x6a) | JTAG_2069_CR1)
#define RF1_2069_PGA5G_INCAP		(((ACREV0) ? 0x64 : 0x6b) | JTAG_2069_CR1)
#define RF1_2069_PAD2G_CFG1		(((ACREV0) ? 0x65 : 0x6c) | JTAG_2069_CR1)
#define RF1_2069_PAD2G_IDAC		(((ACREV0) ? 0x66 : 0x6d) | JTAG_2069_CR1)
#define RF1_2069_PAD2G_SLOPE		(((ACREV0) ? 0x67 : 0x6e) | JTAG_2069_CR1)
#define RF1_2069_PAD2G_TUNE		(((ACREV0) ? 0x68 : 0x6f) | JTAG_2069_CR1)
#define RF1_2069_PAD2G_INCAP		(((ACREV0) ? 0x69 : 0x70) | JTAG_2069_CR1)
#define RF1_2069_PAD5G_CFG1		(((ACREV0) ? 0x6a : 0x71) | JTAG_2069_CR1)
#define RF1_2069_PAD5G_IDAC		(((ACREV0) ? 0x6b : 0x72) | JTAG_2069_CR1)
#define RF1_2069_PAD5G_SLOPE		(((ACREV0) ? 0x6c : 0x73) | JTAG_2069_CR1)
#define RF1_2069_PAD5G_TUNE		(((ACREV0) ? 0x6d : 0x74) | JTAG_2069_CR1)
#define RF1_2069_PAD5G_INCAP		(((ACREV0) ? 0x6e : 0x75) | JTAG_2069_CR1)
#define RF1_2069_PA2G_CFG1		(((ACREV0) ? 0x6f : 0x76) | JTAG_2069_CR1)
#define RF1_2069_PA2G_CFG2		(((ACREV0) ? 0x70 : 0x77) | JTAG_2069_CR1)
#define RF1_2069_PA2G_CFG3		(((ACREV0) ? 0x71 : 0x78) | JTAG_2069_CR1)
#define RF1_2069_PA2G_IDAC1		(((ACREV0) ? 0x72 : 0x79) | JTAG_2069_CR1)
#define RF1_2069_PA2G_IDAC2		(((ACREV0) ? 0x73 : 0x7a) | JTAG_2069_CR1)
#define RF1_2069_PA2G_INCAP		(((ACREV0) ? 0x74 : 0x7b) | JTAG_2069_CR1)
#define RF1_2069_PA5G_CFG1		(((ACREV0) ? 0x75 : 0x7c) | JTAG_2069_CR1)
#define RF1_2069_PA5G_CFG2		(((ACREV0) ? 0x76 : 0x7d) | JTAG_2069_CR1)
#define RF1_2069_PA5G_CFG3		(((ACREV0) ? 0x77 : 0x7e) | JTAG_2069_CR1)
#define RF1_2069_PA5G_IDAC1		(((ACREV0) ? 0x78 : 0x7f) | JTAG_2069_CR1)
#define RF1_2069_PA5G_IDAC2		(((ACREV0) ? 0x79 : 0x80) | JTAG_2069_CR1)
#define RF1_2069_PA5G_INCAP		(((ACREV0) ? 0x7a : 0x81) | JTAG_2069_CR1)
#define RF1_2069_TRSW2G_CFG1		(((ACREV0) ? 0x7b : 0x82) | JTAG_2069_CR1)
#define RF1_2069_TRSW2G_CFG2		(((ACREV0) ? 0x7c : 0x83) | JTAG_2069_CR1)
#define RF1_2069_TRSW2G_CFG3		(((ACREV0) ? 0x7d : 0x84) | JTAG_2069_CR1)
#define RF1_2069_TRSW5G_CFG1		(((ACREV0) ? 0x7e : 0x85) | JTAG_2069_CR1)
#define RF1_2069_TRSW5G_CFG2		(((ACREV0) ? 0x7f : 0x86) | JTAG_2069_CR1)
#define RF1_2069_TRSW5G_CFG3		(((ACREV0) ? 0x80 : 0x87) | JTAG_2069_CR1)
#define RF1_2069_TX_REG_BACKUP_1		(((ACREV0) ? 0x81 : 0x88) | JTAG_2069_CR1)
#define RF1_2069_TX_REG_BACKUP_2		(((ACREV0) ? 0x82 : 0x89) | JTAG_2069_CR1)
#define RF1_2069_TX_REG_BACKUP_3		(((ACREV0) ? 0x83 : 0x8a) | JTAG_2069_CR1)
#define RF1_2069_TX_REG_BACKUP_4		(((ACREV0) ? 0x84 : 0x8b) | JTAG_2069_CR1)
#define RF1_2069_LOGEN2G_CFG2		(((ACREV0) ? 0x10e : 0x120) | JTAG_2069_CR1)
#define RF1_2069_LOGEN2G_IDAC3		(((ACREV0) ? 0x110 : 0x122) | JTAG_2069_CR1)
#define RF1_2069_LOGEN2G_RCCR		(((ACREV0) ? 0x111 : 0x123) | JTAG_2069_CR1)
#define RF1_2069_LOGEN5G_CFG2		(((ACREV0) ? 0x114 : 0x126) | JTAG_2069_CR1)
#define RF1_2069_LOGEN5G_IDAC4		(((ACREV0) ? 0x118 : 0x12a) | JTAG_2069_CR1)
#define RF1_2069_LOGEN5G_RCCR		(((ACREV0) ? 0x119 : 0x12b) | JTAG_2069_CR1)
#define RF1_2069_DAC_CFG1		(((ACREV0) ? 0x11c : 0x12e) | JTAG_2069_CR1)
#define RF1_2069_DAC_CFG2		(((ACREV0) ? 0x11d : 0x12f) | JTAG_2069_CR1)
#define RF1_2069_DAC_BIAS		(((ACREV0) ? 0x11e : 0x130) | JTAG_2069_CR1)
#define RF1_2069_ADC_CFG1		(((ACREV0) ? 0x11f : 0x131) | JTAG_2069_CR1)
#define RF1_2069_ADC_CFG2		(((ACREV0) ? 0x120 : 0x132) | JTAG_2069_CR1)
#define RF1_2069_ADC_CFG3		(((ACREV0) ? 0x121 : 0x133) | JTAG_2069_CR1)
#define RF1_2069_ADC_CFG4		(((ACREV0) ? 0x122 : 0x134) | JTAG_2069_CR1)
#define RF1_2069_ADC_CFG5		(((ACREV0) ? 0x123 : 0x135) | JTAG_2069_CR1)
#define RF1_2069_ADC_BIAS1		(((ACREV0) ? 0x124 : 0x136) | JTAG_2069_CR1)
#define RF1_2069_ADC_BIAS2		(((ACREV0) ? 0x125 : 0x137) | JTAG_2069_CR1)
#define RF1_2069_ADC_RC1		(((ACREV0) ? 0x126 : 0x138) | JTAG_2069_CR1)
#define RF1_2069_ADC_RC2		(((ACREV0) ? 0x127 : 0x139) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE1		(((ACREV0) ? 0x128 : 0x13a) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE2		(((ACREV0) ? 0x129 : 0x13b) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE3		(((ACREV0) ? 0x12a : 0x13c) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE4		(((ACREV0) ? 0x12b : 0x13d) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE5		(((ACREV0) ? 0x12c : 0x13e) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE6		(((ACREV0) ? 0x12d : 0x13f) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE7		(((ACREV0) ? 0x12e : 0x140) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE8		(((ACREV0) ? 0x12f : 0x141) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE9		(((ACREV0) ? 0x130 : 0x142) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE10		(((ACREV0) ? 0x131 : 0x143) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE11		(((ACREV0) ? 0x132 : 0x144) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE12		(((ACREV0) ? 0x133 : 0x145) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE13		(((ACREV0) ? 0x134 : 0x146) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE14		(((ACREV0) ? 0x135 : 0x147) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE15		(((ACREV0) ? 0x136 : 0x148) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE16		(((ACREV0) ? 0x137 : 0x149) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE17		(((ACREV0) ? 0x138 : 0x14a) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE18		(((ACREV0) ? 0x139 : 0x14b) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE19		(((ACREV0) ? 0x13a : 0x14c) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE20		(((ACREV0) ? 0x13b : 0x14d) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE21		(((ACREV0) ? 0x13c : 0x14e) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE22		(((ACREV0) ? 0x13d : 0x14f) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE23		(((ACREV0) ? 0x13e : 0x150) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE24		(((ACREV0) ? 0x13f : 0x151) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE25		(((ACREV0) ? 0x140 : 0x152) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE26		(((ACREV0) ? 0x141 : 0x153) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE27		(((ACREV0) ? 0x142 : 0x154) | JTAG_2069_CR1)
#define RF1_2069_ADC_CALCODE28		(((ACREV0) ? 0x143 : 0x155) | JTAG_2069_CR1)
#define RF1_2069_ADC_STATUS		(((ACREV0) ? 0x144 : 0x156) | JTAG_2069_CR1)
#define RF1_2069_GPAIO_CFG1		(((ACREV0) ? 0x147 : 0x15a) | JTAG_2069_CR1)
#define RF1_2069_RADIO_SPARE1		(((ACREV0) ? 0x157 : 0x16a) | JTAG_2069_CR1)
#define RF1_2069_RADIO_SPARE2		(((ACREV0) ? 0x158 : 0x16b) | JTAG_2069_CR1)
#define RF1_2069_RADIO_SPARE3		(((ACREV0) ? 0x159 : 0x16c) | JTAG_2069_CR1)
#define RF1_2069_RADIO_SPARE4		(((ACREV0) ? 0x15a : 0x16d) | JTAG_2069_CR1)
#define RF1_2069_OVR1		(((ACREV0) ? 0x15d : 0x170) | JTAG_2069_CR1)
#define RF1_2069_OVR3		(((ACREV0) ? 0x15f : 0x172) | JTAG_2069_CR1)
#define RF1_2069_OVR4		(((ACREV0) ? 0x160 : 0x173) | JTAG_2069_CR1)
#define RF1_2069_OVR5		(((ACREV0) ? 0x161 : 0x174) | JTAG_2069_CR1)
#define RF1_2069_OVR6		(((ACREV0) ? 0x162 : 0x175) | JTAG_2069_CR1)
#define RF1_2069_OVR7		(((ACREV0) ? 0x163 : 0x176) | JTAG_2069_CR1)
#define RF1_2069_OVR9		\
	(((ACREV0) ? 0x165 : 0x178) | ((ACREV0) ? JTAG_2069_CR1 : JTAG_2069_PLL))
#define RF1_2069_OVR10		(((ACREV0) ? 0x166 : 0x179) | JTAG_2069_CR1)
#define RF1_2069_OVR11		(((ACREV0) ? 0x167 : 0x17a) | JTAG_2069_CR1)
#define RF1_2069_OVR12		(((ACREV0) ? 0x168 : 0x17b) | JTAG_2069_CR1)
#define RF1_2069_OVR13		(((ACREV0) ? 0x169 : 0x17c) | JTAG_2069_CR1)
#define RF1_2069_OVR18		\
	(((ACREV0) ? 0x16e : 0x181) | ((ACREV0) ? JTAG_2069_CR1 : JTAG_2069_PLL))
#define RF1_2069_OVR19		(((ACREV0) ? 0x16f : 0x182) | JTAG_2069_CR1)
#define RF1_2069_OVR20		(((ACREV0) ? 0x170 : 0x183) | JTAG_2069_CR1)
#define RF1_2069_OVR21		(((ACREV0) ? 0x171 : 0x184) | JTAG_2069_CR1)
#define RF1_2069_OVR23		\
	(((ACREV0) ? 0x173 : 0x186) | ((ACREV0) ? JTAG_2069_CR1 : JTAG_2069_PLL))
#define RF1_2069_DAC_CFG3                        (0x174 | JTAG_2069_CR1)
#define RF1_2069_GP_REGISTER                     (0x178 | JTAG_2069_CR1)
#define RF2_2069_REV_ID		(((ACREV0) ? 0x0 : 0x0) | JTAG_2069_CR2)
#define RF2_2069_DEV_ID		(((ACREV0) ? 0x1 : 0x1) | JTAG_2069_CR2)
#define RF2_2069_TXGM_LOFT_FINE_I		(((ACREV0) ? 0x2 : 0x2) | JTAG_2069_CR2)
#define RF2_2069_TXGM_LOFT_FINE_Q		(((ACREV0) ? 0x3 : 0x3) | JTAG_2069_CR2)
#define RF2_2069_TXGM_LOFT_COARSE_I		(((ACREV0) ? 0x4 : 0x4) | JTAG_2069_CR2)
#define RF2_2069_TXGM_LOFT_COARSE_Q		(((ACREV0) ? 0x5 : 0x5) | JTAG_2069_CR2)
#define RF2_2069_TXGM_LOFT_SCALE		(((ACREV0) ? 0x6 : 0x6) | JTAG_2069_CR2)
#define RF2_2069_BG_CFG1		\
	(((ACREV0) ? 0x7 : 0x7) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_BG_CFG2		\
	(((ACREV0) ? 0x8 : 0x8) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_BG_TRIM1		\
	(((ACREV0) ? 0x9 : 0x9) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_BG_TRIM2		\
	(((ACREV0) ? 0xa : 0xa) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCAL_CFG		\
	(((ACREV0) ? 0xb : 0x10) | \
	 ((ACREV0) ? JTAG_2069_CR2 : ((ACREV3) ? JTAG_2069_CR0 : JTAG_2069_PLL)))
#define RF2_2069_VREG_CFG		\
	(((ACREV0) ? 0xc : 0x11) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_LPO_CFG		\
	(((ACREV0) ? 0xd : 0x12) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_TEMPSENSE_CFG		(((ACREV0) ? 0xe : 0x13) | JTAG_2069_CR2)
#define RF2_2069_VBAT_CFG		(((ACREV0) ? 0xf : 0x14) | JTAG_2069_CR2)
#define RF2_2069_RCCAL_CFG		\
	(((ACREV0) ? 0x10 : 0x15) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_LOGIC1		\
	(((ACREV0) ? 0x11 : 0x16) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_TRC		\
	(((ACREV0) ? 0x12 : 0x17) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_LOGIC2		\
	(((ACREV0) ? 0x13 : 0x18) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_LOGIC3		\
	(((ACREV0) ? 0x14 : 0x19) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_LOGIC4		\
	(((ACREV0) ? 0x15 : 0x1a) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RCCAL_LOGIC5		\
	(((ACREV0) ? 0x16 : 0x1b) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_TESTBUF_CFG1		(((ACREV0) ? 0x17 : 0x1c) | JTAG_2069_CR2)
#define RF2_2069_GPAIO_SEL0		(((ACREV0) ? 0x18 : 0x1d) | JTAG_2069_CR2)
#define RF2_2069_GPAIO_SEL1		(((ACREV0) ? 0x19 : 0x1e) | JTAG_2069_CR2)
#define RF2_2069_IQCAL_CFG1		(((ACREV0) ? 0x1a : 0x1f) | JTAG_2069_CR2)
#define RF2_2069_IQCAL_CFG2		(((ACREV0) ? 0x1b : 0x20) | JTAG_2069_CR2)
#define RF2_2069_IQCAL_CFG3		(((ACREV0) ? 0x1c : 0x21) | JTAG_2069_CR2)
#define RF2_2069_IQCAL_IDAC		(((ACREV0) ? 0x1d : 0x22) | JTAG_2069_CR2)
#define RF2_2069_PA2G_TSSI		(((ACREV0) ? 0x1e : 0x23) | JTAG_2069_CR2)
#define RF2_2069_TX5G_TSSI		(((ACREV0) ? 0x1f : 0x24) | JTAG_2069_CR2)
#define RF2_2069_TXRX2G_CAL_TX		(((ACREV0) ? 0x20 : 0x25) | JTAG_2069_CR2)
#define RF2_2069_TXRX5G_CAL_TX		(((ACREV0) ? 0x21 : 0x26) | JTAG_2069_CR2)
#define RF2_2069_TXRX2G_CAL_RX		(((ACREV0) ? 0x22 : 0x27) | JTAG_2069_CR2)
#define RF2_2069_TXRX5G_CAL_RX		(((ACREV0) ? 0x23 : 0x28) | JTAG_2069_CR2)
#define RF2_2069_AUXPGA_CFG1		(((ACREV0) ? 0x24 : 0x29) | JTAG_2069_CR2)
#define RF2_2069_AUXPGA_VMID		(((ACREV0) ? 0x25 : 0x2a) | JTAG_2069_CR2)
#define RF2_2069_CORE_BIAS		(((ACREV0) ? 0x26 : 0x2b) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_CFG1		(((ACREV0) ? 0x27 : 0x2c) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_CFG2		(((ACREV0) ? 0x28 : 0x2d) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_TUNE		(((ACREV0) ? 0x29 : 0x2f) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_IDAC1		(((ACREV0) ? 0x2a : 0x30) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_IDAC2		(((ACREV0) ? 0x2b : 0x31) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_RSSI		(((ACREV0) ? 0x2c : 0x32) | JTAG_2069_CR2)
#define RF2_2069_LNA2G_RSSI_ANA		(((ACREV0) ? 0x2d : 0x33) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_CFG1		(((ACREV0) ? 0x2e : 0x34) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_CFG2		(((ACREV0) ? 0x2f : 0x35) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_TUNE		(((ACREV0) ? 0x30 : 0x37) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_IDAC1		(((ACREV0) ? 0x31 : 0x38) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_IDAC2		(((ACREV0) ? 0x32 : 0x39) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_RSSI		(((ACREV0) ? 0x33 : 0x3a) | JTAG_2069_CR2)
#define RF2_2069_LNA5G_RSSI_ANA		(((ACREV0) ? 0x34 : 0x3b) | JTAG_2069_CR2)
#define RF2_2069_RXMIX2G_CFG1		(((ACREV0) ? 0x35 : 0x3c) | JTAG_2069_CR2)
#define RF2_2069_RXMIX2G_IDAC		(((ACREV0) ? 0x36 : 0x3d) | JTAG_2069_CR2)
#define RF2_2069_RXMIX5G_CFG1		(((ACREV0) ? 0x37 : 0x3e) | JTAG_2069_CR2)
#define RF2_2069_RXMIX5G_IDAC		(((ACREV0) ? 0x38 : 0x3f) | JTAG_2069_CR2)
#define RF2_2069_RXRF2G_CFG1		(((ACREV0) ? 0x39 : 0x40) | JTAG_2069_CR2)
#define RF2_2069_RXRF2G_CFG2		(((ACREV0) ? 0x3a : 0x41) | JTAG_2069_CR2)
#define RF2_2069_RXRF2G_SPARE		(((ACREV0) ? 0x3b : 0x42) | JTAG_2069_CR2)
#define RF2_2069_RXRF5G_CFG1		(((ACREV0) ? 0x3c : 0x43) | JTAG_2069_CR2)
#define RF2_2069_RXRF5G_CFG2		(((ACREV0) ? 0x3d : 0x44) | JTAG_2069_CR2)
#define RF2_2069_RX_REG_BACKUP_1		(((ACREV0) ? 0x3e : 0x45) | JTAG_2069_CR2)
#define RF2_2069_RX_REG_BACKUP_2		(((ACREV0) ? 0x3f : 0x46) | JTAG_2069_CR2)
#define RF2_2069_RXRF5G_SPARE		(((ACREV0) ? 0x40 : 0x47) | JTAG_2069_CR2)
#define RF2_2069_TIA_CFG1		(((ACREV0) ? 0x41 : 0x48) | JTAG_2069_CR2)
#define RF2_2069_TIA_CFG2		(((ACREV0) ? 0x42 : 0x49) | JTAG_2069_CR2)
#define RF2_2069_TIA_CFG3		(((ACREV0) ? 0x43 : 0x4a) | JTAG_2069_CR2)
#define RF2_2069_TIA_IDAC1		(((ACREV0) ? 0x44 : 0x4b) | JTAG_2069_CR2)
#define RF2_2069_NBRSSI_CONFG		(((ACREV0) ? 0x45 : 0x4c) | JTAG_2069_CR2)
#define RF2_2069_NBRSSI_BIAS		(((ACREV0) ? 0x46 : 0x4d) | JTAG_2069_CR2)
#define RF2_2069_NBRSSI_IB		(((ACREV0) ? 0x47 : 0x4e) | JTAG_2069_CR2)
#define RF2_2069_NBRSSI_TEST		(((ACREV0) ? 0x48 : 0x4f) | JTAG_2069_CR2)
#define RF2_2069_WRSSI3_CONFG		(((ACREV0) ? 0x49 : 0x50) | JTAG_2069_CR2)
#define RF2_2069_WRSSI3_BIAS		(((ACREV0) ? 0x4a : 0x51) | JTAG_2069_CR2)
#define RF2_2069_WRSSI3_TEST		(((ACREV0) ? 0x4b : 0x52) | JTAG_2069_CR2)
#define RF2_2069_LPF_MUX_SWITCHES		(((ACREV0) ? 0x4c : 0x53) | JTAG_2069_CR2)
#define RF2_2069_LPF_ENABLES		(((ACREV0) ? 0x4d : 0x54) | JTAG_2069_CR2)
#define RF2_2069_LPF_MAIN_CONTROLS		(((ACREV0) ? 0x4e : 0x55) | JTAG_2069_CR2)
#define RF2_2069_LPF_CORNER_FREQUENCY_TUNING		(((ACREV0) ? 0x4f : 0x56) | JTAG_2069_CR2)
#define RF2_2069_LPF_DC_LOOP_AND_MISC		(((ACREV0) ? 0x50 : 0x57) | JTAG_2069_CR2)
#define RF2_2069_LPF_BIAS_LEVELS_LOW		(((ACREV0) ? 0x51 : 0x58) | JTAG_2069_CR2)
#define RF2_2069_LPF_BIAS_LEVELS_MID		(((ACREV0) ? 0x52 : 0x59) | JTAG_2069_CR2)
#define RF2_2069_LPF_BIAS_LEVELS_HIGH		(((ACREV0) ? 0x53 : 0x5a) | JTAG_2069_CR2)
#define RF2_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS		\
	(((ACREV0) ? 0x54 : 0x5b) | JTAG_2069_CR2)
#define RF2_2069_LPF_COMMON_MODE_REFERENCE_LEVELS		\
	(((ACREV0) ? 0x55 : 0x5c) | JTAG_2069_CR2)
#define RF2_2069_TX2G_CFG1		(((ACREV0) ? 0x56 : 0x5d) | JTAG_2069_CR2)
#define RF2_2069_TX2G_CFG2		(((ACREV0) ? 0x57 : 0x5e) | JTAG_2069_CR2)
#define RF2_2069_TX5G_CFG1		(((ACREV0) ? 0x58 : 0x5f) | JTAG_2069_CR2)
#define RF2_2069_TXGM_CFG1		(((ACREV0) ? 0x59 : 0x60) | JTAG_2069_CR2)
#define RF2_2069_TXGM_CFG2		(((ACREV0) ? 0x5a : 0x61) | JTAG_2069_CR2)
#define RF2_2069_TXMIX2G_CFG1		(((ACREV0) ? 0x5b : 0x62) | JTAG_2069_CR2)
#define RF2_2069_TXMIX5G_CFG1		(((ACREV0) ? 0x5c : 0x63) | JTAG_2069_CR2)
#define RF2_2069_PGA2G_CFG1		(((ACREV0) ? 0x5d : 0x64) | JTAG_2069_CR2)
#define RF2_2069_PGA2G_CFG2		(((ACREV0) ? 0x5e : 0x65) | JTAG_2069_CR2)
#define RF2_2069_PGA2G_IDAC		(((ACREV0) ? 0x5f : 0x66) | JTAG_2069_CR2)
#define RF2_2069_PGA2G_INCAP		(((ACREV0) ? 0x60 : 0x67) | JTAG_2069_CR2)
#define RF2_2069_PGA5G_CFG1		(((ACREV0) ? 0x61 : 0x68) | JTAG_2069_CR2)
#define RF2_2069_PGA5G_CFG2		(((ACREV0) ? 0x62 : 0x69) | JTAG_2069_CR2)
#define RF2_2069_PGA5G_IDAC		(((ACREV0) ? 0x63 : 0x6a) | JTAG_2069_CR2)
#define RF2_2069_PGA5G_INCAP		(((ACREV0) ? 0x64 : 0x6b) | JTAG_2069_CR2)
#define RF2_2069_PAD2G_CFG1		(((ACREV0) ? 0x65 : 0x6c) | JTAG_2069_CR2)
#define RF2_2069_PAD2G_IDAC		(((ACREV0) ? 0x66 : 0x6d) | JTAG_2069_CR2)
#define RF2_2069_PAD2G_SLOPE		(((ACREV0) ? 0x67 : 0x6e) | JTAG_2069_CR2)
#define RF2_2069_PAD2G_TUNE		(((ACREV0) ? 0x68 : 0x6f) | JTAG_2069_CR2)
#define RF2_2069_PAD2G_INCAP		(((ACREV0) ? 0x69 : 0x70) | JTAG_2069_CR2)
#define RF2_2069_PAD5G_CFG1		(((ACREV0) ? 0x6a : 0x71) | JTAG_2069_CR2)
#define RF2_2069_PAD5G_IDAC		(((ACREV0) ? 0x6b : 0x72) | JTAG_2069_CR2)
#define RF2_2069_PAD5G_SLOPE		(((ACREV0) ? 0x6c : 0x73) | JTAG_2069_CR2)
#define RF2_2069_PAD5G_TUNE		(((ACREV0) ? 0x6d : 0x74) | JTAG_2069_CR2)
#define RF2_2069_PAD5G_INCAP		(((ACREV0) ? 0x6e : 0x75) | JTAG_2069_CR2)
#define RF2_2069_PA2G_CFG1		(((ACREV0) ? 0x6f : 0x76) | JTAG_2069_CR2)
#define RF2_2069_PA2G_CFG2		(((ACREV0) ? 0x70 : 0x77) | JTAG_2069_CR2)
#define RF2_2069_PA2G_CFG3		(((ACREV0) ? 0x71 : 0x78) | JTAG_2069_CR2)
#define RF2_2069_PA2G_IDAC1		(((ACREV0) ? 0x72 : 0x79) | JTAG_2069_CR2)
#define RF2_2069_PA2G_IDAC2		(((ACREV0) ? 0x73 : 0x7a) | JTAG_2069_CR2)
#define RF2_2069_PA2G_INCAP		(((ACREV0) ? 0x74 : 0x7b) | JTAG_2069_CR2)
#define RF2_2069_PA5G_CFG1		(((ACREV0) ? 0x75 : 0x7c) | JTAG_2069_CR2)
#define RF2_2069_PA5G_CFG2		(((ACREV0) ? 0x76 : 0x7d) | JTAG_2069_CR2)
#define RF2_2069_PA5G_CFG3		(((ACREV0) ? 0x77 : 0x7e) | JTAG_2069_CR2)
#define RF2_2069_PA5G_IDAC1		(((ACREV0) ? 0x78 : 0x7f) | JTAG_2069_CR2)
#define RF2_2069_PA5G_IDAC2		(((ACREV0) ? 0x79 : 0x80) | JTAG_2069_CR2)
#define RF2_2069_PA5G_INCAP		(((ACREV0) ? 0x7a : 0x81) | JTAG_2069_CR2)
#define RF2_2069_TRSW2G_CFG1		(((ACREV0) ? 0x7b : 0x82) | JTAG_2069_CR2)
#define RF2_2069_TRSW2G_CFG2		(((ACREV0) ? 0x7c : 0x83) | JTAG_2069_CR2)
#define RF2_2069_TRSW2G_CFG3		(((ACREV0) ? 0x7d : 0x84) | JTAG_2069_CR2)
#define RF2_2069_TRSW5G_CFG1		(((ACREV0) ? 0x7e : 0x85) | JTAG_2069_CR2)
#define RF2_2069_TRSW5G_CFG2		(((ACREV0) ? 0x7f : 0x86) | JTAG_2069_CR2)
#define RF2_2069_TRSW5G_CFG3		(((ACREV0) ? 0x80 : 0x87) | JTAG_2069_CR2)
#define RF2_2069_TX_REG_BACKUP_1		(((ACREV0) ? 0x81 : 0x88) | JTAG_2069_CR2)
#define RF2_2069_TX_REG_BACKUP_2		(((ACREV0) ? 0x82 : 0x89) | JTAG_2069_CR2)
#define RF2_2069_TX_REG_BACKUP_3		(((ACREV0) ? 0x83 : 0x8a) | JTAG_2069_CR2)
#define RF2_2069_TX_REG_BACKUP_4		(((ACREV0) ? 0x84 : 0x8b) | JTAG_2069_CR2)
#define RF2_2069_LOGEN2G_CFG2		(((ACREV0) ? 0x10e : 0x120) | JTAG_2069_CR2)
#define RF2_2069_LOGEN2G_IDAC3		(((ACREV0) ? 0x110 : 0x122) | JTAG_2069_CR2)
#define RF2_2069_LOGEN2G_RCCR		(((ACREV0) ? 0x111 : 0x123) | JTAG_2069_CR2)
#define RF2_2069_LOGEN5G_CFG2		(((ACREV0) ? 0x114 : 0x126) | JTAG_2069_CR2)
#define RF2_2069_LOGEN5G_IDAC4		(((ACREV0) ? 0x118 : 0x12a) | JTAG_2069_CR2)
#define RF2_2069_LOGEN5G_RCCR		(((ACREV0) ? 0x119 : 0x12b) | JTAG_2069_CR2)
#define RF2_2069_DAC_CFG1		(((ACREV0) ? 0x11c : 0x12e) | JTAG_2069_CR2)
#define RF2_2069_DAC_CFG2		(((ACREV0) ? 0x11d : 0x12f) | JTAG_2069_CR2)
#define RF2_2069_DAC_BIAS		(((ACREV0) ? 0x11e : 0x130) | JTAG_2069_CR2)
#define RF2_2069_ADC_CFG1		(((ACREV0) ? 0x11f : 0x131) | JTAG_2069_CR2)
#define RF2_2069_ADC_CFG2		(((ACREV0) ? 0x120 : 0x132) | JTAG_2069_CR2)
#define RF2_2069_ADC_CFG3		(((ACREV0) ? 0x121 : 0x133) | JTAG_2069_CR2)
#define RF2_2069_ADC_CFG4		(((ACREV0) ? 0x122 : 0x134) | JTAG_2069_CR2)
#define RF2_2069_ADC_CFG5		(((ACREV0) ? 0x123 : 0x135) | JTAG_2069_CR2)
#define RF2_2069_ADC_BIAS1		(((ACREV0) ? 0x124 : 0x136) | JTAG_2069_CR2)
#define RF2_2069_ADC_BIAS2		(((ACREV0) ? 0x125 : 0x137) | JTAG_2069_CR2)
#define RF2_2069_ADC_RC1		(((ACREV0) ? 0x126 : 0x138) | JTAG_2069_CR2)
#define RF2_2069_ADC_RC2		(((ACREV0) ? 0x127 : 0x139) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE1		(((ACREV0) ? 0x128 : 0x13a) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE2		(((ACREV0) ? 0x129 : 0x13b) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE3		(((ACREV0) ? 0x12a : 0x13c) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE4		(((ACREV0) ? 0x12b : 0x13d) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE5		(((ACREV0) ? 0x12c : 0x13e) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE6		(((ACREV0) ? 0x12d : 0x13f) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE7		(((ACREV0) ? 0x12e : 0x140) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE8		(((ACREV0) ? 0x12f : 0x141) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE9		(((ACREV0) ? 0x130 : 0x142) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE10		(((ACREV0) ? 0x131 : 0x143) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE11		(((ACREV0) ? 0x132 : 0x144) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE12		(((ACREV0) ? 0x133 : 0x145) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE13		(((ACREV0) ? 0x134 : 0x146) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE14		(((ACREV0) ? 0x135 : 0x147) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE15		(((ACREV0) ? 0x136 : 0x148) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE16		(((ACREV0) ? 0x137 : 0x149) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE17		(((ACREV0) ? 0x138 : 0x14a) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE18		(((ACREV0) ? 0x139 : 0x14b) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE19		(((ACREV0) ? 0x13a : 0x14c) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE20		(((ACREV0) ? 0x13b : 0x14d) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE21		(((ACREV0) ? 0x13c : 0x14e) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE22		(((ACREV0) ? 0x13d : 0x14f) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE23		(((ACREV0) ? 0x13e : 0x150) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE24		(((ACREV0) ? 0x13f : 0x151) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE25		(((ACREV0) ? 0x140 : 0x152) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE26		(((ACREV0) ? 0x141 : 0x153) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE27		(((ACREV0) ? 0x142 : 0x154) | JTAG_2069_CR2)
#define RF2_2069_ADC_CALCODE28		(((ACREV0) ? 0x143 : 0x155) | JTAG_2069_CR2)
#define RF2_2069_ADC_STATUS		(((ACREV0) ? 0x144 : 0x156) | JTAG_2069_CR2)
#define RF2_2069_GPAIO_CFG1		(((ACREV0) ? 0x147 : 0x15a) | JTAG_2069_CR2)
#define RF2_2069_CGPAIO_CFG1		\
	(((ACREV0) ? 0x148 : 0x15b) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_CGPAIO_CFG2		\
	(((ACREV0) ? 0x149 : 0x15c) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_CGPAIO_CFG3		\
	(((ACREV0) ? 0x14a : 0x15d) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_CGPAIO_CFG4		\
	(((ACREV0) ? 0x14b : 0x15e) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_CGPAIO_CFG5		\
	(((ACREV0) ? 0x14c : 0x15f) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_RADIO_SPARE1		(((ACREV0) ? 0x157 : 0x16a) | JTAG_2069_CR2)
#define RF2_2069_RADIO_SPARE2		(((ACREV0) ? 0x158 : 0x16b) | JTAG_2069_CR2)
#define RF2_2069_RADIO_SPARE3		(((ACREV0) ? 0x159 : 0x16c) | JTAG_2069_CR2)
#define RF2_2069_RADIO_SPARE4		(((ACREV0) ? 0x15a : 0x16d) | JTAG_2069_CR2)
#define RF2_2069_OVR1		(((ACREV0) ? 0x15d : 0x170) | JTAG_2069_CR2)
#define RF2_2069_OVR2		\
	(((ACREV0) ? 0x15e : 0x171) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_OVR3		(((ACREV0) ? 0x15f : 0x172) | JTAG_2069_CR2)
#define RF2_2069_OVR4		(((ACREV0) ? 0x160 : 0x173) | JTAG_2069_CR2)
#define RF2_2069_OVR5		(((ACREV0) ? 0x161 : 0x174) | JTAG_2069_CR2)
#define RF2_2069_OVR6		(((ACREV0) ? 0x162 : 0x175) | JTAG_2069_CR2)
#define RF2_2069_OVR7		(((ACREV0) ? 0x163 : 0x176) | JTAG_2069_CR2)
#define RF2_2069_OVR9		\
	(((ACREV0) ? 0x165 : 0x178) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_OVR10		(((ACREV0) ? 0x166 : 0x179) | JTAG_2069_CR2)
#define RF2_2069_OVR11		(((ACREV0) ? 0x167 : 0x17a) | JTAG_2069_CR2)
#define RF2_2069_OVR12		(((ACREV0) ? 0x168 : 0x17b) | JTAG_2069_CR2)
#define RF2_2069_OVR13		(((ACREV0) ? 0x169 : 0x17c) | JTAG_2069_CR2)
#define RF2_2069_OVR18		\
	(((ACREV0) ? 0x16e : 0x181) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_OVR19		(((ACREV0) ? 0x16f : 0x182) | JTAG_2069_CR2)
#define RF2_2069_OVR20		(((ACREV0) ? 0x170 : 0x183) | JTAG_2069_CR2)
#define RF2_2069_OVR21		(((ACREV0) ? 0x171 : 0x184) | JTAG_2069_CR2)
#define RF2_2069_OVR23		\
	(((ACREV0) ? 0x173 : 0x186) | ((ACREV0) ? JTAG_2069_CR2 : JTAG_2069_PLL))
#define RF2_2069_DAC_CFG3                        (0x174 | JTAG_2069_CR2)
#define RF2_2069_GP_REGISTER                     (0x178 | JTAG_2069_CR2)
#define RFP_2069_REV_ID		(((ACREV0) ? 0x0 : 0x0) | JTAG_2069_PLL)
#define RFP_2069_DEV_ID		(((ACREV0) ? 0x1 : 0x1) | JTAG_2069_PLL)
#define RFP_2069_PMU_STAT	(((ACREV0) ? 0xb : 0xb) | JTAG_2069_PLL)
#define RFP_2069_PMU_OP		(((ACREV0) ? 0xb : 0xc) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTALLDO1		(((ACREV0) ? 0x85 : 0x8c) | JTAG_2069_PLL)
#define RFP_2069_PLL_HVLDO1		(((ACREV0) ? 0x86 : 0x8d) | JTAG_2069_PLL)
#define RFP_2069_PLL_HVLDO2		(((ACREV0) ? 0x87 : 0x8e) | JTAG_2069_PLL)
#define RFP_2069_PLL_HVLDO3		(((ACREV0) ? 0x88 : 0x8f) | JTAG_2069_PLL)
#define RFP_2069_PLL_CP1		(((ACREV0) ? 0x8f : 0x97) | JTAG_2069_PLL)
#define RFP_2069_PLL_CP2		(((ACREV0) ? 0x90 : 0x98) | JTAG_2069_PLL)
#define RFP_2069_PLL_CP3		(((ACREV0) ? 0x91 : 0x99) | JTAG_2069_PLL)
#define RFP_2069_PLL_CP4		(((ACREV0) ? 0x92 : 0x9a) | JTAG_2069_PLL)
#define RFP_2069_PLL_CP5		(((ACREV0) ? 0x93 : 0x9b) | JTAG_2069_PLL)
#define RFP_2069_PLL_FRCT1		(((ACREV0) ? 0xc3 : 0xcb) | JTAG_2069_PLL)
#define RFP_2069_PLL_FRCT2		(((ACREV0) ? 0xc4 : 0xcc) | JTAG_2069_PLL)
#define RFP_2069_PLL_FRCT3		(((ACREV0) ? 0xc5 : 0xcd) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF1		(((ACREV0) ? 0xc6 : 0xce) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF2		(((ACREV0) ? 0xc7 : 0xcf) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF3		(((ACREV0) ? 0xc8 : 0xd0) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF4		(((ACREV0) ? 0xc9 : 0xd1) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF5		(((ACREV0) ? 0xca : 0xd2) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF6		(((ACREV0) ? 0xcb : 0xd3) | JTAG_2069_PLL)
#define RFP_2069_PLL_LF7		(((ACREV0) ? 0xcc : 0xd4) | JTAG_2069_PLL)
#define RFP_2069_PLL_MMD1		(((ACREV0) ? 0xcd : 0xd5) | JTAG_2069_PLL)
#define RFP_2069_PLL_MONITOR1		(((ACREV0) ? 0xce : 0xd6) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG1		(((ACREV0) ? 0xcf : 0xd7) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG2		(((ACREV0) ? 0xd0 : 0xd8) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG3		(((ACREV0) ? 0xd1 : 0xd9) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG4		(((ACREV0) ? 0xd2 : 0xda) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG5		(((ACREV0) ? 0xd3 : 0xdb) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFG6		(((ACREV0) ? 0xd4 : 0xdc) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO1		(((ACREV0) ? 0xd5 : 0xdd) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO2		(((ACREV0) ? 0xd6 : 0xde) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO3		(((ACREV0) ? 0xd7 : 0xdf) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO4		(((ACREV0) ? 0xd8 : 0xe0) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO5		(((ACREV0) ? 0xd9 : 0xe1) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO6		(((ACREV0) ? 0xda : 0xe2) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCO8		(((ACREV0) ? 0xdb : 0xe3) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL1		(((ACREV0) ? 0xdc : 0xe4) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL2		(((ACREV0) ? 0xdd : 0xe5) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL3		(((ACREV0) ? 0xde : 0xe6) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL4		(((ACREV0) ? 0xdf : 0xe7) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL5		(((ACREV0) ? 0xe0 : 0xe8) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL6		(((ACREV0) ? 0xe1 : 0xe9) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL7		(((ACREV0) ? 0xe2 : 0xea) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL8		(((ACREV0) ? 0xe3 : 0xeb) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL9		(((ACREV0) ? 0xe4 : 0xec) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL10		(((ACREV0) ? 0xe5 : 0xed) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL11		(((ACREV0) ? 0xe6 : 0xee) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL12		(((ACREV0) ? 0xe7 : 0xef) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL13		(((ACREV0) ? 0xe8 : 0xf0) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL1		(((ACREV0) ? 0xe9 : 0xf1) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL2		(((ACREV0) ? 0xea : 0xf2) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL3		(((ACREV0) ? 0xeb : 0xf3) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL4		(((ACREV0) ? 0xec : 0xf4) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL5		(((ACREV0) ? 0xed : 0xf5) | JTAG_2069_PLL)
#define RFP_2069_PLL_DSPR27		(((ACREV0) ? 0x108 : 0x11a) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFGR1		(((ACREV0) ? 0x109 : 0x11b) | JTAG_2069_PLL)
#define RFP_2069_PLL_CFGR2		(((ACREV0) ? 0x10a : 0x11c) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL14		(((ACREV0) ? 0x10b : 0x11d) | JTAG_2069_PLL)
#define RFP_2069_PLL_VCOCAL15		(((ACREV0) ? 0x10c : 0x11e) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE1		(((ACREV0) ? 0x14d : 0x160) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE2		(((ACREV0) ? 0x14e : 0x161) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE3		(((ACREV0) ? 0x14f : 0x162) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE4		(((ACREV0) ? 0x150 : 0x163) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE5		(((ACREV0) ? 0x151 : 0x164) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE6		(((ACREV0) ? 0x152 : 0x165) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE7		(((ACREV0) ? 0x153 : 0x166) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE8		(((ACREV0) ? 0x154 : 0x167) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE9		(((ACREV0) ? 0x155 : 0x168) | JTAG_2069_PLL)
#define RFP_2069_TOP_SPARE10		(((ACREV0) ? 0x156 : 0x169) | JTAG_2069_PLL)
#define RFP_2069_OVR15		(((ACREV0) ? 0x16b : 0x17e) | JTAG_2069_PLL)
#define RFP_2069_OVR16		(((ACREV0) ? 0x16c : 0x17f) | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL8                       (0x175 | JTAG_2069_PLL)
#define RFP_2069_PLL_XTAL9                       (0x177 | JTAG_2069_PLL)
#define RFP_2069_GP_REGISTER                     (0x178 | JTAG_2069_PLL)
#define RFX_2069_REV_ID		(((ACREV0) ? 0x0 : 0x0) | JTAG_2069_ALL)
#define RFX_2069_DEV_ID		(((ACREV0) ? 0x1 : 0x1) | JTAG_2069_ALL)
#define RFX_2069_TXGM_LOFT_FINE_I		(((ACREV0) ? 0x2 : 0x2) | JTAG_2069_ALL)
#define RFX_2069_TXGM_LOFT_FINE_Q		(((ACREV0) ? 0x3 : 0x3) | JTAG_2069_ALL)
#define RFX_2069_TXGM_LOFT_COARSE_I		(((ACREV0) ? 0x4 : 0x4) | JTAG_2069_ALL)
#define RFX_2069_TXGM_LOFT_COARSE_Q		(((ACREV0) ? 0x5 : 0x5) | JTAG_2069_ALL)
#define RFX_2069_TXGM_LOFT_SCALE		(((ACREV0) ? 0x6 : 0x6) | JTAG_2069_ALL)
#define RFX_2069_TEMPSENSE_CFG		(((ACREV0) ? 0xe : 0x13) | JTAG_2069_ALL)
#define RFX_2069_VBAT_CFG		(((ACREV0) ? 0xf : 0x14) | JTAG_2069_ALL)
#define RFX_2069_TESTBUF_CFG1		(((ACREV0) ? 0x17 : 0x1c) | JTAG_2069_ALL)
#define RFX_2069_GPAIO_SEL0		(((ACREV0) ? 0x18 : 0x1d) | JTAG_2069_ALL)
#define RFX_2069_GPAIO_SEL1		(((ACREV0) ? 0x19 : 0x1e) | JTAG_2069_ALL)
#define RFX_2069_IQCAL_CFG1		(((ACREV0) ? 0x1a : 0x1f) | JTAG_2069_ALL)
#define RFX_2069_IQCAL_CFG2		(((ACREV0) ? 0x1b : 0x20) | JTAG_2069_ALL)
#define RFX_2069_IQCAL_CFG3		(((ACREV0) ? 0x1c : 0x21) | JTAG_2069_ALL)
#define RFX_2069_IQCAL_IDAC		(((ACREV0) ? 0x1d : 0x22) | JTAG_2069_ALL)
#define RFX_2069_PA2G_TSSI		(((ACREV0) ? 0x1e : 0x23) | JTAG_2069_ALL)
#define RFX_2069_TX5G_TSSI		(((ACREV0) ? 0x1f : 0x24) | JTAG_2069_ALL)
#define RFX_2069_TXRX2G_CAL_TX		(((ACREV0) ? 0x20 : 0x25) | JTAG_2069_ALL)
#define RFX_2069_TXRX5G_CAL_TX		(((ACREV0) ? 0x21 : 0x26) | JTAG_2069_ALL)
#define RFX_2069_TXRX2G_CAL_RX		(((ACREV0) ? 0x22 : 0x27) | JTAG_2069_ALL)
#define RFX_2069_TXRX5G_CAL_RX		(((ACREV0) ? 0x23 : 0x28) | JTAG_2069_ALL)
#define RFX_2069_AUXPGA_CFG1		(((ACREV0) ? 0x24 : 0x29) | JTAG_2069_ALL)
#define RFX_2069_AUXPGA_VMID		(((ACREV0) ? 0x25 : 0x2a) | JTAG_2069_ALL)
#define RFX_2069_CORE_BIAS		(((ACREV0) ? 0x26 : 0x2b) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_CFG1		(((ACREV0) ? 0x27 : 0x2c) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_CFG2		(((ACREV0) ? 0x28 : 0x2d) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_TUNE		(((ACREV0) ? 0x29 : 0x2f) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_IDAC1		(((ACREV0) ? 0x2a : 0x30) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_IDAC2		(((ACREV0) ? 0x2b : 0x31) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_RSSI		(((ACREV0) ? 0x2c : 0x32) | JTAG_2069_ALL)
#define RFX_2069_LNA2G_RSSI_ANA		(((ACREV0) ? 0x2d : 0x33) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_CFG1		(((ACREV0) ? 0x2e : 0x34) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_CFG2		(((ACREV0) ? 0x2f : 0x35) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_TUNE		(((ACREV0) ? 0x30 : 0x37) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_IDAC1		(((ACREV0) ? 0x31 : 0x38) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_IDAC2		(((ACREV0) ? 0x32 : 0x39) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_RSSI		(((ACREV0) ? 0x33 : 0x3a) | JTAG_2069_ALL)
#define RFX_2069_LNA5G_RSSI_ANA		(((ACREV0) ? 0x34 : 0x3b) | JTAG_2069_ALL)
#define RFX_2069_RXMIX2G_CFG1		(((ACREV0) ? 0x35 : 0x3c) | JTAG_2069_ALL)
#define RFX_2069_RXMIX2G_IDAC		(((ACREV0) ? 0x36 : 0x3d) | JTAG_2069_ALL)
#define RFX_2069_RXMIX5G_CFG1		(((ACREV0) ? 0x37 : 0x3e) | JTAG_2069_ALL)
#define RFX_2069_RXMIX5G_IDAC		(((ACREV0) ? 0x38 : 0x3f) | JTAG_2069_ALL)
#define RFX_2069_RXRF2G_CFG1		(((ACREV0) ? 0x39 : 0x40) | JTAG_2069_ALL)
#define RFX_2069_RXRF2G_CFG2		(((ACREV0) ? 0x3a : 0x41) | JTAG_2069_ALL)
#define RFX_2069_RXRF2G_SPARE		(((ACREV0) ? 0x3b : 0x42) | JTAG_2069_ALL)
#define RFX_2069_RXRF5G_CFG1		(((ACREV0) ? 0x3c : 0x43) | JTAG_2069_ALL)
#define RFX_2069_RXRF5G_CFG2		(((ACREV0) ? 0x3d : 0x44) | JTAG_2069_ALL)
#define RFX_2069_RX_REG_BACKUP_1		(((ACREV0) ? 0x3e : 0x45) | JTAG_2069_ALL)
#define RFX_2069_RX_REG_BACKUP_2		(((ACREV0) ? 0x3f : 0x46) | JTAG_2069_ALL)
#define RFX_2069_RXRF5G_SPARE		(((ACREV0) ? 0x40 : 0x47) | JTAG_2069_ALL)
#define RFX_2069_TIA_CFG1		(((ACREV0) ? 0x41 : 0x48) | JTAG_2069_ALL)
#define RFX_2069_TIA_CFG2		(((ACREV0) ? 0x42 : 0x49) | JTAG_2069_ALL)
#define RFX_2069_TIA_CFG3		(((ACREV0) ? 0x43 : 0x4a) | JTAG_2069_ALL)
#define RFX_2069_TIA_IDAC1		(((ACREV0) ? 0x44 : 0x4b) | JTAG_2069_ALL)
#define RFX_2069_NBRSSI_CONFG		(((ACREV0) ? 0x45 : 0x4c) | JTAG_2069_ALL)
#define RFX_2069_NBRSSI_BIAS		(((ACREV0) ? 0x46 : 0x4d) | JTAG_2069_ALL)
#define RFX_2069_NBRSSI_IB		(((ACREV0) ? 0x47 : 0x4e) | JTAG_2069_ALL)
#define RFX_2069_NBRSSI_TEST		(((ACREV0) ? 0x48 : 0x4f) | JTAG_2069_ALL)
#define RFX_2069_WRSSI3_CONFG		(((ACREV0) ? 0x49 : 0x50) | JTAG_2069_ALL)
#define RFX_2069_WRSSI3_BIAS		(((ACREV0) ? 0x4a : 0x51) | JTAG_2069_ALL)
#define RFX_2069_WRSSI3_TEST		(((ACREV0) ? 0x4b : 0x52) | JTAG_2069_ALL)
#define RFX_2069_LPF_MUX_SWITCHES		(((ACREV0) ? 0x4c : 0x53) | JTAG_2069_ALL)
#define RFX_2069_LPF_ENABLES		(((ACREV0) ? 0x4d : 0x54) | JTAG_2069_ALL)
#define RFX_2069_LPF_MAIN_CONTROLS		(((ACREV0) ? 0x4e : 0x55) | JTAG_2069_ALL)
#define RFX_2069_LPF_CORNER_FREQUENCY_TUNING		(((ACREV0) ? 0x4f : 0x56) | JTAG_2069_ALL)
#define RFX_2069_LPF_DC_LOOP_AND_MISC		(((ACREV0) ? 0x50 : 0x57) | JTAG_2069_ALL)
#define RFX_2069_LPF_BIAS_LEVELS_LOW		(((ACREV0) ? 0x51 : 0x58) | JTAG_2069_ALL)
#define RFX_2069_LPF_BIAS_LEVELS_MID		(((ACREV0) ? 0x52 : 0x59) | JTAG_2069_ALL)
#define RFX_2069_LPF_BIAS_LEVELS_HIGH		(((ACREV0) ? 0x53 : 0x5a) | JTAG_2069_ALL)
#define RFX_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS	\
	(((ACREV0) ? 0x54 : 0x5b) | JTAG_2069_ALL)
#define RFX_2069_LPF_COMMON_MODE_REFERENCE_LEVELS	(((ACREV0) ? 0x55 : 0x5c) | JTAG_2069_ALL)
#define RFX_2069_TX2G_CFG1		(((ACREV0) ? 0x56 : 0x5d) | JTAG_2069_ALL)
#define RFX_2069_TX2G_CFG2		(((ACREV0) ? 0x57 : 0x5e) | JTAG_2069_ALL)
#define RFX_2069_TX5G_CFG1		(((ACREV0) ? 0x58 : 0x5f) | JTAG_2069_ALL)
#define RFX_2069_TXGM_CFG1		(((ACREV0) ? 0x59 : 0x60) | JTAG_2069_ALL)
#define RFX_2069_TXGM_CFG2		(((ACREV0) ? 0x5a : 0x61) | JTAG_2069_ALL)
#define RFX_2069_TXMIX2G_CFG1		(((ACREV0) ? 0x5b : 0x62) | JTAG_2069_ALL)
#define RFX_2069_TXMIX5G_CFG1		(((ACREV0) ? 0x5c : 0x63) | JTAG_2069_ALL)
#define RFX_2069_PGA2G_CFG1		(((ACREV0) ? 0x5d : 0x64) | JTAG_2069_ALL)
#define RFX_2069_PGA2G_CFG2		(((ACREV0) ? 0x5e : 0x65) | JTAG_2069_ALL)
#define RFX_2069_PGA2G_IDAC		(((ACREV0) ? 0x5f : 0x66) | JTAG_2069_ALL)
#define RFX_2069_PGA2G_INCAP		(((ACREV0) ? 0x60 : 0x67) | JTAG_2069_ALL)
#define RFX_2069_PGA5G_CFG1		(((ACREV0) ? 0x61 : 0x68) | JTAG_2069_ALL)
#define RFX_2069_PGA5G_CFG2		(((ACREV0) ? 0x62 : 0x69) | JTAG_2069_ALL)
#define RFX_2069_PGA5G_IDAC		(((ACREV0) ? 0x63 : 0x6a) | JTAG_2069_ALL)
#define RFX_2069_PGA5G_INCAP		(((ACREV0) ? 0x64 : 0x6b) | JTAG_2069_ALL)
#define RFX_2069_PAD2G_CFG1		(((ACREV0) ? 0x65 : 0x6c) | JTAG_2069_ALL)
#define RFX_2069_PAD2G_IDAC		(((ACREV0) ? 0x66 : 0x6d) | JTAG_2069_ALL)
#define RFX_2069_PAD2G_SLOPE		(((ACREV0) ? 0x67 : 0x6e) | JTAG_2069_ALL)
#define RFX_2069_PAD2G_TUNE		(((ACREV0) ? 0x68 : 0x6f) | JTAG_2069_ALL)
#define RFX_2069_PAD2G_INCAP		(((ACREV0) ? 0x69 : 0x70) | JTAG_2069_ALL)
#define RFX_2069_PAD5G_CFG1		(((ACREV0) ? 0x6a : 0x71) | JTAG_2069_ALL)
#define RFX_2069_PAD5G_IDAC		(((ACREV0) ? 0x6b : 0x72) | JTAG_2069_ALL)
#define RFX_2069_PAD5G_SLOPE		(((ACREV0) ? 0x6c : 0x73) | JTAG_2069_ALL)
#define RFX_2069_PAD5G_TUNE		(((ACREV0) ? 0x6d : 0x74) | JTAG_2069_ALL)
#define RFX_2069_PAD5G_INCAP		(((ACREV0) ? 0x6e : 0x75) | JTAG_2069_ALL)
#define RFX_2069_PA2G_CFG1		(((ACREV0) ? 0x6f : 0x76) | JTAG_2069_ALL)
#define RFX_2069_PA2G_CFG2		(((ACREV0) ? 0x70 : 0x77) | JTAG_2069_ALL)
#define RFX_2069_PA2G_CFG3		(((ACREV0) ? 0x71 : 0x78) | JTAG_2069_ALL)
#define RFX_2069_PA2G_IDAC1		(((ACREV0) ? 0x72 : 0x79) | JTAG_2069_ALL)
#define RFX_2069_PA2G_IDAC2		(((ACREV0) ? 0x73 : 0x7a) | JTAG_2069_ALL)
#define RFX_2069_PA2G_INCAP		(((ACREV0) ? 0x74 : 0x7b) | JTAG_2069_ALL)
#define RFX_2069_PA5G_CFG1		(((ACREV0) ? 0x75 : 0x7c) | JTAG_2069_ALL)
#define RFX_2069_PA5G_CFG2		(((ACREV0) ? 0x76 : 0x7d) | JTAG_2069_ALL)
#define RFX_2069_PA5G_CFG3		(((ACREV0) ? 0x77 : 0x7e) | JTAG_2069_ALL)
#define RFX_2069_PA5G_IDAC1		(((ACREV0) ? 0x78 : 0x7f) | JTAG_2069_ALL)
#define RFX_2069_PA5G_IDAC2		(((ACREV0) ? 0x79 : 0x80) | JTAG_2069_ALL)
#define RFX_2069_PA5G_INCAP		(((ACREV0) ? 0x7a : 0x81) | JTAG_2069_ALL)
#define RFX_2069_TRSW2G_CFG1		(((ACREV0) ? 0x7b : 0x82) | JTAG_2069_ALL)
#define RFX_2069_TRSW2G_CFG2		(((ACREV0) ? 0x7c : 0x83) | JTAG_2069_ALL)
#define RFX_2069_TRSW2G_CFG3		(((ACREV0) ? 0x7d : 0x84) | JTAG_2069_ALL)
#define RFX_2069_TRSW5G_CFG1		(((ACREV0) ? 0x7e : 0x85) | JTAG_2069_ALL)
#define RFX_2069_TRSW5G_CFG2		(((ACREV0) ? 0x7f : 0x86) | JTAG_2069_ALL)
#define RFX_2069_TRSW5G_CFG3		(((ACREV0) ? 0x80 : 0x87) | JTAG_2069_ALL)
#define RFX_2069_TX_REG_BACKUP_1		(((ACREV0) ? 0x81 : 0x88) | JTAG_2069_ALL)
#define RFX_2069_TX_REG_BACKUP_2		(((ACREV0) ? 0x82 : 0x89) | JTAG_2069_ALL)
#define RFX_2069_TX_REG_BACKUP_3		(((ACREV0) ? 0x83 : 0x8a) | JTAG_2069_ALL)
#define RFX_2069_TX_REG_BACKUP_4		(((ACREV0) ? 0x84 : 0x8b) | JTAG_2069_ALL)
#define RFX_2069_LOGEN2G_CFG2		(((ACREV0) ? 0x10e : 0x120) | JTAG_2069_ALL)
#define RFX_2069_LOGEN2G_IDAC3		(((ACREV0) ? 0x110 : 0x122) | JTAG_2069_ALL)
#define RFX_2069_LOGEN2G_RCCR		(((ACREV0) ? 0x111 : 0x123) | JTAG_2069_ALL)
#define RFX_2069_LOGEN5G_CFG2		(((ACREV0) ? 0x114 : 0x126) | JTAG_2069_ALL)
#define RFX_2069_LOGEN5G_IDAC4		(((ACREV0) ? 0x118 : 0x12a) | JTAG_2069_ALL)
#define RFX_2069_LOGEN5G_RCCR		(((ACREV0) ? 0x119 : 0x12b) | JTAG_2069_ALL)
#define RFX_2069_DAC_CFG1		(((ACREV0) ? 0x11c : 0x12e) | JTAG_2069_ALL)
#define RFX_2069_DAC_CFG2		(((ACREV0) ? 0x11d : 0x12f) | JTAG_2069_ALL)
#define RFX_2069_DAC_BIAS		(((ACREV0) ? 0x11e : 0x130) | JTAG_2069_ALL)
#define RFX_2069_ADC_CFG1		(((ACREV0) ? 0x11f : 0x131) | JTAG_2069_ALL)
#define RFX_2069_ADC_CFG2		(((ACREV0) ? 0x120 : 0x132) | JTAG_2069_ALL)
#define RFX_2069_ADC_CFG3		(((ACREV0) ? 0x121 : 0x133) | JTAG_2069_ALL)
#define RFX_2069_ADC_CFG4		(((ACREV0) ? 0x122 : 0x134) | JTAG_2069_ALL)
#define RFX_2069_ADC_CFG5		(((ACREV0) ? 0x123 : 0x135) | JTAG_2069_ALL)
#define RFX_2069_ADC_BIAS1		(((ACREV0) ? 0x124 : 0x136) | JTAG_2069_ALL)
#define RFX_2069_ADC_BIAS2		(((ACREV0) ? 0x125 : 0x137) | JTAG_2069_ALL)
#define RFX_2069_ADC_RC1		(((ACREV0) ? 0x126 : 0x138) | JTAG_2069_ALL)
#define RFX_2069_ADC_RC2		(((ACREV0) ? 0x127 : 0x139) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE1		(((ACREV0) ? 0x128 : 0x13a) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE2		(((ACREV0) ? 0x129 : 0x13b) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE3		(((ACREV0) ? 0x12a : 0x13c) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE4		(((ACREV0) ? 0x12b : 0x13d) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE5		(((ACREV0) ? 0x12c : 0x13e) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE6		(((ACREV0) ? 0x12d : 0x13f) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE7		(((ACREV0) ? 0x12e : 0x140) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE8		(((ACREV0) ? 0x12f : 0x141) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE9		(((ACREV0) ? 0x130 : 0x142) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE10		(((ACREV0) ? 0x131 : 0x143) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE11		(((ACREV0) ? 0x132 : 0x144) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE12		(((ACREV0) ? 0x133 : 0x145) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE13		(((ACREV0) ? 0x134 : 0x146) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE14		(((ACREV0) ? 0x135 : 0x147) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE15		(((ACREV0) ? 0x136 : 0x148) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE16		(((ACREV0) ? 0x137 : 0x149) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE17		(((ACREV0) ? 0x138 : 0x14a) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE18		(((ACREV0) ? 0x139 : 0x14b) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE19		(((ACREV0) ? 0x13a : 0x14c) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE20		(((ACREV0) ? 0x13b : 0x14d) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE21		(((ACREV0) ? 0x13c : 0x14e) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE22		(((ACREV0) ? 0x13d : 0x14f) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE23		(((ACREV0) ? 0x13e : 0x150) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE24		(((ACREV0) ? 0x13f : 0x151) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE25		(((ACREV0) ? 0x140 : 0x152) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE26		(((ACREV0) ? 0x141 : 0x153) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE27		(((ACREV0) ? 0x142 : 0x154) | JTAG_2069_ALL)
#define RFX_2069_ADC_CALCODE28		(((ACREV0) ? 0x143 : 0x155) | JTAG_2069_ALL)
#define RFX_2069_ADC_STATUS		(((ACREV0) ? 0x144 : 0x156) | JTAG_2069_ALL)
#define RFX_2069_GPAIO_CFG1		(((ACREV0) ? 0x147 : 0x15a) | JTAG_2069_ALL)
#define RFX_2069_RADIO_SPARE1		(((ACREV0) ? 0x157 : 0x16a) | JTAG_2069_ALL)
#define RFX_2069_RADIO_SPARE2		(((ACREV0) ? 0x158 : 0x16b) | JTAG_2069_ALL)
#define RFX_2069_RADIO_SPARE3		(((ACREV0) ? 0x159 : 0x16c) | JTAG_2069_ALL)
#define RFX_2069_RADIO_SPARE4		(((ACREV0) ? 0x15a : 0x16d) | JTAG_2069_ALL)
#define RFX_2069_OVR1                            (0x15d | JTAG_2069_ALL)
#define RFX_2069_OVR3		(((ACREV0) ? 0x15f : 0x172) | JTAG_2069_ALL)
#define RFX_2069_OVR4		(((ACREV0) ? 0x160 : 0x173) | JTAG_2069_ALL)
#define RFX_2069_OVR5		(((ACREV0) ? 0x161 : 0x174) | JTAG_2069_ALL)
#define RFX_2069_OVR6		(((ACREV0) ? 0x162 : 0x175) | JTAG_2069_ALL)
#define RFX_2069_OVR7		(((ACREV0) ? 0x163 : 0x176) | JTAG_2069_ALL)
#define RFX_2069_OVR9		\
	(((ACREV0) ? 0x165 : 0x178) | ((ACREV0) ? JTAG_2069_ALL : JTAG_2069_PLL))
#define RFX_2069_OVR10		(((ACREV0) ? 0x166 : 0x179) | JTAG_2069_ALL)
#define RFX_2069_OVR11		(((ACREV0) ? 0x167 : 0x17a) | JTAG_2069_ALL)
#define RFX_2069_OVR12		(((ACREV0) ? 0x168 : 0x17b) | JTAG_2069_ALL)
#define RFX_2069_OVR13		(((ACREV0) ? 0x169 : 0x17c) | JTAG_2069_ALL)
#define RFX_2069_OVR18		\
	(((ACREV0) ? 0x16e : 0x181) | ((ACREV0) ? JTAG_2069_ALL : JTAG_2069_PLL))
#define RFX_2069_OVR19		(((ACREV0) ? 0x16f : 0x182) | JTAG_2069_ALL)
#define RFX_2069_OVR20		(((ACREV0) ? 0x170 : 0x183) | JTAG_2069_ALL)
#define RFX_2069_OVR21		(((ACREV0) ? 0x171 : 0x184) | JTAG_2069_ALL)
#define RFX_2069_OVR23		\
	(((ACREV0) ? 0x173 : 0x186) | ((ACREV0) ? JTAG_2069_ALL : JTAG_2069_PLL))
#define RFX_2069_DAC_CFG3                        (0x174 | JTAG_2069_ALL)
#define RFX_2069_GP_REGISTER                     (0x178 | JTAG_2069_ALL)
#define RF_2069_REV_ID(core)		(((ACREV0) ? 0x0 : 0x0) | (core << 9))
#define RF_2069_DEV_ID(core)		(((ACREV0) ? 0x1 : 0x1) | (core << 9))
#define RF_2069_TXGM_LOFT_FINE_I(core)		(((ACREV0) ? 0x2 : 0x2) | (core << 9))
#define RF_2069_TXGM_LOFT_FINE_Q(core)		(((ACREV0) ? 0x3 : 0x3) | (core << 9))
#define RF_2069_TXGM_LOFT_COARSE_I(core)		(((ACREV0) ? 0x4 : 0x4) | (core << 9))
#define RF_2069_TXGM_LOFT_COARSE_Q(core)		(((ACREV0) ? 0x5 : 0x5) | (core << 9))
#define RF_2069_TXGM_LOFT_SCALE(core)		(((ACREV0) ? 0x6 : 0x6) | (core << 9))
#define RF_2069_TEMPSENSE_CFG(core)		(((ACREV0) ? 0xe : 0x13) | (core << 9))
#define RF_2069_VBAT_CFG(core)		(((ACREV0) ? 0xf : 0x14) | (core << 9))
#define RF_2069_TESTBUF_CFG1(core)		(((ACREV0) ? 0x17 : 0x1c) | (core << 9))
#define RF_2069_GPAIO_SEL0(core)		(((ACREV0) ? 0x18 : 0x1d) | (core << 9))
#define RF_2069_GPAIO_SEL1(core)		(((ACREV0) ? 0x19 : 0x1e) | (core << 9))
#define RF_2069_IQCAL_CFG1(core)		(((ACREV0) ? 0x1a : 0x1f) | (core << 9))
#define RF_2069_IQCAL_CFG2(core)		(((ACREV0) ? 0x1b : 0x20) | (core << 9))
#define RF_2069_IQCAL_CFG3(core)		(((ACREV0) ? 0x1c : 0x21) | (core << 9))
#define RF_2069_IQCAL_IDAC(core)		(((ACREV0) ? 0x1d : 0x22) | (core << 9))
#define RF_2069_PA2G_TSSI(core)		(((ACREV0) ? 0x1e : 0x23) | (core << 9))
#define RF_2069_TX5G_TSSI(core)		(((ACREV0) ? 0x1f : 0x24) | (core << 9))
#define RF_2069_TXRX2G_CAL_TX(core)		(((ACREV0) ? 0x20 : 0x25) | (core << 9))
#define RF_2069_TXRX5G_CAL_TX(core)		(((ACREV0) ? 0x21 : 0x26) | (core << 9))
#define RF_2069_TXRX2G_CAL_RX(core)		(((ACREV0) ? 0x22 : 0x27) | (core << 9))
#define RF_2069_TXRX5G_CAL_RX(core)		(((ACREV0) ? 0x23 : 0x28) | (core << 9))
#define RF_2069_AUXPGA_CFG1(core)		(((ACREV0) ? 0x24 : 0x29) | (core << 9))
#define RF_2069_AUXPGA_VMID(core)		(((ACREV0) ? 0x25 : 0x2a) | (core << 9))
#define RF_2069_CORE_BIAS(core)		(((ACREV0) ? 0x26 : 0x2b) | (core << 9))
#define RF_2069_LNA2G_CFG1(core)		(((ACREV0) ? 0x27 : 0x2c) | (core << 9))
#define RF_2069_LNA2G_CFG2(core)		(((ACREV0) ? 0x28 : 0x2d) | (core << 9))
#define RF_2069_LNA2G_TUNE(core)		(((ACREV0) ? 0x29 : 0x2f) | (core << 9))
#define RF_2069_LNA2G_IDAC1(core)		(((ACREV0) ? 0x2a : 0x30) | (core << 9))
#define RF_2069_LNA2G_IDAC2(core)		(((ACREV0) ? 0x2b : 0x31) | (core << 9))
#define RF_2069_LNA2G_RSSI(core)		(((ACREV0) ? 0x2c : 0x32) | (core << 9))
#define RF_2069_LNA2G_RSSI_ANA(core)		(((ACREV0) ? 0x2d : 0x33) | (core << 9))
#define RF_2069_LNA5G_CFG1(core)		(((ACREV0) ? 0x2e : 0x34) | (core << 9))
#define RF_2069_LNA5G_CFG2(core)		(((ACREV0) ? 0x2f : 0x35) | (core << 9))
#define RF_2069_LNA5G_TUNE(core)		(((ACREV0) ? 0x30 : 0x37) | (core << 9))
#define RF_2069_LNA5G_IDAC1(core)		(((ACREV0) ? 0x31 : 0x38) | (core << 9))
#define RF_2069_LNA5G_IDAC2(core)		(((ACREV0) ? 0x32 : 0x39) | (core << 9))
#define RF_2069_LNA5G_RSSI(core)		(((ACREV0) ? 0x33 : 0x3a) | (core << 9))
#define RF_2069_LNA5G_RSSI_ANA(core)		(((ACREV0) ? 0x34 : 0x3b) | (core << 9))
#define RF_2069_RXMIX2G_CFG1(core)		(((ACREV0) ? 0x35 : 0x3c) | (core << 9))
#define RF_2069_RXMIX2G_IDAC(core)		(((ACREV0) ? 0x36 : 0x3d) | (core << 9))
#define RF_2069_RXMIX5G_CFG1(core)		(((ACREV0) ? 0x37 : 0x3e) | (core << 9))
#define RF_2069_RXMIX5G_IDAC(core)		(((ACREV0) ? 0x38 : 0x3f) | (core << 9))
#define RF_2069_RXRF2G_CFG1(core)		(((ACREV0) ? 0x39 : 0x40) | (core << 9))
#define RF_2069_RXRF2G_CFG2(core)		(((ACREV0) ? 0x3a : 0x41) | (core << 9))
#define RF_2069_RXRF2G_SPARE(core)		(((ACREV0) ? 0x3b : 0x42) | (core << 9))
#define RF_2069_RXRF5G_CFG1(core)		(((ACREV0) ? 0x3c : 0x43) | (core << 9))
#define RF_2069_RXRF5G_CFG2(core)		(((ACREV0) ? 0x3d : 0x44) | (core << 9))
#define RF_2069_RX_REG_BACKUP_1(core)		(((ACREV0) ? 0x3e : 0x45) | (core << 9))
#define RF_2069_RX_REG_BACKUP_2(core)		(((ACREV0) ? 0x3f : 0x46) | (core << 9))
#define RF_2069_RXRF5G_SPARE(core)		(((ACREV0) ? 0x40 : 0x47) | (core << 9))
#define RF_2069_TIA_CFG1(core)		(((ACREV0) ? 0x41 : 0x48) | (core << 9))
#define RF_2069_TIA_CFG2(core)		(((ACREV0) ? 0x42 : 0x49) | (core << 9))
#define RF_2069_TIA_CFG3(core)		(((ACREV0) ? 0x43 : 0x4a) | (core << 9))
#define RF_2069_TIA_IDAC1(core)		(((ACREV0) ? 0x44 : 0x4b) | (core << 9))
#define RF_2069_NBRSSI_CONFG(core)		(((ACREV0) ? 0x45 : 0x4c) | (core << 9))
#define RF_2069_NBRSSI_BIAS(core)		(((ACREV0) ? 0x46 : 0x4d) | (core << 9))
#define RF_2069_NBRSSI_IB(core)		(((ACREV0) ? 0x47 : 0x4e) | (core << 9))
#define RF_2069_NBRSSI_TEST(core)		(((ACREV0) ? 0x48 : 0x4f) | (core << 9))
#define RF_2069_WRSSI3_CONFG(core)		(((ACREV0) ? 0x49 : 0x50) | (core << 9))
#define RF_2069_WRSSI3_BIAS(core)		(((ACREV0) ? 0x4a : 0x51) | (core << 9))
#define RF_2069_WRSSI3_TEST(core)		(((ACREV0) ? 0x4b : 0x52) | (core << 9))
#define RF_2069_LPF_MUX_SWITCHES(core)		(((ACREV0) ? 0x4c : 0x53) | (core << 9))
#define RF_2069_LPF_ENABLES(core)		(((ACREV0) ? 0x4d : 0x54) | (core << 9))
#define RF_2069_LPF_MAIN_CONTROLS(core)		(((ACREV0) ? 0x4e : 0x55) | (core << 9))
#define RF_2069_LPF_CORNER_FREQUENCY_TUNING(core)	(((ACREV0) ? 0x4f : 0x56) | (core << 9))
#define RF_2069_LPF_DC_LOOP_AND_MISC(core)		(((ACREV0) ? 0x50 : 0x57) | (core << 9))
#define RF_2069_LPF_BIAS_LEVELS_LOW(core)		(((ACREV0) ? 0x51 : 0x58) | (core << 9))
#define RF_2069_LPF_BIAS_LEVELS_MID(core)		(((ACREV0) ? 0x52 : 0x59) | (core << 9))
#define RF_2069_LPF_BIAS_LEVELS_HIGH(core)		(((ACREV0) ? 0x53 : 0x5a) | (core << 9))
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS(core)		\
	(((ACREV0) ? 0x54 : 0x5b) | (core << 9))
#define RF_2069_LPF_COMMON_MODE_REFERENCE_LEVELS(core)		\
	(((ACREV0) ? 0x55 : 0x5c) | (core << 9))
#define RF_2069_TX2G_CFG1(core)		(((ACREV0) ? 0x56 : 0x5d) | (core << 9))
#define RF_2069_TX2G_CFG2(core)		(((ACREV0) ? 0x57 : 0x5e) | (core << 9))
#define RF_2069_TX5G_CFG1(core)		(((ACREV0) ? 0x58 : 0x5f) | (core << 9))
#define RF_2069_TXGM_CFG1(core)		(((ACREV0) ? 0x59 : 0x60) | (core << 9))
#define RF_2069_TXGM_CFG2(core)		(((ACREV0) ? 0x5a : 0x61) | (core << 9))
#define RF_2069_TXMIX2G_CFG1(core)		(((ACREV0) ? 0x5b : 0x62) | (core << 9))
#define RF_2069_TXMIX5G_CFG1(core)		(((ACREV0) ? 0x5c : 0x63) | (core << 9))
#define RF_2069_PGA2G_CFG1(core)		(((ACREV0) ? 0x5d : 0x64) | (core << 9))
#define RF_2069_PGA2G_CFG2(core)		(((ACREV0) ? 0x5e : 0x65) | (core << 9))
#define RF_2069_PGA2G_IDAC(core)		(((ACREV0) ? 0x5f : 0x66) | (core << 9))
#define RF_2069_PGA2G_INCAP(core)		(((ACREV0) ? 0x60 : 0x67) | (core << 9))
#define RF_2069_PGA5G_CFG1(core)		(((ACREV0) ? 0x61 : 0x68) | (core << 9))
#define RF_2069_PGA5G_CFG2(core)		(((ACREV0) ? 0x62 : 0x69) | (core << 9))
#define RF_2069_PGA5G_IDAC(core)		(((ACREV0) ? 0x63 : 0x6a) | (core << 9))
#define RF_2069_PGA5G_INCAP(core)		(((ACREV0) ? 0x64 : 0x6b) | (core << 9))
#define RF_2069_PAD2G_CFG1(core)		(((ACREV0) ? 0x65 : 0x6c) | (core << 9))
#define RF_2069_PAD2G_IDAC(core)		(((ACREV0) ? 0x66 : 0x6d) | (core << 9))
#define RF_2069_PAD2G_SLOPE(core)		(((ACREV0) ? 0x67 : 0x6e) | (core << 9))
#define RF_2069_PAD2G_TUNE(core)		(((ACREV0) ? 0x68 : 0x6f) | (core << 9))
#define RF_2069_PAD2G_INCAP(core)		(((ACREV0) ? 0x69 : 0x70) | (core << 9))
#define RF_2069_PAD5G_CFG1(core)		(((ACREV0) ? 0x6a : 0x71) | (core << 9))
#define RF_2069_PAD5G_IDAC(core)		(((ACREV0) ? 0x6b : 0x72) | (core << 9))
#define RF_2069_PAD5G_SLOPE(core)		(((ACREV0) ? 0x6c : 0x73) | (core << 9))
#define RF_2069_PAD5G_TUNE(core)		(((ACREV0) ? 0x6d : 0x74) | (core << 9))
#define RF_2069_PAD5G_INCAP(core)		(((ACREV0) ? 0x6e : 0x75) | (core << 9))
#define RF_2069_PA2G_CFG1(core)		(((ACREV0) ? 0x6f : 0x76) | (core << 9))
#define RF_2069_PA2G_CFG2(core)		(((ACREV0) ? 0x70 : 0x77) | (core << 9))
#define RF_2069_PA2G_CFG3(core)		(((ACREV0) ? 0x71 : 0x78) | (core << 9))
#define RF_2069_PA2G_IDAC1(core)		(((ACREV0) ? 0x72 : 0x79) | (core << 9))
#define RF_2069_PA2G_IDAC2(core)		(((ACREV0) ? 0x73 : 0x7a) | (core << 9))
#define RF_2069_PA2G_INCAP(core)		(((ACREV0) ? 0x74 : 0x7b) | (core << 9))
#define RF_2069_PA5G_CFG1(core)		(((ACREV0) ? 0x75 : 0x7c) | (core << 9))
#define RF_2069_PA5G_CFG2(core)		(((ACREV0) ? 0x76 : 0x7d) | (core << 9))
#define RF_2069_PA5G_CFG3(core)		(((ACREV0) ? 0x77 : 0x7e) | (core << 9))
#define RF_2069_PA5G_IDAC1(core)		(((ACREV0) ? 0x78 : 0x7f) | (core << 9))
#define RF_2069_PA5G_IDAC2(core)		(((ACREV0) ? 0x79 : 0x80) | (core << 9))
#define RF_2069_PA5G_INCAP(core)		(((ACREV0) ? 0x7a : 0x81) | (core << 9))
#define RF_2069_TRSW2G_CFG1(core)		(((ACREV0) ? 0x7b : 0x82) | (core << 9))
#define RF_2069_TRSW2G_CFG2(core)		(((ACREV0) ? 0x7c : 0x83) | (core << 9))
#define RF_2069_TRSW2G_CFG3(core)		(((ACREV0) ? 0x7d : 0x84) | (core << 9))
#define RF_2069_TRSW5G_CFG1(core)		(((ACREV0) ? 0x7e : 0x85) | (core << 9))
#define RF_2069_TRSW5G_CFG2(core)		(((ACREV0) ? 0x7f : 0x86) | (core << 9))
#define RF_2069_TRSW5G_CFG3(core)		(((ACREV0) ? 0x80 : 0x87) | (core << 9))
#define RF_2069_TX_REG_BACKUP_1(core)		(((ACREV0) ? 0x81 : 0x88) | (core << 9))
#define RF_2069_TX_REG_BACKUP_2(core)		(((ACREV0) ? 0x82 : 0x89) | (core << 9))
#define RF_2069_TX_REG_BACKUP_3(core)		(((ACREV0) ? 0x83 : 0x8a) | (core << 9))
#define RF_2069_TX_REG_BACKUP_4(core)		(((ACREV0) ? 0x84 : 0x8b) | (core << 9))
#define RF_2069_LOGEN2G_CFG2(core)		(((ACREV0) ? 0x10e : 0x120) | (core << 9))
#define RF_2069_LOGEN2G_IDAC3(core)		(((ACREV0) ? 0x110 : 0x122) | (core << 9))
#define RF_2069_LOGEN2G_RCCR(core)		(((ACREV0) ? 0x111 : 0x123) | (core << 9))
#define RF_2069_LOGEN5G_CFG2(core)		(((ACREV0) ? 0x114 : 0x126) | (core << 9))
#define RF_2069_LOGEN5G_IDAC4(core)		(((ACREV0) ? 0x118 : 0x12a) | (core << 9))
#define RF_2069_LOGEN5G_RCCR(core)		(((ACREV0) ? 0x119 : 0x12b) | (core << 9))
#define RF_2069_DAC_CFG1(core)		(((ACREV0) ? 0x11c : 0x12e) | (core << 9))
#define RF_2069_DAC_CFG2(core)		(((ACREV0) ? 0x11d : 0x12f) | (core << 9))
#define RF_2069_DAC_BIAS(core)		(((ACREV0) ? 0x11e : 0x130) | (core << 9))
#define RF_2069_ADC_CFG1(core)		(((ACREV0) ? 0x11f : 0x131) | (core << 9))
#define RF_2069_ADC_CFG2(core)		(((ACREV0) ? 0x120 : 0x132) | (core << 9))
#define RF_2069_ADC_CFG3(core)		(((ACREV0) ? 0x121 : 0x133) | (core << 9))
#define RF_2069_ADC_CFG4(core)		(((ACREV0) ? 0x122 : 0x134) | (core << 9))
#define RF_2069_ADC_CFG5(core)		(((ACREV0) ? 0x123 : 0x135) | (core << 9))
#define RF_2069_ADC_BIAS1(core)		(((ACREV0) ? 0x124 : 0x136) | (core << 9))
#define RF_2069_ADC_BIAS2(core)		(((ACREV0) ? 0x125 : 0x137) | (core << 9))
#define RF_2069_ADC_RC1(core)		(((ACREV0) ? 0x126 : 0x138) | (core << 9))
#define RF_2069_ADC_RC2(core)		(((ACREV0) ? 0x127 : 0x139) | (core << 9))
#define RF_2069_ADC_CALCODE1(core)		(((ACREV0) ? 0x128 : 0x13a) | (core << 9))
#define RF_2069_ADC_CALCODE2(core)		(((ACREV0) ? 0x129 : 0x13b) | (core << 9))
#define RF_2069_ADC_CALCODE3(core)		(((ACREV0) ? 0x12a : 0x13c) | (core << 9))
#define RF_2069_ADC_CALCODE4(core)		(((ACREV0) ? 0x12b : 0x13d) | (core << 9))
#define RF_2069_ADC_CALCODE5(core)		(((ACREV0) ? 0x12c : 0x13e) | (core << 9))
#define RF_2069_ADC_CALCODE6(core)		(((ACREV0) ? 0x12d : 0x13f) | (core << 9))
#define RF_2069_ADC_CALCODE7(core)		(((ACREV0) ? 0x12e : 0x140) | (core << 9))
#define RF_2069_ADC_CALCODE8(core)		(((ACREV0) ? 0x12f : 0x141) | (core << 9))
#define RF_2069_ADC_CALCODE9(core)		(((ACREV0) ? 0x130 : 0x142) | (core << 9))
#define RF_2069_ADC_CALCODE10(core)		(((ACREV0) ? 0x131 : 0x143) | (core << 9))
#define RF_2069_ADC_CALCODE11(core)		(((ACREV0) ? 0x132 : 0x144) | (core << 9))
#define RF_2069_ADC_CALCODE12(core)		(((ACREV0) ? 0x133 : 0x145) | (core << 9))
#define RF_2069_ADC_CALCODE13(core)		(((ACREV0) ? 0x134 : 0x146) | (core << 9))
#define RF_2069_ADC_CALCODE14(core)		(((ACREV0) ? 0x135 : 0x147) | (core << 9))
#define RF_2069_ADC_CALCODE15(core)		(((ACREV0) ? 0x136 : 0x148) | (core << 9))
#define RF_2069_ADC_CALCODE16(core)		(((ACREV0) ? 0x137 : 0x149) | (core << 9))
#define RF_2069_ADC_CALCODE17(core)		(((ACREV0) ? 0x138 : 0x14a) | (core << 9))
#define RF_2069_ADC_CALCODE18(core)		(((ACREV0) ? 0x139 : 0x14b) | (core << 9))
#define RF_2069_ADC_CALCODE19(core)		(((ACREV0) ? 0x13a : 0x14c) | (core << 9))
#define RF_2069_ADC_CALCODE20(core)		(((ACREV0) ? 0x13b : 0x14d) | (core << 9))
#define RF_2069_ADC_CALCODE21(core)		(((ACREV0) ? 0x13c : 0x14e) | (core << 9))
#define RF_2069_ADC_CALCODE22(core)		(((ACREV0) ? 0x13d : 0x14f) | (core << 9))
#define RF_2069_ADC_CALCODE23(core)		(((ACREV0) ? 0x13e : 0x150) | (core << 9))
#define RF_2069_ADC_CALCODE24(core)		(((ACREV0) ? 0x13f : 0x151) | (core << 9))
#define RF_2069_ADC_CALCODE25(core)		(((ACREV0) ? 0x140 : 0x152) | (core << 9))
#define RF_2069_ADC_CALCODE26(core)		(((ACREV0) ? 0x141 : 0x153) | (core << 9))
#define RF_2069_ADC_CALCODE27(core)		(((ACREV0) ? 0x142 : 0x154) | (core << 9))
#define RF_2069_ADC_CALCODE28(core)		(((ACREV0) ? 0x143 : 0x155) | (core << 9))
#define RF_2069_ADC_STATUS(core)		(((ACREV0) ? 0x144 : 0x156) | (core << 9))
#define RF_2069_GPAIO_CFG1(core)		(((ACREV0) ? 0x147 : 0x15a) | (core << 9))
#define RF_2069_RADIO_SPARE1(core)		(((ACREV0) ? 0x157 : 0x16a) | (core << 9))
#define RF_2069_RADIO_SPARE2(core)		(((ACREV0) ? 0x158 : 0x16b) | (core << 9))
#define RF_2069_RADIO_SPARE3(core)		(((ACREV0) ? 0x159 : 0x16c) | (core << 9))
#define RF_2069_RADIO_SPARE4(core)		(((ACREV0) ? 0x15a : 0x16d) | (core << 9))
#define RF_2069_OVR3(core)		(((ACREV0) ? 0x15f : 0x172) | (core << 9))
#define RF_2069_OVR4(core)		(((ACREV0) ? 0x160 : 0x173) | (core << 9))
#define RF_2069_OVR5(core)		(((ACREV0) ? 0x161 : 0x174) | (core << 9))
#define RF_2069_OVR6(core)		(((ACREV0) ? 0x162 : 0x175) | (core << 9))
#define RF_2069_OVR7(core)		(((ACREV0) ? 0x163 : 0x176) | (core << 9))
#define RF_2069_OVR9(core)	\
	(((ACREV0) ? 0x165 : 0x178) | ((ACREV0) ? (core << 9) : JTAG_2069_PLL))
#define RF_2069_OVR10(core)		(((ACREV0) ? 0x166 : 0x179) | (core << 9))
#define RF_2069_OVR11(core)		(((ACREV0) ? 0x167 : 0x17a) | (core << 9))
#define RF_2069_OVR12(core)		(((ACREV0) ? 0x168 : 0x17b) | (core << 9))
#define RF_2069_OVR13(core)		(((ACREV0) ? 0x169 : 0x17c) | (core << 9))
#define RF_2069_OVR18(core)	\
	(((ACREV0) ? 0x16e : 0x181) | ((ACREV0) ? (core << 9) : JTAG_2069_PLL))
#define RF_2069_OVR19(core)		(((ACREV0) ? 0x16f : 0x182) | (core << 9))
#define RF_2069_OVR20(core)		(((ACREV0) ? 0x170 : 0x183) | (core << 9))
#define RF_2069_OVR21(core)		(((ACREV0) ? 0x171 : 0x184) | (core << 9))
#define RF_2069_OVR23(core)	\
	(((ACREV0) ? 0x173 : 0x186) | ((ACREV0) ? (core << 9) : JTAG_2069_PLL))
#define RF_2069_DAC_CFG3(core)                   (0x174 | ((core) << 9))
#define RF_2069_GP_REGISTER(core)                (0x178 | ((core) << 9))
#define RFP_2069_PLL_ADC1               (((ACREV0) ? 0x89 : 0x91) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_ADC2		(((ACREV0) ? 0x8a : 0x92) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_ADC3		(((ACREV0) ? 0x8b : 0x93) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_ADC4		(((ACREV0) ? 0x8c : 0x94) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_ADC5		(((ACREV0) ? 0x8d : 0x95) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_ADC6		(((ACREV0) ? 0x8e : 0x96) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP1		(((ACREV0) ? 0x94 : 0x9c) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP2		(((ACREV0) ? 0x95 : 0x9d) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP3		(((ACREV0) ? 0x96 : 0x9e) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP4		(((ACREV0) ? 0x97 : 0x9f) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP5		(((ACREV0) ? 0x98 : 0xa0) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP6		(((ACREV0) ? 0x99 : 0xa1) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP7		(((ACREV0) ? 0x9a : 0xa2) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP8		(((ACREV0) ? 0x9b : 0xa3) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP9		(((ACREV0) ? 0x9c : 0xa4) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP10		(((ACREV0) ? 0x9d : 0xa5) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP11		(((ACREV0) ? 0x9e : 0xa6) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP12		(((ACREV0) ? 0x9f : 0xa7) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP13		(((ACREV0) ? 0xa0 : 0xa8) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP14		(((ACREV0) ? 0xa1 : 0xa9) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP15		(((ACREV0) ? 0xa2 : 0xaa) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP16		(((ACREV0) ? 0xa3 : 0xab) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP17		(((ACREV0) ? 0xa4 : 0xac) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP18		(((ACREV0) ? 0xa5 : 0xad) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP19		(((ACREV0) ? 0xa6 : 0xae) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP20		(((ACREV0) ? 0xa7 : 0xaf) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP21		(((ACREV0) ? 0xa8 : 0xb0) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP22		(((ACREV0) ? 0xa9 : 0xb1) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP23		(((ACREV0) ? 0xaa : 0xb2) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP24		(((ACREV0) ? 0xab : 0xb3) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP25		(((ACREV0) ? 0xac : 0xb4) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP26		(((ACREV0) ? 0xad : 0xb5) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP27		(((ACREV0) ? 0xae : 0xb6) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP28		(((ACREV0) ? 0xaf : 0xb7) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP29		(((ACREV0) ? 0xb0 : 0xb8) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP30		(((ACREV0) ? 0xb1 : 0xb9) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP31		(((ACREV0) ? 0xb2 : 0xba) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP32		(((ACREV0) ? 0xb3 : 0xbb) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP33		(((ACREV0) ? 0xb4 : 0xbc) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP34		(((ACREV0) ? 0xb5 : 0xbd) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP35		(((ACREV0) ? 0xb6 : 0xbe) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP36		(((ACREV0) ? 0xb7 : 0xbf) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP37		(((ACREV0) ? 0xb8 : 0xc0) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP38		(((ACREV0) ? 0xb9 : 0xc1) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP39		(((ACREV0) ? 0xba : 0xc2) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP40		(((ACREV0) ? 0xbb : 0xc3) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP41		(((ACREV0) ? 0xbc : 0xc4) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP42		(((ACREV0) ? 0xbd : 0xc5) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP43		(((ACREV0) ? 0xbe : 0xc6) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP44		(((ACREV0) ? 0xbf : 0xc7) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP45		(((ACREV0) ? 0xc0 : 0xc8) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP46		(((ACREV0) ? 0xc1 : 0xc9) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSP47		(((ACREV0) ? 0xc2 : 0xca) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR1		(((ACREV0) ? 0xee : 0x100) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR2		(((ACREV0) ? 0xef : 0x101) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR3		(((ACREV0) ? 0xf0 : 0x102) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR4		(((ACREV0) ? 0xf1 : 0x103) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR5		(((ACREV0) ? 0xf2 : 0x104) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR6		(((ACREV0) ? 0xf3 : 0x105) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR7		(((ACREV0) ? 0xf4 : 0x106) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR8		(((ACREV0) ? 0xf5 : 0x107) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR9		(((ACREV0) ? 0xf6 : 0x108) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR10		(((ACREV0) ? 0xf7 : 0x109) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR11		(((ACREV0) ? 0xf8 : 0x10a) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR12		(((ACREV0) ? 0xf9 : 0x10b) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR13		(((ACREV0) ? 0xfa : 0x10c) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR14		(((ACREV0) ? 0xfb : 0x10d) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR15		(((ACREV0) ? 0xfc : 0x10e) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR16		(((ACREV0) ? 0xfd : 0x10f) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR17		(((ACREV0) ? 0xfe : 0x110) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR18		(((ACREV0) ? 0xff : 0x111) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR19		(((ACREV0) ? 0x100 : 0x112) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR20		(((ACREV0) ? 0x101 : 0x113) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR21		(((ACREV0) ? 0x102 : 0x114) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR22		(((ACREV0) ? 0x103 : 0x115) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR23		(((ACREV0) ? 0x104 : 0x116) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR24		(((ACREV0) ? 0x105 : 0x117) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR25		(((ACREV0) ? 0x106 : 0x118) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_PLL_DSPR26		(((ACREV0) ? 0x107 : 0x119) |	\
	((ACREV0) ? JTAG_2069_PLL : JTAG_2069_CR0))
#define RFP_2069_OVR1			(0x15d | JTAG_2069_PLL)
#define RFX_2069_OVR1			(0x15d | JTAG_2069_ALL)
#define RF_2069_OVR1(core)		(0x15d | ((core) << 9))
/* GE 16 defs */
#define RF0_2069_GE16_REV_ID                          (0x0 | JTAG_2069_CR0)
#define RF0_2069_GE16_DEV_ID                          (0x1 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_CR0)
#define RF0_2069_GE16_TEMPSENSE_CFG                   (0x13 | JTAG_2069_CR0)
#define RF0_2069_GE16_VBAT_CFG                        (0x14 | JTAG_2069_CR0)
#define RF0_2069_GE16_TESTBUF_CFG1                    (0x1c | JTAG_2069_CR0)
#define RF0_2069_GE16_GPAIO_SEL0                      (0x1d | JTAG_2069_CR0)
#define RF0_2069_GE16_GPAIO_SEL1                      (0x1e | JTAG_2069_CR0)
#define RF0_2069_GE16_IQCAL_CFG1                      (0x1f | JTAG_2069_CR0)
#define RF0_2069_GE16_IQCAL_CFG2                      (0x20 | JTAG_2069_CR0)
#define RF0_2069_GE16_IQCAL_CFG3                      (0x21 | JTAG_2069_CR0)
#define RF0_2069_GE16_IQCAL_IDAC                      (0x22 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_TSSI                       (0x23 | JTAG_2069_CR0)
#define RF0_2069_GE16_TX5G_TSSI                       (0x24 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXRX2G_CAL_TX                   (0x25 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXRX5G_CAL_TX                   (0x26 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXRX2G_CAL_RX                   (0x27 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXRX5G_CAL_RX                   (0x28 | JTAG_2069_CR0)
#define RF0_2069_GE16_AUXPGA_CFG1                     (0x29 | JTAG_2069_CR0)
#define RF0_2069_GE16_AUXPGA_VMID                     (0x2a | JTAG_2069_CR0)
#define RF0_2069_GE16_CORE_BIAS                       (0x2b | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_CFG1                      (0x2c | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_CFG2                      (0x2d | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_CFG3                      (0x2e | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_TUNE                      (0x2f | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_IDAC1                     (0x30 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_IDAC2                     (0x31 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_RSSI                      (0x32 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA2G_RSSI_ANA                  (0x33 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_CFG1                      (0x34 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_CFG2                      (0x35 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_CFG3                      (0x36 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_TUNE                      (0x37 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_IDAC1                     (0x38 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_IDAC2                     (0x39 | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_RSSI                      (0x3a | JTAG_2069_CR0)
#define RF0_2069_GE16_LNA5G_RSSI_ANA                  (0x3b | JTAG_2069_CR0)
#define RF0_2069_GE16_RXMIX2G_CFG1                    (0x3c | JTAG_2069_CR0)
#define RF0_2069_GE16_RXMIX2G_IDAC                    (0x3d | JTAG_2069_CR0)
#define RF0_2069_GE16_RXMIX5G_CFG1                    (0x3e | JTAG_2069_CR0)
#define RF0_2069_GE16_RXMIX5G_IDAC                    (0x3f | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF2G_CFG1                     (0x40 | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF2G_CFG2                     (0x41 | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF2G_SPARE                    (0x42 | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF5G_CFG1                     (0x43 | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF5G_CFG2                     (0x44 | JTAG_2069_CR0)
#define RF0_2069_GE16_RX_REG_BACKUP_1                 (0x45 | JTAG_2069_CR0)
#define RF0_2069_GE16_RX_REG_BACKUP_2                 (0x46 | JTAG_2069_CR0)
#define RF0_2069_GE16_RXRF5G_SPARE                    (0x47 | JTAG_2069_CR0)
#define RF0_2069_GE16_TIA_CFG1                        (0x48 | JTAG_2069_CR0)
#define RF0_2069_GE16_TIA_CFG2                        (0x49 | JTAG_2069_CR0)
#define RF0_2069_GE16_TIA_CFG3                        (0x4a | JTAG_2069_CR0)
#define RF0_2069_GE16_TIA_IDAC1                       (0x4b | JTAG_2069_CR0)
#define RF0_2069_GE16_NBRSSI_CONFG                    (0x4c | JTAG_2069_CR0)
#define RF0_2069_GE16_NBRSSI_BIAS                     (0x4d | JTAG_2069_CR0)
#define RF0_2069_GE16_NBRSSI_IB                       (0x4e | JTAG_2069_CR0)
#define RF0_2069_GE16_NBRSSI_TEST                     (0x4f | JTAG_2069_CR0)
#define RF0_2069_GE16_WRSSI3_CONFG                    (0x50 | JTAG_2069_CR0)
#define RF0_2069_GE16_WRSSI3_BIAS                     (0x51 | JTAG_2069_CR0)
#define RF0_2069_GE16_WRSSI3_TEST                     (0x52 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_MUX_SWITCHES                (0x53 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_ENABLES                     (0x54 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_MAIN_CONTROLS               (0x55 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_CORNER_FREQUENCY_TUNING     (0x56 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_DC_LOOP_AND_MISC            (0x57 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_BIAS_LEVELS_LOW             (0x58 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_BIAS_LEVELS_MID             (0x59 | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_BIAS_LEVELS_HIGH            (0x5a | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x5b | JTAG_2069_CR0)
#define RF0_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS (0x5c | JTAG_2069_CR0)
#define RF0_2069_GE16_TX2G_CFG1                       (0x5d | JTAG_2069_CR0)
#define RF0_2069_GE16_TX2G_CFG2                       (0x5e | JTAG_2069_CR0)
#define RF0_2069_GE16_TX5G_CFG1                       (0x5f | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_CFG1                       (0x60 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXGM_CFG2                       (0x61 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXMIX2G_CFG1                    (0x62 | JTAG_2069_CR0)
#define RF0_2069_GE16_TXMIX5G_CFG1                    (0x63 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA2G_CFG1                      (0x64 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA2G_CFG2                      (0x65 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA2G_IDAC                      (0x66 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA2G_INCAP                     (0x67 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA5G_CFG1                      (0x68 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA5G_CFG2                      (0x69 | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA5G_IDAC                      (0x6a | JTAG_2069_CR0)
#define RF0_2069_GE16_PGA5G_INCAP                     (0x6b | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD2G_CFG1                      (0x6c | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD2G_IDAC                      (0x6d | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD2G_SLOPE                     (0x6e | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD2G_TUNE                      (0x6f | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD2G_INCAP                     (0x70 | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD5G_CFG1                      (0x71 | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD5G_IDAC                      (0x72 | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD5G_SLOPE                     (0x73 | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD5G_TUNE                      (0x74 | JTAG_2069_CR0)
#define RF0_2069_GE16_PAD5G_INCAP                     (0x75 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_CFG1                       (0x76 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_CFG2                       (0x77 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_CFG3                       (0x78 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_IDAC1                      (0x79 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_IDAC2                      (0x7a | JTAG_2069_CR0)
#define RF0_2069_GE16_PA2G_INCAP                      (0x7b | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_CFG1                       (0x7c | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_CFG2                       (0x7d | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_CFG3                       (0x7e | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_IDAC1                      (0x7f | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_IDAC2                      (0x80 | JTAG_2069_CR0)
#define RF0_2069_GE16_PA5G_INCAP                      (0x81 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW2G_CFG1                     (0x82 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW2G_CFG2                     (0x83 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW2G_CFG3                     (0x84 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW5G_CFG1                     (0x85 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW5G_CFG2                     (0x86 | JTAG_2069_CR0)
#define RF0_2069_GE16_TRSW5G_CFG3                     (0x87 | JTAG_2069_CR0)
#define RF0_2069_GE16_TX_REG_BACKUP_1                 (0x88 | JTAG_2069_CR0)
#define RF0_2069_GE16_TX_REG_BACKUP_2                 (0x89 | JTAG_2069_CR0)
#define RF0_2069_GE16_TX_REG_BACKUP_3                 (0x8a | JTAG_2069_CR0)
#define RF0_2069_GE16_TX_REG_BACKUP_4                 (0x8b | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN2G_CFG2                    (0x120 | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN2G_IDAC3                   (0x122 | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN2G_RCCR                    (0x123 | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN5G_CFG2                    (0x126 | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN5G_IDAC4                   (0x12a | JTAG_2069_CR0)
#define RF0_2069_GE16_LOGEN5G_RCCR                    (0x12b | JTAG_2069_CR0)
#define RF0_2069_GE16_DAC_CFG1                        (0x12e | JTAG_2069_CR0)
#define RF0_2069_GE16_DAC_CFG2                        (0x12f | JTAG_2069_CR0)
#define RF0_2069_GE16_DAC_BIAS                        (0x130 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CFG1                        (0x131 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CFG2                        (0x132 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CFG3                        (0x133 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CFG4                        (0x134 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CFG5                        (0x135 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_BIAS1                       (0x136 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_BIAS2                       (0x137 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_RC1                         (0x138 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_RC2                         (0x139 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE1                    (0x13a | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE2                    (0x13b | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE3                    (0x13c | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE4                    (0x13d | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE5                    (0x13e | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE6                    (0x13f | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE7                    (0x140 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE8                    (0x141 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE9                    (0x142 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE10                   (0x143 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE11                   (0x144 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE12                   (0x145 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE13                   (0x146 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE14                   (0x147 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE15                   (0x148 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE16                   (0x149 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE17                   (0x14a | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE18                   (0x14b | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE19                   (0x14c | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE20                   (0x14d | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE21                   (0x14e | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE22                   (0x14f | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE23                   (0x150 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE24                   (0x151 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE25                   (0x152 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE26                   (0x153 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE27                   (0x154 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_CALCODE28                   (0x155 | JTAG_2069_CR0)
#define RF0_2069_GE16_ADC_STATUS                      (0x156 | JTAG_2069_CR0)
#define RF0_2069_GE16_GPAIO_CFG1                      (0x15a | JTAG_2069_CR0)
#define RF0_2069_GE16_RADIO_SPARE1                    (0x16a | JTAG_2069_CR0)
#define RF0_2069_GE16_RADIO_SPARE2                    (0x16b | JTAG_2069_CR0)
#define RF0_2069_GE16_RADIO_SPARE3                    (0x16c | JTAG_2069_CR0)
#define RF0_2069_GE16_RADIO_SPARE4                    (0x16d | JTAG_2069_CR0)
#define RF0_2069_GE16_CLB_REG_0                       (0x16e | JTAG_2069_CR0)
#define RF0_2069_GE16_CLB_REG_1                       (0x16f | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR3                            (0x172 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR4                            (0x173 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR5                            (0x174 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR6                            (0x175 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR7                            (0x176 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR10                           (0x179 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR11                           (0x17a | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR12                           (0x17b | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR13                           (0x17c | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR14                           (0x17d | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR19                           (0x182 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR20                           (0x183 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR21                           (0x184 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR22                           (0x185 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR24                           (0x187 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR25                           (0x188 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR26                           (0x189 | JTAG_2069_CR0)
#define RF0_2069_GE16_OVR28                           (0x18b | JTAG_2069_CR0)
#define RF0_2069_GE16_READOVERRIDES                   (0x192 | JTAG_2069_CR0)
#define RF1_2069_GE16_REV_ID                          (0x0 | JTAG_2069_CR1)
#define RF1_2069_GE16_DEV_ID                          (0x1 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_CR1)
#define RF1_2069_GE16_TEMPSENSE_CFG                   (0x13 | JTAG_2069_CR1)
#define RF1_2069_GE16_VBAT_CFG                        (0x14 | JTAG_2069_CR1)
#define RF1_2069_GE16_TESTBUF_CFG1                    (0x1c | JTAG_2069_CR1)
#define RF1_2069_GE16_GPAIO_SEL0                      (0x1d | JTAG_2069_CR1)
#define RF1_2069_GE16_GPAIO_SEL1                      (0x1e | JTAG_2069_CR1)
#define RF1_2069_GE16_IQCAL_CFG1                      (0x1f | JTAG_2069_CR1)
#define RF1_2069_GE16_IQCAL_CFG2                      (0x20 | JTAG_2069_CR1)
#define RF1_2069_GE16_IQCAL_CFG3                      (0x21 | JTAG_2069_CR1)
#define RF1_2069_GE16_IQCAL_IDAC                      (0x22 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_TSSI                       (0x23 | JTAG_2069_CR1)
#define RF1_2069_GE16_TX5G_TSSI                       (0x24 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXRX2G_CAL_TX                   (0x25 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXRX5G_CAL_TX                   (0x26 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXRX2G_CAL_RX                   (0x27 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXRX5G_CAL_RX                   (0x28 | JTAG_2069_CR1)
#define RF1_2069_GE16_AUXPGA_CFG1                     (0x29 | JTAG_2069_CR1)
#define RF1_2069_GE16_AUXPGA_VMID                     (0x2a | JTAG_2069_CR1)
#define RF1_2069_GE16_CORE_BIAS                       (0x2b | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_CFG1                      (0x2c | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_CFG2                      (0x2d | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_CFG3                      (0x2e | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_TUNE                      (0x2f | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_IDAC1                     (0x30 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_IDAC2                     (0x31 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_RSSI                      (0x32 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA2G_RSSI_ANA                  (0x33 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_CFG1                      (0x34 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_CFG2                      (0x35 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_CFG3                      (0x36 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_TUNE                      (0x37 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_IDAC1                     (0x38 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_IDAC2                     (0x39 | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_RSSI                      (0x3a | JTAG_2069_CR1)
#define RF1_2069_GE16_LNA5G_RSSI_ANA                  (0x3b | JTAG_2069_CR1)
#define RF1_2069_GE16_RXMIX2G_CFG1                    (0x3c | JTAG_2069_CR1)
#define RF1_2069_GE16_RXMIX2G_IDAC                    (0x3d | JTAG_2069_CR1)
#define RF1_2069_GE16_RXMIX5G_CFG1                    (0x3e | JTAG_2069_CR1)
#define RF1_2069_GE16_RXMIX5G_IDAC                    (0x3f | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF2G_CFG1                     (0x40 | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF2G_CFG2                     (0x41 | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF2G_SPARE                    (0x42 | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF5G_CFG1                     (0x43 | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF5G_CFG2                     (0x44 | JTAG_2069_CR1)
#define RF1_2069_GE16_RX_REG_BACKUP_1                 (0x45 | JTAG_2069_CR1)
#define RF1_2069_GE16_RX_REG_BACKUP_2                 (0x46 | JTAG_2069_CR1)
#define RF1_2069_GE16_RXRF5G_SPARE                    (0x47 | JTAG_2069_CR1)
#define RF1_2069_GE16_TIA_CFG1                        (0x48 | JTAG_2069_CR1)
#define RF1_2069_GE16_TIA_CFG2                        (0x49 | JTAG_2069_CR1)
#define RF1_2069_GE16_TIA_CFG3                        (0x4a | JTAG_2069_CR1)
#define RF1_2069_GE16_TIA_IDAC1                       (0x4b | JTAG_2069_CR1)
#define RF1_2069_GE16_NBRSSI_CONFG                    (0x4c | JTAG_2069_CR1)
#define RF1_2069_GE16_NBRSSI_BIAS                     (0x4d | JTAG_2069_CR1)
#define RF1_2069_GE16_NBRSSI_IB                       (0x4e | JTAG_2069_CR1)
#define RF1_2069_GE16_NBRSSI_TEST                     (0x4f | JTAG_2069_CR1)
#define RF1_2069_GE16_WRSSI3_CONFG                    (0x50 | JTAG_2069_CR1)
#define RF1_2069_GE16_WRSSI3_BIAS                     (0x51 | JTAG_2069_CR1)
#define RF1_2069_GE16_WRSSI3_TEST                     (0x52 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_MUX_SWITCHES                (0x53 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_ENABLES                     (0x54 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_MAIN_CONTROLS               (0x55 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_CORNER_FREQUENCY_TUNING     (0x56 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_DC_LOOP_AND_MISC            (0x57 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_BIAS_LEVELS_LOW             (0x58 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_BIAS_LEVELS_MID             (0x59 | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_BIAS_LEVELS_HIGH            (0x5a | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x5b | JTAG_2069_CR1)
#define RF1_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS (0x5c | JTAG_2069_CR1)
#define RF1_2069_GE16_TX2G_CFG1                       (0x5d | JTAG_2069_CR1)
#define RF1_2069_GE16_TX2G_CFG2                       (0x5e | JTAG_2069_CR1)
#define RF1_2069_GE16_TX5G_CFG1                       (0x5f | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_CFG1                       (0x60 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXGM_CFG2                       (0x61 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXMIX2G_CFG1                    (0x62 | JTAG_2069_CR1)
#define RF1_2069_GE16_TXMIX5G_CFG1                    (0x63 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA2G_CFG1                      (0x64 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA2G_CFG2                      (0x65 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA2G_IDAC                      (0x66 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA2G_INCAP                     (0x67 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA5G_CFG1                      (0x68 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA5G_CFG2                      (0x69 | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA5G_IDAC                      (0x6a | JTAG_2069_CR1)
#define RF1_2069_GE16_PGA5G_INCAP                     (0x6b | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD2G_CFG1                      (0x6c | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD2G_IDAC                      (0x6d | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD2G_SLOPE                     (0x6e | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD2G_TUNE                      (0x6f | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD2G_INCAP                     (0x70 | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD5G_CFG1                      (0x71 | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD5G_IDAC                      (0x72 | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD5G_SLOPE                     (0x73 | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD5G_TUNE                      (0x74 | JTAG_2069_CR1)
#define RF1_2069_GE16_PAD5G_INCAP                     (0x75 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_CFG1                       (0x76 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_CFG2                       (0x77 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_CFG3                       (0x78 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_IDAC1                      (0x79 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_IDAC2                      (0x7a | JTAG_2069_CR1)
#define RF1_2069_GE16_PA2G_INCAP                      (0x7b | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_CFG1                       (0x7c | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_CFG2                       (0x7d | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_CFG3                       (0x7e | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_IDAC1                      (0x7f | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_IDAC2                      (0x80 | JTAG_2069_CR1)
#define RF1_2069_GE16_PA5G_INCAP                      (0x81 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW2G_CFG1                     (0x82 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW2G_CFG2                     (0x83 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW2G_CFG3                     (0x84 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW5G_CFG1                     (0x85 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW5G_CFG2                     (0x86 | JTAG_2069_CR1)
#define RF1_2069_GE16_TRSW5G_CFG3                     (0x87 | JTAG_2069_CR1)
#define RF1_2069_GE16_TX_REG_BACKUP_1                 (0x88 | JTAG_2069_CR1)
#define RF1_2069_GE16_TX_REG_BACKUP_2                 (0x89 | JTAG_2069_CR1)
#define RF1_2069_GE16_TX_REG_BACKUP_3                 (0x8a | JTAG_2069_CR1)
#define RF1_2069_GE16_TX_REG_BACKUP_4                 (0x8b | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN2G_CFG2                    (0x120 | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN2G_IDAC3                   (0x122 | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN2G_RCCR                    (0x123 | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN5G_CFG2                    (0x126 | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN5G_IDAC4                   (0x12a | JTAG_2069_CR1)
#define RF1_2069_GE16_LOGEN5G_RCCR                    (0x12b | JTAG_2069_CR1)
#define RF1_2069_GE16_DAC_CFG1                        (0x12e | JTAG_2069_CR1)
#define RF1_2069_GE16_DAC_CFG2                        (0x12f | JTAG_2069_CR1)
#define RF1_2069_GE16_DAC_BIAS                        (0x130 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CFG1                        (0x131 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CFG2                        (0x132 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CFG3                        (0x133 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CFG4                        (0x134 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CFG5                        (0x135 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_BIAS1                       (0x136 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_BIAS2                       (0x137 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_RC1                         (0x138 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_RC2                         (0x139 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE1                    (0x13a | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE2                    (0x13b | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE3                    (0x13c | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE4                    (0x13d | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE5                    (0x13e | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE6                    (0x13f | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE7                    (0x140 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE8                    (0x141 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE9                    (0x142 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE10                   (0x143 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE11                   (0x144 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE12                   (0x145 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE13                   (0x146 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE14                   (0x147 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE15                   (0x148 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE16                   (0x149 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE17                   (0x14a | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE18                   (0x14b | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE19                   (0x14c | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE20                   (0x14d | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE21                   (0x14e | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE22                   (0x14f | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE23                   (0x150 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE24                   (0x151 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE25                   (0x152 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE26                   (0x153 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE27                   (0x154 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_CALCODE28                   (0x155 | JTAG_2069_CR1)
#define RF1_2069_GE16_ADC_STATUS                      (0x156 | JTAG_2069_CR1)
#define RF1_2069_GE16_GPAIO_CFG1                      (0x15a | JTAG_2069_CR1)
#define RF1_2069_GE16_RADIO_SPARE1                    (0x16a | JTAG_2069_CR1)
#define RF1_2069_GE16_RADIO_SPARE2                    (0x16b | JTAG_2069_CR1)
#define RF1_2069_GE16_RADIO_SPARE3                    (0x16c | JTAG_2069_CR1)
#define RF1_2069_GE16_RADIO_SPARE4                    (0x16d | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR3                            (0x172 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR4                            (0x173 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR5                            (0x174 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR6                            (0x175 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR7                            (0x176 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR10                           (0x179 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR11                           (0x17a | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR12                           (0x17b | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR13                           (0x17c | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR14                           (0x17d | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR19                           (0x182 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR20                           (0x183 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR21                           (0x184 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR22                           (0x185 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR24                           (0x187 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR25                           (0x188 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR26                           (0x189 | JTAG_2069_CR1)
#define RF1_2069_GE16_OVR28                           (0x18b | JTAG_2069_CR1)
#define RF1_2069_GE16_READOVERRIDES                   (0x192 | JTAG_2069_CR1)
#define RFP_2069_GE16_REV_ID                          (0x0 | JTAG_2069_PLL)
#define RFP_2069_GE16_DEV_ID                          (0x1 | JTAG_2069_PLL)
#define RFP_2069_GE16_BG_CFG1                         (0x7 | JTAG_2069_PLL)
#define RFP_2069_GE16_BG_CFG2                         (0x8 | JTAG_2069_PLL)
#define RFP_2069_GE16_BG_TRIM1                        (0x9 | JTAG_2069_PLL)
#define RFP_2069_GE16_BG_TRIM2                        (0xa | JTAG_2069_PLL)
#define RFP_2069_GE16_PMU_STAT                        (0xb | JTAG_2069_PLL)
#define RFP_2069_GE16_PMU_OP                          (0xc | JTAG_2069_PLL)
#define RFP_2069_GE16_PMU_CFG1                        (0xd | JTAG_2069_PLL)
#define RFP_2069_GE16_PMU_CFG2                        (0xe | JTAG_2069_PLL)
#define RFP_2069_GE16_PMU_CFG3                        (0xf | JTAG_2069_PLL)
#define RFP_2069_GE16_RCAL_CFG                        (0x10 | \
						       ((ACREV3) ? JTAG_2069_CR0 : JTAG_2069_PLL))
#define RFP_2069_GE16_VREG_CFG                        (0x11 | JTAG_2069_PLL)
#define RFP_2069_GE16_LPO_CFG                         (0x12 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_CFG                       (0x15 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_LOGIC1                    (0x16 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_TRC                       (0x17 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_LOGIC2                    (0x18 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_LOGIC3                    (0x19 | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_LOGIC4                    (0x1a | JTAG_2069_PLL)
#define RFP_2069_GE16_RCCAL_LOGIC5                    (0x1b | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTALLDO1                    (0x8c | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_HVLDO1                      (0x8d | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_HVLDO2                      (0x8e | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_HVLDO3                      (0x8f | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_HVLDO4                      (0x90 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CP1                         (0x97 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CP2                         (0x98 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CP3                         (0x99 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CP4                         (0x9a | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CP5                         (0x9b | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_FRCT1                       (0xcb | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_FRCT2                       (0xcc | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_FRCT3                       (0xcd | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF1                         (0xce | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF2                         (0xcf | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF3                         (0xd0 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF4                         (0xd1 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF5                         (0xd2 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF6                         (0xd3 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_LF7                         (0xd4 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_MMD1                        (0xd5 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_MONITOR1                    (0xd6 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG1                        (0xd7 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG2                        (0xd8 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG3                        (0xd9 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG4                        (0xda | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG5                        (0xdb | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFG6                        (0xdc | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO1                        (0xdd | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO2                        (0xde | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO3                        (0xdf | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO4                        (0xe0 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO5                        (0xe1 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO6                        (0xe2 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCO8                        (0xe3 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL1                     (0xe4 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL2                     (0xe5 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL3                     (0xe6 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL4                     (0xe7 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL5                     (0xe8 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL6                     (0xe9 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL7                     (0xea | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL8                     (0xeb | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL9                     (0xec | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL10                    (0xed | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL11                    (0xee | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL12                    (0xef | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL13                    (0xf0 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL1                       (0xf1 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL2                       (0xf2 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL3                       (0xf3 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL4                       (0xf4 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL5                       (0xf5 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL6                       (0xf6 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL7                       (0xf7 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL8                       (0xf8 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL9                       (0xf9 | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL10                      (0xfa | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL11                      (0xfb | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL12                      (0xfc | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL13                      (0xfd | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL14                      (0xfe | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_XTAL15                      (0xff | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_DSPR27                      (0x11a | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFGR1                       (0x11b | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_CFGR2                       (0x11c | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL14                    (0x11d | JTAG_2069_PLL)
#define RFP_2069_GE16_PLL_VCOCAL15                    (0x11e | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN2G_CFG1                    (0x11f | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN2G_IDAC2                   (0x121 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN2G_TUNE                    (0x124 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_CFG1                    (0x125 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_IDAC1                   (0x127 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_IDAC2                   (0x128 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_IDAC3                   (0x129 | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_TUNE1                   (0x12c | JTAG_2069_PLL)
#define RFP_2069_GE16_LOGEN5G_TUNE2                   (0x12d | JTAG_2069_PLL)
#define RFP_2069_GE16_AFEDIV1                         (0x157 | JTAG_2069_PLL)
#define RFP_2069_GE16_AFEDIV2                         (0x158 | JTAG_2069_PLL)
#define RFP_2069_GE16_AFEDIV3                         (0x159 | JTAG_2069_PLL)
#define RFP_2069_GE16_CGPAIO_CFG1                     (0x15b | JTAG_2069_PLL)
#define RFP_2069_GE16_CGPAIO_CFG2                     (0x15c | JTAG_2069_PLL)
#define RFP_2069_GE16_CGPAIO_CFG3                     (0x15d | JTAG_2069_PLL)
#define RFP_2069_GE16_CGPAIO_CFG4                     (0x15e | JTAG_2069_PLL)
#define RFP_2069_GE16_CGPAIO_CFG5                     (0x15f | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE1                      (0x160 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE2                      (0x161 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE3                      (0x162 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE4                      (0x163 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE5                      (0x164 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE6                      (0x165 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE7                      (0x166 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE8                      (0x167 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE9                      (0x168 | JTAG_2069_PLL)
#define RFP_2069_GE16_TOP_SPARE10                     (0x169 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR2                            (0x171 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR8                            (0x177 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR9                            (0x178 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR15                           (0x17e | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR16                           (0x17f | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR17                           (0x180 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR18                           (0x181 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR23                           (0x186 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR27                           (0x18a | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR29                           (0x18c | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR30                           (0x18d | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR31                           (0x18e | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR32                           (0x18f | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR33                           (0x190 | JTAG_2069_PLL)
#define RFP_2069_GE16_OVR34                           (0x191 | JTAG_2069_PLL)
#define RFP_2069_GE16_READOVERRIDES                   (0x192 | JTAG_2069_PLL)
/* 2069 broadcast registers */
#define RFX_2069_GE16_REV_ID                          (0x0 | JTAG_2069_ALL)
#define RFX_2069_GE16_DEV_ID                          (0x1 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_LOFT_FINE_I                (0x2 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_LOFT_FINE_Q                (0x3 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_LOFT_COARSE_I              (0x4 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_LOFT_COARSE_Q              (0x5 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_LOFT_SCALE                 (0x6 | JTAG_2069_ALL)
#define RFX_2069_GE16_TEMPSENSE_CFG                   (0x13 | JTAG_2069_ALL)
#define RFX_2069_GE16_VBAT_CFG                        (0x14 | JTAG_2069_ALL)
#define RFX_2069_GE16_TESTBUF_CFG1                    (0x1c | JTAG_2069_ALL)
#define RFX_2069_GE16_GPAIO_SEL0                      (0x1d | JTAG_2069_ALL)
#define RFX_2069_GE16_GPAIO_SEL1                      (0x1e | JTAG_2069_ALL)
#define RFX_2069_GE16_IQCAL_CFG1                      (0x1f | JTAG_2069_ALL)
#define RFX_2069_GE16_IQCAL_CFG2                      (0x20 | JTAG_2069_ALL)
#define RFX_2069_GE16_IQCAL_CFG3                      (0x21 | JTAG_2069_ALL)
#define RFX_2069_GE16_IQCAL_IDAC                      (0x22 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_TSSI                       (0x23 | JTAG_2069_ALL)
#define RFX_2069_GE16_TX5G_TSSI                       (0x24 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXRX2G_CAL_TX                   (0x25 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXRX5G_CAL_TX                   (0x26 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXRX2G_CAL_RX                   (0x27 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXRX5G_CAL_RX                   (0x28 | JTAG_2069_ALL)
#define RFX_2069_GE16_AUXPGA_CFG1                     (0x29 | JTAG_2069_ALL)
#define RFX_2069_GE16_AUXPGA_VMID                     (0x2a | JTAG_2069_ALL)
#define RFX_2069_GE16_CORE_BIAS                       (0x2b | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_CFG1                      (0x2c | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_CFG2                      (0x2d | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_CFG3                      (0x2e | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_TUNE                      (0x2f | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_IDAC1                     (0x30 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_IDAC2                     (0x31 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_RSSI                      (0x32 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA2G_RSSI_ANA                  (0x33 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_CFG1                      (0x34 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_CFG2                      (0x35 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_CFG3                      (0x36 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_TUNE                      (0x37 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_IDAC1                     (0x38 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_IDAC2                     (0x39 | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_RSSI                      (0x3a | JTAG_2069_ALL)
#define RFX_2069_GE16_LNA5G_RSSI_ANA                  (0x3b | JTAG_2069_ALL)
#define RFX_2069_GE16_RXMIX2G_CFG1                    (0x3c | JTAG_2069_ALL)
#define RFX_2069_GE16_RXMIX2G_IDAC                    (0x3d | JTAG_2069_ALL)
#define RFX_2069_GE16_RXMIX5G_CFG1                    (0x3e | JTAG_2069_ALL)
#define RFX_2069_GE16_RXMIX5G_IDAC                    (0x3f | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF2G_CFG1                     (0x40 | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF2G_CFG2                     (0x41 | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF2G_SPARE                    (0x42 | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF5G_CFG1                     (0x43 | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF5G_CFG2                     (0x44 | JTAG_2069_ALL)
#define RFX_2069_GE16_RX_REG_BACKUP_1                 (0x45 | JTAG_2069_ALL)
#define RFX_2069_GE16_RX_REG_BACKUP_2                 (0x46 | JTAG_2069_ALL)
#define RFX_2069_GE16_RXRF5G_SPARE                    (0x47 | JTAG_2069_ALL)
#define RFX_2069_GE16_TIA_CFG1                        (0x48 | JTAG_2069_ALL)
#define RFX_2069_GE16_TIA_CFG2                        (0x49 | JTAG_2069_ALL)
#define RFX_2069_GE16_TIA_CFG3                        (0x4a | JTAG_2069_ALL)
#define RFX_2069_GE16_TIA_IDAC1                       (0x4b | JTAG_2069_ALL)
#define RFX_2069_GE16_NBRSSI_CONFG                    (0x4c | JTAG_2069_ALL)
#define RFX_2069_GE16_NBRSSI_BIAS                     (0x4d | JTAG_2069_ALL)
#define RFX_2069_GE16_NBRSSI_IB                       (0x4e | JTAG_2069_ALL)
#define RFX_2069_GE16_NBRSSI_TEST                     (0x4f | JTAG_2069_ALL)
#define RFX_2069_GE16_WRSSI3_CONFG                    (0x50 | JTAG_2069_ALL)
#define RFX_2069_GE16_WRSSI3_BIAS                     (0x51 | JTAG_2069_ALL)
#define RFX_2069_GE16_WRSSI3_TEST                     (0x52 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_MUX_SWITCHES                (0x53 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_ENABLES                     (0x54 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_MAIN_CONTROLS               (0x55 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_CORNER_FREQUENCY_TUNING     (0x56 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_DC_LOOP_AND_MISC            (0x57 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_BIAS_LEVELS_LOW             (0x58 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_BIAS_LEVELS_MID             (0x59 | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_BIAS_LEVELS_HIGH            (0x5a | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS (0x5b | JTAG_2069_ALL)
#define RFX_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS (0x5c | JTAG_2069_ALL)
#define RFX_2069_GE16_TX2G_CFG1                       (0x5d | JTAG_2069_ALL)
#define RFX_2069_GE16_TX2G_CFG2                       (0x5e | JTAG_2069_ALL)
#define RFX_2069_GE16_TX5G_CFG1                       (0x5f | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_CFG1                       (0x60 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXGM_CFG2                       (0x61 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXMIX2G_CFG1                    (0x62 | JTAG_2069_ALL)
#define RFX_2069_GE16_TXMIX5G_CFG1                    (0x63 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA2G_CFG1                      (0x64 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA2G_CFG2                      (0x65 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA2G_IDAC                      (0x66 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA2G_INCAP                     (0x67 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA5G_CFG1                      (0x68 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA5G_CFG2                      (0x69 | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA5G_IDAC                      (0x6a | JTAG_2069_ALL)
#define RFX_2069_GE16_PGA5G_INCAP                     (0x6b | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD2G_CFG1                      (0x6c | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD2G_IDAC                      (0x6d | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD2G_SLOPE                     (0x6e | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD2G_TUNE                      (0x6f | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD2G_INCAP                     (0x70 | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD5G_CFG1                      (0x71 | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD5G_IDAC                      (0x72 | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD5G_SLOPE                     (0x73 | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD5G_TUNE                      (0x74 | JTAG_2069_ALL)
#define RFX_2069_GE16_PAD5G_INCAP                     (0x75 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_CFG1                       (0x76 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_CFG2                       (0x77 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_CFG3                       (0x78 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_IDAC1                      (0x79 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_IDAC2                      (0x7a | JTAG_2069_ALL)
#define RFX_2069_GE16_PA2G_INCAP                      (0x7b | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_CFG1                       (0x7c | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_CFG2                       (0x7d | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_CFG3                       (0x7e | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_IDAC1                      (0x7f | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_IDAC2                      (0x80 | JTAG_2069_ALL)
#define RFX_2069_GE16_PA5G_INCAP                      (0x81 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW2G_CFG1                     (0x82 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW2G_CFG2                     (0x83 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW2G_CFG3                     (0x84 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW5G_CFG1                     (0x85 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW5G_CFG2                     (0x86 | JTAG_2069_ALL)
#define RFX_2069_GE16_TRSW5G_CFG3                     (0x87 | JTAG_2069_ALL)
#define RFX_2069_GE16_TX_REG_BACKUP_1                 (0x88 | JTAG_2069_ALL)
#define RFX_2069_GE16_TX_REG_BACKUP_2                 (0x89 | JTAG_2069_ALL)
#define RFX_2069_GE16_TX_REG_BACKUP_3                 (0x8a | JTAG_2069_ALL)
#define RFX_2069_GE16_TX_REG_BACKUP_4                 (0x8b | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN2G_CFG2                    (0x120 | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN2G_IDAC3                   (0x122 | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN2G_RCCR                    (0x123 | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN5G_CFG2                    (0x126 | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN5G_IDAC4                   (0x12a | JTAG_2069_ALL)
#define RFX_2069_GE16_LOGEN5G_RCCR                    (0x12b | JTAG_2069_ALL)
#define RFX_2069_GE16_DAC_CFG1                        (0x12e | JTAG_2069_ALL)
#define RFX_2069_GE16_DAC_CFG2                        (0x12f | JTAG_2069_ALL)
#define RFX_2069_GE16_DAC_BIAS                        (0x130 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CFG1                        (0x131 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CFG2                        (0x132 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CFG3                        (0x133 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CFG4                        (0x134 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CFG5                        (0x135 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_BIAS1                       (0x136 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_BIAS2                       (0x137 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_RC1                         (0x138 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_RC2                         (0x139 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE1                    (0x13a | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE2                    (0x13b | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE3                    (0x13c | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE4                    (0x13d | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE5                    (0x13e | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE6                    (0x13f | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE7                    (0x140 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE8                    (0x141 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE9                    (0x142 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE10                   (0x143 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE11                   (0x144 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE12                   (0x145 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE13                   (0x146 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE14                   (0x147 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE15                   (0x148 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE16                   (0x149 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE17                   (0x14a | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE18                   (0x14b | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE19                   (0x14c | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE20                   (0x14d | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE21                   (0x14e | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE22                   (0x14f | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE23                   (0x150 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE24                   (0x151 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE25                   (0x152 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE26                   (0x153 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE27                   (0x154 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_CALCODE28                   (0x155 | JTAG_2069_ALL)
#define RFX_2069_GE16_ADC_STATUS                      (0x156 | JTAG_2069_ALL)
#define RFX_2069_GE16_GPAIO_CFG1                      (0x15a | JTAG_2069_ALL)
#define RFX_2069_GE16_RADIO_SPARE1                    (0x16a | JTAG_2069_ALL)
#define RFX_2069_GE16_RADIO_SPARE2                    (0x16b | JTAG_2069_ALL)
#define RFX_2069_GE16_RADIO_SPARE3                    (0x16c | JTAG_2069_ALL)
#define RFX_2069_GE16_RADIO_SPARE4                    (0x16d | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR3                            (0x172 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR4                            (0x173 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR5                            (0x174 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR6                            (0x175 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR7                            (0x176 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR10                           (0x179 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR11                           (0x17a | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR12                           (0x17b | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR13                           (0x17c | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR14                           (0x17d | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR19                           (0x182 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR20                           (0x183 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR21                           (0x184 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR22                           (0x185 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR24                           (0x187 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR25                           (0x188 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR26                           (0x189 | JTAG_2069_ALL)
#define RFX_2069_GE16_OVR28                           (0x18b | JTAG_2069_ALL)
#define RFX_2069_GE16_READOVERRIDES                   (0x192 | JTAG_2069_ALL)

#define RF_2069_GE16_REV_ID(core)                        (0x0 | ((core) << 9))
#define RF_2069_GE16_DEV_ID(core)                        (0x1 | ((core) << 9))
#define RF_2069_GE16_TXGM_LOFT_FINE_I(core)              (0x2 | ((core) << 9))
#define RF_2069_GE16_TXGM_LOFT_FINE_Q(core)              (0x3 | ((core) << 9))
#define RF_2069_GE16_TXGM_LOFT_COARSE_I(core)            (0x4 | ((core) << 9))
#define RF_2069_GE16_TXGM_LOFT_COARSE_Q(core)            (0x5 | ((core) << 9))
#define RF_2069_GE16_TXGM_LOFT_SCALE(core)               (0x6 | ((core) << 9))
#define RF_2069_GE16_BG_CFG1(core)                       (0x7 | ((core) << 9))
#define RF_2069_GE16_TEMPSENSE_CFG(core)                 (0x13 | ((core) << 9))
#define RF_2069_GE16_VBAT_CFG(core)                      (0x14 | ((core) << 9))
#define RF_2069_GE16_TESTBUF_CFG1(core)                  (0x1c | ((core) << 9))
#define RF_2069_GE16_GPAIO_SEL0(core)                    (0x1d | ((core) << 9))
#define RF_2069_GE16_GPAIO_SEL1(core)                    (0x1e | ((core) << 9))
#define RF_2069_GE16_IQCAL_CFG1(core)                    (0x1f | ((core) << 9))
#define RF_2069_GE16_IQCAL_CFG2(core)                    (0x20 | ((core) << 9))
#define RF_2069_GE16_IQCAL_CFG3(core)                    (0x21 | ((core) << 9))
#define RF_2069_GE16_IQCAL_IDAC(core)                    (0x22 | ((core) << 9))
#define RF_2069_GE16_PA2G_TSSI(core)                     (0x23 | ((core) << 9))
#define RF_2069_GE16_TX5G_TSSI(core)                     (0x24 | ((core) << 9))
#define RF_2069_GE16_TXRX2G_CAL_TX(core)                 (0x25 | ((core) << 9))
#define RF_2069_GE16_TXRX5G_CAL_TX(core)                 (0x26 | ((core) << 9))
#define RF_2069_GE16_TXRX2G_CAL_RX(core)                 (0x27 | ((core) << 9))
#define RF_2069_GE16_TXRX5G_CAL_RX(core)                 (0x28 | ((core) << 9))
#define RF_2069_GE16_AUXPGA_CFG1(core)                   (0x29 | ((core) << 9))
#define RF_2069_GE16_AUXPGA_VMID(core)                   (0x2a | ((core) << 9))
#define RF_2069_GE16_CORE_BIAS(core)                     (0x2b | ((core) << 9))
#define RF_2069_GE16_LNA2G_CFG1(core)                    (0x2c | ((core) << 9))
#define RF_2069_GE16_LNA2G_CFG2(core)                    (0x2d | ((core) << 9))
#define RF_2069_GE16_LNA2G_CFG3(core)                    (0x2e | ((core) << 9))
#define RF_2069_GE16_LNA2G_TUNE(core)                    (0x2f | ((core) << 9))
#define RF_2069_GE16_LNA2G_IDAC1(core)                   (0x30 | ((core) << 9))
#define RF_2069_GE16_LNA2G_IDAC2(core)                   (0x31 | ((core) << 9))
#define RF_2069_GE16_LNA2G_RSSI(core)                    (0x32 | ((core) << 9))
#define RF_2069_GE16_LNA2G_RSSI_ANA(core)                (0x33 | ((core) << 9))
#define RF_2069_GE16_LNA5G_CFG1(core)                    (0x34 | ((core) << 9))
#define RF_2069_GE16_LNA5G_CFG2(core)                    (0x35 | ((core) << 9))
#define RF_2069_GE16_LNA5G_CFG3(core)                    (0x36 | ((core) << 9))
#define RF_2069_GE16_LNA5G_TUNE(core)                    (0x37 | ((core) << 9))
#define RF_2069_GE16_LNA5G_IDAC1(core)                   (0x38 | ((core) << 9))
#define RF_2069_GE16_LNA5G_IDAC2(core)                   (0x39 | ((core) << 9))
#define RF_2069_GE16_LNA5G_RSSI(core)                    (0x3a | ((core) << 9))
#define RF_2069_GE16_LNA5G_RSSI_ANA(core)                (0x3b | ((core) << 9))
#define RF_2069_GE16_RXMIX2G_CFG1(core)                  (0x3c | ((core) << 9))
#define RF_2069_GE16_RXMIX2G_IDAC(core)                  (0x3d | ((core) << 9))
#define RF_2069_GE16_RXMIX5G_CFG1(core)                  (0x3e | ((core) << 9))
#define RF_2069_GE16_RXMIX5G_IDAC(core)                  (0x3f | ((core) << 9))
#define RF_2069_GE16_RXRF2G_CFG1(core)                   (0x40 | ((core) << 9))
#define RF_2069_GE16_RXRF2G_CFG2(core)                   (0x41 | ((core) << 9))
#define RF_2069_GE16_RXRF2G_SPARE(core)                  (0x42 | ((core) << 9))
#define RF_2069_GE16_RXRF5G_CFG1(core)                   (0x43 | ((core) << 9))
#define RF_2069_GE16_RXRF5G_CFG2(core)                   (0x44 | ((core) << 9))
#define RF_2069_GE16_RX_REG_BACKUP_1(core)               (0x45 | ((core) << 9))
#define RF_2069_GE16_RX_REG_BACKUP_2(core)               (0x46 | ((core) << 9))
#define RF_2069_GE16_RXRF5G_SPARE(core)                  (0x47 | ((core) << 9))
#define RF_2069_GE16_TIA_CFG1(core)                      (0x48 | ((core) << 9))
#define RF_2069_GE16_TIA_CFG2(core)                      (0x49 | ((core) << 9))
#define RF_2069_GE16_TIA_CFG3(core)                      (0x4a | ((core) << 9))
#define RF_2069_GE16_TIA_IDAC1(core)                     (0x4b | ((core) << 9))
#define RF_2069_GE16_NBRSSI_CONFG(core)                  (0x4c | ((core) << 9))
#define RF_2069_GE16_NBRSSI_BIAS(core)                   (0x4d | ((core) << 9))
#define RF_2069_GE16_NBRSSI_IB(core)                     (0x4e | ((core) << 9))
#define RF_2069_GE16_NBRSSI_TEST(core)                   (0x4f | ((core) << 9))
#define RF_2069_GE16_WRSSI3_CONFG(core)                  (0x50 | ((core) << 9))
#define RF_2069_GE16_WRSSI3_BIAS(core)                   (0x51 | ((core) << 9))
#define RF_2069_GE16_WRSSI3_TEST(core)                   (0x52 | ((core) << 9))
#define RF_2069_GE16_LPF_MUX_SWITCHES(core)              (0x53 | ((core) << 9))
#define RF_2069_GE16_LPF_ENABLES(core)                   (0x54 | ((core) << 9))
#define RF_2069_GE16_LPF_MAIN_CONTROLS(core)             (0x55 | ((core) << 9))
#define RF_2069_GE16_LPF_CORNER_FREQUENCY_TUNING(core)   (0x56 | ((core) << 9))
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC(core)          (0x57 | ((core) << 9))
#define RF_2069_GE16_LPF_BIAS_LEVELS_LOW(core)           (0x58 | ((core) << 9))
#define RF_2069_GE16_LPF_BIAS_LEVELS_MID(core)           (0x59 | ((core) << 9))
#define RF_2069_GE16_LPF_BIAS_LEVELS_HIGH(core)          (0x5a | ((core) << 9))
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS(core) (0x5b | ((core) << 9))
#define RF_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS(core) (0x5c | ((core) << 9))
#define RF_2069_GE16_TX2G_CFG1(core)                     (0x5d | ((core) << 9))
#define RF_2069_GE16_TX2G_CFG2(core)                     (0x5e | ((core) << 9))
#define RF_2069_GE16_TX5G_CFG1(core)                     (0x5f | ((core) << 9))
#define RF_2069_GE16_TXGM_CFG1(core)                     (0x60 | ((core) << 9))
#define RF_2069_GE16_TXGM_CFG2(core)                     (0x61 | ((core) << 9))
#define RF_2069_GE16_TXMIX2G_CFG1(core)                  (0x62 | ((core) << 9))
#define RF_2069_GE16_TXMIX5G_CFG1(core)                  (0x63 | ((core) << 9))
#define RF_2069_GE16_PGA2G_CFG1(core)                    (0x64 | ((core) << 9))
#define RF_2069_GE16_PGA2G_CFG2(core)                    (0x65 | ((core) << 9))
#define RF_2069_GE16_PGA2G_IDAC(core)                    (0x66 | ((core) << 9))
#define RF_2069_GE16_PGA2G_INCAP(core)                   (0x67 | ((core) << 9))
#define RF_2069_GE16_PGA5G_CFG1(core)                    (0x68 | ((core) << 9))
#define RF_2069_GE16_PGA5G_CFG2(core)                    (0x69 | ((core) << 9))
#define RF_2069_GE16_PGA5G_IDAC(core)                    (0x6a | ((core) << 9))
#define RF_2069_GE16_PGA5G_INCAP(core)                   (0x6b | ((core) << 9))
#define RF_2069_GE16_PAD2G_CFG1(core)                    (0x6c | ((core) << 9))
#define RF_2069_GE16_PAD2G_IDAC(core)                    (0x6d | ((core) << 9))
#define RF_2069_GE16_PAD2G_SLOPE(core)                   (0x6e | ((core) << 9))
#define RF_2069_GE16_PAD2G_TUNE(core)                    (0x6f | ((core) << 9))
#define RF_2069_GE16_PAD2G_INCAP(core)                   (0x70 | ((core) << 9))
#define RF_2069_GE16_PAD5G_CFG1(core)                    (0x71 | ((core) << 9))
#define RF_2069_GE16_PAD5G_IDAC(core)                    (0x72 | ((core) << 9))
#define RF_2069_GE16_PAD5G_SLOPE(core)                   (0x73 | ((core) << 9))
#define RF_2069_GE16_PAD5G_TUNE(core)                    (0x74 | ((core) << 9))
#define RF_2069_GE16_PAD5G_INCAP(core)                   (0x75 | ((core) << 9))
#define RF_2069_GE16_PA2G_CFG1(core)                     (0x76 | ((core) << 9))
#define RF_2069_GE16_PA2G_CFG2(core)                     (0x77 | ((core) << 9))
#define RF_2069_GE16_PA2G_CFG3(core)                     (0x78 | ((core) << 9))
#define RF_2069_GE16_PA2G_IDAC1(core)                    (0x79 | ((core) << 9))
#define RF_2069_GE16_PA2G_IDAC2(core)                    (0x7a | ((core) << 9))
#define RF_2069_GE16_PA2G_INCAP(core)                    (0x7b | ((core) << 9))
#define RF_2069_GE16_PA5G_CFG1(core)                     (0x7c | ((core) << 9))
#define RF_2069_GE16_PA5G_CFG2(core)                     (0x7d | ((core) << 9))
#define RF_2069_GE16_PA5G_CFG3(core)                     (0x7e | ((core) << 9))
#define RF_2069_GE16_PA5G_IDAC1(core)                    (0x7f | ((core) << 9))
#define RF_2069_GE16_PA5G_IDAC2(core)                    (0x80 | ((core) << 9))
#define RF_2069_GE16_PA5G_INCAP(core)                    (0x81 | ((core) << 9))
#define RF_2069_GE16_TRSW2G_CFG1(core)                   (0x82 | ((core) << 9))
#define RF_2069_GE16_TRSW2G_CFG2(core)                   (0x83 | ((core) << 9))
#define RF_2069_GE16_TRSW2G_CFG3(core)                   (0x84 | ((core) << 9))
#define RF_2069_GE16_TRSW5G_CFG1(core)                   (0x85 | ((core) << 9))
#define RF_2069_GE16_TRSW5G_CFG2(core)                   (0x86 | ((core) << 9))
#define RF_2069_GE16_TRSW5G_CFG3(core)                   (0x87 | ((core) << 9))
#define RF_2069_GE16_TX_REG_BACKUP_1(core)               (0x88 | ((core) << 9))
#define RF_2069_GE16_TX_REG_BACKUP_2(core)               (0x89 | ((core) << 9))
#define RF_2069_GE16_TX_REG_BACKUP_3(core)               (0x8a | ((core) << 9))
#define RF_2069_GE16_TX_REG_BACKUP_4(core)               (0x8b | ((core) << 9))
#define RF_2069_PLL_CP4(core)                            (0x9a | ((core) << 9))
#define RF_2069_GE16_LOGEN2G_CFG2(core)                  (0x120 | ((core) << 9))
#define RF_2069_GE16_LOGEN2G_IDAC3(core)                 (0x122 | ((core) << 9))
#define RF_2069_GE16_LOGEN2G_RCCR(core)                  (0x123 | ((core) << 9))
#define RF_2069_GE16_LOGEN5G_CFG2(core)                  (0x126 | ((core) << 9))
#define RF_2069_GE16_LOGEN5G_IDAC4(core)                 (0x12a | ((core) << 9))
#define RF_2069_GE16_LOGEN5G_RCCR(core)                  (0x12b | ((core) << 9))
#define RF_2069_GE16_DAC_CFG1(core)                      (0x12e | ((core) << 9))
#define RF_2069_GE16_DAC_CFG2(core)                      (0x12f | ((core) << 9))
#define RF_2069_GE16_DAC_BIAS(core)                      (0x130 | ((core) << 9))
#define RF_2069_GE16_ADC_CFG1(core)                      (0x131 | ((core) << 9))
#define RF_2069_GE16_ADC_CFG2(core)                      (0x132 | ((core) << 9))
#define RF_2069_GE16_ADC_CFG3(core)                      (0x133 | ((core) << 9))
#define RF_2069_GE16_ADC_CFG4(core)                      (0x134 | ((core) << 9))
#define RF_2069_GE16_ADC_CFG5(core)                      (0x135 | ((core) << 9))
#define RF_2069_GE16_ADC_BIAS1(core)                     (0x136 | ((core) << 9))
#define RF_2069_GE16_ADC_BIAS2(core)                     (0x137 | ((core) << 9))
#define RF_2069_GE16_ADC_RC1(core)                       (0x138 | ((core) << 9))
#define RF_2069_GE16_ADC_RC2(core)                       (0x139 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE1(core)                  (0x13a | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE2(core)                  (0x13b | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE3(core)                  (0x13c | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE4(core)                  (0x13d | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE5(core)                  (0x13e | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE6(core)                  (0x13f | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE7(core)                  (0x140 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE8(core)                  (0x141 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE9(core)                  (0x142 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE10(core)                 (0x143 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE11(core)                 (0x144 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE12(core)                 (0x145 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE13(core)                 (0x146 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE14(core)                 (0x147 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE15(core)                 (0x148 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE16(core)                 (0x149 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE17(core)                 (0x14a | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE18(core)                 (0x14b | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE19(core)                 (0x14c | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE20(core)                 (0x14d | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE21(core)                 (0x14e | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE22(core)                 (0x14f | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE23(core)                 (0x150 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE24(core)                 (0x151 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE25(core)                 (0x152 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE26(core)                 (0x153 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE27(core)                 (0x154 | ((core) << 9))
#define RF_2069_GE16_ADC_CALCODE28(core)                 (0x155 | ((core) << 9))
#define RF_2069_GE16_ADC_STATUS(core)                    (0x156 | ((core) << 9))
#define RF_2069_GE16_GPAIO_CFG1(core)                    (0x15a | ((core) << 9))
#define RF_2069_GE16_RADIO_SPARE1(core)                  (0x16a | ((core) << 9))
#define RF_2069_GE16_RADIO_SPARE2(core)                  (0x16b | ((core) << 9))
#define RF_2069_GE16_RADIO_SPARE3(core)                  (0x16c | ((core) << 9))
#define RF_2069_GE16_RADIO_SPARE4(core)                  (0x16d | ((core) << 9))
#define RF_2069_GE16_CLB_REG_0(core)                     (0x16e | ((core) << 9))
#define RF_2069_GE16_CLB_REG_1(core)                     (0x16f | ((core) << 9))
#define RF_2069_GE16_OVR2(core)                          (0x171 | ((core) << 9))
#define RF_2069_GE16_OVR3(core)                          (0x172 | ((core) << 9))
#define RF_2069_GE16_OVR4(core)                          (0x173 | ((core) << 9))
#define RF_2069_GE16_OVR5(core)                          (0x174 | ((core) << 9))
#define RF_2069_GE16_OVR6(core)                          (0x175 | ((core) << 9))
#define RF_2069_GE16_OVR7(core)                          (0x176 | ((core) << 9))
#define RF_2069_GE16_OVR10(core)                         (0x179 | ((core) << 9))
#define RF_2069_GE16_OVR11(core)                         (0x17a | ((core) << 9))
#define RF_2069_GE16_OVR12(core)                         (0x17b | ((core) << 9))
#define RF_2069_GE16_OVR13(core)                         (0x17c | ((core) << 9))
#define RF_2069_GE16_OVR14(core)                         (0x17d | ((core) << 9))
#define RF_2069_GE16_OVR19(core)                         (0x182 | ((core) << 9))
#define RF_2069_GE16_OVR20(core)                         (0x183 | ((core) << 9))
#define RF_2069_GE16_OVR21(core)                         (0x184 | ((core) << 9))
#define RF_2069_GE16_OVR22(core)                         (0x185 | ((core) << 9))
#define RF_2069_GE16_OVR24(core)                         (0x187 | ((core) << 9))
#define RF_2069_GE16_OVR25(core)                         (0x188 | ((core) << 9))
#define RF_2069_GE16_OVR26(core)                         (0x189 | ((core) << 9))
#define RF_2069_GE16_OVR28(core)                         (0x18b | ((core) << 9))
#define RF_2069_GE16_OVR29(core)                         (0x18c | ((core) << 9))
#define RF_2069_GE16_READOVERRIDES(core)                 (0x192 | ((core) << 9))

/* Delta GE32 defs on top of GE16 defs */
#define RF0_2069_GE32_BG_CFG1                         (0x7 | JTAG_2069_CR0)
#define RF0_2069_GE32_BG_CFG2                         (0x8 | JTAG_2069_CR0)
#define RF0_2069_GE32_BG_TRIM1                        (0x9 | JTAG_2069_CR0)
#define RF0_2069_GE32_BG_TRIM2                        (0xa | JTAG_2069_CR0)
#define RF0_2069_GE32_PMU_STAT                        (0xb | JTAG_2069_CR0)
#define RF0_2069_GE32_PMU_OP                          (0xc | JTAG_2069_CR0)
#define RF0_2069_GE32_PMU_CFG1                        (0xd | JTAG_2069_CR0)
#define RF0_2069_GE32_PMU_CFG2                        (0xe | JTAG_2069_CR0)
#define RF0_2069_GE32_PMU_CFG3                        (0xf | JTAG_2069_CR0)
#define RF0_2069_GE32_RCAL_CFG                        (0x10 | JTAG_2069_CR0)
#define RF0_2069_GE32_CGPAIO_CFG1                     (0x15b | JTAG_2069_CR0)
#define RF0_2069_GE32_CGPAIO_CFG2                     (0x15c | JTAG_2069_CR0)
#define RF0_2069_GE32_CGPAIO_CFG3                     (0x15d | JTAG_2069_CR0)
#define RF0_2069_GE32_CGPAIO_CFG4                     (0x15e | JTAG_2069_CR0)
#define RF0_2069_GE32_CGPAIO_CFG5                     (0x15f | JTAG_2069_CR0)
#define RF0_2069_GE32_OVR2                            (0x171 | JTAG_2069_CR0)
#define RF1_2069_GE32_BG_CFG1                         (0x7 | JTAG_2069_CR1)
#define RF1_2069_GE32_BG_CFG2                         (0x8 | JTAG_2069_CR1)
#define RF1_2069_GE32_BG_TRIM1                        (0x9 | JTAG_2069_CR1)
#define RF1_2069_GE32_BG_TRIM2                        (0xa | JTAG_2069_CR1)
#define RF1_2069_GE32_PMU_STAT                        (0xb | JTAG_2069_CR1)
#define RF1_2069_GE32_PMU_OP                          (0xc | JTAG_2069_CR1)
#define RF1_2069_GE32_PMU_CFG1                        (0xd | JTAG_2069_CR1)
#define RF1_2069_GE32_PMU_CFG2                        (0xe | JTAG_2069_CR1)
#define RF1_2069_GE32_PMU_CFG3                        (0xf | JTAG_2069_CR1)
#define RF1_2069_GE32_CGPAIO_CFG1                     (0x15b | JTAG_2069_CR1)
#define RF1_2069_GE32_CGPAIO_CFG2                     (0x15c | JTAG_2069_CR1)
#define RF1_2069_GE32_CGPAIO_CFG3                     (0x15d | JTAG_2069_CR1)
#define RF1_2069_GE32_CGPAIO_CFG4                     (0x15e | JTAG_2069_CR1)
#define RF1_2069_GE32_CGPAIO_CFG5                     (0x15f | JTAG_2069_CR1)
#define RF1_2069_GE32_OVR2                            (0x171 | JTAG_2069_CR1)
#define RFP_2069_GE32_OVR1                            (0x170 | JTAG_2069_PLL)
/* 2069 broadcast registers */
#define RFX_2069_GE32_BG_CFG1                         (0x7 | JTAG_2069_ALL)
#define RFX_2069_GE32_BG_CFG2                         (0x8 | JTAG_2069_ALL)
#define RFX_2069_GE32_BG_TRIM1                        (0x9 | JTAG_2069_ALL)
#define RFX_2069_GE32_BG_TRIM2                        (0xa | JTAG_2069_ALL)
#define RFX_2069_GE32_PMU_STAT                        (0xb | JTAG_2069_ALL)
#define RFX_2069_GE32_PMU_OP                          (0xc | JTAG_2069_ALL)
#define RFX_2069_GE32_PMU_CFG1                        (0xd | JTAG_2069_ALL)
#define RFX_2069_GE32_PMU_CFG2                        (0xe | JTAG_2069_ALL)
#define RFX_2069_GE32_PMU_CFG3                        (0xf | JTAG_2069_ALL)
#define RFX_2069_GE32_CGPAIO_CFG1                     (0x15b | JTAG_2069_ALL)
#define RFX_2069_GE32_CGPAIO_CFG2                     (0x15c | JTAG_2069_ALL)
#define RFX_2069_GE32_CGPAIO_CFG3                     (0x15d | JTAG_2069_ALL)
#define RFX_2069_GE32_CGPAIO_CFG4                     (0x15e | JTAG_2069_ALL)
#define RFX_2069_GE32_CGPAIO_CFG5                     (0x15f | JTAG_2069_ALL)
#define RFX_2069_GE32_OVR2                            (0x171 | JTAG_2069_ALL)
/* Macros for 2069 registers in different cores */
#define RF_2069_GE32_BG_CFG1(core)                    (0x7 | ((core) << 9))
#define RF_2069_GE32_BG_CFG2(core)                    (0x8 | ((core) << 9))
#define RF_2069_GE32_BG_TRIM1(core)                   (0x9 | ((core) << 9))
#define RF_2069_GE32_BG_TRIM2(core)                   (0xa | ((core) << 9))
#define RF_2069_GE32_PMU_STAT(core)                   (0xb | ((core) << 9))
#define RF_2069_GE32_PMU_OP(core)                     (0xc | ((core) << 9))
#define RF_2069_GE32_PMU_CFG1(core)                   (0xd | ((core) << 9))
#define RF_2069_GE32_PMU_CFG2(core)                   (0xe | ((core) << 9))
#define RF_2069_GE32_PMU_CFG3(core)                   (0xf | ((core) << 9))
#define RF_2069_GE32_CGPAIO_CFG1(core)                (0x15b | ((core) << 9))
#define RF_2069_GE32_CGPAIO_CFG2(core)                (0x15c | ((core) << 9))
#define RF_2069_GE32_CGPAIO_CFG3(core)                (0x15d | ((core) << 9))
#define RF_2069_GE32_CGPAIO_CFG4(core)                (0x15e | ((core) << 9))
#define RF_2069_GE32_CGPAIO_CFG5(core)                (0x15f | ((core) << 9))
#define RF_2069_GE32_OVR2(core)                       (0x171 | ((core) << 9))

/* Definitions of 2069 register fields */
#define RF_2069_CORE_BIAS_bias_tx_pu_MASK        0x1
#define RF_2069_CORE_BIAS_bias_tx_pu_SHIFT       0
#define RF_2069_ADC_BIAS1_adc_bias_MASK          0xffff
#define RF_2069_ADC_BIAS1_adc_bias_SHIFT         0
#define RF_2069_TIA_CFG2_opamp_iout_x2_MASK      0x8
#define RF_2069_TIA_CFG2_opamp_iout_x2_SHIFT     3
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_cm_mult_MASK 0xc
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_cm_mult_SHIFT 2
#define RF_2069_RXMIX5G_CFG1_pu_MASK             0x1
#define RF_2069_RXMIX5G_CFG1_pu_SHIFT            0
#define RF_2069_LOGEN5G_IDAC2_tc_ptat_MASK       0x7
#define RF_2069_LOGEN5G_IDAC2_tc_ptat_SHIFT      0
#define RF_2069_OVR4_ovr_afe_iqadc_mode_MASK     0x100
#define RF_2069_OVR4_ovr_afe_iqadc_mode_SHIFT    8
#define RF_2069_TXGM_CFG1_pu_MASK                0x1
#define RF_2069_TXGM_CFG1_pu_SHIFT               0
#define RF_2069_PA2G_INCAP_pa2g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_PA2G_INCAP_pa2g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_TXGM_LOFT_COARSE_I_idac_lo_rf_in_MASK 0xf0
#define RF_2069_TXGM_LOFT_COARSE_I_idac_lo_rf_in_SHIFT 4
#define RF_2069_LNA2G_CFG1_lna1_bypass_MASK      0x20
#define RF_2069_LNA2G_CFG1_lna1_bypass_SHIFT     5
#define RF_2069_ADC_CFG4_Ich_reset_MASK          0x20
#define RF_2069_ADC_CFG4_Ich_reset_SHIFT         5
#define RF_2069_LNA2G_TUNE_lna1_Cgs_size_MASK    0x3000
#define RF_2069_LNA2G_TUNE_lna1_Cgs_size_SHIFT   12
#define RF_2069_TIA_CFG1_HPC_MASK                0xe0
#define RF_2069_TIA_CFG1_HPC_SHIFT               5
#define RF_2069_OVR20_ovr_pa5g_TSSI_range_MASK   0x200
#define RF_2069_OVR20_ovr_pa5g_TSSI_range_SHIFT  9
#define RF_2069_TXGM_LOFT_COARSE_I_idac_lo_rf_ip_MASK 0xf
#define RF_2069_TXGM_LOFT_COARSE_I_idac_lo_rf_ip_SHIFT 0
#define RF_2069_LOGEN2G_IDAC2_logen2g_tc_ptat_MASK 0x7
#define RF_2069_LOGEN2G_IDAC2_logen2g_tc_ptat_SHIFT 0
#define RF_2069_LOGEN5G_CFG1_reset_MASK          0x20
#define RF_2069_LOGEN5G_CFG1_reset_SHIFT         5
#define RF_2069_ADC_CFG4_ctrl_iqadc_mode_MASK    0x3800
#define RF_2069_ADC_CFG4_ctrl_iqadc_mode_SHIFT   11
#define RF_2069_LNA5G_RSSI_dig_wrssi2_pu_MASK    0x100
#define RF_2069_LNA5G_RSSI_dig_wrssi2_pu_SHIFT   8
#define RF_2069_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_MASK 0xff
#define RF_2069_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_SHIFT 0
#define RF_2069_RXRF2G_SPARE_spare_MASK          0xff
#define RF_2069_RXRF2G_SPARE_spare_SHIFT         0
#define RF_2069_LNA5G_CFG2_lna2_pu_MASK          0x1
#define RF_2069_LNA5G_CFG2_lna2_pu_SHIFT         0
#define RF_2069_OVR19_ovr_tia_pu_MASK            0x200
#define RF_2069_OVR19_ovr_tia_pu_SHIFT           9
#define RF_2069_OVR9_ovr_lpf_sw_bq2_rc_MASK      0x2
#define RF_2069_OVR9_ovr_lpf_sw_bq2_rc_SHIFT     1
#define RF_2069_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_half_vdd_control_MASK 0xff00
#define RF_2069_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_half_vdd_control_SHIFT 8
#define RF_2069_IQCAL_CFG3_gvar_MASK             0x70
#define RF_2069_IQCAL_CFG3_gvar_SHIFT            4
#define RF_2069_AUXPGA_CFG1_auxpga_i_pu_MASK     0x1
#define RF_2069_AUXPGA_CFG1_auxpga_i_pu_SHIFT    0
#define RF_2069_OVR5_ovr_tempsense_pu_MASK       0x4000
#define RF_2069_OVR5_ovr_tempsense_pu_SHIFT      14
#define RF_2069_OVR6_ovr_lna2g_lna1_Rout_MASK    0x40
#define RF_2069_OVR6_ovr_lna2g_lna1_Rout_SHIFT   6
#define RF_2069_PA5G_CFG1_pa5g_gain_ctrl_MASK    0xff00
#define RF_2069_PA5G_CFG1_pa5g_gain_ctrl_SHIFT   8
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_rc_pu_MASK 0x40
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_rc_pu_SHIFT 6
#define RF_2069_OVR10_ovr_lpf_pu_bq2_i_MASK      0x100
#define RF_2069_OVR10_ovr_lpf_pu_bq2_i_SHIFT     8
#define RF_2069_OVR13_ovr_pga2g_gc_MASK          0x4
#define RF_2069_OVR13_ovr_pga2g_gc_SHIFT         2
#define RF_2069_PA2G_INCAP_pa2g_idac_incap_compen_main_MASK 0xf
#define RF_2069_PA2G_INCAP_pa2g_idac_incap_compen_main_SHIFT 0
#define RF_2069_RXMIX2G_CFG1_pu_MASK             0x1
#define RF_2069_RXMIX2G_CFG1_pu_SHIFT            0
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC13_MASK 0x200
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC13_SHIFT 9
#define RF_2069_LOGEN5G_CFG2_rx_pu_MASK          0x1
#define RF_2069_LOGEN5G_CFG2_rx_pu_SHIFT         0
#define RF_2069_OVR10_ovr_lpf_pu_bq2_q_MASK      0x80
#define RF_2069_OVR10_ovr_lpf_pu_bq2_q_SHIFT     7
#define RF_2069_TIA_CFG2_Vcmref_in_MASK          0x70
#define RF_2069_TIA_CFG2_Vcmref_in_SHIFT         4
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC21_MASK 0x100
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC21_SHIFT 8
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC24_MASK 0x80
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC24_SHIFT 7
#define RF_2069_TIA_CFG2_Vcmref_out_MASK         0x7
#define RF_2069_TIA_CFG2_Vcmref_out_SHIFT        0
#define RF_2069_WRSSI3_CONFG_wrssi3_Refhigh_puI_MASK 0x1
#define RF_2069_WRSSI3_CONFG_wrssi3_Refhigh_puI_SHIFT 0
#define RF_2069_LOGEN5G_IDAC3_idac_bufb1_MASK    0x7
#define RF_2069_LOGEN5G_IDAC3_idac_bufb1_SHIFT   0
#define RF_2069_ADC_CFG3_Qch_scram_off_MASK      0x8000
#define RF_2069_ADC_CFG3_Qch_scram_off_SHIFT     15
#define RF_2069_LOGEN5G_IDAC3_idac_bufb2_MASK    ((ACREV0_SUB) ? 0x700 : 0x1c0)
#define RF_2069_LOGEN5G_IDAC3_idac_bufb2_SHIFT   ((ACREV0_SUB) ? 8 : 6)
#define RF_2069_WRSSI3_CONFG_wrssi3_Refhigh_puQ_MASK 0x2
#define RF_2069_WRSSI3_CONFG_wrssi3_Refhigh_puQ_SHIFT 1
#define RF_2069_TXRX2G_CAL_TX_i_calPath_pa2g_pu_MASK 0x20
#define RF_2069_TXRX2G_CAL_TX_i_calPath_pa2g_pu_SHIFT 5
#define RF_2069_OVR5_ovr_afe_DAC_pd_partial_MASK 0x100
#define RF_2069_OVR5_ovr_afe_DAC_pd_partial_SHIFT 8
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_rx_byp_MASK 0xf00
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_rx_byp_SHIFT 8
#define RF_2069_LNA2G_IDAC1_lna1_bias_idac_MASK  0xf
#define RF_2069_LNA2G_IDAC1_lna1_bias_idac_SHIFT 0
#define RF_2069_TRSW2G_CFG3_trsw2g_trsw_source_bis_MASK 0x1e00
#define RF_2069_TRSW2G_CFG3_trsw2g_trsw_source_bis_SHIFT 9
#define RF_2069_RX_REG_BACKUP_2_reg_backup_2_MASK 0xffff
#define RF_2069_RX_REG_BACKUP_2_reg_backup_2_SHIFT 0
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC43_MASK 0x40
#define RF_2069_OVR3_ovr_afe_iqadc_ctl_RC43_SHIFT 6
#define RF_2069_ADC_CALCODE6_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE6_Ich_flash_calcode_SHIFT 0
#define RF_2069_TRSW2G_CFG2_trsw2g_trsw_dwell_bis_MASK 0xf0
#define RF_2069_TRSW2G_CFG2_trsw2g_trsw_dwell_bis_SHIFT 4
#define RF_2069_TX5G_CFG1_ampdet_sel_MASK        0x38
#define RF_2069_TX5G_CFG1_ampdet_sel_SHIFT       3
#define RF_2069_AFEDIV1_afediv_pu1_main_MASK     0x1000
#define RF_2069_AFEDIV1_afediv_pu1_main_SHIFT    12
#define RF_2069_OVR12_ovr_lpf_dc_bw_MASK         0x4000
#define RF_2069_OVR12_ovr_lpf_dc_bw_SHIFT        14
#define RF_2069_DAC_CFG1_DAC_disclk_MASK         0x4
#define RF_2069_DAC_CFG1_DAC_disclk_SHIFT        2
#define RF_2069_OVR3_ovr_afe_iqadc_flash_only_MASK 0x8
#define RF_2069_OVR3_ovr_afe_iqadc_flash_only_SHIFT 3
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_ptat_MASK 0x7000
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_ptat_SHIFT 12
#define RF_2069_OVR8_ovr_logen5g_lob_pu_MASK     0x20
#define RF_2069_OVR8_ovr_logen5g_lob_pu_SHIFT    5
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_ptat_MASK 0x7000
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_ptat_SHIFT 12
#define RF_2069_PA2G_CFG1_pa2g_gain_ctrl_MASK    0xff00
#define RF_2069_PA2G_CFG1_pa2g_gain_ctrl_SHIFT   8
#define RF_2069_RX_REG_BACKUP_1_reg_backup_1_MASK 0xffff
#define RF_2069_RX_REG_BACKUP_1_reg_backup_1_SHIFT 0
#define RF_2069_IQCAL_CFG1_PU_tssi_MASK          0x1
#define RF_2069_IQCAL_CFG1_PU_tssi_SHIFT         0
#define RF_2069_OVR5_ovr_vbat_monitor_pu_MASK    0x2000
#define RF_2069_OVR5_ovr_vbat_monitor_pu_SHIFT   13
#define RF_2069_OVR21_ovr_trsw2g_cntrl_MASK      0x1000
#define RF_2069_OVR21_ovr_trsw2g_cntrl_SHIFT     12
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq1_gain_MASK 0xe00
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq1_gain_SHIFT 9
#define RF_2069_OVR12_ovr_pa2g_gain_ctrl_MASK    0x8
#define RF_2069_OVR12_ovr_pa2g_gain_ctrl_SHIFT   3
#define RF_2069_OVR4_ovr_lna5g_lna1_bypass_MASK  0x1000
#define RF_2069_OVR4_ovr_lna5g_lna1_bypass_SHIFT 12
#define RF_2069_LOGEN5G_CFG1_pdet_en_MASK        0xf80
#define RF_2069_LOGEN5G_CFG1_pdet_en_SHIFT       7
#define RF_2069_OVR4_ovr_afe_iqadc_reset_Ich_MASK 0x2000
#define RF_2069_OVR4_ovr_afe_iqadc_reset_Ich_SHIFT 13
#define RF_2069_LNA2G_CFG1_lna1_pu_MASK          0x1
#define RF_2069_LNA2G_CFG1_lna1_pu_SHIFT         0
#define RF_2069_LNA5G_CFG1_lna1_out_short_pu_MASK 0x8000
#define RF_2069_LNA5G_CFG1_lna1_out_short_pu_SHIFT 15
#define RF_2069_PA5G_IDAC1_pa5g_idac_main_MASK   0x3f
#define RF_2069_PA5G_IDAC1_pa5g_idac_main_SHIFT  0
#define RF_2069_RXMIX2G_CFG1_gm_size_MASK        0x700
#define RF_2069_RXMIX2G_CFG1_gm_size_SHIFT       8
#define RF_2069_RXRF5G_SPARE_digital_spare_MASK  0x3ff
#define RF_2069_RXRF5G_SPARE_digital_spare_SHIFT 0
#define RF_2069_OVR12_ovr_lpf_dc_bypass_MASK     0x2000
#define RF_2069_OVR12_ovr_lpf_dc_bypass_SHIFT    13
#define RF_2069_LOGEN5G_IDAC2_idac_bufb_MASK     0x7000
#define RF_2069_LOGEN5G_IDAC2_idac_bufb_SHIFT    12
#define RF_2069_PAD5G_SLOPE_ptat_slope_main_MASK 0xf
#define RF_2069_PAD5G_SLOPE_ptat_slope_main_SHIFT 0
#define RF_2069_TXRX5G_CAL_RX_loopback5g_papdcal_rx_attn_MASK 0x18
#define RF_2069_TXRX5G_CAL_RX_loopback5g_papdcal_rx_attn_SHIFT 3
#define RF_2069_LOGEN5G_IDAC1_idac_vcob_MASK     0x7
#define RF_2069_LOGEN5G_IDAC1_idac_vcob_SHIFT    0
#define RF_2069_NBRSSI_TEST_nbrssi_outsel_MASK   0xf0
#define RF_2069_NBRSSI_TEST_nbrssi_outsel_SHIFT  4
#define RF_2069_IQCAL_CFG1_tssi_bw_MASK          0xc00
#define RF_2069_IQCAL_CFG1_tssi_bw_SHIFT         10
#define RF_2069_NBRSSI_IB_nbrssi_ib_Compcore_MASK 0xffff
#define RF_2069_NBRSSI_IB_nbrssi_ib_Compcore_SHIFT 0
#define RF_2069_PAD2G_INCAP_pad2g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_PAD2G_INCAP_pad2g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_LPF_ENABLES_lpf_pu_MASK          0x1
#define RF_2069_LPF_ENABLES_lpf_pu_SHIFT         0
#define RF_2069_LNA2G_RSSI_ANA_wrssi1_gain_MASK  0x30
#define RF_2069_LNA2G_RSSI_ANA_wrssi1_gain_SHIFT 4
#define RF_2069_ADC_RC2_adc_ctrl_RC_23_20_MASK   0xf0
#define RF_2069_ADC_RC2_adc_ctrl_RC_23_20_SHIFT  4
#define RF_2069_PA2G_IDAC1_pa2g_idac_cas_MASK    0x3f00
#define RF_2069_PA2G_IDAC1_pa2g_idac_cas_SHIFT   8
#define RF_2069_RXRF5G_CFG1_pu_pulse_MASK        0x2
#define RF_2069_RXRF5G_CFG1_pu_pulse_SHIFT       1
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_rx_byp_MASK 0xf00
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_rx_byp_SHIFT 8
#define RF_2069_DAC_CFG1_DAC_twos_enb_MASK       0x8
#define RF_2069_DAC_CFG1_DAC_twos_enb_SHIFT      3
#define RF_2069_OVR13_ovr_pad2g_gc_MASK          0x100
#define RF_2069_OVR13_ovr_pad2g_gc_SHIFT         8
#define RF_2069_LNA5G_CFG1_lna1_bypass_MASK      0x4000
#define RF_2069_LNA5G_CFG1_lna1_bypass_SHIFT     14
#define RF_2069_ADC_CALCODE26_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE26_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_OVR4_ovr_afe_iqadc_pwrup_Ich_MASK 0x8000
#define RF_2069_OVR4_ovr_afe_iqadc_pwrup_Ich_SHIFT 15
#define RF_2069_LNA2G_CFG2_lna2_gm_size_MASK     0x3000
#define RF_2069_LNA2G_CFG2_lna2_gm_size_SHIFT    12
#define RF_2069_IQCAL_CFG3_hpc_MASK              0x7
#define RF_2069_IQCAL_CFG3_hpc_SHIFT             0
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_gain_MASK 0x30
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_gain_SHIFT 4
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_cal_MASK 0x4
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_cal_SHIFT 2
#define RF_2069_PA5G_CFG3_pa5g_ptat_slope_cas_MASK 0xf0
#define RF_2069_PA5G_CFG3_pa5g_ptat_slope_cas_SHIFT 4
#define RF_2069_OVR13_ovr_pga2g_pu_MASK          0x2
#define RF_2069_OVR13_ovr_pga2g_pu_SHIFT         1
#define RF_2069_NBRSSI_BIAS_nbrssi_tstsel_NBW3_MASK 0x4
#define RF_2069_NBRSSI_BIAS_nbrssi_tstsel_NBW3_SHIFT 2
#define RF_2069_TXRX5G_CAL_RX_loopback5g_papdcal_pu_MASK 0x100
#define RF_2069_TXRX5G_CAL_RX_loopback5g_papdcal_pu_SHIFT 8
#define RF_2069_ADC_CALCODE23_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE23_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_PA5G_INCAP_pa5g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_PA5G_INCAP_pa5g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_ADC_CALCODE11_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE11_Qch_flash_calcode_SHIFT 0
#define RF_2069_LOGEN5G_IDAC2_idac_mixb_MASK     0x700
#define RF_2069_LOGEN5G_IDAC2_idac_mixb_SHIFT    8
#define RF_2069_RXMIX2G_IDAC_aux_bias_ptat_MASK  0x7000
#define RF_2069_RXMIX2G_IDAC_aux_bias_ptat_SHIFT 12
#define RF_2069_OVR18_ovr_rxmix5g_gm_size_MASK   0x100
#define RF_2069_OVR18_ovr_rxmix5g_gm_size_SHIFT  8
#define RF_2069_OVR23_ovr_nbrssi_Refmid_puI_MASK 0x100
#define RF_2069_OVR23_ovr_nbrssi_Refmid_puI_SHIFT 8
#define RF_2069_PA2G_INCAP_pa2g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_PA2G_INCAP_pa2g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_Refladder_MASK 0x7000
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_Refladder_SHIFT 12
#define RF_2069_PGA2G_CFG1_pga2g_idac_boost_MASK 0xf00
#define RF_2069_PGA2G_CFG1_pga2g_idac_boost_SHIFT 8
#define RF_2069_LNA2G_CFG1_pwrsw_en_MASK         0x40
#define RF_2069_LNA2G_CFG1_pwrsw_en_SHIFT        6
#define RF_2069_OVR6_ovr_lna5g_dig_wrssi1_pu_MASK 0x1
#define RF_2069_OVR6_ovr_lna5g_dig_wrssi1_pu_SHIFT 0
#define RF_2069_LNA5G_IDAC1_lna1_bias_ptat_MASK  0x70
#define RF_2069_LNA5G_IDAC1_lna1_bias_ptat_SHIFT 4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_rc_MASK 0x800
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_rc_SHIFT 11
#define RF_2069_RXMIX5G_IDAC_gm_main_bias_idac_i_MASK 0xf
#define RF_2069_RXMIX5G_IDAC_gm_main_bias_idac_i_SHIFT 0
#define RF_2069_IQCAL_IDAC_idac_MASK             0x3ff0
#define RF_2069_IQCAL_IDAC_idac_SHIFT            4
#define RF_2069_TEMPSENSE_CFG_tempsense_sel_Vbe_Vbg_MASK 0x2
#define RF_2069_TEMPSENSE_CFG_tempsense_sel_Vbe_Vbg_SHIFT 1
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_Refbuf_MASK 0x1800
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_Refbuf_SHIFT 11
#define RF_2069_OVR23_ovr_nbrssi_Refmid_puQ_MASK 0x200
#define RF_2069_OVR23_ovr_nbrssi_Refmid_puQ_SHIFT 9
#define RF_2069_ADC_CFG4_Ich_calmode_MASK        0x8
#define RF_2069_ADC_CFG4_Ich_calmode_SHIFT       3
#define RF_2069_TX2G_CFG1_ampdet_sel_MASK        0x38
#define RF_2069_TX2G_CFG1_ampdet_sel_SHIFT       3
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_low_MASK 0x300
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_low_SHIFT 8
#define RF_2069_OVR11_ovr_lpf_g_mult_rc_MASK     0x1
#define RF_2069_OVR11_ovr_lpf_g_mult_rc_SHIFT    0
#define RF_2069_TRSW5G_CFG3_trsw5g_trsw_sub_bis_MASK 0x1e0
#define RF_2069_TRSW5G_CFG3_trsw5g_trsw_sub_bis_SHIFT 5
#define RF_2069_TXGM_LOFT_FINE_I_idac_lo_bb_in_MASK 0xf0
#define RF_2069_TXGM_LOFT_FINE_I_idac_lo_bb_in_SHIFT 4
#define RF_2069_LOGEN2G_RCCR_rccr_rx_MASK        0xf0
#define RF_2069_LOGEN2G_RCCR_rccr_rx_SHIFT       4
#define RF_2069_PGA5G_INCAP_idac_incap_compen_MASK 0xf
#define RF_2069_PGA5G_INCAP_idac_incap_compen_SHIFT 0
#define RF_2069_TXGM_LOFT_FINE_I_idac_lo_bb_ip_MASK 0xf
#define RF_2069_TXGM_LOFT_FINE_I_idac_lo_bb_ip_SHIFT 0
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_mid_MASK 0x40
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_mid_SHIFT 6
#define RF_2069_OVR10_ovr_lpf_rc_bw_MASK         0x4
#define RF_2069_OVR10_ovr_lpf_rc_bw_SHIFT        2
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_rx_attn_MASK 0x6
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_rx_attn_SHIFT 1
#define RF_2069_TIA_CFG3_rccal_hpc_MASK          0x1f
#define RF_2069_TIA_CFG3_rccal_hpc_SHIFT         0
#define RF_2069_OVR21_ovr_trsw2g_bt_en_MASK      0x100
#define RF_2069_OVR21_ovr_trsw2g_bt_en_SHIFT     8
#define RF_2069_TEMPSENSE_CFG_pu_MASK            0x1
#define RF_2069_TEMPSENSE_CFG_pu_SHIFT           0
#define RF_2069_CORE_BIAS_bias_rx_pu_MASK        0x2
#define RF_2069_CORE_BIAS_bias_rx_pu_SHIFT       1
#define RF_2069_OVR18_ovr_rxmix2g_gm_size_MASK   0x1000
#define RF_2069_OVR18_ovr_rxmix2g_gm_size_SHIFT  12
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_main_MASK 0xf
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_main_SHIFT 0
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_cm_mult_MASK 0x3
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_cm_mult_SHIFT 0
#define RF_2069_ADC_CALCODE9_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE9_Qch_flash_calcode_SHIFT 0
#define RF_2069_OVR20_ovr_mix5g_gainboost_MASK   0x2
#define RF_2069_OVR20_ovr_mix5g_gainboost_SHIFT  1
#define RF_2069_LOGEN2G_RCCR_rccr_tx_MASK        0xf
#define RF_2069_LOGEN2G_RCCR_rccr_tx_SHIFT       0
#define RF_2069_OVR18_ovr_rxrf2g_pwrsw_en_MASK   0x20
#define RF_2069_OVR18_ovr_rxrf2g_pwrsw_en_SHIFT  5
#define RF_2069_OVR19_ovr_rxrf2g_pwrsw_en_MASK   0x20
#define RF_2069_OVR19_ovr_rxrf2g_pwrsw_en_SHIFT  5
#define RF_2069_OVR4_ovr_lna2g_bias_reset_MASK   0x1
#define RF_2069_OVR4_ovr_lna2g_bias_reset_SHIFT  0
#define RF_2069_DAC_CFG1_DAC_sel_tp_MASK         0x800
#define RF_2069_DAC_CFG1_DAC_sel_tp_SHIFT        11
#define RF_2069_LOGEN2G_CFG2_tx_pu_MASK          0x8
#define RF_2069_LOGEN2G_CFG2_tx_pu_SHIFT         3
#define RF_2069_PAD2G_IDAC_pad2g_idac_main_MASK  0x3f
#define RF_2069_PAD2G_IDAC_pad2g_idac_main_SHIFT 0
#define RF_2069_PA2G_IDAC1_pa2g_idac_main_MASK   0x3f
#define RF_2069_PA2G_IDAC1_pa2g_idac_main_SHIFT  0
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_high_MASK 0x80
#define RF_2069_WRSSI3_CONFG_wrssi3_Refctrl_high_SHIFT 7
#define RF_2069_LNA5G_TUNE_fctl1_MASK            0xf
#define RF_2069_LNA5G_TUNE_fctl1_SHIFT           0
#define RF_2069_TX2G_CFG1_tx2g_bias_pu_MASK      0x2
#define RF_2069_TX2G_CFG1_tx2g_bias_pu_SHIFT     1
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_lpf_MASK 0x200
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_lpf_SHIFT 9
#define RF_2069_LNA5G_TUNE_fctl2_MASK            0xf0
#define RF_2069_LNA5G_TUNE_fctl2_SHIFT           4
#define RF_2069_TX5G_TSSI_pa5g_TSSI_range_MASK   0x2
#define RF_2069_TX5G_TSSI_pa5g_TSSI_range_SHIFT  1
#define RF_2069_OVR11_ovr_lpf_q_biq2_MASK        0x4
#define RF_2069_OVR11_ovr_lpf_q_biq2_SHIFT       2
#define RF_2069_OVR19_ovr_tia_PU_bias_MASK       0x100
#define RF_2069_OVR19_ovr_tia_PU_bias_SHIFT      8
#define RF_2069_TXRX2G_CAL_RX_loopback2g_papdcal_pu_MASK 0x100
#define RF_2069_TXRX2G_CAL_RX_loopback2g_papdcal_pu_SHIFT 8
#define RF_2069_LNA2G_RSSI_dig_wrssi1_pu_MASK    0x1
#define RF_2069_LNA2G_RSSI_dig_wrssi1_pu_SHIFT   0
#define RF_2069_IQCAL_CFG2_tssi_cm_center_MASK   0x300
#define RF_2069_IQCAL_CFG2_tssi_cm_center_SHIFT  8
#define RF_2069_OVR18_ovr_rxmix5g_pu_MASK        0x80
#define RF_2069_OVR18_ovr_rxmix5g_pu_SHIFT       7
#define RF_2069_PGA2G_INCAP_pga2g_dac_incap_compen_MASK 0xf
#define RF_2069_PGA2G_INCAP_pga2g_dac_incap_compen_SHIFT 0
#define RF_2069_OVR13_ovr_pad2g_pu_MASK          0x80
#define RF_2069_OVR13_ovr_pad2g_pu_SHIFT         7
#define RF_2069_OVR3_ovr_afe_iqadc_LowIF_dis_MASK 0x1
#define RF_2069_OVR3_ovr_afe_iqadc_LowIF_dis_SHIFT 0
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_pu_MASK 0x80
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_pu_SHIFT 7
#define RF_2069_LOGEN2G_CFG2_reset_rx_MASK       0x2
#define RF_2069_LOGEN2G_CFG2_reset_rx_SHIFT      1
#define RF_2069_RXRF5G_CFG1_pwrsw_en_MASK        0x8
#define RF_2069_RXRF5G_CFG1_pwrsw_en_SHIFT       3
#define RF_2069_ADC_CFG4_Ich_run_flashcal_MASK   0x2
#define RF_2069_ADC_CFG4_Ich_run_flashcal_SHIFT  1
#define RF_2069_TRSW2G_CFG3_trsw2g_trsw_sub_bis_MASK 0x1e0
#define RF_2069_TRSW2G_CFG3_trsw2g_trsw_sub_bis_SHIFT 5
#define RF_2069_PGA5G_CFG1_idac_boost_MASK       0xf00
#define RF_2069_PGA5G_CFG1_idac_boost_SHIFT      8
#define RF_2069_RXMIX2G_IDAC_main_bias_ptat_MASK 0x700
#define RF_2069_RXMIX2G_IDAC_main_bias_ptat_SHIFT 8
#define RF_2069_VBAT_CFG_bw_MASK                 0xf0
#define RF_2069_VBAT_CFG_bw_SHIFT                4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_adc_MASK 0x200
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_adc_SHIFT 9
#define RF_2069_LOGEN5G_IDAC1_idac_mix_MASK      ((ACREV0_SUB) ? 0xf00 : 0x700)
#define RF_2069_LOGEN5G_IDAC1_idac_mix_SHIFT     8
#define RF_2069_NBRSSI_CONFG_nbrssi_Reflow_puI_MASK 0x10
#define RF_2069_NBRSSI_CONFG_nbrssi_Reflow_puI_SHIFT 4
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_gain_MASK 0x3000
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_gain_SHIFT 12
#define RF_2069_RXMIX2G_CFG1_LO_bias_MASK        0x70
#define RF_2069_RXMIX2G_CFG1_LO_bias_SHIFT       4
#define RF_2069_DAC_CFG2_DACbuf_Cc_MASK          0x3
#define RF_2069_DAC_CFG2_DACbuf_Cc_SHIFT         0
#define RF_2069_OVR3_ovr_afe_ctrl_flash17lvl_MASK 0x2
#define RF_2069_OVR3_ovr_afe_ctrl_flash17lvl_SHIFT 1
#define RF_2069_PA5G_INCAP_pa5g_idac_incap_compen_main_MASK 0xf
#define RF_2069_PA5G_INCAP_pa5g_idac_incap_compen_main_SHIFT 0
#define RF_2069_LOGEN2G_CFG2_reset_tx_MASK       0x1
#define RF_2069_LOGEN2G_CFG2_reset_tx_SHIFT      0
#define RF_2069_OVR9_ovr_lpf_sw_dac_adc_MASK     0x1
#define RF_2069_OVR9_ovr_lpf_sw_dac_adc_SHIFT    0
#define RF_2069_RXRF2G_CFG2_lna2g_epapd_attn_MASK 0xf
#define RF_2069_RXRF2G_CFG2_lna2g_epapd_attn_SHIFT 0
#define RF_2069_LOGEN5G_CFG1_div3_en_MASK        0x40
#define RF_2069_LOGEN5G_CFG1_div3_en_SHIFT       6
#define RF_2069_NBRSSI_CONFG_nbrssi_Reflow_puQ_MASK 0x20
#define RF_2069_NBRSSI_CONFG_nbrssi_Reflow_puQ_SHIFT 5
#define RF_2069_LNA2G_CFG2_lna2_Rout_MASK        0xf0
#define RF_2069_LNA2G_CFG2_lna2_Rout_SHIFT       4
#define RF_2069_PA2G_CFG1_pa2g_bias_reset_MASK   0x8
#define RF_2069_PA2G_CFG1_pa2g_bias_reset_SHIFT  3
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_cmref_0p7_use_adc_MASK 0x40
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_cmref_0p7_use_adc_SHIFT 6
#define RF_2069_ADC_CALCODE2_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE2_Ich_flash_calcode_SHIFT 0
#define RF_2069_TX5G_CFG1_pga5g_pu_MASK          0x4
#define RF_2069_TX5G_CFG1_pga5g_pu_SHIFT         2
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_cascode_MASK 0x700
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_cascode_SHIFT 8
#define RF_2069_ADC_CALCODE14_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE14_Qch_flash_calcode_SHIFT 0
#define RF_2069_TRSW5G_CFG3_trsw5g_gpaio_en_MASK 0x10
#define RF_2069_TRSW5G_CFG3_trsw5g_gpaio_en_SHIFT 4
#define RF_2069_AUXPGA_CFG1_auxpga_i_sel_input_MASK 0xe
#define RF_2069_AUXPGA_CFG1_auxpga_i_sel_input_SHIFT 1
#define RF_2069_OVR19_ovr_pa2g_ctrl_tssi_MASK    0x4
#define RF_2069_OVR19_ovr_pa2g_ctrl_tssi_SHIFT   2
#define RF_2069_OVR21_ovr_rxrf2g_pwrsw_en_MASK   0x8
#define RF_2069_OVR21_ovr_rxrf2g_pwrsw_en_SHIFT  3
#define RF_2069_PGA2G_CFG1_pga2g_gainboost_MASK  0xf000
#define RF_2069_PGA2G_CFG1_pga2g_gainboost_SHIFT 12
#define RF_2069_LNA5G_RSSI_dig_wrssi2_threshold_MASK 0xf000
#define RF_2069_LNA5G_RSSI_dig_wrssi2_threshold_SHIFT 12
#define RF_2069_OVR4_ovr_afe_iqadc_reset_ov_det_MASK 0x800
#define RF_2069_OVR4_ovr_afe_iqadc_reset_ov_det_SHIFT 11
#define RF_2069_PA2G_CFG3_pa2g_sel_bias_type_MASK 0xff00
#define RF_2069_PA2G_CFG3_pa2g_sel_bias_type_SHIFT 8
#define RF_2069_OVR20_ovr_pa2g_ctrl_tssi_sel_MASK 0x4000
#define RF_2069_OVR20_ovr_pa2g_ctrl_tssi_sel_SHIFT 14
#define RF_2069_OVR19_ovr_tia_sel_5G_2G_MASK     0x80
#define RF_2069_OVR19_ovr_tia_sel_5G_2G_SHIFT    7
#define RF_2069_PAD5G_INCAP_idac_incap_compen_main_MASK 0xf
#define RF_2069_PAD5G_INCAP_idac_incap_compen_main_SHIFT 0
#define RF_2069_PA2G_CFG1_pa2g_bias_cas_pu_MASK  0x4
#define RF_2069_PA2G_CFG1_pa2g_bias_cas_pu_SHIFT 2
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq2_bw_MASK 0x38
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq2_bw_SHIFT 3
#define RF_2069_RXRF5G_CFG1_globe_idac_MASK      0x7f0
#define RF_2069_RXRF5G_CFG1_globe_idac_SHIFT     4
#define RF_2069_LOGEN5G_TUNE1_ctune_mix_MASK     0xf0
#define RF_2069_LOGEN5G_TUNE1_ctune_mix_SHIFT    4
#define RF_2069_PGA2G_INCAP_pga2g_ptat_slope_incap_compen_MASK 0x70
#define RF_2069_PGA2G_INCAP_pga2g_ptat_slope_incap_compen_SHIFT 4
#define RF_2069_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_rc_MASK 0xff00
#define RF_2069_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_rc_SHIFT 8
#define RF_2069_ADC_RC1_adc_ctl_RC_4_0_MASK      0x1f
#define RF_2069_ADC_RC1_adc_ctl_RC_4_0_SHIFT     0
#define RF_2069_LNA2G_CFG1_lna1_low_ct_MASK      0x2
#define RF_2069_LNA2G_CFG1_lna1_low_ct_SHIFT     1
#define RF_2069_LOGEN2G_CFG1_logen_pu_MASK       0x1
#define RF_2069_LOGEN2G_CFG1_logen_pu_SHIFT      0
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_aux_bq1_MASK 0x1
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_aux_bq1_SHIFT 0
#define RF_2069_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_DAC_CFG2_DACbuf_Cap_MASK         0xf8
#define RF_2069_DAC_CFG2_DACbuf_Cap_SHIFT        3
#define RF_2069_OVR4_ovr_auxpga_i_sel_input_MASK 0x80
#define RF_2069_OVR4_ovr_auxpga_i_sel_input_SHIFT 7
#define RF_2069_PAD5G_CFG1_gc_MASK               0xff00
#define RF_2069_PAD5G_CFG1_gc_SHIFT              8
#define RF_2069_TEMPSENSE_CFG_swap_amp_MASK      0x4
#define RF_2069_TEMPSENSE_CFG_swap_amp_SHIFT     2
#define RF_2069_OVR19_ovr_trsw2g_pu_MASK         0x40
#define RF_2069_OVR19_ovr_trsw2g_pu_SHIFT        6
#define RF_2069_OVR10_ovr_lpf_sw_iqcal_bq1_MASK  0x2000
#define RF_2069_OVR10_ovr_lpf_sw_iqcal_bq1_SHIFT 13
#define RF_2069_OVR12_ovr_pga5g_gc_MASK          0x1000
#define RF_2069_OVR12_ovr_pga5g_gc_SHIFT         12
#define RF_2069_WRSSI3_TEST_wrssi3_outsel_MASK   0xf0
#define RF_2069_WRSSI3_TEST_wrssi3_outsel_SHIFT  4
#define RF_2069_ADC_CALCODE21_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE21_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_OVR9_ovr_lpf_sw_aux_bq1_MASK     0x20
#define RF_2069_OVR9_ovr_lpf_sw_aux_bq1_SHIFT    5
#define RF_2069_REV_ID_rev_id_MASK               0xff
#define RF_2069_REV_ID_rev_id_SHIFT              0
#define RF_2069_OVR7_ovr_lna5g_lna2_pu_MASK      0x100
#define RF_2069_OVR7_ovr_lna5g_lna2_pu_SHIFT     8
#define RF_2069_RXRF2G_CFG2_lna2g_epapd_en_MASK  0x10
#define RF_2069_RXRF2G_CFG2_lna2g_epapd_en_SHIFT 4
#define RF_2069_AFEDIV2_afediv_repeater2_dsize_MASK 0xf
#define RF_2069_AFEDIV2_afediv_repeater2_dsize_SHIFT 0
#define RF_2069_PA2G_CFG1_pa2g_gpio_sw_pu_MASK   0x10
#define RF_2069_PA2G_CFG1_pa2g_gpio_sw_pu_SHIFT  4
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_pu_MASK 0x80
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_pu_SHIFT 7
#define RF_2069_ADC_CFG3_Ich_scram_off_MASK      0x4000
#define RF_2069_ADC_CFG3_Ich_scram_off_SHIFT     14
#define RF_2069_PA5G_CFG1_pa5g_5gtx_pu_MASK      0x1
#define RF_2069_PA5G_CFG1_pa5g_5gtx_pu_SHIFT     0
#define RF_2069_ADC_CALCODE17_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE17_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_PA2G_CFG1_trsw2g_pu_MASK         0x20
#define RF_2069_PA2G_CFG1_trsw2g_pu_SHIFT        5
#define RF_2069_PAD2G_TUNE_pad2g_idac_tuning_bias_MASK 0xf0
#define RF_2069_PAD2G_TUNE_pad2g_idac_tuning_bias_SHIFT 4
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_ff_mult_MASK 0x300
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_ff_mult_SHIFT 8
#define RF_2069_OVR12_ovr_pa2g_bias_reset_MASK   0x10
#define RF_2069_OVR12_ovr_pa2g_bias_reset_SHIFT  4
#define RF_2069_ADC_CFG4_Qch_calmode_MASK        0x4
#define RF_2069_ADC_CFG4_Qch_calmode_SHIFT       2
#define RF_2069_OVR8_ovr_logen2g_logen_pu_MASK   0x2000
#define RF_2069_OVR8_ovr_logen2g_logen_pu_SHIFT  13
#define RF_2069_AFEDIV2_afediv_repeater1_dsize_MASK 0x1e0
#define RF_2069_AFEDIV2_afediv_repeater1_dsize_SHIFT 5
#define RF_2069_TXGM_CFG2_ctrl_MASK              0x60
#define RF_2069_TXGM_CFG2_ctrl_SHIFT             5
#define RF_2069_LNA5G_RSSI_dig_wrssi1_pu_MASK    0x1
#define RF_2069_LNA5G_RSSI_dig_wrssi1_pu_SHIFT   0
#define RF_2069_OVR13_ovr_pga2g_gainboost_MASK   0x8
#define RF_2069_OVR13_ovr_pga2g_gainboost_SHIFT  3
#define RF_2069_DAC_CFG1_DAC_att_MASK            0xf0
#define RF_2069_DAC_CFG1_DAC_att_SHIFT           4
#define RF_2069_OVR20_ovr_txgm_pu_MASK           0x40
#define RF_2069_OVR20_ovr_txgm_pu_SHIFT          6
#define RF_2069_OVR7_ovr_lna5g_dig_wrssi2_pu_MASK 0x8000
#define RF_2069_OVR7_ovr_lna5g_dig_wrssi2_pu_SHIFT 15
#define RF_2069_PAD2G_TUNE_pad2g_ptat_slope_tuning_bias_MASK 0x700
#define RF_2069_PAD2G_TUNE_pad2g_ptat_slope_tuning_bias_SHIFT 8
#define RF_2069_LNA2G_RSSI_dig_wrssi2_threshold_MASK 0xf000
#define RF_2069_LNA2G_RSSI_dig_wrssi2_threshold_SHIFT 12
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_bypass_MASK 0x20
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_bypass_SHIFT 5
#define RF_2069_TXGM_CFG2_nf_off_MASK            0x10
#define RF_2069_TXGM_CFG2_nf_off_SHIFT           4
#define RF_2069_ADC_RC1_adc_ctl_RC_12_10_MASK    0x1c00
#define RF_2069_ADC_RC1_adc_ctl_RC_12_10_SHIFT   10
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_overload_MASK 0x4
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_overload_SHIFT 2
#define RF_2069_ADC_BIAS2_adc_bias_MASK          0xf
#define RF_2069_ADC_BIAS2_adc_bias_SHIFT         0
#define RF_2069_TXRX2G_CAL_RX_loopback2g_papdcal_rx_attn_MASK 0x18
#define RF_2069_TXRX2G_CAL_RX_loopback2g_papdcal_rx_attn_SHIFT 3
#define RF_2069_TX_REG_BACKUP_4_reg_backup_4_MASK 0xffff
#define RF_2069_TX_REG_BACKUP_4_reg_backup_4_SHIFT 0
#define RF_2069_TIA_IDAC1_Ib_I_MASK              0xf0
#define RF_2069_TIA_IDAC1_Ib_I_SHIFT             4
#define RF_2069_ADC_CFG2_ctrl_afe_tstmd_MASK     0x30
#define RF_2069_ADC_CFG2_ctrl_afe_tstmd_SHIFT    4
#define RF_2069_PAD5G_CFG1_bias_filter_MASK      0x30
#define RF_2069_PAD5G_CFG1_bias_filter_SHIFT     4
#define RF_2069_LNA2G_IDAC1_lna1_bias_ptat_MASK  0x70
#define RF_2069_LNA2G_IDAC1_lna1_bias_ptat_SHIFT 4
#define RF_2069_OVR6_ovr_lna2g_dig_wrssi2_pu_MASK 0x4000
#define RF_2069_OVR6_ovr_lna2g_dig_wrssi2_pu_SHIFT 14
#define RF_2069_OVR7_ovr_lna5g_gctl2_lp_MASK     0x800
#define RF_2069_OVR7_ovr_lna5g_gctl2_lp_SHIFT    11
#define RF_2069_OVR20_ovr_pa5g_ctrl_tssi_MASK    0x800
#define RF_2069_OVR20_ovr_pa5g_ctrl_tssi_SHIFT   11
#define RF_2069_TIA_IDAC1_Ib_Q_MASK              0xf
#define RF_2069_TIA_IDAC1_Ib_Q_SHIFT             0
#define RF_2069_LPF_BIAS_LEVELS_HIGH_lpf_bias_level5_MASK 0xff
#define RF_2069_LPF_BIAS_LEVELS_HIGH_lpf_bias_level5_SHIFT 0
#define RF_2069_LPF_BIAS_LEVELS_HIGH_lpf_bias_level6_MASK 0xff00
#define RF_2069_LPF_BIAS_LEVELS_HIGH_lpf_bias_level6_SHIFT 8
#define RF_2069_OVR13_ovr_pad5g_gc_MASK          0x20
#define RF_2069_OVR13_ovr_pad5g_gc_SHIFT         5
#define RF_2069_ADC_CFG2_iqadc_clamp_ref_ctl_MASK 0x8
#define RF_2069_ADC_CFG2_iqadc_clamp_ref_ctl_SHIFT 3
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_rx_MASK 0xf0
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_rx_SHIFT 4
#define RF_2069_TX_REG_BACKUP_3_reg_backup_3_MASK 0xffff
#define RF_2069_TX_REG_BACKUP_3_reg_backup_3_SHIFT 0
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_bw_MASK 0xf
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_bw_SHIFT 0
#define RF_2069_OVR8_ovr_logen2g_logen_reset_MASK 0x800
#define RF_2069_OVR8_ovr_logen2g_logen_reset_SHIFT 11
#define RF_2069_NBRSSI_BIAS_nbrssi_ib_Compcore_MASK 0x3
#define RF_2069_NBRSSI_BIAS_nbrssi_ib_Compcore_SHIFT 0
#define RF_2069_PA5G_CFG3_pa5g_ptat_slope_main_MASK 0xf
#define RF_2069_PA5G_CFG3_pa5g_ptat_slope_main_SHIFT 0
#define RF_2069_PAD5G_SLOPE_ptat_slope_aux_MASK  0xf0
#define RF_2069_PAD5G_SLOPE_ptat_slope_aux_SHIFT 4
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_MASK 0x40
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_SHIFT 6
#define RF_2069_OVR10_ovr_lpf_bq2_bw_MASK        0x8
#define RF_2069_OVR10_ovr_lpf_bq2_bw_SHIFT       3
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_MASK    0xf000
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_SHIFT   12
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_MASK 0x40
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_SHIFT 6
#define RF_2069_OVR19_ovr_pa2g_TSSI_range_MASK   0x1
#define RF_2069_OVR19_ovr_pa2g_TSSI_range_SHIFT  0
#define RF_2069_TIA_CFG1_DC_loop_bypass_MASK     0x8
#define RF_2069_TIA_CFG1_DC_loop_bypass_SHIFT    3
#define RF_2069_OVR11_ovr_lpf_g_mult_MASK        0x2
#define RF_2069_OVR11_ovr_lpf_g_mult_SHIFT       1
#define RF_2069_PAD2G_CFG1_pad2g_vcas_monitor_sw_MASK 0x4
#define RF_2069_PAD2G_CFG1_pad2g_vcas_monitor_sw_SHIFT 2
#define RF_2069_OVR3_ovr_afe_iqadc_flash_calcode_Qch_MASK 0x10
#define RF_2069_OVR3_ovr_afe_iqadc_flash_calcode_Qch_SHIFT 4
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_tx_MASK 0xf
#define RF_2069_TRSW2G_CFG1_trsw2g_cntrl_tx_SHIFT 0
#define RF_2069_LNA5G_IDAC2_lna2_main_bias_idac_MASK 0xf
#define RF_2069_LNA5G_IDAC2_lna2_main_bias_idac_SHIFT 0
#define RF_2069_ADC_CALCODE5_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE5_Ich_flash_calcode_SHIFT 0
#define RF_2069_ADC_CFG5_iqadc_ctl_misc_MASK     0xffff
#define RF_2069_ADC_CFG5_iqadc_ctl_misc_SHIFT    0
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_cal_MASK 0x800
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_cal_SHIFT 11
#define RF_2069_TX_REG_BACKUP_2_reg_backup_2_MASK 0xffff
#define RF_2069_TX_REG_BACKUP_2_reg_backup_2_SHIFT 0
#define RF_2069_LOGEN5G_CFG1_lob2_pu_MASK        0x10
#define RF_2069_LOGEN5G_CFG1_lob2_pu_SHIFT       4
#define RF_2069_OVR10_ovr_lpf_sw_dac_bq2_MASK    0x8000
#define RF_2069_OVR10_ovr_lpf_sw_dac_bq2_SHIFT   15
#define RF_2069_PA2G_CFG1_pa2g_2gtx_pu_MASK      0x1
#define RF_2069_PA2G_CFG1_pa2g_2gtx_pu_SHIFT     0
#define RF_2069_RXRF2G_CFG1_globe_idac_MASK      0xfe0
#define RF_2069_RXRF2G_CFG1_globe_idac_SHIFT     5
#define RF_2069_ADC_CFG3_clk_ctl_MASK            0xf00
#define RF_2069_ADC_CFG3_clk_ctl_SHIFT           8
#define RF_2069_LNA5G_RSSI_dig_wrssi1_threshold_MASK 0xf0
#define RF_2069_LNA5G_RSSI_dig_wrssi1_threshold_SHIFT 4
#define RF_2069_OVR21_ovr_rx2g_calpath_lna2_pu_MASK 0x40
#define RF_2069_OVR21_ovr_rx2g_calpath_lna2_pu_SHIFT 6
#define RF_2069_LNA5G_IDAC2_lna2_aux_bias_idac_MASK 0xf0
#define RF_2069_LNA5G_IDAC2_lna2_aux_bias_idac_SHIFT 4
#define RF_2069_RXMIX5G_IDAC_gm_aux_bias_ptat_i_MASK 0x7000
#define RF_2069_RXMIX5G_IDAC_gm_aux_bias_ptat_i_SHIFT 12
#define RF_2069_LNA2G_CFG1_tr_rx_en_MASK         0x8
#define RF_2069_LNA2G_CFG1_tr_rx_en_SHIFT        3
#define RF_2069_DAC_CFG1_DAC_pwrup_MASK          0x1
#define RF_2069_DAC_CFG1_DAC_pwrup_SHIFT         0
#define RF_2069_DAC_BIAS_DACbuf_ctl_MASK         0x3ff
#define RF_2069_DAC_BIAS_DACbuf_ctl_SHIFT        0
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_lpf_MASK 0x1
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_lpf_SHIFT 0
#define RF_2069_PAD5G_CFG1_pu_MASK               0x1
#define RF_2069_PAD5G_CFG1_pu_SHIFT              0
#define RF_2069_OVR12_ovr_pga5g_pu_MASK          0x800
#define RF_2069_OVR12_ovr_pga5g_pu_SHIFT         11
#define RF_2069_LOGEN5G_TUNE2_ctune_buf_MASK     0xf
#define RF_2069_LOGEN5G_TUNE2_ctune_buf_SHIFT    0
#define RF_2069_OVR7_ovr_logen2g_tx_pu_MASK      0x8
#define RF_2069_OVR7_ovr_logen2g_tx_pu_SHIFT     3
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_bq2_MASK 0x400
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_dac_bq2_SHIFT 10
#define RF_2069_OVR13_ovr_pga5g_gainboost_MASK   0x1
#define RF_2069_OVR13_ovr_pga5g_gainboost_SHIFT  0
#define RF_2069_OVR6_ovr_lna2g_lna1_bypass_MASK  0x800
#define RF_2069_OVR6_ovr_lna2g_lna1_bypass_SHIFT 11
#define RF_2069_OVR10_ovr_lpf_sw_dac_rc_MASK     0x4000
#define RF_2069_OVR10_ovr_lpf_sw_dac_rc_SHIFT    14
#define RF_2069_LOGEN2G_CFG2_rx_pu_MASK          0x4
#define RF_2069_LOGEN2G_CFG2_rx_pu_SHIFT         2
#define RF_2069_RXRF5G_CFG2_lna5g_epapd_attn_MASK 0xf
#define RF_2069_RXRF5G_CFG2_lna5g_epapd_attn_SHIFT 0
#define RF_2069_TXMIX2G_CFG1_lodc_MASK           0xf0
#define RF_2069_TXMIX2G_CFG1_lodc_SHIFT          4
#define RF_2069_PAD5G_TUNE_idac_tuning_bias_MASK 0xf0
#define RF_2069_PAD5G_TUNE_idac_tuning_bias_SHIFT 4
#define RF_2069_ADC_CFG2_iqadc_ovdrv_ref_ctl_MASK 0x2
#define RF_2069_ADC_CFG2_iqadc_ovdrv_ref_ctl_SHIFT 1
#define RF_2069_OVR5_ovr_iqcal_PU_iqcal_MASK     0x800
#define RF_2069_OVR5_ovr_iqcal_PU_iqcal_SHIFT    11
#define RF_2069_TX_REG_BACKUP_1_reg_backup_1_MASK 0xffff
#define RF_2069_TX_REG_BACKUP_1_reg_backup_1_SHIFT 0
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_iqcal_bq1_MASK 0x1000
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_iqcal_bq1_SHIFT 12
#define RF_2069_OVR9_ovr_logen5g_tx_pu_MASK      0x80
#define RF_2069_OVR9_ovr_logen5g_tx_pu_SHIFT     7
#define RF_2069_ADC_CFG3_pwrup_clk_MASK          0x2000
#define RF_2069_ADC_CFG3_pwrup_clk_SHIFT         13
#define RF_2069_ADC_CFG1_Ich_pwrup_MASK          0x3f
#define RF_2069_ADC_CFG1_Ich_pwrup_SHIFT         0
#define RF_2069_PAD5G_IDAC_idac_cascode_MASK     0x7800
#define RF_2069_PAD5G_IDAC_idac_cascode_SHIFT    11
#define RF_2069_LOGEN2G_IDAC3_idac_qrx_MASK      0x70
#define RF_2069_LOGEN2G_IDAC3_idac_qrx_SHIFT     4
#define RF_2069_TXMIX2G_CFG1_gc_load_MASK        0xf
#define RF_2069_TXMIX2G_CFG1_gc_load_SHIFT       0
#define RF_2069_LOGEN5G_TUNE1_ctune_vcob_MASK    0xf
#define RF_2069_LOGEN5G_TUNE1_ctune_vcob_SHIFT   0
#define RF_2069_PA5G_CFG1_trsw5g_pu_MASK         0x20
#define RF_2069_PA5G_CFG1_trsw5g_pu_SHIFT        5
#define RF_2069_RXRF2G_CFG1_globe_pu_MASK        0x1
#define RF_2069_RXRF2G_CFG1_globe_pu_SHIFT       0
#define RF_2069_DAC_CFG1_DAC_scram_mode_MASK     0x400
#define RF_2069_DAC_CFG1_DAC_scram_mode_SHIFT    10
#define RF_2069_OVR12_ovr_pa2g_2gtx_pu_MASK      0x4
#define RF_2069_OVR12_ovr_pa2g_2gtx_pu_SHIFT     2
#define RF_2069_PGA5G_CFG2_ptat_slope_aux_MASK   0xf0
#define RF_2069_PGA5G_CFG2_ptat_slope_aux_SHIFT  4
#define RF_2069_WRSSI3_TEST_wrssi3_en_test_MASK  0x1
#define RF_2069_WRSSI3_TEST_wrssi3_en_test_SHIFT 0
#define RF_2069_DAC_CFG2_DACbuf_fixed_cap_MASK   0x4
#define RF_2069_DAC_CFG2_DACbuf_fixed_cap_SHIFT  2
#define RF_2069_OVR18_ovr_rx5g_calpath_mix_pu_MASK 0x4000
#define RF_2069_OVR18_ovr_rx5g_calpath_mix_pu_SHIFT 14
#define RF_2069_TIA_CFG2_Ib_tempco_MASK          0x380
#define RF_2069_TIA_CFG2_Ib_tempco_SHIFT         7
#define RF_2069_PA5G_CFG2_pa5g_bias_filter_main_MASK 0xf
#define RF_2069_PA5G_CFG2_pa5g_bias_filter_main_SHIFT 0
#define RF_2069_LOGEN2G_CFG1_logen_reset_MASK    0x4
#define RF_2069_LOGEN2G_CFG1_logen_reset_SHIFT   2
#define RF_2069_LNA2G_IDAC2_lna2_main_bias_idac_MASK 0xf
#define RF_2069_LNA2G_IDAC2_lna2_main_bias_idac_SHIFT 0
#define RF_2069_LOGEN2G_IDAC3_idac_qtx_MASK      0x7000
#define RF_2069_LOGEN2G_IDAC3_idac_qtx_SHIFT     12
#define RF_2069_TRSW2G_CFG2_trsw2g_bt_en_MASK    0x100
#define RF_2069_TRSW2G_CFG2_trsw2g_bt_en_SHIFT   8
#define RF_2069_LPF_BIAS_LEVELS_LOW_lpf_bias_level1_MASK 0xff
#define RF_2069_LPF_BIAS_LEVELS_LOW_lpf_bias_level1_SHIFT 0
#define RF_2069_LNA2G_RSSI_dig_wrssi1_threshold_MASK 0xf0
#define RF_2069_LNA2G_RSSI_dig_wrssi1_threshold_SHIFT 4
#define RF_2069_LPF_BIAS_LEVELS_LOW_lpf_bias_level2_MASK 0xff00
#define RF_2069_LPF_BIAS_LEVELS_LOW_lpf_bias_level2_SHIFT 8
#define RF_2069_OVR5_ovr_afe_iqadc_adc_bias_MASK 0x8000
#define RF_2069_OVR5_ovr_afe_iqadc_adc_bias_SHIFT 15
#define RF_2069_TXMIX5G_CFG1_gainboost_MASK      0xf000
#define RF_2069_TXMIX5G_CFG1_gainboost_SHIFT     12
#define RF_2069_LNA2G_CFG1_lna1_gain_MASK        0x7000
#define RF_2069_LNA2G_CFG1_lna1_gain_SHIFT       12
#define RF_2069_OVR6_ovr_lna2g_lna2_gain_MASK    0x20
#define RF_2069_OVR6_ovr_lna2g_lna2_gain_SHIFT   5
#define RF_2069_ADC_CFG3_flash_calrstb_MASK      0x1000
#define RF_2069_ADC_CFG3_flash_calrstb_SHIFT     12
#define RF_2069_LNA5G_RSSI_dig_wrssi1_drive_strength_MASK 0x2
#define RF_2069_LNA5G_RSSI_dig_wrssi1_drive_strength_SHIFT 1
#define RF_2069_LPF_BIAS_LEVELS_MID_lpf_bias_level3_MASK 0xff
#define RF_2069_LPF_BIAS_LEVELS_MID_lpf_bias_level3_SHIFT 0
#define RF_2069_LPF_BIAS_LEVELS_MID_lpf_bias_level4_MASK 0xff00
#define RF_2069_LPF_BIAS_LEVELS_MID_lpf_bias_level4_SHIFT 8
#define RF_2069_TRSW2G_CFG2_trsw2g_trsw_gate_bis_MASK 0xf
#define RF_2069_TRSW2G_CFG2_trsw2g_trsw_gate_bis_SHIFT 0
#define RF_2069_OVR20_ovr_txmix2g_gc_load_MASK   0x8
#define RF_2069_OVR20_ovr_txmix2g_gc_load_SHIFT  3
#define RF_2069_TESTBUF_CFG1_PU_MASK             0x1
#define RF_2069_TESTBUF_CFG1_PU_SHIFT            0
#define RF_2069_LNA5G_CFG2_ana_wrssi2_pu_MASK    0x4
#define RF_2069_LNA5G_CFG2_ana_wrssi2_pu_SHIFT   2
#define RF_2069_ADC_CALCODE10_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE10_Qch_flash_calcode_SHIFT 0
#define RF_2069_OVR13_ovr_pad5g_pu_MASK          0x10
#define RF_2069_OVR13_ovr_pad5g_pu_SHIFT         4
#define RF_2069_OVR17_ovr_afediv_sel_div_MASK    0x10
#define RF_2069_OVR17_ovr_afediv_sel_div_SHIFT   4
#define RF_2069_OVR21_ovr_afe_DACbuf_Cap_MASK    0x4000
#define RF_2069_OVR21_ovr_afe_DACbuf_Cap_SHIFT   14
#define RF_2069_OVR19_ovr_tia_DC_loop_bypass_MASK 0x4000
#define RF_2069_OVR19_ovr_tia_DC_loop_bypass_SHIFT 14
#define RF_2069_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_qb_2nd_MASK 0x38
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_qb_2nd_SHIFT 3
#define RF_2069_ADC_CALCODE27_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE27_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_LOGEN5G_IDAC1_idac_buf_MASK      ((ACREV0_SUB) ? 0xf000 : 0x7000)
#define RF_2069_LOGEN5G_IDAC1_idac_buf_SHIFT     12
#define RF_2069_TXMIX5G_CFG1_lodc_MASK           0xf0
#define RF_2069_TXMIX5G_CFG1_lodc_SHIFT          4
#define RF_2069_TXRX5G_CAL_TX_i_cal_pa_atten_5g_MASK 0x18
#define RF_2069_TXRX5G_CAL_TX_i_cal_pa_atten_5g_SHIFT 3
#define RF_2069_ADC_RC1_adc_ctl_RC_15_13_MASK    0xe000
#define RF_2069_ADC_RC1_adc_ctl_RC_15_13_SHIFT   13
#define RF_2069_OVR17_ovr_afediv_pu0_inbuf_MASK  0x8
#define RF_2069_OVR17_ovr_afediv_pu0_inbuf_SHIFT 3
#define RF_2069_OVR20_ovr_txgm_sel5g_MASK        0x20
#define RF_2069_OVR20_ovr_txgm_sel5g_SHIFT       5
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_rc_MASK 0x100
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_rc_SHIFT 8
#define RF_2069_TXGM_LOFT_COARSE_Q_idac_lo_rf_qn_MASK 0xf0
#define RF_2069_TXGM_LOFT_COARSE_Q_idac_lo_rf_qn_SHIFT 4
#define RF_2069_TXGM_LOFT_COARSE_Q_idac_lo_rf_qp_MASK 0xf
#define RF_2069_TXGM_LOFT_COARSE_Q_idac_lo_rf_qp_SHIFT 0
#define RF_2069_ADC_CALCODE24_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE24_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_IQCAL_CFG2_sel_nbw_iqcal_MASK    0x2
#define RF_2069_IQCAL_CFG2_sel_nbw_iqcal_SHIFT   1
#define RF_2069_OVR6_ovr_lna2g_lna2_gm_size_MASK 0x10
#define RF_2069_OVR6_ovr_lna2g_lna2_gm_size_SHIFT 4
#define RF_2069_PGA5G_CFG2_tune_MASK             0xf000
#define RF_2069_PGA5G_CFG2_tune_SHIFT            12
#define RF_2069_PA5G_INCAP_pa5g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_PA5G_INCAP_pa5g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_ADC_CFG2_iqadc_reset_ov_det_MASK 0x1
#define RF_2069_ADC_CFG2_iqadc_reset_ov_det_SHIFT 0
#define RF_2069_TX5G_TSSI_pa5g_tssi_pu_MASK      0x1
#define RF_2069_TX5G_TSSI_pa5g_tssi_pu_SHIFT     0
#define RF_2069_LOGEN5G_TUNE2_ctune_buf1_MASK    0xf0
#define RF_2069_LOGEN5G_TUNE2_ctune_buf1_SHIFT   4
#define RF_2069_OVR10_ovr_lpf_bq2_gain_MASK      0x1
#define RF_2069_OVR10_ovr_lpf_bq2_gain_SHIFT     0
#define RF_2069_LOGEN5G_TUNE2_ctune_buf2_MASK    0xf00
#define RF_2069_LOGEN5G_TUNE2_ctune_buf2_SHIFT   8
#define RF_2069_TRSW5G_CFG2_trsw5g_trsw_dwell_bis_MASK 0xf0
#define RF_2069_TRSW5G_CFG2_trsw5g_trsw_dwell_bis_SHIFT 4
#define RF_2069_ADC_CALCODE8_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE8_Qch_flash_calcode_SHIFT 0
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_ff_mult_MASK 0xc0
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_ff_mult_SHIFT 6
#define RF_2069_OVR10_ovr_lpf_pu_dc_i_MASK       0x40
#define RF_2069_OVR10_ovr_lpf_pu_dc_i_SHIFT      6
#define RF_2069_TX2G_CFG1_pga2g_pu_MASK          0x4
#define RF_2069_TX2G_CFG1_pga2g_pu_SHIFT         2
#define RF_2069_ADC_CFG4_iqadc_flash_only_MASK   0x400
#define RF_2069_ADC_CFG4_iqadc_flash_only_SHIFT  10
#define RF_2069_TIA_CFG1_pu_MASK                 0x1
#define RF_2069_TIA_CFG1_pu_SHIFT                0
#define RF_2069_WRSSI3_CONFG_wrssi3_Reflow_puI_MASK 0x10
#define RF_2069_WRSSI3_CONFG_wrssi3_Reflow_puI_SHIFT 4
#define RF_2069_LNA5G_CFG1_lna1_pu_MASK          0x1
#define RF_2069_LNA5G_CFG1_lna1_pu_SHIFT         0
#define RF_2069_OVR20_ovr_tx5g_calpath_pa_pu_MASK 0x1000
#define RF_2069_OVR20_ovr_tx5g_calpath_pa_pu_SHIFT 12
#define RF_2069_OVR8_ovr_logen5g_lob2_pu_MASK    0x80
#define RF_2069_OVR8_ovr_logen5g_lob2_pu_SHIFT   7
#define RF_2069_NBRSSI_CONFG_nbrssi_Refmid_puI_MASK 0x4
#define RF_2069_NBRSSI_CONFG_nbrssi_Refmid_puI_SHIFT 2
#define RF_2069_OVR10_ovr_lpf_pu_dc_q_MASK       0x20
#define RF_2069_OVR10_ovr_lpf_pu_dc_q_SHIFT      5
#define RF_2069_RXMIX5G_CFG1_gm_size_MASK        0x700
#define RF_2069_RXMIX5G_CFG1_gm_size_SHIFT       8
#define RF_2069_OVR3_ovr_afe_DAC_att_MASK        0x1000
#define RF_2069_OVR3_ovr_afe_DAC_att_SHIFT       12
#define RF_2069_PA2G_CFG3_pa2g_ptat_slope_main_MASK 0xf
#define RF_2069_PA2G_CFG3_pa2g_ptat_slope_main_SHIFT 0
#define RF_2069_WRSSI3_CONFG_wrssi3_Reflow_puQ_MASK 0x20
#define RF_2069_WRSSI3_CONFG_wrssi3_Reflow_puQ_SHIFT 5
#define RF_2069_OVR20_ovr_trsw5g_pu_MASK         0x8000
#define RF_2069_OVR20_ovr_trsw5g_pu_SHIFT        15
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_cm_mult_MASK 0x30
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_cm_mult_SHIFT 4
#define RF_2069_OVR13_ovr_pa5g_gain_ctrl_MASK    0x800
#define RF_2069_OVR13_ovr_pa5g_gain_ctrl_SHIFT   11
#define RF_2069_GPAIO_SEL1_sel_16to31_port_MASK  0xffff
#define RF_2069_GPAIO_SEL1_sel_16to31_port_SHIFT 0
#define RF_2069_NBRSSI_CONFG_nbrssi_Refmid_puQ_MASK 0x8
#define RF_2069_NBRSSI_CONFG_nbrssi_Refmid_puQ_SHIFT 3
#define RF_2069_OVR3_ovr_afe_iqadc_flashhspd_MASK 0x4
#define RF_2069_OVR3_ovr_afe_iqadc_flashhspd_SHIFT 2
#define RF_2069_AFEDIV1_afediv_cal_driver_size_MASK 0x380
#define RF_2069_AFEDIV1_afediv_cal_driver_size_SHIFT 7
#define RF_2069_NBRSSI_TEST_nbrssi_sel_test_MASK 0xf00
#define RF_2069_NBRSSI_TEST_nbrssi_sel_test_SHIFT 8
#define RF_2069_OVR3_ovr_afe_auxpga_i_sel_vmid_MASK 0x2000
#define RF_2069_OVR3_ovr_afe_auxpga_i_sel_vmid_SHIFT 13
#define RF_2069_GP_REGISTER_gp_pcie_MASK         0xffff
#define RF_2069_GP_REGISTER_gp_pcie_SHIFT        0
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_qp_gpaio_MASK 0x40
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_qp_gpaio_SHIFT 6
#define RF_2069_PGA5G_INCAP_ptat_slope_incap_compen_MASK 0x70
#define RF_2069_PGA5G_INCAP_ptat_slope_incap_compen_SHIFT 4
#define RF_2069_ADC_RC1_adc_ctl_RC_7_5_MASK      0xe0
#define RF_2069_ADC_RC1_adc_ctl_RC_7_5_SHIFT     5
#define RF_2069_OVR4_ovr_iq_cal_pu_tssi_MASK     0x2
#define RF_2069_OVR4_ovr_iq_cal_pu_tssi_SHIFT    1
#define RF_2069_ADC_CFG4_Qch_reset_MASK          0x10
#define RF_2069_ADC_CFG4_Qch_reset_SHIFT         4
#define RF_2069_OVR21_ovr_lna2g_lna1_out_short_pu_MASK 0x400
#define RF_2069_OVR21_ovr_lna2g_lna1_out_short_pu_SHIFT 10
#define RF_2069_CLB_REG_1_clb_sel_MASK           0x3f
#define RF_2069_CLB_REG_1_clb_sel_SHIFT          0
#define RF_2069_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_0p7_control_MASK 0xff
#define RF_2069_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_0p7_control_SHIFT 0
#define RF_2069_TX2G_CFG1_txmix2g_pu_MASK        0x1
#define RF_2069_TX2G_CFG1_txmix2g_pu_SHIFT       0
#define RF_2069_LOGEN5G_IDAC1_idac_div_MASK      0x70
#define RF_2069_LOGEN5G_IDAC1_idac_div_SHIFT     4
#define RF_2069_AFEDIV2_afediv_pu_repeater1_MASK 0x200
#define RF_2069_AFEDIV2_afediv_pu_repeater1_SHIFT 9
#define RF_2069_OVR21_ovr_lna5g_lna1_out_short_pu_MASK 0x200
#define RF_2069_OVR21_ovr_lna5g_lna1_out_short_pu_SHIFT 9
#define RF_2069_AFEDIV2_afediv_pu_repeater2_MASK 0x10
#define RF_2069_AFEDIV2_afediv_pu_repeater2_SHIFT 4
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_gm2nd_MASK 0xe00
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_gm2nd_SHIFT 9
#define RF_2069_OVR18_ovr_rx2g_calpath_mix_pu_MASK 0x8000
#define RF_2069_OVR18_ovr_rx2g_calpath_mix_pu_SHIFT 15
#define RF_2069_PA5G_IDAC2_pa5g_biasa_aux_MASK   0xff00
#define RF_2069_PA5G_IDAC2_pa5g_biasa_aux_SHIFT  8
#define RF_2069_TIA_IDAC1_DC_Ib1_MASK            0xf000
#define RF_2069_TIA_IDAC1_DC_Ib1_SHIFT           12
#define RF_2069_TIA_IDAC1_DC_Ib2_MASK            0xf00
#define RF_2069_TIA_IDAC1_DC_Ib2_SHIFT           8
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_cal_MASK 0x8
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_cal_SHIFT 3
#define RF_2069_CLB_REG_0_clb_out_MASK           0xffff
#define RF_2069_CLB_REG_0_clb_out_SHIFT          0
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_qn_gpaio_MASK 0x80
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_qn_gpaio_SHIFT 7
#define RF_2069_OVR5_ovr_bias_tx_pu_MASK         0x400
#define RF_2069_OVR5_ovr_bias_tx_pu_SHIFT        10
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq1_adc_MASK 0x2
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq1_adc_SHIFT 1
#define RF_2069_TXMIX2G_CFG1_gainboost_MASK      0xf000
#define RF_2069_TXMIX2G_CFG1_gainboost_SHIFT     12
#define RF_2069_LOGEN5G_CFG1_vcobuf_pu_MASK      0x2
#define RF_2069_LOGEN5G_CFG1_vcobuf_pu_SHIFT     1
#define RF_2069_GPAIO_SEL0_sel_0to15_port_MASK   0xffff
#define RF_2069_GPAIO_SEL0_sel_0to15_port_SHIFT  0
#define RF_2069_GPAIO_CFG1_gpaio_pu_MASK         0x1
#define RF_2069_GPAIO_CFG1_gpaio_pu_SHIFT        0
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_Qch_cal_state_MASK 0x1
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_Qch_cal_state_SHIFT 0
#define RF_2069_ADC_CALCODE1_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE1_Ich_flash_calcode_SHIFT 0
#define RF_2069_OVR20_ovr_txmix2g_gainboost_MASK 0x10
#define RF_2069_OVR20_ovr_txmix2g_gainboost_SHIFT 4
#define RF_2069_ADC_CALCODE13_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE13_Qch_flash_calcode_SHIFT 0
#define RF_2069_ADC_RC2_adc_ctrl_RC_17_16_MASK   0x3
#define RF_2069_ADC_RC2_adc_ctrl_RC_17_16_SHIFT  0
#define RF_2069_OVR9_ovr_lpf_sw_bq1_adc_MASK     0x8
#define RF_2069_OVR9_ovr_lpf_sw_bq1_adc_SHIFT    3
#define RF_2069_ADC_CFG4_ctrl_flashhspd_MASK     0x200
#define RF_2069_ADC_CFG4_ctrl_flashhspd_SHIFT    9
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq1_bw_MASK 0x7
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq1_bw_SHIFT 0
#define RF_2069_PGA2G_IDAC_pga2g_idac_main_MASK  0x3f
#define RF_2069_PGA2G_IDAC_pga2g_idac_main_SHIFT 0
#define RF_2069_DAC_CFG1_DAC_scram_off_MASK      0x300
#define RF_2069_DAC_CFG1_DAC_scram_off_SHIFT     8
#define RF_2069_ADC_CFG2_iqadc_clamp_en_MASK     0x4
#define RF_2069_ADC_CFG2_iqadc_clamp_en_SHIFT    2
#define RF_2069_RXMIX5G_CFG1_bias_reset_MASK     0x2
#define RF_2069_RXMIX5G_CFG1_bias_reset_SHIFT    1
#define RF_2069_LNA2G_TUNE_tx_tune_MASK          0xf00
#define RF_2069_LNA2G_TUNE_tx_tune_SHIFT         8
#define RF_2069_OVR7_ovr_logen2g_rx_pu_MASK      0x10
#define RF_2069_OVR7_ovr_logen2g_rx_pu_SHIFT     4
#define RF_2069_LOGEN2G_TUNE_logen2g_buftune_2nd_MASK 0xf0
#define RF_2069_LOGEN2G_TUNE_logen2g_buftune_2nd_SHIFT 4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_ip_gpaio_MASK 0x10
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_ip_gpaio_SHIFT 4
#define RF_2069_LOGEN5G_RCCR_rccr_rx_MASK        0xf0
#define RF_2069_LOGEN5G_RCCR_rccr_rx_SHIFT       4
#define RF_2069_DAC_CFG1_DAC_invclk_MASK         0x1000
#define RF_2069_DAC_CFG1_DAC_invclk_SHIFT        12
#define RF_2069_LNA5G_CFG1_FBC_MASK              0x30
#define RF_2069_LNA5G_CFG1_FBC_SHIFT             4
#define RF_2069_OVR7_ovr_lna5g_lna1_pu_MASK      0x200
#define RF_2069_OVR7_ovr_lna5g_lna1_pu_SHIFT     9
#define RF_2069_RADIO_SPARE4_radio_spare4_MASK   0xffff
#define RF_2069_RADIO_SPARE4_radio_spare4_SHIFT  0
#define RF_2069_OVR9_ovr_logen5g_rx_pu_MASK      0x100
#define RF_2069_OVR9_ovr_logen5g_rx_pu_SHIFT     8
#define RF_2069_ADC_CFG2_rst_clk_MASK            0x40
#define RF_2069_ADC_CFG2_rst_clk_SHIFT           6
#define RF_2069_OVR8_ovr_logen2g_pu_MASK         0x4000
#define RF_2069_OVR8_ovr_logen2g_pu_SHIFT        14
#define RF_2069_LOGEN5G_RCCR_rccr_tx_MASK        0xf
#define RF_2069_LOGEN5G_RCCR_rccr_tx_SHIFT       0
#define RF_2069_OVR18_ovr_rxrf5g_globe_pu_MASK   0x10
#define RF_2069_OVR18_ovr_rxrf5g_globe_pu_SHIFT  4
#define RF_2069_IQCAL_CFG2_hg_iqcal_MASK         0x1
#define RF_2069_IQCAL_CFG2_hg_iqcal_SHIFT        0
#define RF_2069_PAD5G_IDAC_ptat_slope_tuning_bias_MASK 0x700
#define RF_2069_PAD5G_IDAC_ptat_slope_tuning_bias_SHIFT 8
#define RF_2069_LOGEN5G_IDAC4_idac_qrx_MASK      0x70
#define RF_2069_LOGEN5G_IDAC4_idac_qrx_SHIFT     4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_in_gpaio_MASK 0x20
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_in_gpaio_SHIFT 5
#define RF_2069_RADIO_SPARE3_radio_spare3_MASK   0xffff
#define RF_2069_RADIO_SPARE3_radio_spare3_SHIFT  0
#define RF_2069_LPF_ENABLES_lpf_pu_bq1_i_MASK    0x2
#define RF_2069_LPF_ENABLES_lpf_pu_bq1_i_SHIFT   1
#define RF_2069_PA2G_TSSI_pa2g_tssi_pu_MASK      0x1
#define RF_2069_PA2G_TSSI_pa2g_tssi_pu_SHIFT     0
#define RF_2069_TXRX5G_CAL_TX_i_cal_pad_atten_5g_MASK 0x3
#define RF_2069_TXRX5G_CAL_TX_i_cal_pad_atten_5g_SHIFT 0
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_cr_pu_MASK 0x20
#define RF_2069_TXRX2G_CAL_RX_loopback2g_rxiqcal_cr_pu_SHIFT 5
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_pu_MASK 0x100
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi2_pu_SHIFT 8
#define RF_2069_TXGM_CFG1_gc_pmos_MASK           0x700
#define RF_2069_TXGM_CFG1_gc_pmos_SHIFT          8
#define RF_2069_LNA5G_RSSI_dig_wrssi2_drive_strength_MASK 0x200
#define RF_2069_LNA5G_RSSI_dig_wrssi2_drive_strength_SHIFT 9
#define RF_2069_IQCAL_CFG2_iq_cm_center_MASK     0xf0
#define RF_2069_IQCAL_CFG2_iq_cm_center_SHIFT    4
#define RF_2069_LPF_ENABLES_lpf_pu_bq1_q_MASK    0x4
#define RF_2069_LPF_ENABLES_lpf_pu_bq1_q_SHIFT   2
#define RF_2069_LOGEN5G_IDAC4_idac_qtx_MASK      0x7000
#define RF_2069_LOGEN5G_IDAC4_idac_qtx_SHIFT     12
#define RF_2069_LNA5G_CFG2_gctl2_MASK            0x380
#define RF_2069_LNA5G_CFG2_gctl2_SHIFT           7
#define RF_2069_PA2G_TSSI_pa2g_TSSI_range_MASK   0x2
#define RF_2069_PA2G_TSSI_pa2g_TSSI_range_SHIFT  1
#define RF_2069_PGA5G_CFG2_ptat_slope_main_MASK  0xf
#define RF_2069_PGA5G_CFG2_ptat_slope_main_SHIFT 0
#define RF_2069_OVR6_ovr_lna2g_dig_wrssi1_pu_MASK 0x8000
#define RF_2069_OVR6_ovr_lna2g_dig_wrssi1_pu_SHIFT 15
#define RF_2069_OVR7_ovr_lna5g_gctl1_ln_MASK     0x2000
#define RF_2069_OVR7_ovr_lna5g_gctl1_ln_SHIFT    13
#define RF_2069_RADIO_SPARE2_radio_spare2_MASK   0xffff
#define RF_2069_RADIO_SPARE2_radio_spare2_SHIFT  0
#define RF_2069_OVR6_ovr_lna2g_lna2_pu_MASK      0x8
#define RF_2069_OVR6_ovr_lna2g_lna2_pu_SHIFT     3
#define RF_2069_TXGM_LOFT_SCALE_idac_lo_rf_diode_MASK 0x3
#define RF_2069_TXGM_LOFT_SCALE_idac_lo_rf_diode_SHIFT 0
#define RF_2069_LOGEN2G_CFG1_div1p5_pu_MASK      0x2
#define RF_2069_LOGEN2G_CFG1_div1p5_pu_SHIFT     1
#define RF_2069_LNA2G_CFG2_lna2_pu_MASK          0x1
#define RF_2069_LNA2G_CFG2_lna2_pu_SHIFT         0
#define RF_2069_ADC_CALCODE18_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE18_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_LNA5G_IDAC2_lna2_main_bias_ptat_MASK 0x700
#define RF_2069_LNA5G_IDAC2_lna2_main_bias_ptat_SHIFT 8
#define RF_2069_TX5G_TSSI_pa5g_ctrl_tssi_sel_MASK 0x4
#define RF_2069_TX5G_TSSI_pa5g_ctrl_tssi_sel_SHIFT 2
#define RF_2069_TXRX2G_CAL_TX_i_cal_pad_atten_2g_MASK 0x3
#define RF_2069_TXRX2G_CAL_TX_i_cal_pad_atten_2g_SHIFT 0
#define RF_2069_PA2G_IDAC2_pa2g_biasa_main_MASK  0xff
#define RF_2069_PA2G_IDAC2_pa2g_biasa_main_SHIFT 0
#define RF_2069_TXGM_LOFT_FINE_Q_idac_lo_bb_qn_MASK 0xf0
#define RF_2069_TXGM_LOFT_FINE_Q_idac_lo_bb_qn_SHIFT 4
#define RF_2069_LNA5G_CFG1_gctl1_MASK            0x3800
#define RF_2069_LNA5G_CFG1_gctl1_SHIFT           11
#define RF_2069_OVR3_ovr_auxpga_i_pu_MASK        0x8000
#define RF_2069_OVR3_ovr_auxpga_i_pu_SHIFT       15
#define RF_2069_PA2G_IDAC2_pa2g_biasa_aux_MASK   0xff00
#define RF_2069_PA2G_IDAC2_pa2g_biasa_aux_SHIFT  8
#define RF_2069_OVR3_ovr_afe_iqadc_clamp_en_MASK 0x400
#define RF_2069_OVR3_ovr_afe_iqadc_clamp_en_SHIFT 10
#define RF_2069_TXGM_LOFT_FINE_Q_idac_lo_bb_qp_MASK 0xf
#define RF_2069_TXGM_LOFT_FINE_Q_idac_lo_bb_qp_SHIFT 0
#define RF_2069_RXMIX5G_CFG1_LO_bias_MASK        0x70
#define RF_2069_RXMIX5G_CFG1_LO_bias_SHIFT       4
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_high_MASK 0x40
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_high_SHIFT 6
#define RF_2069_LOGEN2G_TUNE_logen2g_buftune_MASK 0xf00
#define RF_2069_LOGEN2G_TUNE_logen2g_buftune_SHIFT 8
#define RF_2069_OVR6_ovr_lna2g_tr_rx_en_MASK     0x2
#define RF_2069_OVR6_ovr_lna2g_tr_rx_en_SHIFT    1
#define RF_2069_PAD2G_CFG1_pad2g_bias_filter_bypass_MASK 0x2
#define RF_2069_PAD2G_CFG1_pad2g_bias_filter_bypass_SHIFT 1
#define RF_2069_LNA5G_IDAC2_lna2_aux_bias_ptat_MASK 0x7000
#define RF_2069_LNA5G_IDAC2_lna2_aux_bias_ptat_SHIFT 12
#define RF_2069_OVR10_ovr_lpf_sw_tia_bq1_MASK    0x1000
#define RF_2069_OVR10_ovr_lpf_sw_tia_bq1_SHIFT   12
#define RF_2069_NBRSSI_BIAS_nbrssi_misc_MASK     0xff00
#define RF_2069_NBRSSI_BIAS_nbrssi_misc_SHIFT    8
#define RF_2069_OVR10_ovr_lpf_bq1_bw_MASK        0x10
#define RF_2069_OVR10_ovr_lpf_bq1_bw_SHIFT       4
#define RF_2069_ADC_CALCODE15_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE15_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_RADIO_SPARE1_radio_spare1_MASK   0xffff
#define RF_2069_RADIO_SPARE1_radio_spare1_SHIFT  0
#define RF_2069_OVR7_ovr_logen2g_reset_rx_MASK   0x2
#define RF_2069_OVR7_ovr_logen2g_reset_rx_SHIFT  1
#define RF_2069_PA5G_CFG3_pa5g_sel_bias_type_MASK 0xff00
#define RF_2069_PA5G_CFG3_pa5g_sel_bias_type_SHIFT 8
#define RF_2069_TXMIX2G_CFG1_tune_MASK           0xf00
#define RF_2069_TXMIX2G_CFG1_tune_SHIFT          8
#define RF_2069_DAC_CFG1_DAC_rst_MASK            0x2000
#define RF_2069_DAC_CFG1_DAC_rst_SHIFT           13
#define RF_2069_OVR17_ovr_afediv_pu_repeater1_MASK 0x2
#define RF_2069_OVR17_ovr_afediv_pu_repeater1_SHIFT 1
#define RF_2069_OVR17_ovr_afediv_pu_repeater2_MASK 0x1
#define RF_2069_OVR17_ovr_afediv_pu_repeater2_SHIFT 0
#define RF_2069_OVR6_ovr_lna2g_lna1_low_ct_MASK  0x100
#define RF_2069_OVR6_ovr_lna2g_lna1_low_ct_SHIFT 8
#define RF_2069_OVR19_ovr_testbuf_sel_test_port_MASK 0x8000
#define RF_2069_OVR19_ovr_testbuf_sel_test_port_SHIFT 15
#define RF_2069_OVR7_ovr_lna5g_tr_rx_en_MASK     0x80
#define RF_2069_OVR7_ovr_lna5g_tr_rx_en_SHIFT    7
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_boost_MASK 0x7000
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_boost_SHIFT 12
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_tia_bq1_MASK 0x2000
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_tia_bq1_SHIFT 13
#define RF_2069_OVR20_ovr_mix5g_gc_load_MASK     0x1
#define RF_2069_OVR20_ovr_mix5g_gc_load_SHIFT    0
#define RF_2069_LOGEN5G_CFG1_lob1_pu_MASK        0x8
#define RF_2069_LOGEN5G_CFG1_lob1_pu_SHIFT       3
#define RF_2069_TESTBUF_CFG1_sel_test_port_MASK  0x70
#define RF_2069_TESTBUF_CFG1_sel_test_port_SHIFT 4
#define RF_2069_PAD5G_IDAC_idac_main_MASK        0x3f
#define RF_2069_PAD5G_IDAC_idac_main_SHIFT       0
#define RF_2069_OVR7_ovr_logen2g_reset_tx_MASK   0x1
#define RF_2069_OVR7_ovr_logen2g_reset_tx_SHIFT  0
#define RF_2069_TIA_CFG2_Rdc_out_MASK            0x3000
#define RF_2069_TIA_CFG2_Rdc_out_SHIFT           12
#define RF_2069_TXRX2G_CAL_TX_i_calPath_pad2g_pu_MASK 0x4
#define RF_2069_TXRX2G_CAL_TX_i_calPath_pad2g_pu_SHIFT 2
#define RF_2069_ADC_CALCODE4_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE4_Ich_flash_calcode_SHIFT 0
#define RF_2069_PA5G_IDAC1_pa5g_idac_cas_MASK    0x3f00
#define RF_2069_PA5G_IDAC1_pa5g_idac_cas_SHIFT   8
#define RF_2069_PAD2G_TUNE_pad2g_tune_MASK       0x7
#define RF_2069_PAD2G_TUNE_pad2g_tune_SHIFT      0
#define RF_2069_PGA5G_IDAC_idac_aux_MASK         0x3f00
#define RF_2069_PGA5G_IDAC_idac_aux_SHIFT        8
#define RF_2069_PA5G_CFG1_pa5g_bias_pu_MASK      0x2
#define RF_2069_PA5G_CFG1_pa5g_bias_pu_SHIFT     1
#define RF_2069_OVR12_ovr_pa5g_bias_cas_pu_MASK  0x2
#define RF_2069_OVR12_ovr_pa5g_bias_cas_pu_SHIFT 1
#define RF_2069_DAC_CFG1_DAC_dualDAC_en_MASK     0x2
#define RF_2069_DAC_CFG1_DAC_dualDAC_en_SHIFT    1
#define RF_2069_LNA2G_RSSI_dig_wrssi1_drive_strength_MASK 0x2
#define RF_2069_LNA2G_RSSI_dig_wrssi1_drive_strength_SHIFT 1
#define RF_2069_ADC_CFG4_Qch_run_flashcal_MASK   0x1
#define RF_2069_ADC_CFG4_Qch_run_flashcal_SHIFT  0
#define RF_2069_ADC_RC1_adc_ctl_RC_9_8_MASK      0x300
#define RF_2069_ADC_RC1_adc_ctl_RC_9_8_SHIFT     8
#define RF_2069_PA5G_CFG1_pa5g_bias_cas_pu_MASK  0x4
#define RF_2069_PA5G_CFG1_pa5g_bias_cas_pu_SHIFT 2
#define RF_2069_RXMIX5G_IDAC_gm_main_bias_ptat_i_MASK 0x700
#define RF_2069_RXMIX5G_IDAC_gm_main_bias_ptat_i_SHIFT 8
#define RF_2069_LNA2G_IDAC2_lna2_main_bias_ptat_MASK 0x700
#define RF_2069_LNA2G_IDAC2_lna2_main_bias_ptat_SHIFT 8
#define RF_2069_LPF_ENABLES_lpf_pu_bq2_i_MASK    0x8
#define RF_2069_LPF_ENABLES_lpf_pu_bq2_i_SHIFT   3
#define RF_2069_TXRX5G_CAL_RX_loopback5g_lna12_mux_MASK 0x1
#define RF_2069_TXRX5G_CAL_RX_loopback5g_lna12_mux_SHIFT 0
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_powersaving_MASK 0x400
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_powersaving_SHIFT 10
#define RF_2069_RXRF2G_CFG1_pwrsw_en_MASK        0x10
#define RF_2069_RXRF2G_CFG1_pwrsw_en_SHIFT       4
#define RF_2069_LNA2G_TUNE_lna2_freq_tune_MASK   0xf0
#define RF_2069_LNA2G_TUNE_lna2_freq_tune_SHIFT  4
#define RF_2069_OVR17_ovr_afediv_pu1_main_MASK   0x4
#define RF_2069_OVR17_ovr_afediv_pu1_main_SHIFT  2
#define RF_2069_LPF_ENABLES_lpf_pu_bq2_q_MASK    0x10
#define RF_2069_LPF_ENABLES_lpf_pu_bq2_q_SHIFT   4
#define RF_2069_TX5G_TSSI_pa5g_ctrl_tssi_MASK    0xf0
#define RF_2069_TX5G_TSSI_pa5g_ctrl_tssi_SHIFT   4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq1_bq2_MASK 0x4
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq1_bq2_SHIFT 2
#define RF_2069_PGA5G_CFG2_ptat_slope_boost_MASK 0x700
#define RF_2069_PGA5G_CFG2_ptat_slope_boost_SHIFT 8
#define RF_2069_LNA2G_CFG1_lna1_Rout_MASK        0xf00
#define RF_2069_LNA2G_CFG1_lna1_Rout_SHIFT       8
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_Refbuf_MASK 0xc00
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_Refbuf_SHIFT 10
#define RF_2069_OVR6_ovr_lna2g_lna2_Rout_MASK    0x4
#define RF_2069_OVR6_ovr_lna2g_lna2_Rout_SHIFT   2
#define RF_2069_OVR21_ovr_trsw5g_cntrl_MASK      0x800
#define RF_2069_OVR21_ovr_trsw5g_cntrl_SHIFT     11
#define RF_2069_PA2G_CFG1_pa2g_bias_pu_MASK      0x2
#define RF_2069_PA2G_CFG1_pa2g_bias_pu_SHIFT     1
#define RF_2069_OVR12_ovr_pa5g_bias_pu_MASK      0x1
#define RF_2069_OVR12_ovr_pa5g_bias_pu_SHIFT     0
#define RF_2069_IQCAL_CFG1_PU_iqcal_MASK         0x2
#define RF_2069_IQCAL_CFG1_PU_iqcal_SHIFT        1
#define RF_2069_OVR9_ovr_lpf_sw_bq1_bq2_MASK     0x10
#define RF_2069_OVR9_ovr_lpf_sw_bq1_bq2_SHIFT    4
#define RF_2069_LNA5G_CFG1_pwrsw_en_MASK         0x40
#define RF_2069_LNA5G_CFG1_pwrsw_en_SHIFT        6
#define RF_2069_LOGEN2G_CFG1_pdet_en_MASK        0x78
#define RF_2069_LOGEN2G_CFG1_pdet_en_SHIFT       3
#define RF_2069_PA2G_CFG3_pa2g_ptat_slope_cas_MASK 0xf0
#define RF_2069_PA2G_CFG3_pa2g_ptat_slope_cas_SHIFT 4
#define RF_2069_OVR23_ovr_wrssi3_Reflow_puI_MASK 0x10
#define RF_2069_OVR23_ovr_wrssi3_Reflow_puI_SHIFT 4
#define RF_2069_OVR7_ovr_lna5g_lna1_lowpwr_MASK  0x400
#define RF_2069_OVR7_ovr_lna5g_lna1_lowpwr_SHIFT 10
#define RF_2069_LOGEN5G_IDAC2_idac_load_MASK     0x70
#define RF_2069_LOGEN5G_IDAC2_idac_load_SHIFT    4
#define RF_2069_RXMIX2G_CFG1_LO_bias_sel_MASK    0x4
#define RF_2069_RXMIX2G_CFG1_LO_bias_sel_SHIFT   2
#define RF_2069_OVR23_ovr_wrssi3_Reflow_puQ_MASK 0x20
#define RF_2069_OVR23_ovr_wrssi3_Reflow_puQ_SHIFT 5
#define RF_2069_TXMIX5G_CFG1_tune_MASK           0xf00
#define RF_2069_TXMIX5G_CFG1_tune_SHIFT          8
#define RF_2069_PA5G_CFG2_pa5g_bias_filter_aux_MASK 0xf0
#define RF_2069_PA5G_CFG2_pa5g_bias_filter_aux_SHIFT 4
#define RF_2069_OVR23_ovr_nbrssi_Refhigh_puI_MASK 0x40
#define RF_2069_OVR23_ovr_nbrssi_Refhigh_puI_SHIFT 6
#define RF_2069_TIA_CFG1_sel_5G_2G_MASK          0x10
#define RF_2069_TIA_CFG1_sel_5G_2G_SHIFT         4
#define RF_2069_PAD5G_CFG1_vcas_monitor_sw_MASK  0x4
#define RF_2069_PAD5G_CFG1_vcas_monitor_sw_SHIFT 2
#define RF_2069_LNA5G_CFG1_FB_EN_MASK            0x8
#define RF_2069_LNA5G_CFG1_FB_EN_SHIFT           3
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_main_MASK 0xf0
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_main_SHIFT 4
#define RF_2069_ADC_CFG2_afe_pwrup_clk_MASK      0x80
#define RF_2069_ADC_CFG2_afe_pwrup_clk_SHIFT     7
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_Refladder_MASK 0xe000
#define RF_2069_WRSSI3_CONFG_wrssi3_ib_Refladder_SHIFT 13
#define RF_2069_LNA5G_CFG2_ana_wrssi1_pu_MASK    0x2
#define RF_2069_LNA5G_CFG2_ana_wrssi1_pu_SHIFT   1
#define RF_2069_OVR23_ovr_nbrssi_Refhigh_puQ_MASK 0x80
#define RF_2069_OVR23_ovr_nbrssi_Refhigh_puQ_SHIFT 7
#define RF_2069_OVR5_ovr_bias_rx_pu_MASK         0x200
#define RF_2069_OVR5_ovr_bias_rx_pu_SHIFT        9
#define RF_2069_AFEDIV3_afediv_sel_divext_MASK   0x1
#define RF_2069_AFEDIV3_afediv_sel_divext_SHIFT  0
#define RF_2069_OVR3_ovr_afe_iqadc_flash_calcode_Ich_MASK 0x20
#define RF_2069_OVR3_ovr_afe_iqadc_flash_calcode_Ich_SHIFT 5
#define RF_2069_OVR20_ovr_pa5g_tssi_pu_MASK      0x400
#define RF_2069_OVR20_ovr_pa5g_tssi_pu_SHIFT     10
#define RF_2069_OVR4_ovr_afe_iqadc_reset_Qch_MASK 0x400
#define RF_2069_OVR4_ovr_afe_iqadc_reset_Qch_SHIFT 10
#define RF_2069_OVR12_ovr_pa2g_bias_pu_MASK      0x20
#define RF_2069_OVR12_ovr_pa2g_bias_pu_SHIFT     5
#define RF_2069_PA2G_TSSI_pa2g_ctrl_tssi_MASK    0xf0
#define RF_2069_PA2G_TSSI_pa2g_ctrl_tssi_SHIFT   4
#define RF_2069_LNA2G_CFG1_bias_reset_MASK       0x10
#define RF_2069_LNA2G_CFG1_bias_reset_SHIFT      4
#define RF_2069_OVR4_ovr_afe_rst_clk_MASK        0x10
#define RF_2069_OVR4_ovr_afe_rst_clk_SHIFT       4
#define RF_2069_OVR18_ovr_tempsense_swap_amp_MASK 0x1
#define RF_2069_OVR18_ovr_tempsense_swap_amp_SHIFT 0
#define RF_2069_OVR20_ovr_tx5g_bias_pu_MASK      0x2000
#define RF_2069_OVR20_ovr_tx5g_bias_pu_SHIFT     13
#define RF_2069_LOGEN5G_CFG1_lob_pu_MASK         0x4
#define RF_2069_LOGEN5G_CFG1_lob_pu_SHIFT        2
#define RF_2069_OVR12_ovr_pa2g_bias_cas_pu_MASK  0x40
#define RF_2069_OVR12_ovr_pa2g_bias_cas_pu_SHIFT 6
#define RF_2069_PA5G_CFG1_pa5g_bias_reset_MASK   0x8
#define RF_2069_PA5G_CFG1_pa5g_bias_reset_SHIFT  3
#define RF_2069_AUXPGA_CFG1_auxpga_i_bias_ctrl_MASK 0x3000
#define RF_2069_AUXPGA_CFG1_auxpga_i_bias_ctrl_SHIFT 12
#define RF_2069_WRSSI3_TEST_wrssi3_sel_test_MASK 0xf00
#define RF_2069_WRSSI3_TEST_wrssi3_sel_test_SHIFT 8
#define RF_2069_WRSSI3_CONFG_wrssi3_Refmid_puI_MASK 0x4
#define RF_2069_WRSSI3_CONFG_wrssi3_Refmid_puI_SHIFT 2
#define RF_2069_TRSW2G_CFG3_trsw2g_gpaio_en_MASK 0x10
#define RF_2069_TRSW2G_CFG3_trsw2g_gpaio_en_SHIFT 4
#define RF_2069_PGA5G_CFG1_gc_MASK               0xff
#define RF_2069_PGA5G_CFG1_gc_SHIFT              0
#define RF_2069_TX2G_CFG2_notch2g_tune_MASK      0xf
#define RF_2069_TX2G_CFG2_notch2g_tune_SHIFT     0
#define RF_2069_WRSSI3_BIAS_wrssi3_misc_MASK     0xff
#define RF_2069_WRSSI3_BIAS_wrssi3_misc_SHIFT    0
#define RF_2069_TIA_CFG1_PU_bias_MASK            0x4
#define RF_2069_TIA_CFG1_PU_bias_SHIFT           2
#define RF_2069_ADC_CFG3_ctl_flash_MASK          0x3f
#define RF_2069_ADC_CFG3_ctl_flash_SHIFT         0
#define RF_2069_WRSSI3_CONFG_wrssi3_Refmid_puQ_MASK 0x8
#define RF_2069_WRSSI3_CONFG_wrssi3_Refmid_puQ_SHIFT 3
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq2_gain_MASK 0x7000
#define RF_2069_LPF_MAIN_CONTROLS_lpf_bq2_gain_SHIFT 12
#define RF_2069_OVR4_ovr_afe_iqadc_pwrup_Qch_MASK 0x4000
#define RF_2069_OVR4_ovr_afe_iqadc_pwrup_Qch_SHIFT 14
#define RF_2069_PAD5G_INCAP_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_PAD5G_INCAP_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_OVR18_ovr_rxrf5g_pu_pulse_MASK   0x8
#define RF_2069_OVR18_ovr_rxrf5g_pu_pulse_SHIFT  3
#define RF_2069_OVR6_ovr_lna2g_lna1_gain_MASK    0x200
#define RF_2069_OVR6_ovr_lna2g_lna1_gain_SHIFT   9
#define RF_2069_TX5G_CFG1_bias_pu_MASK           0x2
#define RF_2069_TX5G_CFG1_bias_pu_SHIFT          1
#define RF_2069_LOGEN2G_IDAC3_idac_irx_MASK      0x7
#define RF_2069_LOGEN2G_IDAC3_idac_irx_SHIFT     0
#define RF_2069_PA2G_CFG2_pa2g_bias_filter_main_MASK 0xf
#define RF_2069_PA2G_CFG2_pa2g_bias_filter_main_SHIFT 0
#define RF_2069_LOGEN5G_CFG2_reset_rx_MASK       0x8
#define RF_2069_LOGEN5G_CFG2_reset_rx_SHIFT      3
#define RF_2069_NBRSSI_CONFG_nbrssi_Refhigh_puI_MASK 0x1
#define RF_2069_NBRSSI_CONFG_nbrssi_Refhigh_puI_SHIFT 0
#define RF_2069_OVR19_ovr_pa2g_tssi_pu_MASK      0x2
#define RF_2069_OVR19_ovr_pa2g_tssi_pu_SHIFT     1
#define RF_2069_PA2G_TSSI_pa2g_ctrl_tssi_sel_MASK 0x4
#define RF_2069_PA2G_TSSI_pa2g_ctrl_tssi_sel_SHIFT 2
#define RF_2069_PA5G_CFG2_pa5g_bias_cas_MASK     0xff00
#define RF_2069_PA5G_CFG2_pa5g_bias_cas_SHIFT    8
#define RF_2069_PA5G_IDAC2_pa5g_biasa_main_MASK  0xff
#define RF_2069_PA5G_IDAC2_pa5g_biasa_main_SHIFT 0
#define RF_2069_TIA_CFG2_BW_MASK                 0xc000
#define RF_2069_TIA_CFG2_BW_SHIFT                14
#define RF_2069_TXGM_CFG1_sel5g_MASK             0x2
#define RF_2069_TXGM_CFG1_sel5g_SHIFT            1
#define RF_2069_ADC_CALCODE7_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE7_Ich_flash_calcode_SHIFT 0
#define RF_2069_PAD2G_CFG1_pad2g_gc_MASK         0xff00
#define RF_2069_PAD2G_CFG1_pad2g_gc_SHIFT        8
#define RF_2069_OVR19_ovr_tx2g_bias_pu_MASK      0x20
#define RF_2069_OVR19_ovr_tx2g_bias_pu_SHIFT     5
#define RF_2069_TRSW5G_CFG3_trsw5g_trsw_source_bis_MASK 0x1e00
#define RF_2069_TRSW5G_CFG3_trsw5g_trsw_source_bis_SHIFT 9
#define RF_2069_NBRSSI_CONFG_nbrssi_Refhigh_puQ_MASK 0x2
#define RF_2069_NBRSSI_CONFG_nbrssi_Refhigh_puQ_SHIFT 1
#define RF_2069_OVR19_ovr_tia_HPC_MASK           0x400
#define RF_2069_OVR19_ovr_tia_HPC_SHIFT          10
#define RF_2069_VBAT_CFG_monitor_pu_MASK         0x1
#define RF_2069_VBAT_CFG_monitor_pu_SHIFT        0
#define RF_2069_LOGEN2G_CFG1_ocl_ctrl_MASK       0x80
#define RF_2069_LOGEN2G_CFG1_ocl_ctrl_SHIFT      7
#define RF_2069_OVR19_ovr_tia_GainI_MASK         0x1000
#define RF_2069_OVR19_ovr_tia_GainI_SHIFT        12
#define RF_2069_PA5G_CFG1_pa5g_gpio_sw_pu_MASK   0x10
#define RF_2069_PA5G_CFG1_pa5g_gpio_sw_pu_SHIFT  4
#define RF_2069_LOGEN2G_IDAC3_idac_itx_MASK      0x700
#define RF_2069_LOGEN2G_IDAC3_idac_itx_SHIFT     8
#define RF_2069_DAC_CFG1_DAC_pd_partial_MASK     0x4000
#define RF_2069_DAC_CFG1_DAC_pd_partial_SHIFT    14
#define RF_2069_OVR8_ovr_logen5g_lob1_pu_MASK    0x40
#define RF_2069_OVR8_ovr_logen5g_lob1_pu_SHIFT   6
#define RF_2069_LPF_ENABLES_lpf_pu_dc_i_MASK     0x20
#define RF_2069_LPF_ENABLES_lpf_pu_dc_i_SHIFT    5
#define RF_2069_ADC_CALCODE28_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE28_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_LOGEN5G_CFG2_reset_tx_MASK       0x4
#define RF_2069_LOGEN5G_CFG2_reset_tx_SHIFT      2
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_gm_MASK 0x7000
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_gm_SHIFT 12
#define RF_2069_LNA2G_CFG1_lna1_out_short_pu_MASK 0x4
#define RF_2069_LNA2G_CFG1_lna1_out_short_pu_SHIFT 2
#define RF_2069_PGA2G_CFG1_gc_MASK               0xff
#define RF_2069_PGA2G_CFG1_gc_SHIFT              0
#define RF_2069_OVR19_ovr_tia_GainQ_MASK         0x800
#define RF_2069_OVR19_ovr_tia_GainQ_SHIFT        11
#define RF_2069_TXRX5G_CAL_RX_loopback5g_cal_pu_MASK 0x200
#define RF_2069_TXRX5G_CAL_RX_loopback5g_cal_pu_SHIFT 9
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_Ich_cal_state_MASK 0x2
#define RF_2069_ADC_STATUS_i_wrf_jtag_afe_iqadc_Ich_cal_state_SHIFT 1
#define RF_2069_PAD5G_INCAP_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_PAD5G_INCAP_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_TXGM_LOFT_SCALE_idac_lo_bb_diode_MASK 0xc
#define RF_2069_TXGM_LOFT_SCALE_idac_lo_bb_diode_SHIFT 2
#define RF_2069_LPF_ENABLES_lpf_pu_dc_q_MASK     0x40
#define RF_2069_LPF_ENABLES_lpf_pu_dc_q_SHIFT    6
#define RF_2069_TXMIX5G_CFG1_gc_load_MASK        0xf
#define RF_2069_TXMIX5G_CFG1_gc_load_SHIFT       0
#define RF_2069_OVR12_ovr_misc_pwrsw_en_MASK     0x100
#define RF_2069_OVR12_ovr_misc_pwrsw_en_SHIFT    8
#define RF_2069_LNA2G_TUNE_lna1_freq_tune_MASK   0xf
#define RF_2069_LNA2G_TUNE_lna1_freq_tune_SHIFT  0
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_gain_MASK 0x300
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_gain_SHIFT 8
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_lpf_MASK 0x2
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_lpf_SHIFT 1
#define RF_2069_ADC_CALCODE25_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE25_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_PAD2G_IDAC_pad2g_idac_cascode_MASK 0xf00
#define RF_2069_PAD2G_IDAC_pad2g_idac_cascode_SHIFT 8
#define RF_2069_PA5G_INCAP_pa5g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_PA5G_INCAP_pa5g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_TXRX2G_CAL_RX_loopback2g_cal_pu_MASK 0x200
#define RF_2069_TXRX2G_CAL_RX_loopback2g_cal_pu_SHIFT 9
#define RF_2069_OVR3_ovr_afe_DAC_pwrup_MASK      0x800
#define RF_2069_OVR3_ovr_afe_DAC_pwrup_SHIFT     11
#define RF_2069_PA2G_INCAP_pa2g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_PA2G_INCAP_pa2g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_TXRX2G_CAL_RX_loopback2g_lna12_mux_MASK 0x1
#define RF_2069_TXRX2G_CAL_RX_loopback2g_lna12_mux_SHIFT 0
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_ff_mult_MASK 0xc00
#define RF_2069_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_ff_mult_SHIFT 10
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_powersaving_MASK 0x8000
#define RF_2069_NBRSSI_CONFG_nbrssi_ib_powersaving_SHIFT 15
#define RF_2069_OVR20_ovr_pa5g_ctrl_tssi_sel_MASK 0x100
#define RF_2069_OVR20_ovr_pa5g_ctrl_tssi_sel_SHIFT 8
#define RF_2069_ADC_CALCODE22_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE22_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_OVR8_ovr_logen5g_pu_MASK         0x400
#define RF_2069_OVR8_ovr_logen5g_pu_SHIFT        10
#define RF_2069_PAD5G_TUNE_idac_aux_MASK         0x3f00
#define RF_2069_PAD5G_TUNE_idac_aux_SHIFT        8
#define RF_2069_IQCAL_IDAC_tssi_bias_MASK        0xf
#define RF_2069_IQCAL_IDAC_tssi_bias_SHIFT       0
#define RF_2069_TXGM_CFG1_gc_res_MASK            0x3000
#define RF_2069_TXGM_CFG1_gc_res_SHIFT           12
#define RF_2069_PAD5G_INCAP_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_PAD5G_INCAP_idac_incap_compen_aux_SHIFT 8
#define RF_2069_OVR20_ovr_txmix2g_pu_MASK        0x4
#define RF_2069_OVR20_ovr_txmix2g_pu_SHIFT       2
#define RF_2069_OVR10_ovr_lpf_bq1_gain_MASK      0x2
#define RF_2069_OVR10_ovr_lpf_bq1_gain_SHIFT     1
#define RF_2069_LOGEN5G_CFG1_logen_pu_MASK       0x1
#define RF_2069_LOGEN5G_CFG1_logen_pu_SHIFT      0
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_idac_MASK 0xf0
#define RF_2069_LNA2G_IDAC2_lna2_aux_bias_idac_SHIFT 4
#define RF_2069_PGA2G_INCAP_pad2g_idac_aux_MASK  0x3f00
#define RF_2069_PGA2G_INCAP_pad2g_idac_aux_SHIFT 8
#define RF_2069_RXMIX5G_CFG1_LO_bias_sel_MASK    0x8
#define RF_2069_RXMIX5G_CFG1_LO_bias_sel_SHIFT   3
#define RF_2069_NBRSSI_TEST_nbrssi_en_test_MASK  0x1
#define RF_2069_NBRSSI_TEST_nbrssi_en_test_SHIFT 0
#define RF_2069_IQCAL_CFG1_sel_sw_MASK           0xf0
#define RF_2069_IQCAL_CFG1_sel_sw_SHIFT          4
#define RF_2069_OVR8_ovr_logen5g_reset_MASK      0x200
#define RF_2069_OVR8_ovr_logen5g_reset_SHIFT     9
#define RF_2069_RXRF5G_CFG2_lna5g_epapd_en_MASK  0x10
#define RF_2069_RXRF5G_CFG2_lna5g_epapd_en_SHIFT 4
#define RF_2069_OVR18_ovr_rxmix5g_bias_reset_MASK 0x400
#define RF_2069_OVR18_ovr_rxmix5g_bias_reset_SHIFT 10
#define RF_2069_TXGM_CFG1_gc_iout_MASK           0x70
#define RF_2069_TXGM_CFG1_gc_iout_SHIFT          4
#define RF_2069_AFEDIV1_afediv_pu2_cal_MASK      0x800
#define RF_2069_AFEDIV1_afediv_pu2_cal_SHIFT     11
#define RF_2069_PGA2G_CFG2_pga2g_tune_MASK       0x7
#define RF_2069_PGA2G_CFG2_pga2g_tune_SHIFT      0
#define RF_2069_OVR21_ovr_rxrf2g_pwrsw_lna2g_en_MASK 0x10
#define RF_2069_OVR21_ovr_rxrf2g_pwrsw_lna2g_en_SHIFT 4
#define RF_2069_ADC_CFG4_ctrl_flash17lvl_MASK    0x4000
#define RF_2069_ADC_CFG4_ctrl_flash17lvl_SHIFT   14
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_hold_MASK 0x10
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_dc_hold_SHIFT 4
#define RF_2069_ADC_CALCODE12_Qch_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE12_Qch_flash_calcode_SHIFT 0
#define RF_2069_OVR19_ovr_tia_DC_loop_PU_MASK    0x2000
#define RF_2069_OVR19_ovr_tia_DC_loop_PU_SHIFT   13
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_rx_MASK 0xf0
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_rx_SHIFT 4
#define RF_2069_OVR12_ovr_lpf_dc_hold_MASK       0x8000
#define RF_2069_OVR12_ovr_lpf_dc_hold_SHIFT      15
#define RF_2069_LNA2G_RSSI_dig_wrssi2_drive_strength_MASK 0x200
#define RF_2069_LNA2G_RSSI_dig_wrssi2_drive_strength_SHIFT 9
#define RF_2069_LOGEN5G_CFG2_tx_pu_MASK          0x2
#define RF_2069_LOGEN5G_CFG2_tx_pu_SHIFT         1
#define RF_2069_OVR13_ovr_pad2g_bias_filter_bypass_MASK 0x200
#define RF_2069_OVR13_ovr_pad2g_bias_filter_bypass_SHIFT 9
#define RF_2069_RXMIX5G_IDAC_gm_aux_bias_idac_i_MASK 0xf0
#define RF_2069_RXMIX5G_IDAC_gm_aux_bias_idac_i_SHIFT 4
#define RF_2069_OVR21_ovr_mix5g_pu_MASK          0x8000
#define RF_2069_OVR21_ovr_mix5g_pu_SHIFT         15
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_qb_MASK 0x1c0
#define RF_2069_LOGEN2G_IDAC2_logen2g_idac_qb_SHIFT 6
#define RF_2069_OVR4_global_alt_dc_en_MASK       0x200
#define RF_2069_OVR4_global_alt_dc_en_SHIFT      9
#define RF_2069_PA2G_CFG2_pa2g_bias_filter_aux_MASK 0xf0
#define RF_2069_PA2G_CFG2_pa2g_bias_filter_aux_SHIFT 4
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_tx_MASK 0xf
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_tx_SHIFT 0
#define RF_2069_OVR8_ovr_logen2g_div1p5_pu_MASK  0x1000
#define RF_2069_OVR8_ovr_logen2g_div1p5_pu_SHIFT 12
#define RF_2069_TXGM_CFG2_filter_MASK            0xf
#define RF_2069_TXGM_CFG2_filter_SHIFT           0
#define RF_2069_OVR18_ovr_rxrf5g_pwrsw_en_MASK   0x4
#define RF_2069_OVR19_ovr_rxrf5g_pwrsw_en_SHIFT  2
#define RF_2069_OVR19_ovr_rxrf5g_pwrsw_en_MASK   0x4
#define RF_2069_OVR18_ovr_rxrf5g_pwrsw_en_SHIFT  2
#define RF_2069_PAD2G_CFG1_pad2g_pu_MASK         0x1
#define RF_2069_PAD2G_CFG1_pad2g_pu_SHIFT        0
#define RF_2069_DAC_CFG2_DAC_misc_MASK           0xff00
#define RF_2069_DAC_CFG2_DAC_misc_SHIFT          8
#define RF_2069_AUXPGA_VMID_auxpga_i_sel_vmid_MASK 0x3ff
#define RF_2069_AUXPGA_VMID_auxpga_i_sel_vmid_SHIFT 0
#define RF_2069_PAD5G_SLOPE_ptat_slope_cascode_MASK 0x700
#define RF_2069_PAD5G_SLOPE_ptat_slope_cascode_SHIFT 8
#define RF_2069_OVR19_ovr_tx2g_calpath_pad_pu_MASK 0x8
#define RF_2069_OVR19_ovr_tx2g_calpath_pad_pu_SHIFT 3
#define RF_2069_DAC_CFG3_DAC_pd_mode_MASK        0x3
#define RF_2069_DAC_CFG3_DAC_pd_mode_SHIFT       0
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_sw_spare_MASK 0x780
#define RF_2069_LPF_DC_LOOP_AND_MISC_lpf_sw_spare_SHIFT 7
#define RF_2069_TXRX2G_CAL_TX_i_cal_pa_atten_2g_MASK 0x18
#define RF_2069_TXRX2G_CAL_TX_i_cal_pa_atten_2g_SHIFT 3
#define RF_2069_RXMIX2G_IDAC_aux_bias_idac_MASK  0xf0
#define RF_2069_RXMIX2G_IDAC_aux_bias_idac_SHIFT 4
#define RF_2069_OVR3_ovr_auxpga_i_sel_gain_MASK  0x4000
#define RF_2069_OVR3_ovr_auxpga_i_sel_gain_SHIFT 14
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_low_MASK 0x80
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_low_SHIFT 7
#define RF_2069_OVR23_ovr_afe_clk_ctl_MASK       0x1000
#define RF_2069_OVR23_ovr_afe_clk_ctl_SHIFT      12
#define RF_2069_LNA5G_CFG1_gctl1_ln_MASK         0x700
#define RF_2069_LNA5G_CFG1_gctl1_ln_SHIFT        8
#define RF_2069_TIA_CFG3_Spare_MASK              0xff00
#define RF_2069_TIA_CFG3_Spare_SHIFT             8
#define RF_2069_OVR10_ovr_lpf_pu_MASK            0x800
#define RF_2069_OVR10_ovr_lpf_pu_SHIFT           11
#define RF_2069_PAD5G_CFG1_bias_filter_bypass_MASK 0x2
#define RF_2069_PAD5G_CFG1_bias_filter_bypass_SHIFT 1
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_pu_MASK 0x1
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_pu_SHIFT 0
#define RF_2069_OVR4_ovr_afe_iqadc_rx_div4_en_MASK 0x20
#define RF_2069_OVR4_ovr_afe_iqadc_rx_div4_en_SHIFT 5
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_mid_MASK 0x300
#define RF_2069_NBRSSI_CONFG_nbrssi_Refctrl_mid_SHIFT 8
#define RF_2069_LNA5G_IDAC1_lna1_bias_idac_MASK  0xf
#define RF_2069_LNA5G_IDAC1_lna1_bias_idac_SHIFT 0
#define RF_2069_OVR9_ovr_logen5g_reset_rx_MASK   0x400
#define RF_2069_OVR9_ovr_logen5g_reset_rx_SHIFT  10
#define RF_2069_DEV_ID_dev_id_MASK               0xffff
#define RF_2069_DEV_ID_dev_id_SHIFT              0
#define RF_2069_OVR23_ovr_nbrssi_Reflow_puI_MASK 0x400
#define RF_2069_OVR23_ovr_nbrssi_Reflow_puI_SHIFT 10
#define RF_2069_PAD2G_CFG1_pad2g_bias_filter_MASK 0xf0
#define RF_2069_PAD2G_CFG1_pad2g_bias_filter_SHIFT 4
#define RF_2069_ADC_CFG1_Qch_pwrup_MASK          0x3f00
#define RF_2069_ADC_CFG1_Qch_pwrup_SHIFT         8
#define RF_2069_LNA5G_CFG1_lna1_lowpwr_MASK      0x2
#define RF_2069_LNA5G_CFG1_lna1_lowpwr_SHIFT     1
#define RF_2069_OVR23_ovr_nbrssi_Reflow_puQ_MASK 0x800
#define RF_2069_OVR23_ovr_nbrssi_Reflow_puQ_SHIFT 11
#define RF_2069_OVR6_ovr_lna2g_lna1_pu_MASK      0x80
#define RF_2069_OVR6_ovr_lna2g_lna1_pu_SHIFT     7
#define RF_2069_TESTBUF_CFG1_GPIO_EN_MASK        0x2
#define RF_2069_TESTBUF_CFG1_GPIO_EN_SHIFT       1
#define RF_2069_OVR9_ovr_logen5g_reset_tx_MASK   0x200
#define RF_2069_OVR9_ovr_logen5g_reset_tx_SHIFT  9
#define RF_2069_OVR18_ovr_rxrf2g_globe_pu_MASK   0x40
#define RF_2069_OVR18_ovr_rxrf2g_globe_pu_SHIFT  6
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_aux_MASK 0xf0
#define RF_2069_PAD2G_SLOPE_pad2g_ptat_slope_aux_SHIFT 4
#define RF_2069_TX5G_CFG1_mix5g_pu_MASK          0x1
#define RF_2069_TX5G_CFG1_mix5g_pu_SHIFT         0
#define RF_2069_LNA5G_CFG1_tr_rx_en_MASK         0x4
#define RF_2069_LNA5G_CFG1_tr_rx_en_SHIFT        2
#define RF_2069_OVR21_ovr_afe_DACbuf_fixed_cap_MASK 0x2000
#define RF_2069_OVR21_ovr_afe_DACbuf_fixed_cap_SHIFT 13
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_cr_pu_MASK 0x20
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_cr_pu_SHIFT 5
#define RF_2069_LNA2G_RSSI_dig_wrssi2_pu_MASK    0x100
#define RF_2069_LNA2G_RSSI_dig_wrssi2_pu_SHIFT   8
#define RF_2069_TRSW5G_CFG2_trsw5g_trsw_gate_bis_MASK 0xf
#define RF_2069_TRSW5G_CFG2_trsw5g_trsw_gate_bis_SHIFT 0
#define RF_2069_TIA_CFG1_GainI_MASK              0xf000
#define RF_2069_TIA_CFG1_GainI_SHIFT             12
#define RF_2069_LNA5G_CFG2_gctl2_lp_MASK         0x30
#define RF_2069_LNA5G_CFG2_gctl2_lp_SHIFT        4
#define RF_2069_TXRX5G_CAL_TX_i_calPath_pad_pu_5g_MASK 0x4
#define RF_2069_TXRX5G_CAL_TX_i_calPath_pad_pu_5g_SHIFT 2
#define RF_2069_AFEDIV1_afediv_sel_div_MASK      0x70
#define RF_2069_AFEDIV1_afediv_sel_div_SHIFT     4
#define RF_2069_AFEDIV1_afediv_pu0_inbuf_MASK    0x400
#define RF_2069_AFEDIV1_afediv_pu0_inbuf_SHIFT   10
#define RF_2069_TIA_CFG1_GainQ_MASK              0xf00
#define RF_2069_TIA_CFG1_GainQ_SHIFT             8
#define RF_2069_IQCAL_CFG1_tssi_GPIO_ctrl_MASK   0x300
#define RF_2069_IQCAL_CFG1_tssi_GPIO_ctrl_SHIFT  8
#define RF_2069_PAD5G_TUNE_tune_MASK             0xf
#define RF_2069_PAD5G_TUNE_tune_SHIFT            0
#define RF_2069_PGA5G_CFG1_gainboost_MASK        0xf000
#define RF_2069_PGA5G_CFG1_gainboost_SHIFT       12
#define RF_2069_AUXPGA_CFG1_auxpga_i_sel_gain_MASK 0x700
#define RF_2069_AUXPGA_CFG1_auxpga_i_sel_gain_SHIFT 8
#define RF_2069_LPF_MAIN_CONTROLS_lpf_q_biq2_MASK 0x8000
#define RF_2069_LPF_MAIN_CONTROLS_lpf_q_biq2_SHIFT 15
#define RF_2069_OVR21_ovr_rxrf5g_pwrsw_lna5g_en_MASK 0x20
#define RF_2069_OVR21_ovr_rxrf5g_pwrsw_lna5g_en_SHIFT 5
#define RF_2069_TIA_CFG1_DC_loop_PU_MASK         0x2
#define RF_2069_TIA_CFG1_DC_loop_PU_SHIFT        1
#define RF_2069_RXRF5G_CFG1_globe_pu_MASK        0x1
#define RF_2069_RXRF5G_CFG1_globe_pu_SHIFT       0
#define RF_2069_LPF_MAIN_CONTROLS_lpf_rc_bw_MASK 0x1c0
#define RF_2069_LPF_MAIN_CONTROLS_lpf_rc_bw_SHIFT 6
#define RF_2069_OVR18_ovr_tempsense_sel_Vbe_Vbg_MASK 0x2
#define RF_2069_OVR18_ovr_tempsense_sel_Vbe_Vbg_SHIFT 1
#define RF_2069_LOGEN5G_IDAC3_idac_buf1_MASK     ((ACREV0_SUB) ? 0xf0 : 0x38)
#define RF_2069_LOGEN5G_IDAC3_idac_buf1_SHIFT    ((ACREV0_SUB) ? 4 : 3)
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_adc_MASK 0x8
#define RF_2069_LPF_MUX_SWITCHES_lpf_sw_bq2_adc_SHIFT 3
#define RF_2069_LOGEN5G_IDAC3_idac_buf2_MASK     ((ACREV0_SUB) ? 0xf000 : 0xe00)
#define RF_2069_LOGEN5G_IDAC3_idac_buf2_SHIFT    ((ACREV0_SUB) ? 12 : 9)
#define RF_2069_OVR19_ovr_tx2g_calpath_pa_pu_MASK 0x10
#define RF_2069_OVR19_ovr_tx2g_calpath_pa_pu_SHIFT 4
#define RF_2069_PAD2G_INCAP_pad2g_idac_incap_compen_main_MASK 0xf
#define RF_2069_PAD2G_INCAP_pad2g_idac_incap_compen_main_SHIFT 0
#define RF_2069_OVR13_ovr_pad5g_bias_filter_bypass_MASK 0x40
#define RF_2069_OVR13_ovr_pad5g_bias_filter_bypass_SHIFT 6
#define RF_2069_OVR10_ovr_lpf_pu_bq1_i_MASK      0x400
#define RF_2069_OVR10_ovr_lpf_pu_bq1_i_SHIFT     10
#define RF_2069_PGA2G_IDAC_pga2g_idac_aux_MASK   0x3f00
#define RF_2069_PGA2G_IDAC_pga2g_idac_aux_SHIFT  8
#define RF_2069_IQCAL_CFG1_sel_ext_tssi_MASK     0x4
#define RF_2069_IQCAL_CFG1_sel_ext_tssi_SHIFT    2
#define RF_2069_ADC_CALCODE3_Ich_flash_calcode_MASK 0xffff
#define RF_2069_ADC_CALCODE3_Ich_flash_calcode_SHIFT 0
#define RF_2069_ADC_CALCODE20_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE20_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_ADC_CALCODE19_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE19_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_AFEDIV1_afediv_main_driver_size_MASK 0xf
#define RF_2069_AFEDIV1_afediv_main_driver_size_SHIFT 0
#define RF_2069_LNA5G_TUNE_tx_tune_MASK          0xf00
#define RF_2069_LNA5G_TUNE_tx_tune_SHIFT         8
#define RF_2069_OVR13_ovr_pa5g_5gtx_pu_MASK      0x400
#define RF_2069_OVR13_ovr_pa5g_5gtx_pu_SHIFT     10
#define RF_2069_TIA_CFG2_opamp_Cc_MASK           0xc00
#define RF_2069_TIA_CFG2_opamp_Cc_SHIFT          10
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_cal_MASK 0x8
#define RF_2069_LNA2G_RSSI_ANA_ana_wrssi1_cal_SHIFT 3
#define RF_2069_OVR5_ovr_testbuf_PU_MASK         0x1000
#define RF_2069_OVR5_ovr_testbuf_PU_SHIFT        12
#define RF_2069_OVR9_ovr_lpf_sw_bq2_adc_MASK     0x4
#define RF_2069_OVR9_ovr_lpf_sw_bq2_adc_SHIFT    2
#define RF_2069_OVR10_ovr_lpf_pu_bq1_q_MASK      0x200
#define RF_2069_OVR10_ovr_lpf_pu_bq1_q_SHIFT     9
#define RF_2069_TXRX5G_CAL_TX_i_calPath_pa_pu_5g_MASK 0x20
#define RF_2069_TXRX5G_CAL_TX_i_calPath_pa_pu_5g_SHIFT 5
#define RF_2069_LOGEN5G_IDAC4_idac_irx_MASK      0x7
#define RF_2069_LOGEN5G_IDAC4_idac_irx_SHIFT     0
#define RF_2069_OVR23_ovr_wrssi3_Refmid_puI_MASK 0x4
#define RF_2069_OVR23_ovr_wrssi3_Refmid_puI_SHIFT 2
#define RF_2069_OVR1_global_alt_dc_en_MASK       0x8000
#define RF_2069_OVR1_global_alt_dc_en_SHIFT      15
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_MASK    0xf000
#define RF_2069_TRSW5G_CFG1_trsw5g_cntrl_SHIFT   12
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rx_attn_MASK 0x6
#define RF_2069_TXRX5G_CAL_RX_loopback5g_rxiqcal_rx_attn_SHIFT 1
#define RF_2069_RXMIX2G_IDAC_main_bias_idac_MASK 0xf
#define RF_2069_RXMIX2G_IDAC_main_bias_idac_SHIFT 0
#define RF_2069_AUXPGA_CFG1_auxpga_i_vcm_ctrl_MASK 0x30
#define RF_2069_AUXPGA_CFG1_auxpga_i_vcm_ctrl_SHIFT 4
#define RF_2069_ADC_CALCODE16_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_ADC_CALCODE16_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_lpf_MASK 0x2
#define RF_2069_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_lpf_SHIFT 1
#define RF_2069_OVR23_ovr_wrssi3_Refmid_puQ_MASK 0x8
#define RF_2069_OVR23_ovr_wrssi3_Refmid_puQ_SHIFT 3
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_aux_MASK 0xf00
#define RF_2069_PGA2G_CFG2_pga2g_ptat_slope_aux_SHIFT 8
#define RF_2069_LNA2G_TUNE_lna1_avdd_cap_MASK    0xc000
#define RF_2069_LNA2G_TUNE_lna1_avdd_cap_SHIFT   14
#define RF_2069_OVR7_ovr_lna5g_gctl1_MASK        0x4000
#define RF_2069_OVR7_ovr_lna5g_gctl1_SHIFT       14
#define RF_2069_OVR23_ovr_wrssi3_Refhigh_puI_MASK 0x1
#define RF_2069_OVR23_ovr_wrssi3_Refhigh_puI_SHIFT 0
#define RF_2069_OVR7_ovr_lna5g_gctl2_MASK        0x1000
#define RF_2069_OVR7_ovr_lna5g_gctl2_SHIFT       12
#define RF_2069_ADC_RC2_adc_ctrl_RC_19_18_MASK   0xc
#define RF_2069_ADC_RC2_adc_ctrl_RC_19_18_SHIFT  2
#define RF_2069_LNA2G_CFG2_lna2_gain_MASK        0x700
#define RF_2069_LNA2G_CFG2_lna2_gain_SHIFT       8
#define RF_2069_PGA5G_IDAC_idac_main_MASK        0x3f
#define RF_2069_PGA5G_IDAC_idac_main_SHIFT       0
#define RF_2069_LOGEN5G_IDAC4_idac_itx_MASK      0x700
#define RF_2069_LOGEN5G_IDAC4_idac_itx_SHIFT     8
#define RF_2069_OVR8_ovr_logen5g_vcobuf_pu_MASK  0x100
#define RF_2069_OVR8_ovr_logen5g_vcobuf_pu_SHIFT 8
#define RF_2069_OVR23_ovr_wrssi3_Refhigh_puQ_MASK 0x2
#define RF_2069_OVR23_ovr_wrssi3_Refhigh_puQ_SHIFT 1
#define RF_2069_OVR18_ovr_rxmix2g_pu_MASK        0x800
#define RF_2069_OVR18_ovr_rxmix2g_pu_SHIFT       11
#define RF_2069_OVR13_ovr_pa5g_bias_reset_MASK   0x1000
#define RF_2069_OVR13_ovr_pa5g_bias_reset_SHIFT  12
#define RF_2069_OVR20_ovr_txgm_gc_iout_MASK      0x80
#define RF_2069_OVR20_ovr_txgm_gc_iout_SHIFT     7
#define RF_2069_PA2G_CFG2_pa2g_bias_cas_MASK     0xff00
#define RF_2069_PA2G_CFG2_pa2g_bias_cas_SHIFT    8
#define RF_2069_CGPAIO_CFG3_sel_16to31_port_MASK 0xffff
#define RF_2069_CGPAIO_CFG3_sel_16to31_port_SHIFT 0
#define RF_2069_VREG_CFG_vout_sel_MASK           0x300
#define RF_2069_VREG_CFG_vout_sel_SHIFT          8
#define RF_2069_RCCAL_LOGIC1_rccal_START_MASK    0x1
#define RF_2069_RCCAL_LOGIC1_rccal_START_SHIFT   0
#define RF_2069_RCCAL_CFG_fixed_cap_MASK         0x2000
#define RF_2069_RCCAL_CFG_fixed_cap_SHIFT        13
#define RF_2069_RCCAL_CFG_gpio_en_MASK           0x4
#define RF_2069_RCCAL_CFG_gpio_en_SHIFT          2
#define RF_2069_BG_CFG1_ate_rcal_trim_MASK       0xf000
#define RF_2069_BG_CFG1_ate_rcal_trim_SHIFT      12
#define RF_2069_RCCAL_CFG_rccal_mode_MASK        0x2
#define RF_2069_RCCAL_CFG_rccal_mode_SHIFT       1
#define RF_2069_RCAL_CFG_pu_MASK                 0x1
#define RF_2069_RCAL_CFG_pu_SHIFT                0
#define RF_2069_CGPAIO_CFG1_cgpaio_pu_MASK       0x1
#define RF_2069_CGPAIO_CFG1_cgpaio_pu_SHIFT      0
#define RF_2069_BG_CFG2_m_count_trim_MASK        0x3f
#define RF_2069_BG_CFG2_m_count_trim_SHIFT       0
#define RF_2069_CGPAIO_CFG5_cgpaio_tssi_muxselgpaio_MASK 0xff
#define RF_2069_CGPAIO_CFG5_cgpaio_tssi_muxselgpaio_SHIFT 0
#define RF_2069_LPO_CFG_pu_MASK                  0x1
#define RF_2069_LPO_CFG_pu_SHIFT                 0
#define RF_2069_BG_CFG2_n_count_trim_MASK        0x3f00
#define RF_2069_BG_CFG2_n_count_trim_SHIFT       8
#define RF_2069_RCCAL_LOGIC4_rccal_N1_MASK       0x1fff
#define RF_2069_RCCAL_LOGIC4_rccal_N1_SHIFT      0
#define RF_2069_RCCAL_LOGIC1_rccal_P1_MASK       0xc0
#define RF_2069_RCCAL_LOGIC1_rccal_P1_SHIFT      6
#define RF_2069_BG_CFG1_bg_pulse_MASK            0x2
#define RF_2069_BG_CFG1_bg_pulse_SHIFT           1
#define RF_2069_RCCAL_LOGIC1_rccal_Q1_MASK       0x30
#define RF_2069_RCCAL_LOGIC1_rccal_Q1_SHIFT      4
#define RF_2069_BG_TRIM2_afe_vbgtrim_MASK        0x3f
#define RF_2069_BG_TRIM2_afe_vbgtrim_SHIFT       0
#define RF_2069_VREG_CFG_pup_MASK                0x1
#define RF_2069_VREG_CFG_pup_SHIFT               0
#define RF_2069_RCCAL_LOGIC1_rccal_R1_MASK       0xc
#define RF_2069_RCCAL_LOGIC1_rccal_R1_SHIFT      2
#define RF_2069_BG_CFG1_rcal_trim_MASK           0xf0
#define RF_2069_BG_CFG1_rcal_trim_SHIFT          4
#define RF_2069_RCCAL_LOGIC1_rccal_X1_MASK       0xff00
#define RF_2069_RCCAL_LOGIC1_rccal_X1_SHIFT      8
#define RF_2069_VREG_CFG_bg_filter_en_MASK       0x10
#define RF_2069_VREG_CFG_bg_filter_en_SHIFT      4
#define RF_2069_OVR2_ovr_otp_rcal_sel_MASK       0x2
#define RF_2069_OVR2_ovr_otp_rcal_sel_SHIFT      1
#define RF_2069_LPO_CFG_gpio_en_MASK             0x2
#define RF_2069_LPO_CFG_gpio_en_SHIFT            1
#define RF_2069_OVR2_ovr_bg_rcal_trim_MASK       0x4
#define RF_2069_OVR2_ovr_bg_rcal_trim_SHIFT      2
#define RF_2069_OVR2_ovr_vreg_bg_filter_en_MASK  0x10
#define RF_2069_OVR2_ovr_vreg_bg_filter_en_SHIFT 4
#define RF_2069_RCCAL_CFG_bi_MASK                0xf80
#define RF_2069_RCCAL_CFG_bi_SHIFT               7
#define RF_2069_RCCAL_LOGIC5_rccal_raw_dacbuf_MASK 0x3e0
#define RF_2069_RCCAL_LOGIC5_rccal_raw_dacbuf_SHIFT 5
#define RF_2069_RCCAL_LOGIC2_rccal_DONE_MASK     0x10
#define RF_2069_RCCAL_LOGIC2_rccal_DONE_SHIFT    4
#define RF_2069_RCAL_CFG_i_wrf_jtag_rcal_valid_MASK 0x8
#define RF_2069_RCAL_CFG_i_wrf_jtag_rcal_valid_SHIFT 3
#define RF_2069_RCAL_CFG_div_cntrl_MASK          0x6
#define RF_2069_RCAL_CFG_div_cntrl_SHIFT         1
#define RF_2069_CGPAIO_CFG4_cgpaio_tssi_muxsel0_MASK 0xff00
#define RF_2069_CGPAIO_CFG4_cgpaio_tssi_muxsel0_SHIFT 8
#define RF_2069_CGPAIO_CFG4_cgpaio_tssi_muxsel1_MASK 0xff
#define RF_2069_CGPAIO_CFG4_cgpaio_tssi_muxsel1_SHIFT 0
#define RF_2069_RCCAL_LOGIC2_rccal_adc_code_MASK 0xf
#define RF_2069_RCCAL_LOGIC2_rccal_adc_code_SHIFT 0
#define RF_2069_CGPAIO_CFG5_cgpaio_tssi_muxsel2_MASK 0xff00
#define RF_2069_CGPAIO_CFG5_cgpaio_tssi_muxsel2_SHIFT 8
#define RF_2069_RCCAL_CFG_pu_MASK                0x1
#define RF_2069_RCCAL_CFG_pu_SHIFT               0
#define RF_2069_OVR2_ovr_bg_pu_MASK              0x8
#define RF_2069_OVR2_ovr_bg_pu_SHIFT             3
#define RF_2069_OVR2_ovr_bg_pulse_MASK           0x20
#define RF_2069_OVR2_ovr_bg_pulse_SHIFT          5
#define RF_2069_RCCAL_CFG_sc_MASK                0x18
#define RF_2069_RCCAL_CFG_sc_SHIFT               3
#define RF_2069_RCCAL_CFG_sr_MASK                0x1000
#define RF_2069_RCCAL_CFG_sr_SHIFT               12
#define RF_2069_RCAL_CFG_i_wrf_jtag_rcal_value_MASK 0x1f0
#define RF_2069_RCAL_CFG_i_wrf_jtag_rcal_value_SHIFT 4
#define RF_2069_BG_CFG1_bias_lpf_r_MASK          0xf00
#define RF_2069_BG_CFG1_bias_lpf_r_SHIFT         8
#define RF_2069_BG_CFG1_pu_MASK                  0x1
#define RF_2069_BG_CFG1_pu_SHIFT                 0
#define RF_2069_BG_TRIM1_wlan_vptrim_MASK        0x3f
#define RF_2069_BG_TRIM1_wlan_vptrim_SHIFT       0
#define RF_2069_RCCAL_LOGIC5_rccal_raw_adc1p2_MASK 0x1f
#define RF_2069_RCCAL_LOGIC5_rccal_raw_adc1p2_SHIFT 0
#define RF_2069_OVR2_ovr_vreg_pup_MASK           0x1
#define RF_2069_OVR2_ovr_vreg_pup_SHIFT          0
#define RF_2069_BG_CFG1_bg_ate_rcal_trim_en_MASK 0x4
#define RF_2069_BG_CFG1_bg_ate_rcal_trim_en_SHIFT 2
#define RF_2069_CGPAIO_CFG2_sel_0to15_port_MASK  0xffff
#define RF_2069_CGPAIO_CFG2_sel_0to15_port_SHIFT 0
#define RF_2069_BG_TRIM1_wlan_vbgtrim_MASK       0x3f00
#define RF_2069_BG_TRIM1_wlan_vbgtrim_SHIFT      8
#define RF_2069_RCCAL_LOGIC3_rccal_N0_MASK       0x1fff
#define RF_2069_RCCAL_LOGIC3_rccal_N0_SHIFT      0
#define RF_2069_BG_TRIM2_pmu_vbgtrim_MASK        0x3f00
#define RF_2069_BG_TRIM2_pmu_vbgtrim_SHIFT       8
#define RF_2069_RCCAL_TRC_rccal_Trc_MASK         0x1fff
#define RF_2069_RCCAL_TRC_rccal_Trc_SHIFT        0
#define RF_2069_PLL_VCOCAL5_rfpll_vcocal_delayAfterRefresh_MASK 0xff00
#define RF_2069_PLL_VCOCAL5_rfpll_vcocal_delayAfterRefresh_SHIFT 8
#define RF_2069_TOP_SPARE2_spare2_MASK           0xffff
#define RF_2069_TOP_SPARE2_spare2_SHIFT          0
#define RF_2069_PLL_FRCT1_rfpll_frct_stop_mod_MASK 0x1
#define RF_2069_PLL_FRCT1_rfpll_frct_stop_mod_SHIFT 0
#define RF_2069_OVR14_ovr_rfpll_bias_reset_cp_MASK 0x4
#define RF_2069_OVR14_ovr_rfpll_bias_reset_cp_SHIFT 2
#define RF_2069_PLL_VCO3_rfpll_vco_cvar_extra_MASK 0x1e00
#define RF_2069_PLL_VCO3_rfpll_vco_cvar_extra_SHIFT 9
#define RF_2069_PLL_XTAL9_xtal_xtal_rboff_MASK   0x1
#define RF_2069_PLL_XTAL9_xtal_xtal_rboff_SHIFT  0
#define RF_2069_PLL_VCO2_rfpll_vco_USE_2p5V_MASK 0x80
#define RF_2069_PLL_VCO2_rfpll_vco_USE_2p5V_SHIFT 7
#define RF_2069_PLL_VCO6_rfpll_vco_ib_bias_opamp_fastsettle_MASK 0xf
#define RF_2069_PLL_VCO6_rfpll_vco_ib_bias_opamp_fastsettle_SHIFT 0
#define RF_2069_PLL_CP3_rfpll_cp_idump_offset_up_MASK 0xf
#define RF_2069_PLL_CP3_rfpll_cp_idump_offset_up_SHIFT 0
#define RF_2069_PLL_VCOCAL4_rfpll_vcocal_delayAfterCloseLoop_MASK 0xff00
#define RF_2069_PLL_VCOCAL4_rfpll_vcocal_delayAfterCloseLoop_SHIFT 8
#define RF_2069_PLL_DSP19_rfpll_dsp_cal_regInI_1_MASK 0x3fff
#define RF_2069_PLL_DSP19_rfpll_dsp_cal_regInI_1_SHIFT 0
#define RF_2069_PLL_DSP20_rfpll_dsp_cal_regInI_2_MASK 0x3fff
#define RF_2069_PLL_DSP20_rfpll_dsp_cal_regInI_2_SHIFT 0
#define RF_2069_PLL_ADC4_rfpll_adc_R2_prog_MASK  0x1f
#define RF_2069_PLL_ADC4_rfpll_adc_R2_prog_SHIFT 0
#define RF_2069_PLL_XTAL3_xtal_xtal_swcap_out_MASK 0xf
#define RF_2069_PLL_XTAL3_xtal_xtal_swcap_out_SHIFT 0
#define RF_2069_PLL_VCOCAL7_rfpll_vcocal_force_caps_val_MASK 0x7ff
#define RF_2069_PLL_VCOCAL7_rfpll_vcocal_force_caps_val_SHIFT 0
#define RF_2069_PLL_CP2_rfpll_cp_idac_op_int_MASK 0xf0
#define RF_2069_PLL_CP2_rfpll_cp_idac_op_int_SHIFT 4
#define RF_2069_PLL_DSP43_rfpll_dsp_mmd_clk_edge_sel_MASK 0x400
#define RF_2069_PLL_DSP43_rfpll_dsp_mmd_clk_edge_sel_SHIFT 10
#define RF_2069_PLL_DSP34_rfpll_dsp_hys4_MASK    0xffff
#define RF_2069_PLL_DSP34_rfpll_dsp_hys4_SHIFT   0
#define RF_2069_PLL_VCO6_rfpll_vco_ALC_ref_ctrl_MASK 0xf00
#define RF_2069_PLL_VCO6_rfpll_vco_ALC_ref_ctrl_SHIFT 8
#define RF_2069_PLL_XTAL8_xtal_repeater3_size_MASK 0x1e
#define RF_2069_PLL_XTAL8_xtal_repeater3_size_SHIFT 1
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_hold_vctrl_ovr_MASK 0x8
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_hold_vctrl_ovr_SHIFT 3
#define RF_2069_PLL_CP1_rfpll_cp_en_spur_cancel_MASK 0x10
#define RF_2069_PLL_CP1_rfpll_cp_en_spur_cancel_SHIFT 4
#define RF_2069_PLL_DSPR17_rfpll_dsp_dcInIm_Rd_MASK 0x3fff
#define RF_2069_PLL_DSPR17_rfpll_dsp_dcInIm_Rd_SHIFT 0
#define RF_2069_PLL_CFG2_rfpll_rst_n_MASK        0x1
#define RF_2069_PLL_CFG2_rfpll_rst_n_SHIFT       0
#define RF_2069_PLL_DSPR18_rfpll_dsp_dcInRe_Rd_MASK 0x3fff
#define RF_2069_PLL_DSPR18_rfpll_dsp_dcInRe_Rd_SHIFT 0
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_VCO_MASK 0x1000
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_VCO_SHIFT 12
#define RF_2069_PLL_DSPR1_rfpll_dsp_aOut_1_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR1_rfpll_dsp_aOut_1_Rd_SHIFT 0
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_CP_byp_MASK 0x8
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_CP_byp_SHIFT 3
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_refresh_cap_cnt_MASK 0x380
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_refresh_cap_cnt_SHIFT 7
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_dcadj_1p2_MASK 0x780
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_dcadj_1p2_SHIFT 7
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_CP_MASK 0x2000
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_CP_SHIFT 13
#define RF_2069_PLL_DSPR13_rfpll_dsp_cal_crdc_ReIn_rd_2_MASK 0x3fff
#define RF_2069_PLL_DSPR13_rfpll_dsp_cal_crdc_ReIn_rd_2_SHIFT 0
#define RF_2069_OVR15_ovr_rfpll_vco_pu_MASK      0x400
#define RF_2069_OVR15_ovr_rfpll_vco_pu_SHIFT     10
#define RF_2069_PLL_VCO4_rfpll_vco_kvco_ctrl_MASK 0x60
#define RF_2069_PLL_VCO4_rfpll_vco_kvco_ctrl_SHIFT 5
#define RF_2069_PLL_DSP17_rfpll_dsp_cal_lpPhOffst_1_MASK 0xffff
#define RF_2069_PLL_DSP17_rfpll_dsp_cal_lpPhOffst_1_SHIFT 0
#define RF_2069_OVR15_ovr_ldo_1p2_xtalldo1p2_BG_pu_MASK 0x40
#define RF_2069_OVR15_ovr_ldo_1p2_xtalldo1p2_BG_pu_SHIFT 6
#define RF_2069_PLL_ADC1_rfpll_adc_ADC_resv_MASK 0xff
#define RF_2069_PLL_ADC1_rfpll_adc_ADC_resv_SHIFT 0
#define RF_2069_TOP_SPARE9_spare9_MASK           0xffff
#define RF_2069_TOP_SPARE9_spare9_SHIFT          0
#define RF_2069_PLL_ADC6_rfpll_adc_reset_MASK    0x100
#define RF_2069_PLL_ADC6_rfpll_adc_reset_SHIFT   8
#define RF_2069_PLL_MMD1_rfpll_mmd_sel_sglag_MASK 0x2
#define RF_2069_PLL_MMD1_rfpll_mmd_sel_sglag_SHIFT 1
#define RF_2069_PLL_DSP14_rfpll_dsp_cal_crdcGn_1_MASK 0x3fff
#define RF_2069_PLL_DSP14_rfpll_dsp_cal_crdcGn_1_SHIFT 0
#define RF_2069_PLL_LF4_rfpll_lf_lf_r1_MASK      0xff00
#define RF_2069_PLL_LF4_rfpll_lf_lf_r1_SHIFT     8
#define RF_2069_PLL_LF4_rfpll_lf_lf_r2_MASK      0xff
#define RF_2069_PLL_LF4_rfpll_lf_lf_r2_SHIFT     0
#define RF_2069_PLL_LF1_rfpll_lf_en_cm_bgfilter_MASK 0x200
#define RF_2069_PLL_LF1_rfpll_lf_en_cm_bgfilter_SHIFT 9
#define RF_2069_PLL_XTAL2_xtal_pu_RCCAL1_MASK    0x40
#define RF_2069_PLL_XTAL2_xtal_pu_RCCAL1_SHIFT   6
#define RF_2069_OVR16_ovr_rfpll_vcobuf_pu_MASK   0x100
#define RF_2069_OVR16_ovr_rfpll_vcobuf_pu_SHIFT  8
#define RF_2069_PLL_XTAL2_xtal_pu_corebuf_bb_MASK 0x4
#define RF_2069_PLL_XTAL2_xtal_pu_corebuf_bb_SHIFT 2
#define RF_2069_PLL_VCO3_rfpll_vco_ib_ctrl_MASK  0xf0
#define RF_2069_PLL_VCO3_rfpll_vco_ib_ctrl_SHIFT 4
#define RF_2069_PLL_DSP26_rfpll_dsp_dsGnAdj_MASK 0x7
#define RF_2069_PLL_DSP26_rfpll_dsp_dsGnAdj_SHIFT 0
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_fsmCrdcEnBypass_MASK 0x80
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_fsmCrdcEnBypass_SHIFT 7
#define RF_2069_PLL_DSPR5_rfpll_dsp_cCrdcOutRd_1_MASK 0x3fff
#define RF_2069_PLL_DSPR5_rfpll_dsp_cCrdcOutRd_1_SHIFT 0
#define RF_2069_PLL_LF1_rfpll_lf_en_2v_vcovdd_MASK 0x40
#define RF_2069_PLL_LF1_rfpll_lf_en_2v_vcovdd_SHIFT 6
#define RF_2069_TOP_SPARE1_spare1_MASK           0xffff
#define RF_2069_TOP_SPARE1_spare1_SHIFT          0
#define RF_2069_PLL_DSP28_rfpll_dsp_hys1_MASK    0xffff
#define RF_2069_PLL_DSP28_rfpll_dsp_hys1_SHIFT   0
#define RF_2069_PLL_ADC5_rfpll_adc_dither_pd_MASK 0x800
#define RF_2069_PLL_ADC5_rfpll_adc_dither_pd_SHIFT 11
#define RF_2069_PLL_ADC3_rfpll_adc_Idac2_vdd_MASK 0xf0
#define RF_2069_PLL_ADC3_rfpll_adc_Idac2_vdd_SHIFT 4
#define RF_2069_PLL_DSP21_rfpll_dsp_cal_regInQ_1_MASK 0x3fff
#define RF_2069_PLL_DSP21_rfpll_dsp_cal_regInQ_1_SHIFT 0
#define RF_2069_PLL_XTAL3_xtal_xtal_swcap_in_MASK 0xf0
#define RF_2069_PLL_XTAL3_xtal_xtal_swcap_in_SHIFT 4
#define RF_2069_PLL_CP1_rfpll_cp_en_fract_mode_MASK 0x80
#define RF_2069_PLL_CP1_rfpll_cp_en_fract_mode_SHIFT 7
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_adc_dcVal_MASK 0x100
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_adc_dcVal_SHIFT 8
#define RF_2069_PLL_DSPR16_rfpll_dsp_crdcInRe_Rd_MASK 0x3fff
#define RF_2069_PLL_DSPR16_rfpll_dsp_crdcInRe_Rd_SHIFT 0
#define RF_2069_PLL_XTAL8_xtal_repeater3_pu_MASK 0x20
#define RF_2069_PLL_XTAL8_xtal_repeater3_pu_SHIFT 5
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_VCO_MASK 0x8000
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_VCO_SHIFT 15
#define RF_2069_PLL_ADC6_rfpll_adc_pu_MASK       0x400
#define RF_2069_PLL_ADC6_rfpll_adc_pu_SHIFT      10
#define RF_2069_PLL_XTAL4_xtal_xtbufstrg_MASK    0x70
#define RF_2069_PLL_XTAL4_xtal_xtbufstrg_SHIFT   4
#define RF_2069_PLL_XTAL4_xtal_outbufcalstrg_MASK 0xc
#define RF_2069_PLL_XTAL4_xtal_outbufcalstrg_SHIFT 2
#define RF_2069_PLL_DSP37_rfpll_dsp_jtag_adc_gnCntrl_MASK 0x3
#define RF_2069_PLL_DSP37_rfpll_dsp_jtag_adc_gnCntrl_SHIFT 0
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_aux_MASK 0x400
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_aux_SHIFT 10
#define RF_2069_OVR15_ovr_xtal_pu_corebuf_pfd_MASK 0x10
#define RF_2069_OVR15_ovr_xtal_pu_corebuf_pfd_SHIFT 4
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_ref_timeout_MASK 0xff
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_ref_timeout_SHIFT 0
#define RF_2069_PLL_ADC5_rfpll_adc_gm_rc_MASK    0xf
#define RF_2069_PLL_ADC5_rfpll_adc_gm_rc_SHIFT   0
#define RF_2069_PLL_CFG1_rfpll_vco_pu_MASK       0x100
#define RF_2069_PLL_CFG1_rfpll_vco_pu_SHIFT      8
#define RF_2069_PLL_XTAL2_xtal_pu_corebuf_pfd_MASK 0x2
#define RF_2069_PLL_XTAL2_xtal_pu_corebuf_pfd_SHIFT 1
#define RF_2069_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovrVal_MASK 0x200
#define RF_2069_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovrVal_SHIFT 9
#define RF_2069_PLL_DSP25_rfpll_dsp_dcInit_MASK  0x3ff
#define RF_2069_PLL_DSP25_rfpll_dsp_dcInit_SHIFT 0
#define RF_2069_OVR14_ovr_xtal_core_lp_MASK      0x2000
#define RF_2069_OVR14_ovr_xtal_core_lp_SHIFT     13
#define RF_2069_PLL_CP2_rfpll_cp_idac_op_core_MASK 0xf00
#define RF_2069_PLL_CP2_rfpll_cp_idac_op_core_SHIFT 8
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_mmdIn_MASK 0x20
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_mmdIn_SHIFT 5
#define RF_2069_PLL_HVLDO1_ldo_2p5_lowpwren_VCO_MASK 0x40
#define RF_2069_PLL_HVLDO1_ldo_2p5_lowpwren_VCO_SHIFT 6
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_MASK 0x10
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_SHIFT 4
#define RF_2069_PLL_DSPR9_rfpll_dsp_cal_crdc_ImIn_rd_2_MASK 0x3fff
#define RF_2069_PLL_DSPR9_rfpll_dsp_cal_crdc_ImIn_rd_2_SHIFT 0
#define RF_2069_PLL_MMD1_rfpll_mmd_sel_sglead_MASK 0x1
#define RF_2069_PLL_MMD1_rfpll_mmd_sel_sglead_SHIFT 0
#define RF_2069_PLL_CFG1_rfpll_pfd_delay_MASK    0x70
#define RF_2069_PLL_CFG1_rfpll_pfd_delay_SHIFT   4
#define RF_2069_TOP_SPARE8_spare8_MASK           0xffff
#define RF_2069_TOP_SPARE8_spare8_SHIFT          0
#define RF_2069_OVR15_ovr_xtal_pu_corebuf_bb_MASK 0x20
#define RF_2069_OVR15_ovr_xtal_pu_corebuf_bb_SHIFT 5
#define RF_2069_PLL_DSP35_rfpll_dsp_hys5_MASK    0xf
#define RF_2069_PLL_DSP35_rfpll_dsp_hys5_SHIFT   0
#define RF_2069_PLL_XTAL9_xtal_xtal_sel_halfres_MASK 0x2
#define RF_2069_PLL_XTAL9_xtal_xtal_sel_halfres_SHIFT 1
#define RF_2069_PLL_XTAL9_xtal_buf2strg_MASK     0x38
#define RF_2069_PLL_XTAL9_xtal_buf2strg_SHIFT    3
#define RF_2069_PLL_DSPR10_rfpll_dsp_cal_crdc_PhIn_rd_1_MASK 0xffff
#define RF_2069_PLL_DSPR10_rfpll_dsp_cal_crdc_PhIn_rd_1_SHIFT 0
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_BG_pu_MASK 0x10
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_BG_pu_SHIFT 4
#define RF_2069_PLL_CP5_rfpll_cp_ioff_extra_MASK 0x1
#define RF_2069_PLL_CP5_rfpll_cp_ioff_extra_SHIFT 0
#define RF_2069_PLL_VCOCAL3_rfpll_vcocal_clk_gate_ovr_MASK 0x4
#define RF_2069_PLL_VCOCAL3_rfpll_vcocal_clk_gate_ovr_SHIFT 2
#define RF_2069_PLL_ADC5_rfpll_adc_en_tp_in_MASK 0x100
#define RF_2069_PLL_ADC5_rfpll_adc_en_tp_in_SHIFT 8
#define RF_2069_PLL_VCOCAL3_rfpll_vcocal_compLevelIn_MASK 0x3
#define RF_2069_PLL_VCOCAL3_rfpll_vcocal_compLevelIn_SHIFT 0
#define RF_2069_PLL_DSP26_rfpll_dsp_dcSrchOn_MASK 0x2000
#define RF_2069_PLL_DSP26_rfpll_dsp_dcSrchOn_SHIFT 13
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_VCO_aux_MASK 0x200
#define RF_2069_OVR16_ovr_ldo_2p5_pu_ldo_VCO_aux_SHIFT 9
#define RF_2069_OVR14_ovr_pmu_synth_pwrsw_pu_MASK 0x200
#define RF_2069_OVR14_ovr_pmu_synth_pwrsw_pu_SHIFT 9
#define RF_2069_PLL_CP1_rfpll_cp_i_spur_cancel_MASK 0xf
#define RF_2069_PLL_CP1_rfpll_cp_i_spur_cancel_SHIFT 0
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_VCO_aux_vout_sel_MASK 0xf0
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_VCO_aux_vout_sel_SHIFT 4
#define RF_2069_PLL_ADC6_rfpll_adc_clk_out_flip_MASK 0x200
#define RF_2069_PLL_ADC6_rfpll_adc_clk_out_flip_SHIFT 9
#define RF_2069_PLL_VCOCAL12_rfpll_vcocal_ref_value_MASK 0xffff
#define RF_2069_PLL_VCOCAL12_rfpll_vcocal_ref_value_SHIFT 0
#define RF_2069_PLL_DSP13_rfpll_dsp_autoEn_1_MASK 0x2
#define RF_2069_PLL_DSP13_rfpll_dsp_autoEn_1_SHIFT 1
#define RF_2069_PLL_DSP13_rfpll_dsp_autoEn_2_MASK 0x1
#define RF_2069_PLL_DSP13_rfpll_dsp_autoEn_2_SHIFT 0
#define RF_2069_PLL_XTAL5_xtal_sel_BT_MASK       0x6000
#define RF_2069_PLL_XTAL5_xtal_sel_BT_SHIFT      13
#define RF_2069_OVR14_ovr_pmu_wlpmu_en_MASK      0x20
#define RF_2069_OVR14_ovr_pmu_wlpmu_en_SHIFT     5
#define RF_2069_PLL_DSP24_rfpll_dsp_cpRng_MASK   0x3ff
#define RF_2069_PLL_DSP24_rfpll_dsp_cpRng_SHIFT  0
#define RF_2069_PLL_VCO1_rfpll_vco_EN_DEGEN_ovrVal_MASK 0x1000
#define RF_2069_PLL_VCO1_rfpll_vco_EN_DEGEN_ovrVal_SHIFT 12
#define RF_2069_PLL_ADC2_rfpll_adc_CAP1_MASK     0x3e0
#define RF_2069_PLL_ADC2_rfpll_adc_CAP1_SHIFT    5
#define RF_2069_PLL_ADC2_rfpll_adc_CAP2_MASK     0x1f
#define RF_2069_PLL_ADC2_rfpll_adc_CAP2_SHIFT    0
#define RF_2069_PLL_CP3_rfpll_cp_idn_integer_MASK 0x3f0
#define RF_2069_PLL_CP3_rfpll_cp_idn_integer_SHIFT 4
#define RF_2069_PLL_MONITOR1_rfpll_monitor_vth_high_MASK 0xf0
#define RF_2069_PLL_MONITOR1_rfpll_monitor_vth_high_SHIFT 4
#define RF_2069_OVR22_ovr_xtal_coresize_pmos_MASK 0x200
#define RF_2069_OVR22_ovr_xtal_coresize_pmos_SHIFT 9
#define RF_2069_PLL_LF5_rfpll_lf_lf_r3_MASK      0xff
#define RF_2069_PLL_LF5_rfpll_lf_lf_r3_SHIFT     0
#define RF_2069_PLL_XTAL9_xtal_xtal_extclk_en_MASK 0x40
#define RF_2069_PLL_XTAL9_xtal_xtal_extclk_en_SHIFT 6
#define RF_2069_PLL_DSP40_rfpll_dsp_mSrchOn_1_MASK 0x2
#define RF_2069_PLL_DSP40_rfpll_dsp_mSrchOn_1_SHIFT 1
#define RF_2069_PLL_DSP40_rfpll_dsp_mSrchOn_2_MASK 0x1
#define RF_2069_PLL_DSP40_rfpll_dsp_mSrchOn_2_SHIFT 0
#define RF_2069_PLL_DSPR21_rfpll_dsp_imOut_1_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR21_rfpll_dsp_imOut_1_Rd_SHIFT 0
#define RF_2069_PLL_DSPR22_rfpll_dsp_imOut_2_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR22_rfpll_dsp_imOut_2_Rd_SHIFT 0
#define RF_2069_PLL_XTAL9_xtal_pu_corebuf2_pfd_MASK 0x4
#define RF_2069_PLL_XTAL9_xtal_pu_corebuf2_pfd_SHIFT 2
#define RF_2069_PLL_ADC3_rfpll_adc_Idac2_vss_MASK 0xf
#define RF_2069_PLL_ADC3_rfpll_adc_Idac2_vss_SHIFT 0
#define RF_2069_PLL_VCO8_rfpll_vco_vctrl_buf_device_width_MASK 0x38
#define RF_2069_PLL_VCO8_rfpll_vco_vctrl_buf_device_width_SHIFT 3
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_CP_MASK 0x20
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowquiescenten_CP_SHIFT 5
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_VCO_aux_MASK 0x400
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_VCO_aux_SHIFT 10
#define RF_2069_PLL_VCO1_rfpll_vco_ICAL_MASK     0x1f
#define RF_2069_PLL_VCO1_rfpll_vco_ICAL_SHIFT    0
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_byp_MASK 0x4
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_byp_SHIFT 2
#define RF_2069_PLL_ADC6_rfpll_adc_vref_rc_MASK  0x3
#define RF_2069_PLL_ADC6_rfpll_adc_vref_rc_SHIFT 0
#define RF_2069_PLL_DSP30_rfpll_dsp_hys2_MASK    0xffff
#define RF_2069_PLL_DSP30_rfpll_dsp_hys2_SHIFT   0
#define RF_2069_PLL_DSP29_rfpll_dsp_hys2_MASK    0xf
#define RF_2069_PLL_DSP29_rfpll_dsp_hys2_SHIFT   0
#define RF_2069_PLL_DSP5_rfpll_dsp_aaifInSel_MASK 0x3
#define RF_2069_PLL_DSP5_rfpll_dsp_aaifInSel_SHIFT 0
#define RF_2069_PLL_VCO2_rfpll_vco_cal_en_MASK   0x40
#define RF_2069_PLL_VCO2_rfpll_vco_cal_en_SHIFT  6
#define RF_2069_PLL_DSP37_rfpll_dsp_jtag_adc_dcVal_MASK 0xffc
#define RF_2069_PLL_DSP37_rfpll_dsp_jtag_adc_dcVal_SHIFT 2
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_regCrdcEn_MASK 0x1
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_regCrdcEn_SHIFT 0
#define RF_2069_TOP_SPARE10_spare10_MASK         0xffff
#define RF_2069_TOP_SPARE10_spare10_SHIFT        0
#define RF_2069_PLL_CP5_rfpll_open_gnd_cp_lf_MASK 0x2
#define RF_2069_PLL_CP5_rfpll_open_gnd_cp_lf_SHIFT 1
#define RF_2069_PLL_XTAL2_xtal_pu_bbdrv_MASK     0x20
#define RF_2069_PLL_XTAL2_xtal_pu_bbdrv_SHIFT    5
#define RF_2069_OVR15_ovr_ldo_1p2_xtalldo1p2_vref_bias_reset_MASK 0x80
#define RF_2069_OVR15_ovr_ldo_1p2_xtalldo1p2_vref_bias_reset_SHIFT 7
#define RF_2069_PLL_CP1_rfpll_cp_en_noise_cancel_MASK 0x40
#define RF_2069_PLL_CP1_rfpll_cp_en_noise_cancel_SHIFT 6
#define RF_2069_TOP_SPARE7_spare7_MASK           0xffff
#define RF_2069_TOP_SPARE7_spare7_SHIFT          0
#define RF_2069_PLL_VCO2_rfpll_vco_cvar_MASK     0xf
#define RF_2069_PLL_VCO2_rfpll_vco_cvar_SHIFT    0
#define RF_2069_PLL_XTAL4_xtal_outbufBBstrg_MASK 0x7000
#define RF_2069_PLL_XTAL4_xtal_outbufBBstrg_SHIFT 12
#define RF_2069_PLL_VCOCAL15_rfpll_vcocal_refresh_cal_valid_MASK 0x800
#define RF_2069_PLL_VCOCAL15_rfpll_vcocal_refresh_cal_valid_SHIFT 11
#define RF_2069_PLL_ADC5_rfpll_adc_dither_rstn_MASK 0x400
#define RF_2069_PLL_ADC5_rfpll_adc_dither_rstn_SHIFT 10
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_artload_MASK 0x80
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_artload_SHIFT 7
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_gnSclMux_1_MASK 0x18
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_gnSclMux_1_SHIFT 3
#define RF_2069_OVR22_ovr_xtal_clock_HSIC_pu_MASK 0x2000
#define RF_2069_OVR22_ovr_xtal_clock_HSIC_pu_SHIFT 13
#define RF_2069_OVR15_ovr_rfpll_rst_n_MASK       0x800
#define RF_2069_OVR15_ovr_rfpll_rst_n_SHIFT      11
#define RF_2069_PLL_XTAL8_xtal_repeater2_size_MASK 0x3c0
#define RF_2069_PLL_XTAL8_xtal_repeater2_size_SHIFT 6
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_mode_MASK 0x100
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_mode_SHIFT 8
#define RF_2069_OVR15_rfpll_vco_EN_DEGEN_jtag_ovr_MASK 0x200
#define RF_2069_OVR15_rfpll_vco_EN_DEGEN_jtag_ovr_SHIFT 9
#define RF_2069_PLL_DSPR4_rfpll_dsp_aaif5OutRd_MASK 0x3fff
#define RF_2069_PLL_DSPR4_rfpll_dsp_aaif5OutRd_SHIFT 0
#define RF_2069_PLL_HVLDO1_ldo_2p5_bias_reset_VCO_MASK 0x100
#define RF_2069_PLL_HVLDO1_ldo_2p5_bias_reset_VCO_SHIFT 8
#define RF_2069_PLL_CFG2_rfpll_pfd_open_MASK     0x2
#define RF_2069_PLL_CFG2_rfpll_pfd_open_SHIFT    1
#define RF_2069_PLL_DSP36_rfpll_dsp_hys5_MASK    0xffff
#define RF_2069_PLL_DSP36_rfpll_dsp_hys5_SHIFT   0
#define RF_2069_PLL_DSP1_rfpll_dsp_SpurFreqErr_1_MASK 0x3fff
#define RF_2069_PLL_DSP1_rfpll_dsp_SpurFreqErr_1_SHIFT 0
#define RF_2069_OVR22_ovr_xtal_buf_low_pow_MASK  0x8000
#define RF_2069_OVR22_ovr_xtal_buf_low_pow_SHIFT 15
#define RF_2069_PLL_FRCT2_rfpll_frct_wild_base_MASK 0xffff
#define RF_2069_PLL_FRCT2_rfpll_frct_wild_base_SHIFT 0
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_autoEn_MASK 0x100
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_autoEn_SHIFT 8
#define RF_2069_OVR14_ovr_pmu_vrefadj_cbuck_MASK 0x40
#define RF_2069_OVR14_ovr_pmu_vrefadj_cbuck_SHIFT 6
#define RF_2069_PLL_XTAL5_xtal_bufstrg_BT_MASK   0x1c0
#define RF_2069_PLL_XTAL5_xtal_bufstrg_BT_SHIFT  6
#define RF_2069_PLL_CFGR2_rfpll_spare2_MASK      0xff00
#define RF_2069_PLL_CFGR2_rfpll_spare2_SHIFT     8
#define RF_2069_PLL_CFGR2_rfpll_spare3_MASK      0xff
#define RF_2069_PLL_CFGR2_rfpll_spare3_SHIFT     0
#define RF_2069_OVR22_ovr_xtal_buf_pu_MASK       0x4000
#define RF_2069_OVR22_ovr_xtal_buf_pu_SHIFT      14
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_pull_down_sw_MASK 0x6
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_pull_down_sw_SHIFT 1
#define RF_2069_PLL_DSPR27_rfpll_monitor_vctrl_level_MASK 0x3
#define RF_2069_PLL_DSPR27_rfpll_monitor_vctrl_level_SHIFT 0
#define RF_2069_PLL_VCOCAL5_rfpll_vcocal_delayBeforeOpenLoop_MASK 0xff
#define RF_2069_PLL_VCOCAL5_rfpll_vcocal_delayBeforeOpenLoop_SHIFT 0
#define RF_2069_PLL_DSPR3_rfpll_dsp_aaif1OutRd_MASK 0x3fff
#define RF_2069_PLL_DSPR3_rfpll_dsp_aaif1OutRd_SHIFT 0
#define RF_2069_PLL_CFGR1_rfpll_spare0_MASK      0xff00
#define RF_2069_PLL_CFGR1_rfpll_spare0_SHIFT     8
#define RF_2069_PLL_CFGR1_rfpll_spare1_MASK      0xff
#define RF_2069_PLL_CFGR1_rfpll_spare1_SHIFT     0
#define RF_2069_PLL_ADC5_rfpll_adc_gm_idac_MASK  0xf0
#define RF_2069_PLL_ADC5_rfpll_adc_gm_idac_SHIFT 4
#define RF_2069_PLL_VCO6_rfpll_vco_ib_bias_opamp_MASK 0xf0
#define RF_2069_PLL_VCO6_rfpll_vco_ib_bias_opamp_SHIFT 4
#define RF_2069_PLL_DSP43_rfpll_dsp_mmdDelaySel_MASK 0x1800
#define RF_2069_PLL_DSP43_rfpll_dsp_mmdDelaySel_SHIFT 11
#define RF_2069_PLL_ADC5_rfpll_adc_double_speed_MASK 0x200
#define RF_2069_PLL_ADC5_rfpll_adc_double_speed_SHIFT 9
#define RF_2069_PLL_XTAL1_xtal_coresize_nmos_MASK 0x3e0
#define RF_2069_PLL_XTAL1_xtal_coresize_nmos_SHIFT 5
#define RF_2069_PLL_CP1_rfpll_cp_en_offset_cancel_MASK 0x20
#define RF_2069_PLL_CP1_rfpll_cp_en_offset_cancel_SHIFT 5
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_vref_bias_reset_MASK 0x1
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_vref_bias_reset_SHIFT 0
#define RF_2069_PLL_XTAL8_xtal_repeater2_pu_MASK 0x400
#define RF_2069_PLL_XTAL8_xtal_repeater2_pu_SHIFT 10
#define RF_2069_PLL_XTAL8_xtal_repeater_pseudo_diff_en_MASK 0x1
#define RF_2069_PLL_XTAL8_xtal_repeater_pseudo_diff_en_SHIFT 0
#define RF_2069_PLL_DSPR12_rfpll_dsp_cal_crdc_ReIn_rd_1_MASK 0x3fff
#define RF_2069_PLL_DSPR12_rfpll_dsp_cal_crdc_ReIn_rd_1_SHIFT 0
#define RF_2069_OVR15_ovr_rfpll_cal_rst_n_MASK   0x8000
#define RF_2069_OVR15_ovr_rfpll_cal_rst_n_SHIFT  15
#define RF_2069_PLL_VCO6_rfpll_vco_bypass_vctrl_buf_MASK 0x2000
#define RF_2069_PLL_VCO6_rfpll_vco_bypass_vctrl_buf_SHIFT 13
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_chrgPmpScl_MASK 0x40
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_chrgPmpScl_SHIFT 6
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_CP_vout_sel_MASK 0xf00
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_CP_vout_sel_SHIFT 8
#define RF_2069_TOP_SPARE6_spare6_MASK           0xffff
#define RF_2069_TOP_SPARE6_spare6_SHIFT          0
#define RF_2069_PLL_ADC5_rfpll_adc_dither_clk_flip_MASK 0x1000
#define RF_2069_PLL_ADC5_rfpll_adc_dither_clk_flip_SHIFT 12
#define RF_2069_PLL_DSP38_rfpll_dsp_jtag_chrgPmpScl_MASK 0xff
#define RF_2069_PLL_DSP38_rfpll_dsp_jtag_chrgPmpScl_SHIFT 0
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_byp_MASK 0x100
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_byp_SHIFT 8
#define RF_2069_PLL_VCO6_rfpll_vco_bias_mode_MASK 0x1000
#define RF_2069_PLL_VCO6_rfpll_vco_bias_mode_SHIFT 12
#define RF_2069_PLL_CFG1_rfpll_pfd_en_MASK       0xf
#define RF_2069_PLL_CFG1_rfpll_pfd_en_SHIFT      0
#define RF_2069_PLL_DSP31_rfpll_dsp_hys3_MASK    0xf
#define RF_2069_PLL_DSP31_rfpll_dsp_hys3_SHIFT   0
#define RF_2069_PLL_ADC5_rfpll_adc_ditherMode_MASK 0x6000
#define RF_2069_PLL_ADC5_rfpll_adc_ditherMode_SHIFT 13
#define RF_2069_PLL_VCO1_rfpll_vco_HDRM_CAL_MASK 0x1e0
#define RF_2069_PLL_VCO1_rfpll_vco_HDRM_CAL_SHIFT 5
#define RF_2069_OVR15_rfpll_vco_FAST_SETTLE_jtag_ovr_MASK 0x100
#define RF_2069_OVR15_rfpll_vco_FAST_SETTLE_jtag_ovr_SHIFT 8
#define RF_2069_PLL_DSPR20_rfpll_dsp_gnVal_Rd_MASK 0x3ff
#define RF_2069_PLL_DSPR20_rfpll_dsp_gnVal_Rd_SHIFT 0
#define RF_2069_PLL_CP4_rfpll_cp_op_int_on_MASK  0x1
#define RF_2069_PLL_CP4_rfpll_cp_op_int_on_SHIFT 0
#define RF_2069_PLL_XTAL2_xtal_pu_BT_MASK        0x100
#define RF_2069_PLL_XTAL2_xtal_pu_BT_SHIFT       8
#define RF_2069_PLL_DSP26_rfpll_dsp_dcStrlLvl_MASK 0x1ff8
#define RF_2069_PLL_DSP26_rfpll_dsp_dcStrlLvl_SHIFT 3
#define RF_2069_PLL_CFG1_rfpll_synth_pu_MASK     0x200
#define RF_2069_PLL_CFG1_rfpll_synth_pu_SHIFT    9
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_gnReg_MASK 0x60
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_gnReg_SHIFT 5
#define RF_2069_PLL_DSPR14_rfpll_dsp_crdcEn_Rd_MASK 0x100
#define RF_2069_PLL_DSPR14_rfpll_dsp_crdcEn_Rd_SHIFT 8
#define RF_2069_PLL_VCO3_rfpll_vco_en_alc_MASK   0x100
#define RF_2069_PLL_VCO3_rfpll_vco_en_alc_SHIFT  8
#define RF_2069_PLL_MMD1_rfpll_mmd_en_dac_p_MASK 0x8
#define RF_2069_PLL_MMD1_rfpll_mmd_en_dac_p_SHIFT 3
#define RF_2069_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_MASK 0x780
#define RF_2069_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_SHIFT 7
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_VCO_vout_sel_MASK 0xf
#define RF_2069_PLL_HVLDO3_ldo_2p5_ldo_VCO_vout_sel_SHIFT 0
#define RF_2069_PLL_DSP11_rfpll_dsp_adc_regInI_MASK 0x3fff
#define RF_2069_PLL_DSP11_rfpll_dsp_adc_regInI_SHIFT 0
#define RF_2069_OVR22_ovr_xtal_core_MASK         0x1000
#define RF_2069_OVR22_ovr_xtal_core_SHIFT        12
#define RF_2069_PLL_DSP9_rfpll_dsp_adc_SpurFreq_2_MASK 0x7fff
#define RF_2069_PLL_DSP9_rfpll_dsp_adc_SpurFreq_2_SHIFT 0
#define RF_2069_PLL_CP2_rfpll_cp_idn_dac_pulse_MASK 0xf
#define RF_2069_PLL_CP2_rfpll_cp_idn_dac_pulse_SHIFT 0
#define RF_2069_PLL_VCO4_rfpll_vco_tempco_MASK   0xf
#define RF_2069_PLL_VCO4_rfpll_vco_tempco_SHIFT  0
#define RF_2069_PLL_VCOCAL6_rfpll_vcocal_force_caps_MASK 0x1
#define RF_2069_PLL_VCOCAL6_rfpll_vcocal_force_caps_SHIFT 0
#define RF_2069_PLL_DSP23_rfpll_dsp_clk_enable_MASK 0x1
#define RF_2069_PLL_DSP23_rfpll_dsp_clk_enable_SHIFT 0
#define RF_2069_PLL_XTAL4_xtal_xt_res_bpass_MASK 0x380
#define RF_2069_PLL_XTAL4_xtal_xt_res_bpass_SHIFT 7
#define RF_2069_OVR14_ovr_rfpll_afe_pu_MASK      0x10
#define RF_2069_OVR14_ovr_rfpll_afe_pu_SHIFT     4
#define RF_2069_OVR15_ovr_rfpll_en_vcocal_MASK   0x4000
#define RF_2069_OVR15_ovr_rfpll_en_vcocal_SHIFT  14
#define RF_2069_PLL_HVLDO2_ldo_2p5_bias_reset_VCO_aux_MASK 0x40
#define RF_2069_PLL_HVLDO2_ldo_2p5_bias_reset_VCO_aux_SHIFT 6
#define RF_2069_PLL_DSPR15_rfpll_dsp_crdcInIm_Rd_MASK 0x3fff
#define RF_2069_PLL_DSPR15_rfpll_dsp_crdcInIm_Rd_SHIFT 0
#define RF_2069_OVR14_ovr_rfpll_bias_reset_vco_MASK 0x2
#define RF_2069_OVR14_ovr_rfpll_bias_reset_vco_SHIFT 1
#define RF_2069_PLL_ADC4_rfpll_adc_Idac3_vdd_MASK 0xe00
#define RF_2069_PLL_ADC4_rfpll_adc_Idac3_vdd_SHIFT 9
#define RF_2069_PLL_CP4_rfpll_cp_ioff_MASK       0xff00
#define RF_2069_PLL_CP4_rfpll_cp_ioff_SHIFT      8
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_en_MASK 0x4
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_en_SHIFT 2
#define RF_2069_PLL_CP4_rfpll_cp_kpd_scale_MASK  0xfe
#define RF_2069_PLL_CP4_rfpll_cp_kpd_scale_SHIFT 1
#define RF_2069_PLL_LF1_rfpll_lf_en_cp_down_MASK 0x20
#define RF_2069_PLL_LF1_rfpll_lf_en_cp_down_SHIFT 5
#define RF_2069_PLL_DSP18_rfpll_dsp_cal_lpPhOffst_2_MASK 0xffff
#define RF_2069_PLL_DSP18_rfpll_dsp_cal_lpPhOffst_2_SHIFT 0
#define RF_2069_PLL_DSP15_rfpll_dsp_cal_crdcGn_2_MASK 0x3fff
#define RF_2069_PLL_DSP15_rfpll_dsp_cal_crdcGn_2_SHIFT 0
#define RF_2069_PLL_DSP39_rfpll_dsp_loopLn_MASK  0xf
#define RF_2069_PLL_DSP39_rfpll_dsp_loopLn_SHIFT 0
#define RF_2069_PLL_DSPR20_rfpll_dsp_gnCntrl_Rd_MASK 0xc00
#define RF_2069_PLL_DSPR20_rfpll_dsp_gnCntrl_Rd_SHIFT 10
#define RF_2069_PLL_DSP3_rfpll_dsp_SpurFreq_1_MASK 0x7fff
#define RF_2069_PLL_DSP3_rfpll_dsp_SpurFreq_1_SHIFT 0
#define RF_2069_PLL_DSP41_rfpll_dsp_mmdCntrlIn_MASK 0xffff
#define RF_2069_PLL_DSP41_rfpll_dsp_mmdCntrlIn_SHIFT 0
#define RF_2069_PLL_DSP46_rfpll_dsp_thtaRng_1_MASK 0xffff
#define RF_2069_PLL_DSP46_rfpll_dsp_thtaRng_1_SHIFT 0
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_refresh_cal_MASK 0x400
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_refresh_cal_SHIFT 10
#define RF_2069_PLL_LF2_rfpll_lf_lf_c1_MASK      0xff00
#define RF_2069_PLL_LF2_rfpll_lf_lf_c1_SHIFT     8
#define RF_2069_PLL_FRCT1_rfpll_frct_dith_sel_MASK 0x8
#define RF_2069_PLL_FRCT1_rfpll_frct_dith_sel_SHIFT 3
#define RF_2069_PLL_LF2_rfpll_lf_lf_c2_MASK      0xff
#define RF_2069_PLL_LF2_rfpll_lf_lf_c2_SHIFT     0
#define RF_2069_PLL_DSPR8_rfpll_dsp_cal_crdc_ImIn_rd_1_MASK 0x3fff
#define RF_2069_PLL_DSPR8_rfpll_dsp_cal_crdc_ImIn_rd_1_SHIFT 0
#define RF_2069_TOP_SPARE5_spare5_MASK           0xffff
#define RF_2069_TOP_SPARE5_spare5_SHIFT          0
#define RF_2069_PLL_VCOCAL15_rfpll_vcocal_read_cap_val_MASK 0x7ff
#define RF_2069_PLL_VCOCAL15_rfpll_vcocal_read_cap_val_SHIFT 0
#define RF_2069_OVR15_ovr_rfpll_pll_pu_MASK      0x1000
#define RF_2069_OVR15_ovr_rfpll_pll_pu_SHIFT     12
#define RF_2069_PLL_DSPR6_rfpll_dsp_cCrdcOutRd_2_MASK 0x3fff
#define RF_2069_PLL_DSPR6_rfpll_dsp_cCrdcOutRd_2_SHIFT 0
#define RF_2069_PLL_DSP8_rfpll_dsp_adc_SpurFreq_1_MASK 0x7fff
#define RF_2069_PLL_DSP8_rfpll_dsp_adc_SpurFreq_1_SHIFT 0
#define RF_2069_PLL_DSP5_rfpll_dsp_aSrchOn_1_MASK 0x8
#define RF_2069_PLL_DSP5_rfpll_dsp_aSrchOn_1_SHIFT 3
#define RF_2069_PLL_DSP5_rfpll_dsp_aSrchOn_2_MASK 0x4
#define RF_2069_PLL_DSP5_rfpll_dsp_aSrchOn_2_SHIFT 2
#define RF_2069_OVR22_ovr_xtal_core_change_MASK  0x800
#define RF_2069_OVR22_ovr_xtal_core_change_SHIFT 11
#define RF_2069_OVR15_ovr_rfpll_cp_pu_MASK       0x2
#define RF_2069_OVR15_ovr_rfpll_cp_pu_SHIFT      1
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_CP_artload_MASK 0x2
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_CP_artload_SHIFT 1
#define RF_2069_OVR14_ovr_pmu_AFEldo_pu_MASK     0x1000
#define RF_2069_OVR14_ovr_pmu_AFEldo_pu_SHIFT    12
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_CP_MASK 0x4000
#define RF_2069_OVR16_ovr_ldo_2p5_bias_reset_CP_SHIFT 14
#define RF_2069_PLL_DSP45_rfpll_dsp_stlTmr2_MASK 0xffff
#define RF_2069_PLL_DSP45_rfpll_dsp_stlTmr2_SHIFT 0
#define RF_2069_OVR15_ovr_rfpll_synth_pu_MASK    0x1
#define RF_2069_OVR15_ovr_rfpll_synth_pu_SHIFT   0
#define RF_2069_PLL_DSP47_rfpll_dsp_thtaRng_2_MASK 0xffff
#define RF_2069_PLL_DSP47_rfpll_dsp_thtaRng_2_SHIFT 0
#define RF_2069_PLL_DSP22_rfpll_dsp_cal_regInQ_2_MASK 0x3fff
#define RF_2069_PLL_DSP22_rfpll_dsp_cal_regInQ_2_SHIFT 0
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_xtl_div_MASK 0x1f
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_xtl_div_SHIFT 0
#define RF_2069_PLL_MMD1_rfpll_mmd_input_sel_MASK 0x4
#define RF_2069_PLL_MMD1_rfpll_mmd_input_sel_SHIFT 2
#define RF_2069_PLL_XTAL2_xtal_pu_core_MASK      0x8
#define RF_2069_PLL_XTAL2_xtal_pu_core_SHIFT     3
#define RF_2069_PLL_DSP43_rfpll_dsp_regrdMux_MASK 0x8
#define RF_2069_PLL_DSP43_rfpll_dsp_regrdMux_SHIFT 3
#define RF_2069_PLL_DSP43_rfpll_dsp_mmd_ctrl_clk_enable_MASK 0x200
#define RF_2069_PLL_DSP43_rfpll_dsp_mmd_ctrl_clk_enable_SHIFT 9
#define RF_2069_PLL_HVLDO1_ldo_2p5_bias_reset_CP_MASK 0x200
#define RF_2069_PLL_HVLDO1_ldo_2p5_bias_reset_CP_SHIFT 9
#define RF_2069_PLL_LF7_rfpll_lf_lf_rf_cm_MASK   0xff00
#define RF_2069_PLL_LF7_rfpll_lf_lf_rf_cm_SHIFT  8
#define RF_2069_PLL_XTAL5_xtal_sel_RCCAL1_MASK   0x1800
#define RF_2069_PLL_XTAL5_xtal_sel_RCCAL1_SHIFT  11
#define RF_2069_OVR15_ovr_rfpll_vcocal_cal_MASK  0x4
#define RF_2069_OVR15_ovr_rfpll_vcocal_cal_SHIFT 2
#define RF_2069_PLL_XTAL2_xtal_pu_RCCAL_MASK     0x80
#define RF_2069_PLL_XTAL2_xtal_pu_RCCAL_SHIFT    7
#define RF_2069_PLL_DSP7_rfpll_dsp_adc_SpurFreqErr_2_MASK 0x3fff
#define RF_2069_PLL_DSP7_rfpll_dsp_adc_SpurFreqErr_2_SHIFT 0
#define RF_2069_PLL_DSP32_rfpll_dsp_hys3_MASK    0xffff
#define RF_2069_PLL_DSP32_rfpll_dsp_hys3_SHIFT   0
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_rdEn_jtag_MASK 0x2
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_rdEn_jtag_SHIFT 1
#define RF_2069_PLL_LF1_rfpll_lf_bcon_cm_MASK    0x180
#define RF_2069_PLL_LF1_rfpll_lf_bcon_cm_SHIFT   7
#define RF_2069_OVR14_ovr_pmu_TXldo_pu_MASK      0x100
#define RF_2069_OVR14_ovr_pmu_TXldo_pu_SHIFT     8
#define RF_2069_PLL_DSPR23_rfpll_dsp_magOut_2_Rd_MASK 0xff
#define RF_2069_PLL_DSPR23_rfpll_dsp_magOut_2_Rd_SHIFT 0
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_rst_n_MASK 0x40
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_rst_n_SHIFT 6
#define RF_2069_PLL_CFG1_rfpll_monitor_pu_MASK   0x400
#define RF_2069_PLL_CFG1_rfpll_monitor_pu_SHIFT  10
#define RF_2069_PLL_CP1_rfpll_cp_en_dac_pulse_MASK 0x100
#define RF_2069_PLL_CP1_rfpll_cp_en_dac_pulse_SHIFT 8
#define RF_2069_PLL_FRCT3_rfpll_frct_wild_base_MASK 0xffff
#define RF_2069_PLL_FRCT3_rfpll_frct_wild_base_SHIFT 0
#define RF_2069_PLL_DSP43_rfpll_dsp_spur_cncl_enable_1_MASK 0x2
#define RF_2069_PLL_DSP43_rfpll_dsp_spur_cncl_enable_1_SHIFT 1
#define RF_2069_PLL_DSPR18_rfpll_dsp_dc_done_MASK 0x8000
#define RF_2069_PLL_DSPR18_rfpll_dsp_dc_done_SHIFT 15
#define RF_2069_PLL_DSP43_rfpll_dsp_spur_cncl_enable_2_MASK 0x1
#define RF_2069_PLL_DSP43_rfpll_dsp_spur_cncl_enable_2_SHIFT 0
#define RF_2069_PLL_XTAL8_xtal_repeater1_size_MASK 0x7800
#define RF_2069_PLL_XTAL8_xtal_repeater1_size_SHIFT 11
#define RF_2069_PLL_DSP43_rfpll_dsp_regrdEn_jtag_MASK 0x10
#define RF_2069_PLL_DSP43_rfpll_dsp_regrdEn_jtag_SHIFT 4
#define RF_2069_PLL_ADC4_rfpll_adc_NTFZeroShift_MASK 0x20
#define RF_2069_PLL_ADC4_rfpll_adc_NTFZeroShift_SHIFT 5
#define RF_2069_PLL_DSP43_rfpll_dsp_rst_n_MASK   0x4
#define RF_2069_PLL_DSP43_rfpll_dsp_rst_n_SHIFT  2
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowpwren_VCO_aux_MASK 0x200
#define RF_2069_PLL_HVLDO2_ldo_2p5_lowpwren_VCO_aux_SHIFT 9
#define RF_2069_PLL_ADC4_rfpll_adc_Idac3_vss_MASK 0x1c0
#define RF_2069_PLL_ADC4_rfpll_adc_Idac3_vss_SHIFT 6
#define RF_2069_PLL_XTAL9_xtal_route_xtfreq2x_en_MASK 0x80
#define RF_2069_PLL_XTAL9_xtal_route_xtfreq2x_en_SHIFT 7
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_adc_gnCntrl_MASK 0x80
#define RF_2069_PLL_DSP43_rfpll_dsp_ovr_adc_gnCntrl_SHIFT 7
#define RF_2069_PLL_ADC3_rfpll_adc_Idac1_vdd_MASK 0xf000
#define RF_2069_PLL_ADC3_rfpll_adc_Idac1_vdd_SHIFT 12
#define RF_2069_TOP_SPARE4_spare4_MASK           0xffff
#define RF_2069_TOP_SPARE4_spare4_SHIFT          0
#define RF_2069_PLL_DSPR27_rfpll_monitor_need_refresh_MASK 0x4
#define RF_2069_PLL_DSPR27_rfpll_monitor_need_refresh_SHIFT 2
#define RF_2069_PLL_XTAL8_xtal_repeater1_pu_MASK 0x8000
#define RF_2069_PLL_XTAL8_xtal_repeater1_pu_SHIFT 15
#define RF_2069_PLL_VCOCAL4_rfpll_vcocal_delayAfterOpenLoop_MASK 0xff
#define RF_2069_PLL_VCOCAL4_rfpll_vcocal_delayAfterOpenLoop_SHIFT 0
#define RF_2069_PLL_CFG6_rfpll_spare6_MASK       0xff00
#define RF_2069_PLL_CFG6_rfpll_spare6_SHIFT      8
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_cal_MASK 0x2000
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_cal_SHIFT 13
#define RF_2069_PLL_CFG6_rfpll_spare7_MASK       0xff
#define RF_2069_PLL_CFG6_rfpll_spare7_SHIFT      0
#define RF_2069_PLL_DSP44_rfpll_dsp_stlTmr_MASK  0xff
#define RF_2069_PLL_DSP44_rfpll_dsp_stlTmr_SHIFT 0
#define RF_2069_PLL_XTAL5_xtal_bufstrg_RCCAL_MASK 0x7
#define RF_2069_PLL_XTAL5_xtal_bufstrg_RCCAL_SHIFT 0
#define RF_2069_PLL_CFG1_rfpll_vco_buf_pu_MASK   0x800
#define RF_2069_PLL_CFG1_rfpll_vco_buf_pu_SHIFT  11
#define RF_2069_PLL_VCOCAL11_rfpll_vcocal_ref_value_MASK 0xfff
#define RF_2069_PLL_VCOCAL11_rfpll_vcocal_ref_value_SHIFT 0
#define RF_2069_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_1p2_MASK 0x7800
#define RF_2069_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_1p2_SHIFT 11
#define RF_2069_PLL_VCOCAL14_rfpll_vcocal_dump_count_reg_MASK 0xff
#define RF_2069_PLL_VCOCAL14_rfpll_vcocal_dump_count_reg_SHIFT 0
#define RF_2069_PLL_DSPR20_rfpll_dsp_dsOut_MASK  0xc000
#define RF_2069_PLL_DSPR20_rfpll_dsp_dsOut_SHIFT 14
#define RF_2069_OVR15_ovr_xtal_pu_core_MASK      0x8
#define RF_2069_OVR15_ovr_xtal_pu_core_SHIFT     3
#define RF_2069_PLL_CFG5_rfpll_spare4_MASK       0xff00
#define RF_2069_PLL_CFG5_rfpll_spare4_SHIFT      8
#define RF_2069_PLL_CFG5_rfpll_spare5_MASK       0xff
#define RF_2069_PLL_CFG5_rfpll_spare5_SHIFT      0
#define RF_2069_PLL_DSPR20_rfpll_dsp_dsOutRd_MASK 0x3000
#define RF_2069_PLL_DSPR20_rfpll_dsp_dsOutRd_SHIFT 12
#define RF_2069_PLL_XTAL4_xtal_doubler_delay_MASK 0xc00
#define RF_2069_PLL_XTAL4_xtal_doubler_delay_SHIFT 10
#define RF_2069_PLL_VCO1_rfpll_vco_EN_DEGEN_ovr_MASK 0x800
#define RF_2069_PLL_VCO1_rfpll_vco_EN_DEGEN_ovr_SHIFT 11
#define RF_2069_PLL_LF1_rfpll_lf_en_vctrl_tp_MASK 0x10
#define RF_2069_PLL_LF1_rfpll_lf_en_vctrl_tp_SHIFT 4
#define RF_2069_PLL_LF3_rfpll_lf_lf_c3_MASK      0xff00
#define RF_2069_PLL_LF3_rfpll_lf_lf_c3_SHIFT     8
#define RF_2069_PLL_LF3_rfpll_lf_lf_c4_MASK      0xff
#define RF_2069_PLL_LF3_rfpll_lf_lf_c4_SHIFT     0
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_ref_val_MASK 0x3
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_ref_val_SHIFT 0
#define RF_2069_OVR15_ovr_rfpll_monitor_pu_MASK  0x2000
#define RF_2069_OVR15_ovr_rfpll_monitor_pu_SHIFT 13
#define RF_2069_OVR14_ovr_pmu_LNLDO_pu_MASK      0x800
#define RF_2069_OVR14_ovr_pmu_LNLDO_pu_SHIFT     11
#define RF_2069_PLL_XTAL4_xtal_outbufstrg_MASK   0x3
#define RF_2069_PLL_XTAL4_xtal_outbufstrg_SHIFT  0
#define RF_2069_PLL_DSPR2_rfpll_dsp_aOut_2_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR2_rfpll_dsp_aOut_2_Rd_SHIFT 0
#define RF_2069_PLL_CFG4_rfpll_spare2_MASK       0xff00
#define RF_2069_PLL_CFG4_rfpll_spare2_SHIFT      8
#define RF_2069_PLL_CFG4_rfpll_spare3_MASK       0xff
#define RF_2069_PLL_CFG4_rfpll_spare3_SHIFT      0
#define RF_2069_PLL_XTAL3_xtal_refsel_MASK       0x700
#define RF_2069_PLL_XTAL3_xtal_refsel_SHIFT      8
#define RF_2069_PLL_VCO8_rfpll_vco_vctrl_buf_ical_MASK 0x7
#define RF_2069_PLL_VCO8_rfpll_vco_vctrl_buf_ical_SHIFT 0
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lms_GnOffstEn_1_MASK 0x8
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lms_GnOffstEn_1_SHIFT 3
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lms_GnOffstEn_2_MASK 0x4
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lms_GnOffstEn_2_SHIFT 2
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_crdcOutSel_1_MASK 0x20
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_crdcOutSel_1_SHIFT 5
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_crdcOutSel_2_MASK 0x10
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_crdcOutSel_2_SHIFT 4
#define RF_2069_PLL_ADC6_rfpll_adc_vcm_ctrl_amp1_MASK 0xe0
#define RF_2069_PLL_ADC6_rfpll_adc_vcm_ctrl_amp1_SHIFT 5
#define RF_2069_PLL_DSPR27_rfpll_dsp_spurFreqSwtch_Rd_MASK 0x8
#define RF_2069_PLL_DSPR27_rfpll_dsp_spurFreqSwtch_Rd_SHIFT 3
#define RF_2069_PLL_CFG3_rfpll_spare0_MASK       0xff00
#define RF_2069_PLL_CFG3_rfpll_spare0_SHIFT      8
#define RF_2069_PLL_ADC6_rfpll_adc_vcm_ctrl_amp2_MASK 0x1c
#define RF_2069_PLL_ADC6_rfpll_adc_vcm_ctrl_amp2_SHIFT 2
#define RF_2069_PLL_CFG3_rfpll_spare1_MASK       0xff
#define RF_2069_PLL_CFG3_rfpll_spare1_SHIFT      0
#define RF_2069_PLL_VCOCAL14_rfpll_vcocal_done_cal_MASK 0x100
#define RF_2069_PLL_VCOCAL14_rfpll_vcocal_done_cal_SHIFT 8
#define RF_2069_PLL_VCO5_rfpll_vco_tmx_mode_MASK 0x3
#define RF_2069_PLL_VCO5_rfpll_vco_tmx_mode_SHIFT 0
#define RF_2069_PLL_DSP6_rfpll_dsp_adc_SpurFreqErr_1_MASK 0x3fff
#define RF_2069_PLL_DSP6_rfpll_dsp_adc_SpurFreqErr_1_SHIFT 0
#define RF_2069_PLL_VCO4_rfpll_vco_por_MASK      0x10
#define RF_2069_PLL_VCO4_rfpll_vco_por_SHIFT     4
#define RF_2069_PLL_LF6_rfpll_lf_cm_pu_MASK      0x1
#define RF_2069_PLL_LF6_rfpll_lf_cm_pu_SHIFT     0
#define RF_2069_PLL_DSPR25_rfpll_dsp_reOut_1_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR25_rfpll_dsp_reOut_1_Rd_SHIFT 0
#define RF_2069_PLL_DSPR26_rfpll_dsp_reOut_2_Rd_MASK 0xffff
#define RF_2069_PLL_DSPR26_rfpll_dsp_reOut_2_Rd_SHIFT 0
#define RF_2069_PLL_VCO2_rfpll_vco_cal_en_empco_MASK 0x20
#define RF_2069_PLL_VCO2_rfpll_vco_cal_en_empco_SHIFT 5
#define RF_2069_PLL_DSPR11_rfpll_dsp_cal_crdc_PhIn_rd_2_MASK 0xffff
#define RF_2069_PLL_DSPR11_rfpll_dsp_cal_crdc_PhIn_rd_2_SHIFT 0
#define RF_2069_PLL_DSP2_rfpll_dsp_SpurFreqErr_2_MASK 0x3fff
#define RF_2069_PLL_DSP2_rfpll_dsp_SpurFreqErr_2_SHIFT 0
#define RF_2069_PLL_XTAL5_xtal_sel_RCCAL_MASK    0x600
#define RF_2069_PLL_XTAL5_xtal_sel_RCCAL_SHIFT   9
#define RF_2069_PLL_LF1_rfpll_lf_extvcbin_MASK   0xf
#define RF_2069_PLL_LF1_rfpll_lf_extvcbin_SHIFT  0
#define RF_2069_PLL_DSP12_rfpll_dsp_adc_regInQ_MASK 0x3fff
#define RF_2069_PLL_DSP12_rfpll_dsp_adc_regInQ_SHIFT 0
#define RF_2069_PLL_DSP33_rfpll_dsp_hys4_MASK    0xf
#define RF_2069_PLL_DSP33_rfpll_dsp_hys4_SHIFT   0
#define RF_2069_OVR22_ovr_xtal_coresize_nmos_MASK 0x400
#define RF_2069_OVR22_ovr_xtal_coresize_nmos_SHIFT 10
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_polarityFlip_MASK 0x4
#define RF_2069_PLL_DSP10_rfpll_dsp_adc_polarityFlip_SHIFT 2
#define RF_2069_PLL_HVLDO1_ldo_2p5_pu_ldo_VCO_MASK 0x10
#define RF_2069_PLL_HVLDO1_ldo_2p5_pu_ldo_VCO_SHIFT 4
#define RF_2069_TOP_SPARE3_spare3_MASK           0xffff
#define RF_2069_TOP_SPARE3_spare3_SHIFT          0
#define RF_2069_PLL_FRCT1_rfpll_frct_mash_sel_MASK 0x2
#define RF_2069_PLL_FRCT1_rfpll_frct_mash_sel_SHIFT 1
#define RF_2069_PLL_FRCT1_rfpll_frct_lsb_sel_MASK 0x4
#define RF_2069_PLL_FRCT1_rfpll_frct_lsb_sel_SHIFT 2
#define RF_2069_PLL_XTAL1_xtal_coresize_pmos_MASK 0x1f
#define RF_2069_PLL_XTAL1_xtal_coresize_pmos_SHIFT 0
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_calSetCount_MASK 0x1ffe
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_calSetCount_SHIFT 1
#define RF_2069_PLL_DSPR7_rfpll_dsp_cal_2_Dn_Rd_MASK 0x1
#define RF_2069_PLL_DSPR7_rfpll_dsp_cal_2_Dn_Rd_SHIFT 0
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_1_MASK 0x7c00
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_1_SHIFT 10
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_2_MASK 0x1f
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_2_SHIFT 0
#define RF_2069_OVR22_ovr_xtal_pu_MASK           0x100
#define RF_2069_OVR22_ovr_xtal_pu_SHIFT          8
#define RF_2069_PLL_ADC3_rfpll_adc_Idac1_vss_MASK 0xf00
#define RF_2069_PLL_ADC3_rfpll_adc_Idac1_vss_SHIFT 8
#define RF_2069_PLL_DSP4_rfpll_dsp_SpurFreq_2_MASK 0x7fff
#define RF_2069_PLL_DSP4_rfpll_dsp_SpurFreq_2_SHIFT 0
#define RF_2069_PLL_DSP42_rfpll_dsp_mmdCntrlIn_MASK 0xffff
#define RF_2069_PLL_DSP42_rfpll_dsp_mmdCntrlIn_SHIFT 0
#define RF_2069_PLL_DSP39_rfpll_dsp_jtag_mmdIn_MASK 0xfff0
#define RF_2069_PLL_DSP39_rfpll_dsp_jtag_mmdIn_SHIFT 4
#define RF_2069_PLL_DSPR7_rfpll_dsp_cal_1_Dn_Rd_MASK 0x2
#define RF_2069_PLL_DSPR7_rfpll_dsp_cal_1_Dn_Rd_SHIFT 1
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_lowquiescenten_MASK 0x20
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_lowquiescenten_SHIFT 5
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_numCapChange_MASK 0x7
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_numCapChange_SHIFT 0
#define RF_2069_PLL_CP1_rfpll_cp_en_up_unit_MASK 0x1e00
#define RF_2069_PLL_CP1_rfpll_cp_en_up_unit_SHIFT 9
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_ctl_MASK 0xf80
#define RF_2069_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_ctl_SHIFT 7
#define RF_2069_PLL_VCO2_rfpll_vco_cap_mode_MASK 0x10
#define RF_2069_PLL_VCO2_rfpll_vco_cap_mode_SHIFT 4
#define RF_2069_PLL_DSPR18_rfpll_dsp_dc_done_Rd_MASK 0x4000
#define RF_2069_PLL_DSPR18_rfpll_dsp_dc_done_Rd_SHIFT 14
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_gear_4_MASK 0x3e00
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_gear_4_SHIFT 9
#define RF_2069_PLL_DSPR24_rfpll_dsp_mmd_ctrl_ce_out_MASK 0x1
#define RF_2069_PLL_DSPR24_rfpll_dsp_mmd_ctrl_ce_out_SHIFT 0
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_gear_8_MASK 0x1f0
#define RF_2069_PLL_VCOCAL9_rfpll_vcocal_gear_8_SHIFT 4
#define RF_2069_PLL_LF7_rfpll_lf_lf_rs_cm_MASK   0xff
#define RF_2069_PLL_LF7_rfpll_lf_lf_rs_cm_SHIFT  0
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_artload_MASK 0x1
#define RF_2069_PLL_HVLDO2_ldo_2p5_ldo_VCO_artload_SHIFT 0
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_pll_val_MASK 0x3ffc
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_pll_val_SHIFT 2
#define RF_2069_PLL_VCO3_rfpll_vco_ib_ctrl_1p2_MASK 0xf
#define RF_2069_PLL_VCO3_rfpll_vco_ib_ctrl_1p2_SHIFT 0
#define RF_2069_OVR16_ovr_rfpll_vcocal_rstn_MASK 0x800
#define RF_2069_OVR16_ovr_rfpll_vcocal_rstn_SHIFT 11
#define RF_2069_PLL_HVLDO1_ldo_2p5_pu_ldo_CP_MASK 0x20
#define RF_2069_PLL_HVLDO1_ldo_2p5_pu_ldo_CP_SHIFT 5
#define RF_2069_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovr_MASK 0x400
#define RF_2069_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovr_SHIFT 10
#define RF_2069_PLL_CFG1_rfpll_pfd_cal_rstb_MASK 0x80
#define RF_2069_PLL_CFG1_rfpll_pfd_cal_rstb_SHIFT 7
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_16_MASK 0x3e0
#define RF_2069_PLL_VCOCAL8_rfpll_vcocal_gear_16_SHIFT 5
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lmsPhOffstEn_1_MASK 0x2
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lmsPhOffstEn_1_SHIFT 1
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lmsPhOffstEn_2_MASK 0x1
#define RF_2069_PLL_DSP16_rfpll_dsp_cal_lmsPhOffstEn_2_SHIFT 0
#define RF_2069_OVR14_ovr_pmu_RXldo_pu_MASK      0x400
#define RF_2069_OVR14_ovr_pmu_RXldo_pu_SHIFT     10
#define RF_2069_PLL_VCOCAL6_rfpll_vcocal_enableTimeOut_MASK 0x3e
#define RF_2069_PLL_VCOCAL6_rfpll_vcocal_enableTimeOut_SHIFT 1
#define RF_2069_PLL_DSP27_rfpll_dsp_hys1_MASK    0xf
#define RF_2069_PLL_DSP27_rfpll_dsp_hys1_SHIFT   0
#define RF_2069_PLL_MONITOR1_rfpll_monitor_vth_low_MASK 0xf
#define RF_2069_PLL_MONITOR1_rfpll_monitor_vth_low_SHIFT 0
#define RF_2069_OVR14_ovr_pmu_VCOldo_pu_MASK     0x80
#define RF_2069_OVR14_ovr_pmu_VCOldo_pu_SHIFT    7
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_caps_sel_MASK 0xe00
#define RF_2069_PLL_VCOCAL2_rfpll_vcocal_cal_caps_sel_SHIFT 9
#define RF_2069_PLL_HVLDO2_ldo_2p5_pu_ldo_VCO_aux_MASK 0x800
#define RF_2069_PLL_HVLDO2_ldo_2p5_pu_ldo_VCO_aux_SHIFT 11
#define RF_2069_PLL_VCO2_rfpll_vco_ICAL_1p2_MASK 0x1f00
#define RF_2069_PLL_VCO2_rfpll_vco_ICAL_1p2_SHIFT 8
#define RF_2069_PLL_DSPR19_rfpll_dsp_dsInRd_MASK 0xffff
#define RF_2069_PLL_DSPR19_rfpll_dsp_dsInRd_SHIFT 0
#define RF_2069_PLL_XTAL2_xtal_pu_pfddrv_MASK    0x1
#define RF_2069_PLL_XTAL2_xtal_pu_pfddrv_SHIFT   0
#define RF_2069_PLL_XTAL5_xtal_bufstrg_RCCAL1_MASK 0x38
#define RF_2069_PLL_XTAL5_xtal_bufstrg_RCCAL1_SHIFT 3
#define RF_2069_PLL_DSPR23_rfpll_dsp_magOut_1_Rd_MASK 0xff00
#define RF_2069_PLL_DSPR23_rfpll_dsp_magOut_1_Rd_SHIFT 8
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_ovr_mode_MASK 0x4000
#define RF_2069_PLL_VCOCAL10_rfpll_vcocal_ovr_mode_SHIFT 14
#define RF_2069_PLL_CP1_rfpll_cp_pu_MASK         0x2000
#define RF_2069_PLL_CP1_rfpll_cp_pu_SHIFT        13
#define RF_2069_PLL_XTAL2_xtal_pu_caldrv_MASK    0x10
#define RF_2069_PLL_XTAL2_xtal_pu_caldrv_SHIFT   4
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_calSetCountOvr_MASK 0x1
#define RF_2069_PLL_VCOCAL1_rfpll_vcocal_calSetCountOvr_SHIFT 0
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_dcadj_MASK 0x78
#define RF_2069_PLL_VCO5_rfpll_vco_tempco_dcadj_SHIFT 3
#define RF_2069_PLL_DSPR14_rfpll_dsp_chrgPmpScl_Rd_MASK 0xff
#define RF_2069_PLL_DSPR14_rfpll_dsp_chrgPmpScl_Rd_SHIFT 0
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_test_gpaio_MASK 0x20
#define RF_2069_PLL_VCOCAL13_rfpll_vcocal_test_gpaio_SHIFT 5

/* GE REV16 MASK and SHIFTS */
#define RF_2069_GE16_RXMIX5G_CFG1_gm_size_MASK        0x700
#define RF_2069_GE16_RXMIX5G_CFG1_gm_size_SHIFT       8
#define RF_2069_GE16_OVR10_ovr_logen5g_reset_rx_MASK  0x400
#define RF_2069_GE16_OVR10_ovr_logen5g_reset_rx_SHIFT 10
#define RF_2069_GE16_OVR3_ovr_afe_DAC_att_MASK        0x1000
#define RF_2069_GE16_OVR3_ovr_afe_DAC_att_SHIFT       12
#define RF_2069_GE16_LNA2G_CFG2_lna2_Rout_MASK        0xf0
#define RF_2069_GE16_LNA2G_CFG2_lna2_Rout_SHIFT       4
#define RF_2069_GE16_PA2G_CFG3_pa2g_ptat_slope_main_MASK 0xf
#define RF_2069_GE16_PA2G_CFG3_pa2g_ptat_slope_main_SHIFT 0
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_reset_MASK   0x8
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_reset_SHIFT  3
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refmid_puI_MASK 0x4
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refmid_puI_SHIFT 2
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Reflow_puQ_MASK 0x20
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Reflow_puQ_SHIFT 5
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_gpaio_en_MASK 0x10
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_gpaio_en_SHIFT 4
#define RF_2069_GE16_TIA_CFG2_opamp_iout_x2_MASK      0x8
#define RF_2069_GE16_TIA_CFG2_opamp_iout_x2_SHIFT     3
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_cmref_0p7_use_adc_MASK 0x40
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_cmref_0p7_use_adc_SHIFT 6
#define RF_2069_GE16_ADC_BIAS1_adc_bias_MASK          0xffff
#define RF_2069_GE16_ADC_BIAS1_adc_bias_SHIFT         0
#define RF_2069_GE16_CORE_BIAS_bias_tx_pu_MASK        0x1
#define RF_2069_GE16_CORE_BIAS_bias_tx_pu_SHIFT       0
#define RF_2069_GE16_TX2G_CFG2_notch2g_tune_MASK      0xf
#define RF_2069_GE16_TX2G_CFG2_notch2g_tune_SHIFT     0
#define RF_2069_GE16_PGA5G_CFG1_gc_MASK               0xff
#define RF_2069_GE16_PGA5G_CFG1_gc_SHIFT              0
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_cm_mult_MASK 0xc
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_cm_mult_SHIFT 2
#define RF_2069_GE16_WRSSI3_BIAS_wrssi3_misc_MASK     0xff
#define RF_2069_GE16_WRSSI3_BIAS_wrssi3_misc_SHIFT    0
#define RF_2069_GE16_ADC_CALCODE2_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE2_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_cm_mult_MASK 0x30
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_cm_mult_SHIFT 4
#define RF_2069_GE16_TIA_CFG1_PU_bias_MASK            0x4
#define RF_2069_GE16_TIA_CFG1_PU_bias_SHIFT           2
#define RF_2069_GE16_RXMIX5G_CFG1_pu_MASK             0x1
#define RF_2069_GE16_RXMIX5G_CFG1_pu_SHIFT            0
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_cascode_MASK 0x700
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_cascode_SHIFT 8
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flashhspd_MASK 0x4
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flashhspd_SHIFT 2
#define RF_2069_GE16_OVR25_ovr_lna5g_lna2_main_bias_idac_MASK 0x400
#define RF_2069_GE16_OVR25_ovr_lna5g_lna2_main_bias_idac_SHIFT 10
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refmid_puQ_MASK 0x8
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refmid_puQ_SHIFT 3
#define RF_2069_GE16_GPAIO_SEL1_sel_16to31_port_MASK  0xffff
#define RF_2069_GE16_GPAIO_SEL1_sel_16to31_port_SHIFT 0
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_mode_MASK     0x100
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_mode_SHIFT    8
#define RF_2069_GE16_ADC_CALCODE14_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE14_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_ADC_CFG3_ctl_flash_MASK          0x3f
#define RF_2069_GE16_ADC_CFG3_ctl_flash_SHIFT         0
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_gpaio_en_MASK 0x10
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_gpaio_en_SHIFT 4
#define RF_2069_GE16_OVR3_ovr_afe_auxpga_i_sel_vmid_MASK 0x2000
#define RF_2069_GE16_OVR3_ovr_afe_auxpga_i_sel_vmid_SHIFT 13
#define RF_2069_GE16_OVR21_ovr_pa2g_ctrl_tssi_sel_MASK 0x4000
#define RF_2069_GE16_OVR21_ovr_pa2g_ctrl_tssi_sel_SHIFT 14
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_sel_test_MASK 0xf00
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_sel_test_SHIFT 8
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refmid_puQ_MASK 0x8
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refmid_puQ_SHIFT 3
#define RF_2069_GE16_TXGM_CFG1_pu_MASK                0x1
#define RF_2069_GE16_TXGM_CFG1_pu_SHIFT               0
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq2_gain_MASK 0x7000
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq2_gain_SHIFT 12
#define RF_2069_GE16_PAD5G_INCAP_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_GE16_PAD5G_INCAP_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_GE16_PA2G_INCAP_pa2g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_GE16_PA2G_INCAP_pa2g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_pwrup_Qch_MASK 0x4000
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_pwrup_Qch_SHIFT 14
#define RF_2069_GE16_OVR13_ovr_pga5g_gc_MASK          0x1000
#define RF_2069_GE16_OVR13_ovr_pga5g_gc_SHIFT         12
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_sel_input_MASK 0xe
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_sel_input_SHIFT 1
#define RF_2069_GE16_PGA2G_CFG1_pga2g_gainboost_MASK  0xf000
#define RF_2069_GE16_PGA2G_CFG1_pga2g_gainboost_SHIFT 12
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_irx_MASK  0x10
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_irx_SHIFT 4
#define RF_2069_GE16_OVR24_ovr_nbrssi_Reflow_puI_MASK 0x400
#define RF_2069_GE16_OVR24_ovr_nbrssi_Reflow_puI_SHIFT 10
#define RF_2069_GE16_OVR20_ovr_pa2g_ctrl_tssi_MASK    0x4
#define RF_2069_GE16_OVR20_ovr_pa2g_ctrl_tssi_SHIFT   2
#define RF_2069_GE16_TXGM_LOFT_COARSE_I_idac_lo_rf_in_MASK 0xf0
#define RF_2069_GE16_TXGM_LOFT_COARSE_I_idac_lo_rf_in_SHIFT 4
#define RF_2069_GE16_PA2G_CFG3_pa2g_sel_bias_type_MASK 0xff00
#define RF_2069_GE16_PA2G_CFG3_pa2g_sel_bias_type_SHIFT 8
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_gain_MASK    0x200
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_gain_SHIFT   9
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_ov_det_MASK 0x800
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_ov_det_SHIFT 11
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_threshold_MASK 0xf000
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_threshold_SHIFT 12
#define RF_2069_GE16_TIA_CFG1_HPC_MASK                0xe0
#define RF_2069_GE16_TIA_CFG1_HPC_SHIFT               5
#define RF_2069_GE16_OVR10_ovr_lpf_sw_aux_bq1_MASK    0x20
#define RF_2069_GE16_OVR10_ovr_lpf_sw_aux_bq1_SHIFT   5
#define RF_2069_GE16_ADC_CFG4_Ich_reset_MASK          0x20
#define RF_2069_GE16_ADC_CFG4_Ich_reset_SHIFT         5
#define RF_2069_GE16_LNA2G_CFG1_lna1_bypass_MASK      0x20
#define RF_2069_GE16_LNA2G_CFG1_lna1_bypass_SHIFT     5
#define RF_2069_GE16_LNA2G_TUNE_lna1_Cgs_size_MASK    0x3000
#define RF_2069_GE16_LNA2G_TUNE_lna1_Cgs_size_SHIFT   12
#define RF_2069_GE16_TXGM_LOFT_COARSE_I_idac_lo_rf_ip_MASK 0xf
#define RF_2069_GE16_TXGM_LOFT_COARSE_I_idac_lo_rf_ip_SHIFT 0
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_qp_gpaio_MASK 0x40
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_qp_gpaio_SHIFT 6
#define RF_2069_GE16_TX5G_CFG1_bias_pu_MASK           0x2
#define RF_2069_GE16_TX5G_CFG1_bias_pu_SHIFT          1
#define RF_2069_GE16_PAD5G_INCAP_idac_incap_compen_main_MASK 0xf
#define RF_2069_GE16_PAD5G_INCAP_idac_incap_compen_main_SHIFT 0
#define RF_2069_GE16_OVR20_ovr_tia_sel_5G_2G_MASK     0x80
#define RF_2069_GE16_OVR20_ovr_tia_sel_5G_2G_SHIFT    7
#define RF_2069_GE16_PGA5G_INCAP_ptat_slope_incap_compen_MASK 0x70
#define RF_2069_GE16_PGA5G_INCAP_ptat_slope_incap_compen_SHIFT 4
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_filter_main_MASK 0xf
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_filter_main_SHIFT 0
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_cas_pu_MASK  0x4
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_cas_pu_SHIFT 2
#define RF_2069_GE16_OVR14_ovr_pga2g_gc_MASK          0x4
#define RF_2069_GE16_OVR14_ovr_pga2g_gc_SHIFT         2
#define RF_2069_GE16_OVR10_ovr_logen5g_reset_tx_MASK  0x200
#define RF_2069_GE16_OVR10_ovr_logen5g_reset_tx_SHIFT 9
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_irx_MASK      0x7
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_irx_SHIFT     0
#define RF_2069_GE16_ADC_CFG4_ctrl_iqadc_mode_MASK    0x3800
#define RF_2069_GE16_ADC_CFG4_ctrl_iqadc_mode_SHIFT   11
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_7_5_MASK      0xe0
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_7_5_SHIFT     5
#define RF_2069_GE16_LOGEN5G_CFG2_reset_rx_MASK       0x8
#define RF_2069_GE16_LOGEN5G_CFG2_reset_rx_SHIFT      3
#define RF_2069_GE16_LNA5G_CFG3_lna5g_kill_sw_size_MASK 0xf
#define RF_2069_GE16_LNA5G_CFG3_lna5g_kill_sw_size_SHIFT 0
#define RF_2069_GE16_RXRF5G_CFG1_globe_idac_MASK      0x7f0
#define RF_2069_GE16_RXRF5G_CFG1_globe_idac_SHIFT     4
#define RF_2069_GE16_PA5G_IDAC2_pa5g_biasa_main_MASK  0xff
#define RF_2069_GE16_PA5G_IDAC2_pa5g_biasa_main_SHIFT 0
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_cas_MASK     0xff00
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_cas_SHIFT    8
#define RF_2069_GE16_PA2G_TSSI_pa2g_ctrl_tssi_sel_MASK 0x4
#define RF_2069_GE16_PA2G_TSSI_pa2g_ctrl_tssi_sel_SHIFT 2
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refmid_puI_MASK 0x100
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refmid_puI_SHIFT 8
#define RF_2069_GE16_OVR24_ovr_nbrssi_Reflow_puQ_MASK 0x800
#define RF_2069_GE16_OVR24_ovr_nbrssi_Reflow_puQ_SHIFT 11
#define RF_2069_GE16_OVR20_ovr_pa2g_tssi_pu_MASK      0x2
#define RF_2069_GE16_OVR20_ovr_pa2g_tssi_pu_SHIFT     1
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refhigh_puI_MASK 0x1
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refhigh_puI_SHIFT 0
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq2_bw_MASK 0x38
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq2_bw_SHIFT 3
#define RF_2069_GE16_TXGM_CFG1_sel5g_MASK             0x2
#define RF_2069_GE16_TXGM_CFG1_sel5g_SHIFT            1
#define RF_2069_GE16_TIA_CFG2_BW_MASK                 0xc000
#define RF_2069_GE16_TIA_CFG2_BW_SHIFT                14
#define RF_2069_GE16_RXRF2G_SPARE_spare_MASK          0xff
#define RF_2069_GE16_RXRF2G_SPARE_spare_SHIFT         0
#define RF_2069_GE16_OVR4_ovr_iq_cal_pu_tssi_MASK     0x2
#define RF_2069_GE16_OVR4_ovr_iq_cal_pu_tssi_SHIFT    1
#define RF_2069_GE16_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_MASK 0xff
#define RF_2069_GE16_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_SHIFT 0
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_pu_MASK    0x100
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_pu_SHIFT   8
#define RF_2069_GE16_PGA2G_INCAP_pga2g_ptat_slope_incap_compen_MASK 0x70
#define RF_2069_GE16_PGA2G_INCAP_pga2g_ptat_slope_incap_compen_SHIFT 4
#define RF_2069_GE16_OVR20_ovr_tia_pu_MASK            0x200
#define RF_2069_GE16_OVR20_ovr_tia_pu_SHIFT           9
#define RF_2069_GE16_ADC_CFG4_Qch_reset_MASK          0x10
#define RF_2069_GE16_ADC_CFG4_Qch_reset_SHIFT         4
#define RF_2069_GE16_LNA5G_CFG2_lna2_pu_MASK          0x1
#define RF_2069_GE16_LNA5G_CFG2_lna2_pu_SHIFT         0
#define RF_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_0p7_control_MASK 0xff
#define RF_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_0p7_control_SHIFT 0
#define RF_2069_GE16_CLB_REG_1_clb_sel_MASK           0x3f
#define RF_2069_GE16_CLB_REG_1_clb_sel_SHIFT          0
#define RF_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_half_vdd_control_MASK 0xff00
#define RF_2069_GE16_LPF_COMMON_MODE_REFERENCE_LEVELS_lpf_cmref_half_vdd_control_SHIFT 8
#define RF_2069_GE16_TX2G_CFG1_txmix2g_pu_MASK        0x1
#define RF_2069_GE16_TX2G_CFG1_txmix2g_pu_SHIFT       0
#define RF_2069_GE16_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_rc_MASK 0xff00
#define RF_2069_GE16_LPF_CORNER_FREQUENCY_TUNING_lpf_g_mult_rc_SHIFT 8
#define RF_2069_GE16_ADC_CALCODE7_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE7_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_IQCAL_CFG3_gvar_MASK             0x70
#define RF_2069_GE16_IQCAL_CFG3_gvar_SHIFT            4
#define RF_2069_GE16_PAD2G_CFG1_pad2g_gc_MASK         0xff00
#define RF_2069_GE16_PAD2G_CFG1_pad2g_gc_SHIFT        8
#define RF_2069_GE16_READOVERRIDES_read_overrides_MASK 0x1
#define RF_2069_GE16_READOVERRIDES_read_overrides_SHIFT 0
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_4_0_MASK      0x1f
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_4_0_SHIFT     0
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_trsw_source_bis_MASK 0x1e00
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_trsw_source_bis_SHIFT 9
#define RF_2069_GE16_OVR20_ovr_tx2g_bias_pu_MASK      0x20
#define RF_2069_GE16_OVR20_ovr_tx2g_bias_pu_SHIFT     5
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refmid_puQ_MASK 0x200
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refmid_puQ_SHIFT 9
#define RF_2069_GE16_OVR20_ovr_tia_HPC_MASK           0x400
#define RF_2069_GE16_OVR20_ovr_tia_HPC_SHIFT          10
#define RF_2069_GE16_OVR14_ovr_pa5g_bias_reset_MASK   0x1000
#define RF_2069_GE16_OVR14_ovr_pa5g_bias_reset_SHIFT  12
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refhigh_puQ_MASK 0x2
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refhigh_puQ_SHIFT 1
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_aux_bq1_MASK 0x1
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_aux_bq1_SHIFT 0
#define RF_2069_GE16_LNA2G_CFG1_lna1_low_ct_MASK      0x2
#define RF_2069_GE16_LNA2G_CFG1_lna1_low_ct_SHIFT     1
#define RF_2069_GE16_VBAT_CFG_monitor_pu_MASK         0x1
#define RF_2069_GE16_VBAT_CFG_monitor_pu_SHIFT        0
#define RF_2069_GE16_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_GE16_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_pu_MASK     0x1
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_pu_SHIFT    0
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_itx_MASK  0x40
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_itx_SHIFT 6
#define RF_2069_GE16_DAC_CFG2_DACbuf_Cap_MASK         0xf8
#define RF_2069_GE16_DAC_CFG2_DACbuf_Cap_SHIFT        3
#define RF_2069_GE16_OVR5_ovr_tempsense_pu_MASK       0x4000
#define RF_2069_GE16_OVR5_ovr_tempsense_pu_SHIFT      14
#define RF_2069_GE16_TEMPSENSE_CFG_swap_amp_MASK      0x4
#define RF_2069_GE16_TEMPSENSE_CFG_swap_amp_SHIFT     2
#define RF_2069_GE16_PAD5G_CFG1_gc_MASK               0xff00
#define RF_2069_GE16_PAD5G_CFG1_gc_SHIFT              8
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_Rout_MASK    0x40
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_Rout_SHIFT   6
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_cas_pu_MASK  0x40
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_cas_pu_SHIFT 6
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq1_adc_MASK    0x8
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq1_adc_SHIFT   3
#define RF_2069_GE16_PA5G_CFG1_pa5g_gain_ctrl_MASK    0xff00
#define RF_2069_GE16_PA5G_CFG1_pa5g_gain_ctrl_SHIFT   8
#define RF_2069_GE16_OVR22_ovr_afe_DACbuf_Cap_MASK    0x4000
#define RF_2069_GE16_OVR22_ovr_afe_DACbuf_Cap_SHIFT   14
#define RF_2069_GE16_OVR21_ovr_txgm_pu_MASK           0x40
#define RF_2069_GE16_OVR21_ovr_txgm_pu_SHIFT          6
#define RF_2069_GE16_OVR20_ovr_trsw2g_pu_MASK         0x40
#define RF_2069_GE16_OVR20_ovr_trsw2g_pu_SHIFT        6
#define RF_2069_GE16_OVR20_ovr_tia_GainI_MASK         0x1000
#define RF_2069_GE16_OVR20_ovr_tia_GainI_SHIFT        12
#define RF_2069_GE16_PA5G_CFG1_pa5g_gpio_sw_pu_MASK   0x10
#define RF_2069_GE16_PA5G_CFG1_pa5g_gpio_sw_pu_SHIFT  4
#define RF_2069_GE16_OVR11_ovr_lpf_bq2_bw_MASK        0x8
#define RF_2069_GE16_OVR11_ovr_lpf_bq2_bw_SHIFT       3
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_rc_pu_MASK 0x40
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_rc_pu_SHIFT 6
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_itx_MASK      0x700
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_itx_SHIFT     8
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_dc_i_MASK     0x20
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_dc_i_SHIFT    5
#define RF_2069_GE16_DAC_CFG1_DAC_pd_partial_MASK     0x4000
#define RF_2069_GE16_DAC_CFG1_DAC_pd_partial_SHIFT    14
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_outsel_MASK   0xf0
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_outsel_SHIFT  4
#define RF_2069_GE16_PA5G_IDAC2_pa5g_biasa_aux_MASK   0xff00
#define RF_2069_GE16_PA5G_IDAC2_pa5g_biasa_aux_SHIFT  8
#define RF_2069_GE16_LOGEN5G_CFG2_reset_tx_MASK       0x4
#define RF_2069_GE16_LOGEN5G_CFG2_reset_tx_SHIFT      2
#define RF_2069_GE16_ADC_CALCODE28_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE28_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_LNA2G_CFG3_lna2g_kill_sw_size_MASK 0xf
#define RF_2069_GE16_LNA2G_CFG3_lna2g_kill_sw_size_SHIFT 0
#define RF_2069_GE16_TIA_IDAC1_DC_Ib1_MASK            0xf000
#define RF_2069_GE16_TIA_IDAC1_DC_Ib1_SHIFT           12
#define RF_2069_GE16_OVR12_ovr_lpf_g_mult_MASK        0x2
#define RF_2069_GE16_OVR12_ovr_lpf_g_mult_SHIFT       1
#define RF_2069_GE16_ADC_CALCODE21_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE21_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_TIA_IDAC1_DC_Ib2_MASK            0xf00
#define RF_2069_GE16_TIA_IDAC1_DC_Ib2_SHIFT           8
#define RF_2069_GE16_REV_ID_rev_id_MASK               0xff
#define RF_2069_GE16_REV_ID_rev_id_SHIFT              0
#define RF_2069_GE16_LNA2G_CFG1_lna1_out_short_pu_MASK 0x4
#define RF_2069_GE16_LNA2G_CFG1_lna1_out_short_pu_SHIFT 2
#define RF_2069_GE16_RXRF2G_CFG2_lna2g_epapd_en_MASK  0x10
#define RF_2069_GE16_RXRF2G_CFG2_lna2g_epapd_en_SHIFT 4
#define RF_2069_GE16_PGA2G_CFG1_gc_MASK               0xff
#define RF_2069_GE16_PGA2G_CFG1_gc_SHIFT              0
#define RF_2069_GE16_OVR7_ovr_lna5g_lna2_pu_MASK      0x100
#define RF_2069_GE16_OVR7_ovr_lna5g_lna2_pu_SHIFT     8
#define RF_2069_GE16_OVR13_ovr_pa2g_2gtx_pu_MASK      0x4
#define RF_2069_GE16_OVR13_ovr_pa2g_2gtx_pu_SHIFT     2
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_cal_pu_MASK 0x200
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_cal_pu_SHIFT 9
#define RF_2069_GE16_OVR20_ovr_tia_GainQ_MASK         0x800
#define RF_2069_GE16_OVR20_ovr_tia_GainQ_SHIFT        11
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_cal_MASK 0x8
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_cal_SHIFT 3
#define RF_2069_GE16_TXGM_LOFT_SCALE_idac_lo_bb_diode_MASK 0xc
#define RF_2069_GE16_TXGM_LOFT_SCALE_idac_lo_bb_diode_SHIFT 2
#define RF_2069_GE16_PAD5G_INCAP_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_GE16_PAD5G_INCAP_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_GE16_PA2G_INCAP_pa2g_idac_incap_compen_main_MASK 0xf
#define RF_2069_GE16_PA2G_INCAP_pa2g_idac_incap_compen_main_SHIFT 0
#define RF_2069_GE16_CLB_REG_0_clb_out_MASK           0xffff
#define RF_2069_GE16_CLB_REG_0_clb_out_SHIFT          0
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_Ich_cal_state_MASK 0x2
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_Ich_cal_state_SHIFT 1
#define RF_2069_GE16_RXMIX2G_CFG1_pu_MASK             0x1
#define RF_2069_GE16_RXMIX2G_CFG1_pu_SHIFT            0
#define RF_2069_GE16_OVR25_ovr_rxmix5g_gm_aux_bias_idac_i_MASK 0x80
#define RF_2069_GE16_OVR25_ovr_rxmix5g_gm_aux_bias_idac_i_SHIFT 7
#define RF_2069_GE16_TXMIX5G_CFG1_gc_load_MASK        0xf
#define RF_2069_GE16_TXMIX5G_CFG1_gc_load_SHIFT       0
#define RF_2069_GE16_PA2G_CFG1_pa2g_gpio_sw_pu_MASK   0x10
#define RF_2069_GE16_PA2G_CFG1_pa2g_gpio_sw_pu_SHIFT  4
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC13_MASK 0x200
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC13_SHIFT 9
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_dc_q_MASK     0x40
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_dc_q_SHIFT    6
#define RF_2069_GE16_TIA_CFG2_Vcmref_in_MASK          0x70
#define RF_2069_GE16_TIA_CFG2_Vcmref_in_SHIFT         4
#define RF_2069_GE16_OVR5_ovr_bias_tx_pu_MASK         0x400
#define RF_2069_GE16_OVR5_ovr_bias_tx_pu_SHIFT        10
#define RF_2069_GE16_OVR22_ovr_rxrf2g_pwrsw_en_MASK   0x8
#define RF_2069_GE16_OVR22_ovr_rxrf2g_pwrsw_en_SHIFT  3
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_qn_gpaio_MASK 0x80
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_qn_gpaio_SHIFT 7
#define RF_2069_GE16_LOGEN5G_CFG2_rx_pu_MASK          0x1
#define RF_2069_GE16_LOGEN5G_CFG2_rx_pu_SHIFT         0
#define RF_2069_GE16_TXMIX2G_CFG1_gainboost_MASK      0xf000
#define RF_2069_GE16_TXMIX2G_CFG1_gainboost_SHIFT     12
#define RF_2069_GE16_OVR19_ovr_rxrf5g_pu_pulse_MASK   0x8
#define RF_2069_GE16_OVR19_ovr_rxrf5g_pu_pulse_SHIFT  3
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq1_adc_MASK 0x2
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq1_adc_SHIFT 1
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_pu_MASK 0x80
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_pu_SHIFT 7
#define RF_2069_GE16_OVR21_ovr_pa5g_TSSI_range_MASK   0x200
#define RF_2069_GE16_OVR21_ovr_pa5g_TSSI_range_SHIFT  9
#define RF_2069_GE16_GPAIO_SEL0_sel_0to15_port_MASK   0xffff
#define RF_2069_GE16_GPAIO_SEL0_sel_0to15_port_SHIFT  0
#define RF_2069_GE16_LNA2G_TUNE_lna1_freq_tune_MASK   0xf
#define RF_2069_GE16_LNA2G_TUNE_lna1_freq_tune_SHIFT  0
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_gain_MASK 0x300
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_gain_SHIFT 8
#define RF_2069_GE16_GPAIO_CFG1_gpaio_pu_MASK         0x1
#define RF_2069_GE16_GPAIO_CFG1_gpaio_pu_SHIFT        0
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_lpf_MASK 0x2
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_lpf_SHIFT 1
#define RF_2069_GE16_PAD2G_IDAC_pad2g_idac_cascode_MASK 0xf00
#define RF_2069_GE16_PAD2G_IDAC_pad2g_idac_cascode_SHIFT 8
#define RF_2069_GE16_PA5G_CFG1_pa5g_5gtx_pu_MASK      0x1
#define RF_2069_GE16_PA5G_CFG1_pa5g_5gtx_pu_SHIFT     0
#define RF_2069_GE16_ADC_CFG3_Ich_scram_off_MASK      0x4000
#define RF_2069_GE16_ADC_CFG3_Ich_scram_off_SHIFT     14
#define RF_2069_GE16_ADC_CALCODE25_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE25_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_Qch_cal_state_MASK 0x1
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_Qch_cal_state_SHIFT 0
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_cal_pu_MASK 0x200
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_cal_pu_SHIFT 9
#define RF_2069_GE16_PA5G_INCAP_pa5g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_GE16_PA5G_INCAP_pa5g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC21_MASK 0x100
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC21_SHIFT 8
#define RF_2069_GE16_OVR21_ovr_pa5g_ctrl_tssi_sel_MASK 0x100
#define RF_2069_GE16_OVR21_ovr_pa5g_ctrl_tssi_sel_SHIFT 8
#define RF_2069_GE16_ADC_CALCODE17_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE17_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_ADC_CALCODE1_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE1_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_PA2G_CFG1_trsw2g_pu_MASK         0x20
#define RF_2069_GE16_PA2G_CFG1_trsw2g_pu_SHIFT        5
#define RF_2069_GE16_PA2G_INCAP_pa2g_ptat_slope_incap_compen_aux_MASK 0x7000
#define RF_2069_GE16_PA2G_INCAP_pa2g_ptat_slope_incap_compen_aux_SHIFT 12
#define RF_2069_GE16_OVR3_ovr_afe_DAC_pwrup_MASK      0x800
#define RF_2069_GE16_OVR3_ovr_afe_DAC_pwrup_SHIFT     11
#define RF_2069_GE16_OVR21_ovr_trsw5g_pu_MASK         0x8000
#define RF_2069_GE16_OVR21_ovr_trsw5g_pu_SHIFT        15
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_lna12_mux_MASK 0x1
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_lna12_mux_SHIFT 0
#define RF_2069_GE16_TIA_CFG2_Vcmref_out_MASK         0x7
#define RF_2069_GE16_TIA_CFG2_Vcmref_out_SHIFT        0
#define RF_2069_GE16_PAD2G_TUNE_pad2g_idac_tuning_bias_MASK 0xf0
#define RF_2069_GE16_PAD2G_TUNE_pad2g_idac_tuning_bias_SHIFT 4
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC24_MASK 0x80
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC24_SHIFT 7
#define RF_2069_GE16_ADC_CALCODE13_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE13_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_ff_mult_MASK 0x300
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq2_ff_mult_SHIFT 8
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refhigh_puI_MASK 0x1
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refhigh_puI_SHIFT 0
#define RF_2069_GE16_ADC_CFG4_Qch_calmode_MASK        0x4
#define RF_2069_GE16_ADC_CFG4_Qch_calmode_SHIFT       2
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_17_16_MASK   0x3
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_17_16_SHIFT  0
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_powersaving_MASK 0x8000
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_powersaving_SHIFT 15
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_ff_mult_MASK 0xc00
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_dc_ff_mult_SHIFT 10
#define RF_2069_GE16_ADC_CFG4_ctrl_flashhspd_MASK     0x200
#define RF_2069_GE16_ADC_CFG4_ctrl_flashhspd_SHIFT    9
#define RF_2069_GE16_OVR25_ovr_rxmix5g_gm_main_bias_idac_i_MASK 0x40
#define RF_2069_GE16_OVR25_ovr_rxmix5g_gm_main_bias_idac_i_SHIFT 6
#define RF_2069_GE16_ADC_CFG3_Qch_scram_off_MASK      0x8000
#define RF_2069_GE16_ADC_CFG3_Qch_scram_off_SHIFT     15
#define RF_2069_GE16_TXGM_CFG2_ctrl_MASK              0x60
#define RF_2069_GE16_TXGM_CFG2_ctrl_SHIFT             5
#define RF_2069_GE16_PGA2G_IDAC_pga2g_idac_main_MASK  0x3f
#define RF_2069_GE16_PGA2G_IDAC_pga2g_idac_main_SHIFT 0
#define RF_2069_GE16_OVR10_ovr_logen5g_tx_pu_MASK     0x80
#define RF_2069_GE16_OVR10_ovr_logen5g_tx_pu_SHIFT    7
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq1_bw_MASK 0x7
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq1_bw_SHIFT 0
#define RF_2069_GE16_OVR14_ovr_pad5g_bias_filter_bypass_MASK 0x40
#define RF_2069_GE16_OVR14_ovr_pad5g_bias_filter_bypass_SHIFT 6
#define RF_2069_GE16_DAC_CFG1_DAC_scram_off_MASK      0x300
#define RF_2069_GE16_DAC_CFG1_DAC_scram_off_SHIFT     8
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_pu_MASK    0x1
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_pu_SHIFT   0
#define RF_2069_GE16_DAC_CFG1_DAC_att_MASK            0xf0
#define RF_2069_GE16_DAC_CFG1_DAC_att_SHIFT           4
#define RF_2069_GE16_ADC_CFG2_iqadc_clamp_en_MASK     0x4
#define RF_2069_GE16_ADC_CFG2_iqadc_clamp_en_SHIFT    2
#define RF_2069_GE16_PAD5G_TUNE_idac_aux_MASK         0x3f00
#define RF_2069_GE16_PAD5G_TUNE_idac_aux_SHIFT        8
#define RF_2069_GE16_OVR13_ovr_lpf_dc_bypass_MASK     0x2000
#define RF_2069_GE16_OVR13_ovr_lpf_dc_bypass_SHIFT    13
#define RF_2069_GE16_ADC_CALCODE22_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE22_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refhigh_puQ_MASK 0x2
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refhigh_puQ_SHIFT 1
#define RF_2069_GE16_TXRX2G_CAL_TX_i_calPath_pa2g_pu_MASK 0x20
#define RF_2069_GE16_TXRX2G_CAL_TX_i_calPath_pa2g_pu_SHIFT 5
#define RF_2069_GE16_TXGM_CFG1_gc_res_MASK            0x3000
#define RF_2069_GE16_TXGM_CFG1_gc_res_SHIFT           12
#define RF_2069_GE16_PAD2G_TUNE_pad2g_ptat_slope_tuning_bias_MASK 0x700
#define RF_2069_GE16_PAD2G_TUNE_pad2g_ptat_slope_tuning_bias_SHIFT 8
#define RF_2069_GE16_OVR7_ovr_lna5g_dig_wrssi2_pu_MASK 0x8000
#define RF_2069_GE16_OVR7_ovr_lna5g_dig_wrssi2_pu_SHIFT 15
#define RF_2069_GE16_IQCAL_IDAC_tssi_bias_MASK        0xf
#define RF_2069_GE16_IQCAL_IDAC_tssi_bias_SHIFT       0
#define RF_2069_GE16_PAD5G_INCAP_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_GE16_PAD5G_INCAP_idac_incap_compen_aux_SHIFT 8
#define RF_2069_GE16_TX5G_CFG1_pu_MASK                0x4
#define RF_2069_GE16_TX5G_CFG1_pu_SHIFT               2
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_rx_byp_MASK 0xf00
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_rx_byp_SHIFT 8
#define RF_2069_GE16_RXMIX5G_CFG1_bias_reset_MASK     0x2
#define RF_2069_GE16_RXMIX5G_CFG1_bias_reset_SHIFT    1
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_threshold_MASK 0xf000
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_threshold_SHIFT 12
#define RF_2069_GE16_LNA2G_TUNE_tx_tune_MASK          0xf00
#define RF_2069_GE16_LNA2G_TUNE_tx_tune_SHIFT         8
#define RF_2069_GE16_TXGM_CFG2_nf_off_MASK            0x10
#define RF_2069_GE16_TXGM_CFG2_nf_off_SHIFT           4
#define RF_2069_GE16_OVR7_ovr_logen2g_rx_pu_MASK      0x10
#define RF_2069_GE16_OVR7_ovr_logen2g_rx_pu_SHIFT     4
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_bypass_MASK 0x20
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_bypass_SHIFT 5
#define RF_2069_GE16_LNA2G_IDAC1_lna1_bias_idac_MASK  0xf
#define RF_2069_GE16_LNA2G_IDAC1_lna1_bias_idac_SHIFT 0
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_trsw_source_bis_MASK 0x1e00
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_trsw_source_bis_SHIFT 9
#define RF_2069_GE16_OVR22_ovr_rx2g_calpath_lna2_pu_MASK 0x40
#define RF_2069_GE16_OVR22_ovr_rx2g_calpath_lna2_pu_SHIFT 6
#define RF_2069_GE16_RX_REG_BACKUP_2_reg_backup_2_MASK 0xffff
#define RF_2069_GE16_RX_REG_BACKUP_2_reg_backup_2_SHIFT 0
#define RF_2069_GE16_OVR24_ovr_wrssi3_Reflow_puI_MASK 0x10
#define RF_2069_GE16_OVR24_ovr_wrssi3_Reflow_puI_SHIFT 4
#define RF_2069_GE16_OVR22_ovr_trsw2g_bt_en_MASK      0x100
#define RF_2069_GE16_OVR22_ovr_trsw2g_bt_en_SHIFT     8
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC43_MASK 0x40
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_ctl_RC43_SHIFT 6
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_qrx_MASK  0x2
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_qrx_SHIFT 1
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_ip_gpaio_MASK 0x10
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_ip_gpaio_SHIFT 4
#define RF_2069_GE16_ADC_CALCODE6_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE6_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_OVR25_ovr_lpf_bias_level1_MASK   0x2
#define RF_2069_GE16_OVR25_ovr_lpf_bias_level1_SHIFT  1
#define RF_2069_GE16_OVR14_ovr_pad5g_gc_MASK          0x20
#define RF_2069_GE16_OVR14_ovr_pad5g_gc_SHIFT         5
#define RF_2069_GE16_OVR11_ovr_lpf_bq1_bw_MASK        0x10
#define RF_2069_GE16_OVR11_ovr_lpf_bq1_bw_SHIFT       4
#define RF_2069_GE16_OVR25_ovr_lpf_bias_level2_MASK   0x1
#define RF_2069_GE16_OVR25_ovr_lpf_bias_level2_SHIFT  0
#define RF_2069_GE16_LOGEN5G_RCCR_rccr_rx_MASK        0xf0
#define RF_2069_GE16_LOGEN5G_RCCR_rccr_rx_SHIFT       4
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_12_10_MASK    0x1c00
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_12_10_SHIFT   10
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_papdcal_rx_attn_MASK 0x18
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_papdcal_rx_attn_SHIFT 3
#define RF_2069_GE16_TX5G_CFG1_ampdet_sel_MASK        0x38
#define RF_2069_GE16_TX5G_CFG1_ampdet_sel_SHIFT       3
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_trsw_dwell_bis_MASK 0xf0
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_trsw_dwell_bis_SHIFT 4
#define RF_2069_GE16_PGA2G_INCAP_pad2g_idac_aux_MASK  0x3f00
#define RF_2069_GE16_PGA2G_INCAP_pad2g_idac_aux_SHIFT 8
#define RF_2069_GE16_OVR11_ovr_lpf_sw_iqcal_bq1_MASK  0x2000
#define RF_2069_GE16_OVR11_ovr_lpf_sw_iqcal_bq1_SHIFT 13
#define RF_2069_GE16_ADC_BIAS2_adc_bias_MASK          0xf
#define RF_2069_GE16_ADC_BIAS2_adc_bias_SHIFT         0
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_overload_MASK 0x4
#define RF_2069_GE16_ADC_STATUS_i_wrf_jtag_afe_iqadc_overload_SHIFT 2
#define RF_2069_GE16_DAC_CFG1_DAC_invclk_MASK         0x1000
#define RF_2069_GE16_DAC_CFG1_DAC_invclk_SHIFT        12
#define RF_2069_GE16_LNA2G_IDAC2_lna2_aux_bias_idac_MASK 0xf0
#define RF_2069_GE16_LNA2G_IDAC2_lna2_aux_bias_idac_SHIFT 4
#define RF_2069_GE16_TX_REG_BACKUP_4_reg_backup_4_MASK 0xffff
#define RF_2069_GE16_TX_REG_BACKUP_4_reg_backup_4_SHIFT 0
#define RF_2069_GE16_RXMIX5G_CFG1_LO_bias_sel_MASK    0x8
#define RF_2069_GE16_RXMIX5G_CFG1_LO_bias_sel_SHIFT   3
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refmid_puI_MASK 0x4
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refmid_puI_SHIFT 2
#define RF_2069_GE16_OVR24_ovr_wrssi3_Reflow_puQ_MASK 0x20
#define RF_2069_GE16_OVR24_ovr_wrssi3_Reflow_puQ_SHIFT 5
#define RF_2069_GE16_DAC_CFG1_DAC_disclk_MASK         0x4
#define RF_2069_GE16_DAC_CFG1_DAC_disclk_SHIFT        2
#define RF_2069_GE16_TIA_IDAC1_Ib_I_MASK              0xf0
#define RF_2069_GE16_TIA_IDAC1_Ib_I_SHIFT             4
#define RF_2069_GE16_RADIO_SPARE4_radio_spare4_MASK   0xffff
#define RF_2069_GE16_RADIO_SPARE4_radio_spare4_SHIFT  0
#define RF_2069_GE16_OVR7_ovr_lna5g_lna1_pu_MASK      0x200
#define RF_2069_GE16_OVR7_ovr_lna5g_lna1_pu_SHIFT     9
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_en_test_MASK  0x1
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_en_test_SHIFT 0
#define RF_2069_GE16_LNA5G_CFG1_FBC_MASK              0x30
#define RF_2069_GE16_LNA5G_CFG1_FBC_SHIFT             4
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_only_MASK 0x8
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_only_SHIFT 3
#define RF_2069_GE16_OVR19_ovr_rxmix5g_bias_reset_MASK 0x400
#define RF_2069_GE16_OVR19_ovr_rxmix5g_bias_reset_SHIFT 10
#define RF_2069_GE16_ADC_CFG2_ctrl_afe_tstmd_MASK     0x30
#define RF_2069_GE16_ADC_CFG2_ctrl_afe_tstmd_SHIFT    4
#define RF_2069_GE16_PAD5G_CFG1_bias_filter_MASK      0x30
#define RF_2069_GE16_PAD5G_CFG1_bias_filter_SHIFT     4
#define RF_2069_GE16_OVR6_ovr_lna2g_dig_wrssi2_pu_MASK 0x4000
#define RF_2069_GE16_OVR6_ovr_lna2g_dig_wrssi2_pu_SHIFT 14
#define RF_2069_GE16_OVR22_ovr_trsw5g_cntrl_MASK      0x800
#define RF_2069_GE16_OVR22_ovr_trsw5g_cntrl_SHIFT     11
#define RF_2069_GE16_IQCAL_CFG1_sel_sw_MASK           0xf0
#define RF_2069_GE16_IQCAL_CFG1_sel_sw_SHIFT          4
#define RF_2069_GE16_LNA2G_IDAC1_lna1_bias_ptat_MASK  0x70
#define RF_2069_GE16_LNA2G_IDAC1_lna1_bias_ptat_SHIFT 4
#define RF_2069_GE16_OVR13_ovr_pa5g_bias_pu_MASK      0x1
#define RF_2069_GE16_OVR13_ovr_pa5g_bias_pu_SHIFT     0
#define RF_2069_GE16_RXRF5G_CFG2_lna5g_epapd_en_MASK  0x10
#define RF_2069_GE16_RXRF5G_CFG2_lna5g_epapd_en_SHIFT 4
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl2_lp_MASK     0x800
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl2_lp_SHIFT    11
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refmid_puQ_MASK 0x8
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refmid_puQ_SHIFT 3
#define RF_2069_GE16_TXGM_CFG1_gc_iout_MASK           0x70
#define RF_2069_GE16_TXGM_CFG1_gc_iout_SHIFT          4
#define RF_2069_GE16_TIA_IDAC1_Ib_Q_MASK              0xf
#define RF_2069_GE16_TIA_IDAC1_Ib_Q_SHIFT             0
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_reset_MASK   0x10
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_reset_SHIFT  4
#define RF_2069_GE16_ADC_CFG2_rst_clk_MASK            0x40
#define RF_2069_GE16_ADC_CFG2_rst_clk_SHIFT           6
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_qtx_MASK  0x8
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_qtx_SHIFT 3
#define RF_2069_GE16_OVR11_ovr_lpf_sw_dac_rc_MASK     0x4000
#define RF_2069_GE16_OVR11_ovr_lpf_sw_dac_rc_SHIFT    14
#define RF_2069_GE16_LPF_BIAS_LEVELS_HIGH_lpf_bias_level5_MASK 0xff
#define RF_2069_GE16_LPF_BIAS_LEVELS_HIGH_lpf_bias_level5_SHIFT 0
#define RF_2069_GE16_PGA2G_CFG2_pga2g_tune_MASK       0x7
#define RF_2069_GE16_PGA2G_CFG2_pga2g_tune_SHIFT      0
#define RF_2069_GE16_PA2G_CFG1_pa2g_gain_ctrl_MASK    0xff00
#define RF_2069_GE16_PA2G_CFG1_pa2g_gain_ctrl_SHIFT   8
#define RF_2069_GE16_LPF_BIAS_LEVELS_HIGH_lpf_bias_level6_MASK 0xff00
#define RF_2069_GE16_LPF_BIAS_LEVELS_HIGH_lpf_bias_level6_SHIFT 8
#define RF_2069_GE16_LNA2G_IDAC2_lna2_aux_bias_ptat_MASK 0x7000
#define RF_2069_GE16_LNA2G_IDAC2_lna2_aux_bias_ptat_SHIFT 12
#define RF_2069_GE16_RX_REG_BACKUP_1_reg_backup_1_MASK 0xffff
#define RF_2069_GE16_RX_REG_BACKUP_1_reg_backup_1_SHIFT 0
#define RF_2069_GE16_OVR22_ovr_mix5g_pu_MASK          0x8000
#define RF_2069_GE16_OVR22_ovr_mix5g_pu_SHIFT         15
#define RF_2069_GE16_LOGEN5G_RCCR_rccr_tx_MASK        0xf
#define RF_2069_GE16_LOGEN5G_RCCR_rccr_tx_SHIFT       0
#define RF_2069_GE16_OVR14_ovr_pga2g_gainboost_MASK   0x8
#define RF_2069_GE16_OVR14_ovr_pga2g_gainboost_SHIFT  3
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq1_i_MASK      0x400
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq1_i_SHIFT     10
#define RF_2069_GE16_OVR14_ovr_pa5g_gain_ctrl_MASK    0x800
#define RF_2069_GE16_OVR14_ovr_pa5g_gain_ctrl_SHIFT   11
#define RF_2069_GE16_IQCAL_CFG1_PU_tssi_MASK          0x1
#define RF_2069_GE16_IQCAL_CFG1_PU_tssi_SHIFT         0
#define RF_2069_GE16_OVR5_ovr_vbat_monitor_pu_MASK    0x2000
#define RF_2069_GE16_OVR5_ovr_vbat_monitor_pu_SHIFT   13
#define RF_2069_GE16_OVR14_ovr_pa5g_5gtx_pu_MASK      0x400
#define RF_2069_GE16_OVR14_ovr_pa5g_5gtx_pu_SHIFT     10
#define RF_2069_GE16_ADC_CFG4_ctrl_flash17lvl_MASK    0x4000
#define RF_2069_GE16_ADC_CFG4_ctrl_flash17lvl_SHIFT   14
#define RF_2069_GE16_IQCAL_CFG2_hg_iqcal_MASK         0x1
#define RF_2069_GE16_IQCAL_CFG2_hg_iqcal_SHIFT        0
#define RF_2069_GE16_OVR4_ovr_lna5g_lna1_bypass_MASK  0x1000
#define RF_2069_GE16_OVR4_ovr_lna5g_lna1_bypass_SHIFT 12
#define RF_2069_GE16_OVR25_ovr_tia_Ib_I_MASK          0x8
#define RF_2069_GE16_OVR25_ovr_tia_Ib_I_SHIFT         3
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq1_gain_MASK 0xe00
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_bq1_gain_SHIFT 9
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_hold_MASK 0x10
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_hold_SHIFT 4
#define RF_2069_GE16_TX_REG_BACKUP_3_reg_backup_3_MASK 0xffff
#define RF_2069_GE16_TX_REG_BACKUP_3_reg_backup_3_SHIFT 0
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_rx_MASK 0xf0
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_rx_SHIFT 4
#define RF_2069_GE16_ADC_CFG2_iqadc_clamp_ref_ctl_MASK 0x8
#define RF_2069_GE16_ADC_CFG2_iqadc_clamp_ref_ctl_SHIFT 3
#define RF_2069_GE16_PAD5G_IDAC_ptat_slope_tuning_bias_MASK 0x700
#define RF_2069_GE16_PAD5G_IDAC_ptat_slope_tuning_bias_SHIFT 8
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_bw_MASK 0xf
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_dc_bw_SHIFT 0
#define RF_2069_GE16_OVR25_ovr_lna2g_lna2_aux_bias_idac_MASK 0x4000
#define RF_2069_GE16_OVR25_ovr_lna2g_lna2_aux_bias_idac_SHIFT 14
#define RF_2069_GE16_RADIO_SPARE3_radio_spare3_MASK   0xffff
#define RF_2069_GE16_RADIO_SPARE3_radio_spare3_SHIFT  0
#define RF_2069_GE16_OVR20_ovr_tia_DC_loop_PU_MASK    0x2000
#define RF_2069_GE16_OVR20_ovr_tia_DC_loop_PU_SHIFT   13
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq1_q_MASK      0x200
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq1_q_SHIFT     9
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_in_gpaio_MASK 0x20
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_in_gpaio_SHIFT 5
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_qrx_MASK      0x70
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_qrx_SHIFT     4
#define RF_2069_GE16_ADC_CALCODE12_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE12_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_OVR14_ovr_pad2g_gc_MASK          0x100
#define RF_2069_GE16_OVR14_ovr_pad2g_gc_SHIFT         8
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_ib_Compcore_MASK 0x3
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_ib_Compcore_SHIFT 0
#define RF_2069_GE16_PA5G_CFG3_pa5g_ptat_slope_main_MASK 0xf
#define RF_2069_GE16_PA5G_CFG3_pa5g_ptat_slope_main_SHIFT 0
#define RF_2069_GE16_OVR19_ovr_rx5g_calpath_mix_pu_MASK 0x4000
#define RF_2069_GE16_OVR19_ovr_rx5g_calpath_mix_pu_SHIFT 14
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_rx_MASK 0xf0
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_rx_SHIFT 4
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_aux_MASK  0xf0
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_aux_SHIFT 4
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_Ich_MASK 0x2000
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_Ich_SHIFT 13
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_MASK 0x40
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_rc_pu_SHIFT 6
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_MASK    0xf000
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_SHIFT   12
#define RF_2069_GE16_PA2G_TSSI_pa2g_tssi_pu_MASK      0x1
#define RF_2069_GE16_PA2G_TSSI_pa2g_tssi_pu_SHIFT     0
#define RF_2069_GE16_OVR25_ovr_tia_Ib_Q_MASK          0x4
#define RF_2069_GE16_OVR25_ovr_tia_Ib_Q_SHIFT         2
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq1_i_MASK    0x2
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq1_i_SHIFT   1
#define RF_2069_GE16_LNA2G_CFG1_lna1_pu_MASK          0x1
#define RF_2069_GE16_LNA2G_CFG1_lna1_pu_SHIFT         0
#define RF_2069_GE16_LNA5G_CFG1_lna1_out_short_pu_MASK 0x8000
#define RF_2069_GE16_LNA5G_CFG1_lna1_out_short_pu_SHIFT 15
#define RF_2069_GE16_TXRX5G_CAL_TX_i_cal_pad_atten_5g_MASK 0x3
#define RF_2069_GE16_TXRX5G_CAL_TX_i_cal_pad_atten_5g_SHIFT 0
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_cr_pu_MASK 0x20
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_cr_pu_SHIFT 5
#define RF_2069_GE16_TIA_CFG1_DC_loop_bypass_MASK     0x8
#define RF_2069_GE16_TIA_CFG1_DC_loop_bypass_SHIFT    3
#define RF_2069_GE16_RXMIX2G_CFG1_gm_size_MASK        0x700
#define RF_2069_GE16_RXMIX2G_CFG1_gm_size_SHIFT       8
#define RF_2069_GE16_PA5G_IDAC1_pa5g_idac_main_MASK   0x3f
#define RF_2069_GE16_PA5G_IDAC1_pa5g_idac_main_SHIFT  0
#define RF_2069_GE16_OVR26_ovr_lpf_bias_level3_MASK   0x2
#define RF_2069_GE16_OVR26_ovr_lpf_bias_level3_SHIFT  1
#define RF_2069_GE16_OVR20_ovr_pa2g_TSSI_range_MASK   0x1
#define RF_2069_GE16_OVR20_ovr_pa2g_TSSI_range_SHIFT  0
#define RF_2069_GE16_TXGM_CFG1_gc_pmos_MASK           0x700
#define RF_2069_GE16_TXGM_CFG1_gc_pmos_SHIFT          8
#define RF_2069_GE16_RXRF5G_SPARE_digital_spare_MASK  0x3ff
#define RF_2069_GE16_RXRF5G_SPARE_digital_spare_SHIFT 0
#define RF_2069_GE16_PAD2G_CFG1_pad2g_vcas_monitor_sw_MASK 0x4
#define RF_2069_GE16_PAD2G_CFG1_pad2g_vcas_monitor_sw_SHIFT 2
#define RF_2069_GE16_OVR26_ovr_lpf_bias_level4_MASK   0x1
#define RF_2069_GE16_OVR26_ovr_lpf_bias_level4_SHIFT  0
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_drive_strength_MASK 0x200
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_drive_strength_SHIFT 9
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_pu_MASK 0x100
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_pu_SHIFT 8
#define RF_2069_GE16_OVR13_ovr_pga5g_pu_MASK          0x800
#define RF_2069_GE16_OVR13_ovr_pga5g_pu_SHIFT         11
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_main_MASK 0xf
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_main_SHIFT 0
#define RF_2069_GE16_OVR21_ovr_mix5g_gc_load_MASK     0x1
#define RF_2069_GE16_OVR21_ovr_mix5g_gc_load_SHIFT    0
#define RF_2069_GE16_LOGEN5G_CFG2_tx_pu_MASK          0x2
#define RF_2069_GE16_LOGEN5G_CFG2_tx_pu_SHIFT         1
#define RF_2069_GE16_RXMIX5G_IDAC_gm_aux_bias_idac_i_MASK 0xf0
#define RF_2069_GE16_RXMIX5G_IDAC_gm_aux_bias_idac_i_SHIFT 4
#define RF_2069_GE16_OVR22_ovr_rxrf2g_pwrsw_lna2g_en_MASK 0x10
#define RF_2069_GE16_OVR22_ovr_rxrf2g_pwrsw_lna2g_en_SHIFT 4
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_drive_strength_MASK 0x200
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi2_drive_strength_SHIFT 9
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_papdcal_rx_attn_MASK 0x18
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_papdcal_rx_attn_SHIFT 3
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_outsel_MASK   0xf0
#define RF_2069_GE16_NBRSSI_TEST_nbrssi_outsel_SHIFT  4
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq1_q_MASK    0x4
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq1_q_SHIFT   2
#define RF_2069_GE16_IQCAL_CFG2_iq_cm_center_MASK     0xf0
#define RF_2069_GE16_IQCAL_CFG2_iq_cm_center_SHIFT    4
#define RF_2069_GE16_NBRSSI_IB_nbrssi_ib_Compcore_MASK 0xffff
#define RF_2069_GE16_NBRSSI_IB_nbrssi_ib_Compcore_SHIFT 0
#define RF_2069_GE16_PAD2G_INCAP_pad2g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_GE16_PAD2G_INCAP_pad2g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_tx_MASK 0xf
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_tx_SHIFT 0
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_calcode_Qch_MASK 0x10
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_calcode_Qch_SHIFT 4
#define RF_2069_GE16_OVR14_ovr_pga2g_pu_MASK          0x2
#define RF_2069_GE16_OVR14_ovr_pga2g_pu_SHIFT         1
#define RF_2069_GE16_LNA5G_IDAC2_lna2_main_bias_idac_MASK 0xf
#define RF_2069_GE16_LNA5G_IDAC2_lna2_main_bias_idac_SHIFT 0
#define RF_2069_GE16_OVR25_ovr_lna2g_lna1_bias_idac_MASK 0x8000
#define RF_2069_GE16_OVR25_ovr_lna2g_lna1_bias_idac_SHIFT 15
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_MASK          0x1
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_SHIFT         0
#define RF_2069_GE16_OVR22_ovr_lna2g_lna1_out_short_pu_MASK 0x400
#define RF_2069_GE16_OVR22_ovr_lna2g_lna1_out_short_pu_SHIFT 10
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_qtx_MASK      0x7000
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_qtx_SHIFT     12
#define RF_2069_GE16_ADC_CALCODE5_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE5_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_LNA2G_RSSI_ANA_wrssi1_gain_MASK  0x30
#define RF_2069_GE16_LNA2G_RSSI_ANA_wrssi1_gain_SHIFT 4
#define RF_2069_GE16_TX_REG_BACKUP_2_reg_backup_2_MASK 0xffff
#define RF_2069_GE16_TX_REG_BACKUP_2_reg_backup_2_SHIFT 0
#define RF_2069_GE16_RXRF5G_CFG1_pu_pulse_MASK        0x2
#define RF_2069_GE16_RXRF5G_CFG1_pu_pulse_SHIFT       1
#define RF_2069_GE16_PA2G_IDAC1_pa2g_idac_cas_MASK    0x3f00
#define RF_2069_GE16_PA2G_IDAC1_pa2g_idac_cas_SHIFT   8
#define RF_2069_GE16_OVR22_ovr_afe_DACbuf_fixed_cap_MASK 0x2000
#define RF_2069_GE16_OVR22_ovr_afe_DACbuf_fixed_cap_SHIFT 13
#define RF_2069_GE16_OVR21_ovr_pa5g_tssi_pu_MASK      0x400
#define RF_2069_GE16_OVR21_ovr_pa5g_tssi_pu_SHIFT     10
#define RF_2069_GE16_OVR11_ovr_lpf_bq2_gain_MASK      0x1
#define RF_2069_GE16_OVR11_ovr_lpf_bq2_gain_SHIFT     0
#define RF_2069_GE16_OVR11_ovr_lpf_pu_dc_i_MASK       0x40
#define RF_2069_GE16_OVR11_ovr_lpf_pu_dc_i_SHIFT      6
#define RF_2069_GE16_ADC_CFG5_iqadc_ctl_misc_MASK     0xffff
#define RF_2069_GE16_ADC_CFG5_iqadc_ctl_misc_SHIFT    0
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_23_20_MASK   0xf0
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_23_20_SHIFT  4
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_cal_MASK 0x800
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_cal_SHIFT 11
#define RF_2069_GE16_RXRF2G_CFG1_globe_idac_MASK      0xfe0
#define RF_2069_GE16_RXRF2G_CFG1_globe_idac_SHIFT     5
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_main_MASK  0xf
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_main_SHIFT 0
#define RF_2069_GE16_PA2G_TSSI_pa2g_TSSI_range_MASK   0x2
#define RF_2069_GE16_PA2G_TSSI_pa2g_TSSI_range_SHIFT  1
#define RF_2069_GE16_PA2G_CFG1_pa2g_2gtx_pu_MASK      0x1
#define RF_2069_GE16_PA2G_CFG1_pa2g_2gtx_pu_SHIFT     0
#define RF_2069_GE16_OVR4_global_alt_dc_en_MASK       0x200
#define RF_2069_GE16_OVR4_global_alt_dc_en_SHIFT      9
#define RF_2069_GE16_LNA5G_CFG2_gctl2_MASK            0x380
#define RF_2069_GE16_LNA5G_CFG2_gctl2_SHIFT           7
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_tx_MASK 0xf
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_tx_SHIFT 0
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_filter_aux_MASK 0xf0
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_filter_aux_SHIFT 4
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_pu_MASK      0x20
#define RF_2069_GE16_OVR13_ovr_pa2g_bias_pu_SHIFT     5
#define RF_2069_GE16_RADIO_SPARE2_radio_spare2_MASK   0xffff
#define RF_2069_GE16_RADIO_SPARE2_radio_spare2_SHIFT  0
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl1_ln_MASK     0x2000
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl1_ln_SHIFT    13
#define RF_2069_GE16_OVR6_ovr_lna2g_dig_wrssi1_pu_MASK 0x8000
#define RF_2069_GE16_OVR6_ovr_lna2g_dig_wrssi1_pu_SHIFT 15
#define RF_2069_GE16_ADC_CFG3_clk_ctl_MASK            0xf00
#define RF_2069_GE16_ADC_CFG3_clk_ctl_SHIFT           8
#define RF_2069_GE16_TXGM_CFG2_filter_MASK            0xf
#define RF_2069_GE16_TXGM_CFG2_filter_SHIFT           0
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_rx_byp_MASK 0xf00
#define RF_2069_GE16_TRSW2G_CFG1_trsw2g_cntrl_rx_byp_SHIFT 8
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_pu_MASK      0x8
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_pu_SHIFT     3
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_threshold_MASK 0xf0
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_threshold_SHIFT 4
#define RF_2069_GE16_PAD2G_CFG1_pad2g_pu_MASK         0x1
#define RF_2069_GE16_PAD2G_CFG1_pad2g_pu_SHIFT        0
#define RF_2069_GE16_DAC_CFG1_DAC_twos_enb_MASK       0x8
#define RF_2069_GE16_DAC_CFG1_DAC_twos_enb_SHIFT      3
#define RF_2069_GE16_TXGM_LOFT_SCALE_idac_lo_rf_diode_MASK 0x3
#define RF_2069_GE16_TXGM_LOFT_SCALE_idac_lo_rf_diode_SHIFT 0
#define RF_2069_GE16_OVR19_ovr_rxrf5g_globe_pu_MASK   0x10
#define RF_2069_GE16_OVR19_ovr_rxrf5g_globe_pu_SHIFT  4
#define RF_2069_GE16_OVR13_ovr_lpf_dc_bw_MASK         0x4000
#define RF_2069_GE16_OVR13_ovr_lpf_dc_bw_SHIFT        14
#define RF_2069_GE16_LNA5G_IDAC2_lna2_aux_bias_idac_MASK 0xf0
#define RF_2069_GE16_LNA5G_IDAC2_lna2_aux_bias_idac_SHIFT 4
#define RF_2069_GE16_OVR21_ovr_tx5g_bias_pu_MASK      0x2000
#define RF_2069_GE16_OVR21_ovr_tx5g_bias_pu_SHIFT     13
#define RF_2069_GE16_RXMIX5G_IDAC_gm_aux_bias_ptat_i_MASK 0x7000
#define RF_2069_GE16_RXMIX5G_IDAC_gm_aux_bias_ptat_i_SHIFT 12
#define RF_2069_GE16_OVR11_ovr_lpf_pu_dc_q_MASK       0x20
#define RF_2069_GE16_OVR11_ovr_lpf_pu_dc_q_SHIFT      5
#define RF_2069_GE16_DAC_CFG2_DAC_misc_MASK           0xff00
#define RF_2069_GE16_DAC_CFG2_DAC_misc_SHIFT          8
#define RF_2069_GE16_LNA5G_CFG1_lna1_bypass_MASK      0x4000
#define RF_2069_GE16_LNA5G_CFG1_lna1_bypass_SHIFT     14
#define RF_2069_GE16_OVR22_ovr_lna5g_lna1_out_short_pu_MASK 0x200
#define RF_2069_GE16_OVR22_ovr_lna5g_lna1_out_short_pu_SHIFT 9
#define RF_2069_GE16_OVR12_ovr_lpf_g_mult_rc_MASK     0x1
#define RF_2069_GE16_OVR12_ovr_lpf_g_mult_rc_SHIFT    0
#define RF_2069_GE16_AUXPGA_VMID_auxpga_i_sel_vmid_MASK 0x3ff
#define RF_2069_GE16_AUXPGA_VMID_auxpga_i_sel_vmid_SHIFT 0
#define RF_2069_GE16_DAC_BIAS_DACbuf_ctl_MASK         0x3ff
#define RF_2069_GE16_DAC_BIAS_DACbuf_ctl_SHIFT        0
#define RF_2069_GE16_DAC_CFG1_DAC_pwrup_MASK          0x1
#define RF_2069_GE16_DAC_CFG1_DAC_pwrup_SHIFT         0
#define RF_2069_GE16_LNA2G_CFG1_tr_rx_en_MASK         0x8
#define RF_2069_GE16_LNA2G_CFG1_tr_rx_en_SHIFT        3
#define RF_2069_GE16_PAD5G_CFG1_pu_MASK               0x1
#define RF_2069_GE16_PAD5G_CFG1_pu_SHIFT              0
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_pwrup_Ich_MASK 0x8000
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_pwrup_Ich_SHIFT 15
#define RF_2069_GE16_ADC_CALCODE26_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE26_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_LNA2G_CFG2_lna2_pu_MASK          0x1
#define RF_2069_GE16_LNA2G_CFG2_lna2_pu_SHIFT         0
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_lpf_MASK 0x1
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi2_lpf_SHIFT 0
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_cascode_MASK 0x700
#define RF_2069_GE16_PAD5G_SLOPE_ptat_slope_cascode_SHIFT 8
#define RF_2069_GE16_ADC_CALCODE18_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE18_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_IQCAL_CFG3_hpc_MASK              0x7
#define RF_2069_GE16_IQCAL_CFG3_hpc_SHIFT             0
#define RF_2069_GE16_LNA2G_CFG2_lna2_gm_size_MASK     0x3000
#define RF_2069_GE16_LNA2G_CFG2_lna2_gm_size_SHIFT    12
#define RF_2069_GE16_TXRX2G_CAL_TX_i_cal_pad_atten_2g_MASK 0x3
#define RF_2069_GE16_TXRX2G_CAL_TX_i_cal_pad_atten_2g_SHIFT 0
#define RF_2069_GE16_TX5G_TSSI_pa5g_ctrl_tssi_sel_MASK 0x4
#define RF_2069_GE16_TX5G_TSSI_pa5g_ctrl_tssi_sel_SHIFT 2
#define RF_2069_GE16_OVR25_ovr_rxmix2g_aux_bias_idac_MASK 0x200
#define RF_2069_GE16_OVR25_ovr_rxmix2g_aux_bias_idac_SHIFT 9
#define RF_2069_GE16_OVR20_ovr_tx2g_calpath_pad_pu_MASK 0x8
#define RF_2069_GE16_OVR20_ovr_tx2g_calpath_pad_pu_SHIFT 3
#define RF_2069_GE16_LNA5G_IDAC2_lna2_main_bias_ptat_MASK 0x700
#define RF_2069_GE16_LNA5G_IDAC2_lna2_main_bias_ptat_SHIFT 8
#define RF_2069_GE16_TXGM_LOFT_FINE_Q_idac_lo_bb_qn_MASK 0xf0
#define RF_2069_GE16_TXGM_LOFT_FINE_Q_idac_lo_bb_qn_SHIFT 4
#define RF_2069_GE16_PA2G_IDAC2_pa2g_biasa_main_MASK  0xff
#define RF_2069_GE16_PA2G_IDAC2_pa2g_biasa_main_SHIFT 0
#define RF_2069_GE16_OVR7_ovr_logen2g_tx_pu_MASK      0x8
#define RF_2069_GE16_OVR7_ovr_logen2g_tx_pu_SHIFT     3
#define RF_2069_GE16_OVR21_ovr_txgm_gc_iout_MASK      0x80
#define RF_2069_GE16_OVR21_ovr_txgm_gc_iout_SHIFT     7
#define RF_2069_GE16_PA2G_IDAC2_pa2g_biasa_aux_MASK   0xff00
#define RF_2069_GE16_PA2G_IDAC2_pa2g_biasa_aux_SHIFT  8
#define RF_2069_GE16_OVR3_ovr_auxpga_i_pu_MASK        0x8000
#define RF_2069_GE16_OVR3_ovr_auxpga_i_pu_SHIFT       15
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_bq2_MASK 0x400
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_bq2_SHIFT 10
#define RF_2069_GE16_LNA5G_CFG1_gctl1_MASK            0x3800
#define RF_2069_GE16_LNA5G_CFG1_gctl1_SHIFT           11
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_gain_MASK 0x30
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_gain_SHIFT 4
#define RF_2069_GE16_TXGM_LOFT_FINE_Q_idac_lo_bb_qp_MASK 0xf
#define RF_2069_GE16_TXGM_LOFT_FINE_Q_idac_lo_bb_qp_SHIFT 0
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_bypass_MASK  0x800
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_bypass_SHIFT 11
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_clamp_en_MASK 0x400
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_clamp_en_SHIFT 10
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_sw_spare_MASK 0x780
#define RF_2069_GE16_LPF_DC_LOOP_AND_MISC_lpf_sw_spare_SHIFT 7
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_cal_MASK 0x4
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_cal_SHIFT 2
#define RF_2069_GE16_TXRX2G_CAL_TX_i_cal_pa_atten_2g_MASK 0x18
#define RF_2069_GE16_TXRX2G_CAL_TX_i_cal_pa_atten_2g_SHIFT 3
#define RF_2069_GE16_PA5G_CFG3_pa5g_ptat_slope_cas_MASK 0xf0
#define RF_2069_GE16_PA5G_CFG3_pa5g_ptat_slope_cas_SHIFT 4
#define RF_2069_GE16_TXMIX2G_CFG1_lodc_MASK           0xf0
#define RF_2069_GE16_TXMIX2G_CFG1_lodc_SHIFT          4
#define RF_2069_GE16_RXRF5G_CFG2_lna5g_epapd_attn_MASK 0xf
#define RF_2069_GE16_RXRF5G_CFG2_lna5g_epapd_attn_SHIFT 0
#define RF_2069_GE16_RXMIX5G_CFG1_LO_bias_MASK        0x70
#define RF_2069_GE16_RXMIX5G_CFG1_LO_bias_SHIFT       4
#define RF_2069_GE16_RXMIX2G_IDAC_aux_bias_idac_MASK  0xf0
#define RF_2069_GE16_RXMIX2G_IDAC_aux_bias_idac_SHIFT 4
#define RF_2069_GE16_LOGEN2G_CFG2_rx_pu_MASK          0x4
#define RF_2069_GE16_LOGEN2G_CFG2_rx_pu_SHIFT         2
#define RF_2069_GE16_OVR25_ovr_lna2g_lna2_main_bias_idac_MASK 0x2000
#define RF_2069_GE16_OVR25_ovr_lna2g_lna2_main_bias_idac_SHIFT 13
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_high_MASK 0x40
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_high_SHIFT 6
#define RF_2069_GE16_PAD5G_TUNE_idac_tuning_bias_MASK 0xf0
#define RF_2069_GE16_PAD5G_TUNE_idac_tuning_bias_SHIFT 4
#define RF_2069_GE16_OVR6_ovr_lna2g_tr_rx_en_MASK     0x2
#define RF_2069_GE16_OVR6_ovr_lna2g_tr_rx_en_SHIFT    1
#define RF_2069_GE16_OVR3_ovr_auxpga_i_sel_gain_MASK  0x4000
#define RF_2069_GE16_OVR3_ovr_auxpga_i_sel_gain_SHIFT 14
#define RF_2069_GE16_OVR12_ovr_lpf_q_biq2_MASK        0x4
#define RF_2069_GE16_OVR12_ovr_lpf_q_biq2_SHIFT       2
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_tstsel_NBW3_MASK 0x4
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_tstsel_NBW3_SHIFT 2
#define RF_2069_GE16_PAD2G_CFG1_pad2g_bias_filter_bypass_MASK 0x2
#define RF_2069_GE16_PAD2G_CFG1_pad2g_bias_filter_bypass_SHIFT 1
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_low_MASK 0x80
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_low_SHIFT 7
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_papdcal_pu_MASK 0x100
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_papdcal_pu_SHIFT 8
#define RF_2069_GE16_TX_REG_BACKUP_1_reg_backup_1_MASK 0xffff
#define RF_2069_GE16_TX_REG_BACKUP_1_reg_backup_1_SHIFT 0
#define RF_2069_GE16_OVR5_ovr_iqcal_PU_iqcal_MASK     0x800
#define RF_2069_GE16_OVR5_ovr_iqcal_PU_iqcal_SHIFT    11
#define RF_2069_GE16_ADC_CFG2_iqadc_ovdrv_ref_ctl_MASK 0x2
#define RF_2069_GE16_ADC_CFG2_iqadc_ovdrv_ref_ctl_SHIFT 1
#define RF_2069_GE16_LNA5G_IDAC2_lna2_aux_bias_ptat_MASK 0x7000
#define RF_2069_GE16_LNA5G_IDAC2_lna2_aux_bias_ptat_SHIFT 12
#define RF_2069_GE16_TIA_CFG3_Spare_MASK              0xff00
#define RF_2069_GE16_TIA_CFG3_Spare_SHIFT             8
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_misc_MASK     0xff00
#define RF_2069_GE16_NBRSSI_BIAS_nbrssi_misc_SHIFT    8
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_iqcal_bq1_MASK 0x1000
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_iqcal_bq1_SHIFT 12
#define RF_2069_GE16_ADC_CALCODE23_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE23_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_LNA5G_CFG1_gctl1_ln_MASK         0x700
#define RF_2069_GE16_LNA5G_CFG1_gctl1_ln_SHIFT        8
#define RF_2069_GE16_RADIO_SPARE1_radio_spare1_MASK   0xffff
#define RF_2069_GE16_RADIO_SPARE1_radio_spare1_SHIFT  0
#define RF_2069_GE16_ADC_CALCODE15_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE15_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_PA5G_INCAP_pa5g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_GE16_PA5G_INCAP_pa5g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_GE16_OVR7_ovr_logen2g_reset_rx_MASK   0x2
#define RF_2069_GE16_OVR7_ovr_logen2g_reset_rx_SHIFT  1
#define RF_2069_GE16_PAD5G_CFG1_bias_filter_bypass_MASK 0x2
#define RF_2069_GE16_PAD5G_CFG1_bias_filter_bypass_SHIFT 1
#define RF_2069_GE16_PA5G_CFG3_pa5g_sel_bias_type_MASK 0xff00
#define RF_2069_GE16_PA5G_CFG3_pa5g_sel_bias_type_SHIFT 8
#define RF_2069_GE16_TXMIX2G_CFG1_tune_MASK           0xf00
#define RF_2069_GE16_TXMIX2G_CFG1_tune_SHIFT          8
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_rx_div4_en_MASK 0x20
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_rx_div4_en_SHIFT 5
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_qrx_MASK  0x20
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_qrx_SHIFT 5
#define RF_2069_GE16_OVR21_ovr_txgm_sel5g_MASK        0x20
#define RF_2069_GE16_OVR21_ovr_txgm_sel5g_SHIFT       5
#define RF_2069_GE16_OVR19_ovr_rxmix5g_pu_MASK        0x80
#define RF_2069_GE16_OVR19_ovr_rxmix5g_pu_SHIFT       7
#define RF_2069_GE16_ADC_CALCODE11_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE11_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_pu_MASK 0x1
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_pu_SHIFT 0
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_mid_MASK 0x300
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refctrl_mid_SHIFT 8
#define RF_2069_GE16_DAC_CFG1_DAC_rst_MASK            0x2000
#define RF_2069_GE16_DAC_CFG1_DAC_rst_SHIFT           13
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_low_ct_MASK  0x100
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_low_ct_SHIFT 8
#define RF_2069_GE16_ADC_CFG3_pwrup_clk_MASK          0x2000
#define RF_2069_GE16_ADC_CFG3_pwrup_clk_SHIFT         13
#define RF_2069_GE16_ADC_CFG1_Ich_pwrup_MASK          0x3f
#define RF_2069_GE16_ADC_CFG1_Ich_pwrup_SHIFT         0
#define RF_2069_GE16_OVR20_ovr_testbuf_sel_test_port_MASK 0x8000
#define RF_2069_GE16_OVR20_ovr_testbuf_sel_test_port_SHIFT 15
#define RF_2069_GE16_LNA5G_IDAC1_lna1_bias_idac_MASK  0xf
#define RF_2069_GE16_LNA5G_IDAC1_lna1_bias_idac_SHIFT 0
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_boost_MASK 0x7000
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_boost_SHIFT 12
#define RF_2069_GE16_PAD5G_IDAC_idac_cascode_MASK     0x7800
#define RF_2069_GE16_PAD5G_IDAC_idac_cascode_SHIFT    11
#define RF_2069_GE16_OVR7_ovr_lna5g_tr_rx_en_MASK     0x80
#define RF_2069_GE16_OVR7_ovr_lna5g_tr_rx_en_SHIFT    7
#define RF_2069_GE16_TXMIX2G_CFG1_gc_load_MASK        0xf
#define RF_2069_GE16_TXMIX2G_CFG1_gc_load_SHIFT       0
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_qrx_MASK      0x70
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_qrx_SHIFT     4
#define RF_2069_GE16_RXMIX2G_IDAC_aux_bias_ptat_MASK  0x7000
#define RF_2069_GE16_RXMIX2G_IDAC_aux_bias_ptat_SHIFT 12
#define RF_2069_GE16_PA5G_CFG1_trsw5g_pu_MASK         0x20
#define RF_2069_GE16_PA5G_CFG1_trsw5g_pu_SHIFT        5
#define RF_2069_GE16_DEV_ID_dev_id_MASK               0xffff
#define RF_2069_GE16_DEV_ID_dev_id_SHIFT              0
#define RF_2069_GE16_PAD2G_CFG1_pad2g_bias_filter_MASK 0xf0
#define RF_2069_GE16_PAD2G_CFG1_pad2g_bias_filter_SHIFT 4
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_tia_bq1_MASK 0x2000
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_tia_bq1_SHIFT 13
#define RF_2069_GE16_RXRF2G_CFG1_globe_pu_MASK        0x1
#define RF_2069_GE16_RXRF2G_CFG1_globe_pu_SHIFT       0
#define RF_2069_GE16_OVR19_ovr_rxrf5g_pwrsw_en_MASK   0x4
#define RF_2069_GE16_OVR19_ovr_rxrf5g_pwrsw_en_SHIFT  2
#define RF_2069_GE16_ADC_CFG1_Qch_pwrup_MASK          0x3f00
#define RF_2069_GE16_ADC_CFG1_Qch_pwrup_SHIFT         8
#define RF_2069_GE16_DAC_CFG1_DAC_scram_mode_MASK     0x400
#define RF_2069_GE16_DAC_CFG1_DAC_scram_mode_SHIFT    10
#define RF_2069_GE16_TESTBUF_CFG1_sel_test_port_MASK  0x70
#define RF_2069_GE16_TESTBUF_CFG1_sel_test_port_SHIFT 4
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_aux_MASK   0xf0
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_aux_SHIFT  4
#define RF_2069_GE16_LNA5G_CFG1_lna1_lowpwr_MASK      0x2
#define RF_2069_GE16_LNA5G_CFG1_lna1_lowpwr_SHIFT     1
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_en_test_MASK  0x1
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_en_test_SHIFT 0
#define RF_2069_GE16_TESTBUF_CFG1_GPIO_EN_MASK        0x2
#define RF_2069_GE16_TESTBUF_CFG1_GPIO_EN_SHIFT       1
#define RF_2069_GE16_PAD5G_IDAC_idac_main_MASK        0x3f
#define RF_2069_GE16_PAD5G_IDAC_idac_main_SHIFT       0
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_pu_MASK      0x80
#define RF_2069_GE16_OVR6_ovr_lna2g_lna1_pu_SHIFT     7
#define RF_2069_GE16_TXRX2G_CAL_TX_i_calPath_pad2g_pu_MASK 0x4
#define RF_2069_GE16_TXRX2G_CAL_TX_i_calPath_pad2g_pu_SHIFT 2
#define RF_2069_GE16_TIA_CFG2_Rdc_out_MASK            0x3000
#define RF_2069_GE16_TIA_CFG2_Rdc_out_SHIFT           12
#define RF_2069_GE16_PA2G_INCAP_pa2g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_GE16_PA2G_INCAP_pa2g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_GE16_OVR7_ovr_logen2g_reset_tx_MASK   0x1
#define RF_2069_GE16_OVR7_ovr_logen2g_reset_tx_SHIFT  0
#define RF_2069_GE16_PGA2G_CFG1_pga2g_idac_boost_MASK 0xf00
#define RF_2069_GE16_PGA2G_CFG1_pga2g_idac_boost_SHIFT 8
#define RF_2069_GE16_OVR21_ovr_txmix2g_gainboost_MASK 0x10
#define RF_2069_GE16_OVR21_ovr_txmix2g_gainboost_SHIFT 4
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_Refladder_MASK 0x7000
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_Refladder_SHIFT 12
#define RF_2069_GE16_DAC_CFG2_DACbuf_fixed_cap_MASK   0x4
#define RF_2069_GE16_DAC_CFG2_DACbuf_fixed_cap_SHIFT  2
#define RF_2069_GE16_TIA_CFG2_Ib_tempco_MASK          0x380
#define RF_2069_GE16_TIA_CFG2_Ib_tempco_SHIFT         7
#define RF_2069_GE16_PA5G_IDAC1_pa5g_idac_cas_MASK    0x3f00
#define RF_2069_GE16_PA5G_IDAC1_pa5g_idac_cas_SHIFT   8
#define RF_2069_GE16_OVR6_ovr_lna5g_dig_wrssi1_pu_MASK 0x1
#define RF_2069_GE16_OVR6_ovr_lna5g_dig_wrssi1_pu_SHIFT 0
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_qtx_MASK  0x80
#define RF_2069_GE16_OVR28_ovr_logen2g_idac_qtx_SHIFT 7
#define RF_2069_GE16_ADC_CALCODE4_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE4_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_LNA2G_CFG1_pwrsw_en_MASK         0x40
#define RF_2069_GE16_LNA2G_CFG1_pwrsw_en_SHIFT        6
#define RF_2069_GE16_RXMIX5G_IDAC_gm_main_bias_idac_i_MASK 0xf
#define RF_2069_GE16_RXMIX5G_IDAC_gm_main_bias_idac_i_SHIFT 0
#define RF_2069_GE16_PAD2G_TUNE_pad2g_tune_MASK       0x7
#define RF_2069_GE16_PAD2G_TUNE_pad2g_tune_SHIFT      0
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_filter_main_MASK 0xf
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_filter_main_SHIFT 0
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_rc_MASK 0x800
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_rc_SHIFT 11
#define RF_2069_GE16_LNA5G_IDAC1_lna1_bias_ptat_MASK  0x70
#define RF_2069_GE16_LNA5G_IDAC1_lna1_bias_ptat_SHIFT 4
#define RF_2069_GE16_OVR14_ovr_pga5g_gainboost_MASK   0x1
#define RF_2069_GE16_OVR14_ovr_pga5g_gainboost_SHIFT  0
#define RF_2069_GE16_IQCAL_IDAC_idac_MASK             0x3ff0
#define RF_2069_GE16_IQCAL_IDAC_idac_SHIFT            4
#define RF_2069_GE16_TEMPSENSE_CFG_tempsense_sel_Vbe_Vbg_MASK 0x2
#define RF_2069_GE16_TEMPSENSE_CFG_tempsense_sel_Vbe_Vbg_SHIFT 1
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_Refbuf_MASK 0x1800
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_Refbuf_SHIFT 11
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_aux_MASK 0xf0
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_aux_SHIFT 4
#define RF_2069_GE16_OVR11_ovr_lpf_pu_MASK            0x800
#define RF_2069_GE16_OVR11_ovr_lpf_pu_SHIFT           11
#define RF_2069_GE16_LNA2G_IDAC2_lna2_main_bias_idac_MASK 0xf
#define RF_2069_GE16_LNA2G_IDAC2_lna2_main_bias_idac_SHIFT 0
#define RF_2069_GE16_TX5G_CFG1_mix5g_pu_MASK          0x1
#define RF_2069_GE16_TX5G_CFG1_mix5g_pu_SHIFT         0
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_qtx_MASK      0x7000
#define RF_2069_GE16_LOGEN2G_IDAC3_idac_qtx_SHIFT     12
#define RF_2069_GE16_OVR14_ovr_pad5g_pu_MASK          0x10
#define RF_2069_GE16_OVR14_ovr_pad5g_pu_SHIFT         4
#define RF_2069_GE16_LNA5G_CFG1_tr_rx_en_MASK         0x4
#define RF_2069_GE16_LNA5G_CFG1_tr_rx_en_SHIFT        2
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq2_i_MASK      0x100
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq2_i_SHIFT     8
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_bt_en_MASK    0x100
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_bt_en_SHIFT   8
#define RF_2069_GE16_PGA5G_IDAC_idac_aux_MASK         0x3f00
#define RF_2069_GE16_PGA5G_IDAC_idac_aux_SHIFT        8
#define RF_2069_GE16_TX2G_CFG1_ampdet_sel_MASK        0x38
#define RF_2069_GE16_TX2G_CFG1_ampdet_sel_SHIFT       3
#define RF_2069_GE16_OVR21_ovr_pa5g_ctrl_tssi_MASK    0x800
#define RF_2069_GE16_OVR21_ovr_pa5g_ctrl_tssi_SHIFT   11
#define RF_2069_GE16_LPF_BIAS_LEVELS_LOW_lpf_bias_level1_MASK 0xff
#define RF_2069_GE16_LPF_BIAS_LEVELS_LOW_lpf_bias_level1_SHIFT 0
#define RF_2069_GE16_ADC_CFG4_Ich_calmode_MASK        0x8
#define RF_2069_GE16_ADC_CFG4_Ich_calmode_SHIFT       3
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_low_MASK 0x300
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_low_SHIFT 8
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_cr_pu_MASK 0x20
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_cr_pu_SHIFT 5
#define RF_2069_GE16_TXMIX5G_CFG1_gainboost_MASK      0xf000
#define RF_2069_GE16_TXMIX5G_CFG1_gainboost_SHIFT     12
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_pu_MASK      0x2
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_pu_SHIFT     1
#define RF_2069_GE16_OVR5_ovr_afe_iqadc_adc_bias_MASK 0x8000
#define RF_2069_GE16_OVR5_ovr_afe_iqadc_adc_bias_SHIFT 15
#define RF_2069_GE16_OVR19_ovr_rxmix2g_pu_MASK        0x800
#define RF_2069_GE16_OVR19_ovr_rxmix2g_pu_SHIFT       11
#define RF_2069_GE16_LPF_BIAS_LEVELS_LOW_lpf_bias_level2_MASK 0xff00
#define RF_2069_GE16_LPF_BIAS_LEVELS_LOW_lpf_bias_level2_SHIFT 8
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_threshold_MASK 0xf0
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_threshold_SHIFT 4
#define RF_2069_GE16_TRSW5G_CFG2_trsw5g_trsw_gate_bis_MASK 0xf
#define RF_2069_GE16_TRSW5G_CFG2_trsw5g_trsw_gate_bis_SHIFT 0
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_pu_MASK    0x100
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi2_pu_SHIFT   8
#define RF_2069_GE16_TIA_CFG1_GainI_MASK              0xf000
#define RF_2069_GE16_TIA_CFG1_GainI_SHIFT             12
#define RF_2069_GE16_LNA2G_CFG1_lna1_gain_MASK        0x7000
#define RF_2069_GE16_LNA2G_CFG1_lna1_gain_SHIFT       12
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_trsw_sub_bis_MASK 0x1e0
#define RF_2069_GE16_TRSW5G_CFG3_trsw5g_trsw_sub_bis_SHIFT 5
#define RF_2069_GE16_OVR25_ovr_rxmix2g_main_bias_idac_MASK 0x100
#define RF_2069_GE16_OVR25_ovr_rxmix2g_main_bias_idac_SHIFT 8
#define RF_2069_GE16_DAC_CFG1_DAC_dualDAC_en_MASK     0x2
#define RF_2069_GE16_DAC_CFG1_DAC_dualDAC_en_SHIFT    1
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_drive_strength_MASK 0x2
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_drive_strength_SHIFT 1
#define RF_2069_GE16_LNA5G_CFG2_gctl2_lp_MASK         0x30
#define RF_2069_GE16_LNA5G_CFG2_gctl2_lp_SHIFT        4
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq1_bq2_MASK    0x10
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq1_bq2_SHIFT   4
#define RF_2069_GE16_ADC_CFG4_Qch_run_flashcal_MASK   0x1
#define RF_2069_GE16_ADC_CFG4_Qch_run_flashcal_SHIFT  0
#define RF_2069_GE16_TXGM_LOFT_FINE_I_idac_lo_bb_in_MASK 0xf0
#define RF_2069_GE16_TXGM_LOFT_FINE_I_idac_lo_bb_in_SHIFT 4
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_gain_MASK    0x20
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_gain_SHIFT   5
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq2_q_MASK      0x80
#define RF_2069_GE16_OVR11_ovr_lpf_pu_bq2_q_SHIFT     7
#define RF_2069_GE16_RXMIX5G_IDAC_gm_main_bias_ptat_i_MASK 0x700
#define RF_2069_GE16_RXMIX5G_IDAC_gm_main_bias_ptat_i_SHIFT 8
#define RF_2069_GE16_PGA5G_INCAP_idac_incap_compen_MASK 0xf
#define RF_2069_GE16_PGA5G_INCAP_idac_incap_compen_SHIFT 0
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_cas_pu_MASK  0x4
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_cas_pu_SHIFT 2
#define RF_2069_GE16_LOGEN2G_RCCR_rccr_rx_MASK        0xf0
#define RF_2069_GE16_LOGEN2G_RCCR_rccr_rx_SHIFT       4
#define RF_2069_GE16_ADC_CFG3_flash_calrstb_MASK      0x1000
#define RF_2069_GE16_ADC_CFG3_flash_calrstb_SHIFT     12
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_9_8_MASK      0x300
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_9_8_SHIFT     8
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_drive_strength_MASK 0x2
#define RF_2069_GE16_LNA5G_RSSI_dig_wrssi1_drive_strength_SHIFT 1
#define RF_2069_GE16_TXGM_LOFT_FINE_I_idac_lo_bb_ip_MASK 0xf
#define RF_2069_GE16_TXGM_LOFT_FINE_I_idac_lo_bb_ip_SHIFT 0
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_mid_MASK 0x40
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_mid_SHIFT 6
#define RF_2069_GE16_TXRX5G_CAL_TX_i_calPath_pad_pu_5g_MASK 0x4
#define RF_2069_GE16_TXRX5G_CAL_TX_i_calPath_pad_pu_5g_SHIFT 2
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_rx_attn_MASK 0x6
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_rxiqcal_rx_attn_SHIFT 1
#define RF_2069_GE16_OVR13_ovr_pa2g_gain_ctrl_MASK    0x8
#define RF_2069_GE16_OVR13_ovr_pa2g_gain_ctrl_SHIFT   3
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq2_i_MASK    0x8
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq2_i_SHIFT   3
#define RF_2069_GE16_LPF_BIAS_LEVELS_MID_lpf_bias_level3_MASK 0xff
#define RF_2069_GE16_LPF_BIAS_LEVELS_MID_lpf_bias_level3_SHIFT 0
#define RF_2069_GE16_LNA2G_IDAC2_lna2_main_bias_ptat_MASK 0x700
#define RF_2069_GE16_LNA2G_IDAC2_lna2_main_bias_ptat_SHIFT 8
#define RF_2069_GE16_TIA_CFG1_GainQ_MASK              0xf00
#define RF_2069_GE16_TIA_CFG1_GainQ_SHIFT             8
#define RF_2069_GE16_LPF_BIAS_LEVELS_MID_lpf_bias_level4_MASK 0xff00
#define RF_2069_GE16_LPF_BIAS_LEVELS_MID_lpf_bias_level4_SHIFT 8
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_lna12_mux_MASK 0x1
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_lna12_mux_SHIFT 0
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_trsw_gate_bis_MASK 0xf
#define RF_2069_GE16_TRSW2G_CFG2_trsw2g_trsw_gate_bis_SHIFT 0
#define RF_2069_GE16_TIA_CFG3_rccal_hpc_MASK          0x1f
#define RF_2069_GE16_TIA_CFG3_rccal_hpc_SHIFT         0
#define RF_2069_GE16_IQCAL_CFG1_tssi_GPIO_ctrl_MASK   0x300
#define RF_2069_GE16_IQCAL_CFG1_tssi_GPIO_ctrl_SHIFT  8
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_powersaving_MASK 0x400
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_powersaving_SHIFT 10
#define RF_2069_GE16_TEMPSENSE_CFG_pu_MASK            0x1
#define RF_2069_GE16_TEMPSENSE_CFG_pu_SHIFT           0
#define RF_2069_GE16_RXRF2G_CFG1_pwrsw_en_MASK        0x10
#define RF_2069_GE16_RXRF2G_CFG1_pwrsw_en_SHIFT       4
#define RF_2069_GE16_PAD5G_TUNE_tune_MASK             0xf
#define RF_2069_GE16_PAD5G_TUNE_tune_SHIFT            0
#define RF_2069_GE16_OVR19_ovr_rxmix5g_gm_size_MASK   0x100
#define RF_2069_GE16_OVR19_ovr_rxmix5g_gm_size_SHIFT  8
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq2_rc_MASK     0x2
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq2_rc_SHIFT    1
#define RF_2069_GE16_TESTBUF_CFG1_PU_MASK             0x1
#define RF_2069_GE16_TESTBUF_CFG1_PU_SHIFT            0
#define RF_2069_GE16_PGA5G_CFG1_gainboost_MASK        0xf000
#define RF_2069_GE16_PGA5G_CFG1_gainboost_SHIFT       12
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_irx_MASK  0x1
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_irx_SHIFT 0
#define RF_2069_GE16_CORE_BIAS_bias_rx_pu_MASK        0x2
#define RF_2069_GE16_CORE_BIAS_bias_rx_pu_SHIFT       1
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_main_MASK 0xf
#define RF_2069_GE16_PAD2G_SLOPE_pad2g_ptat_slope_main_SHIFT 0
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq2_adc_MASK    0x4
#define RF_2069_GE16_OVR10_ovr_lpf_sw_bq2_adc_SHIFT   2
#define RF_2069_GE16_LNA2G_TUNE_lna2_freq_tune_MASK   0xf0
#define RF_2069_GE16_LNA2G_TUNE_lna2_freq_tune_SHIFT  4
#define RF_2069_GE16_TX5G_TSSI_pa5g_ctrl_tssi_MASK    0xf0
#define RF_2069_GE16_TX5G_TSSI_pa5g_ctrl_tssi_SHIFT   4
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq2_q_MASK    0x10
#define RF_2069_GE16_LPF_ENABLES_lpf_pu_bq2_q_SHIFT   4
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_cm_mult_MASK 0x3
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_cm_mult_SHIFT 0
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_sel_gain_MASK 0x700
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_sel_gain_SHIFT 8
#define RF_2069_GE16_LNA5G_CFG2_ana_wrssi2_pu_MASK    0x4
#define RF_2069_GE16_LNA5G_CFG2_ana_wrssi2_pu_SHIFT   2
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_boost_MASK 0x700
#define RF_2069_GE16_PGA5G_CFG2_ptat_slope_boost_SHIFT 8
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq1_bq2_MASK 0x4
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq1_bq2_SHIFT 2
#define RF_2069_GE16_ADC_CALCODE9_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE9_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_ADC_CALCODE10_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE10_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_q_biq2_MASK 0x8000
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_q_biq2_SHIFT 15
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_Refbuf_MASK 0xc00
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_ib_Refbuf_SHIFT 10
#define RF_2069_GE16_LNA2G_CFG1_lna1_Rout_MASK        0xf00
#define RF_2069_GE16_LNA2G_CFG1_lna1_Rout_SHIFT       8
#define RF_2069_GE16_TIA_CFG1_DC_loop_PU_MASK         0x2
#define RF_2069_GE16_TIA_CFG1_DC_loop_PU_SHIFT        1
#define RF_2069_GE16_OVR25_ovr_lna5g_lna2_aux_bias_idac_MASK 0x800
#define RF_2069_GE16_OVR25_ovr_lna5g_lna2_aux_bias_idac_SHIFT 11
#define RF_2069_GE16_RXRF5G_CFG1_globe_pu_MASK        0x1
#define RF_2069_GE16_RXRF5G_CFG1_globe_pu_SHIFT       0
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_Rout_MASK    0x4
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_Rout_SHIFT   2
#define RF_2069_GE16_OVR14_ovr_pad2g_pu_MASK          0x80
#define RF_2069_GE16_OVR14_ovr_pad2g_pu_SHIFT         7
#define RF_2069_GE16_LOGEN2G_RCCR_rccr_tx_MASK        0xf
#define RF_2069_GE16_LOGEN2G_RCCR_rccr_tx_SHIFT       0
#define RF_2069_GE16_OVR4_ovr_lna2g_bias_reset_MASK   0x1
#define RF_2069_GE16_OVR4_ovr_lna2g_bias_reset_SHIFT  0
#define RF_2069_GE16_OVR20_ovr_tia_DC_loop_bypass_MASK 0x4000
#define RF_2069_GE16_OVR20_ovr_tia_DC_loop_bypass_SHIFT 14
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_pu_MASK      0x2
#define RF_2069_GE16_PA2G_CFG1_pa2g_bias_pu_SHIFT     1
#define RF_2069_GE16_OVR19_ovr_rxrf2g_globe_pu_MASK   0x40
#define RF_2069_GE16_OVR19_ovr_rxrf2g_globe_pu_SHIFT  6
#define RF_2069_GE16_DAC_CFG1_DAC_sel_tp_MASK         0x800
#define RF_2069_GE16_DAC_CFG1_DAC_sel_tp_SHIFT        11
#define RF_2069_GE16_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_main_MASK 0x70
#define RF_2069_GE16_PAD2G_INCAP_pad2g_ptat_slope_incap_compen_main_SHIFT 4
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_rc_bw_MASK 0x1c0
#define RF_2069_GE16_LPF_MAIN_CONTROLS_lpf_rc_bw_SHIFT 6
#define RF_2069_GE16_LOGEN2G_CFG2_tx_pu_MASK          0x8
#define RF_2069_GE16_LOGEN2G_CFG2_tx_pu_SHIFT         3
#define RF_2069_GE16_PAD2G_IDAC_pad2g_idac_main_MASK  0x3f
#define RF_2069_GE16_PAD2G_IDAC_pad2g_idac_main_SHIFT 0
#define RF_2069_GE16_IQCAL_CFG1_PU_iqcal_MASK         0x2
#define RF_2069_GE16_IQCAL_CFG1_PU_iqcal_SHIFT        1
#define RF_2069_GE16_PA2G_IDAC1_pa2g_idac_main_MASK   0x3f
#define RF_2069_GE16_PA2G_IDAC1_pa2g_idac_main_SHIFT  0
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_high_MASK 0x80
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Refctrl_high_SHIFT 7
#define RF_2069_GE16_OVR11_ovr_lpf_bq1_gain_MASK      0x2
#define RF_2069_GE16_OVR11_ovr_lpf_bq1_gain_SHIFT     1
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_adc_MASK 0x8
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_adc_SHIFT 3
#define RF_2069_GE16_TX2G_CFG1_tx2g_bias_pu_MASK      0x2
#define RF_2069_GE16_TX2G_CFG1_tx2g_bias_pu_SHIFT     1
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_itx_MASK  0x4
#define RF_2069_GE16_OVR28_ovr_logen5g_idac_itx_SHIFT 2
#define RF_2069_GE16_OVR20_ovr_tx2g_calpath_pa_pu_MASK 0x10
#define RF_2069_GE16_OVR20_ovr_tx2g_calpath_pa_pu_SHIFT 4
#define RF_2069_GE16_LNA5G_CFG1_pwrsw_en_MASK         0x40
#define RF_2069_GE16_LNA5G_CFG1_pwrsw_en_SHIFT        6
#define RF_2069_GE16_LNA5G_TUNE_fctl1_MASK            0xf
#define RF_2069_GE16_LNA5G_TUNE_fctl1_SHIFT           0
#define RF_2069_GE16_TX5G_TSSI_pa5g_TSSI_range_MASK   0x2
#define RF_2069_GE16_TX5G_TSSI_pa5g_TSSI_range_SHIFT  1
#define RF_2069_GE16_PAD2G_INCAP_pad2g_idac_incap_compen_main_MASK 0xf
#define RF_2069_GE16_PAD2G_INCAP_pad2g_idac_incap_compen_main_SHIFT 0
#define RF_2069_GE16_OVR11_ovr_lpf_sw_dac_bq2_MASK    0x8000
#define RF_2069_GE16_OVR11_ovr_lpf_sw_dac_bq2_SHIFT   15
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_lpf_MASK 0x200
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_lpf_SHIFT 9
#define RF_2069_GE16_LNA5G_TUNE_fctl2_MASK            0xf0
#define RF_2069_GE16_LNA5G_TUNE_fctl2_SHIFT           4
#define RF_2069_GE16_PA2G_CFG3_pa2g_ptat_slope_cas_MASK 0xf0
#define RF_2069_GE16_PA2G_CFG3_pa2g_ptat_slope_cas_SHIFT 4
#define RF_2069_GE16_PGA2G_IDAC_pga2g_idac_aux_MASK   0x3f00
#define RF_2069_GE16_PGA2G_IDAC_pga2g_idac_aux_SHIFT  8
#define RF_2069_GE16_OVR20_ovr_tia_PU_bias_MASK       0x100
#define RF_2069_GE16_OVR20_ovr_tia_PU_bias_SHIFT      8
#define RF_2069_GE16_ADC_CALCODE27_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE27_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_TXMIX5G_CFG1_lodc_MASK           0xf0
#define RF_2069_GE16_TXMIX5G_CFG1_lodc_SHIFT          4
#define RF_2069_GE16_OVR7_ovr_lna5g_lna1_lowpwr_MASK  0x400
#define RF_2069_GE16_OVR7_ovr_lna5g_lna1_lowpwr_SHIFT 10
#define RF_2069_GE16_ADC_CALCODE3_Ich_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE3_Ich_flash_calcode_SHIFT 0
#define RF_2069_GE16_ADC_CALCODE20_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE20_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_ADC_CALCODE19_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE19_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_IQCAL_CFG1_sel_ext_tssi_MASK     0x4
#define RF_2069_GE16_IQCAL_CFG1_sel_ext_tssi_SHIFT    2
#define RF_2069_GE16_TIA_CFG2_opamp_Cc_MASK           0xc00
#define RF_2069_GE16_TIA_CFG2_opamp_Cc_SHIFT          10
#define RF_2069_GE16_OVR22_ovr_rxrf5g_pwrsw_lna5g_en_MASK 0x20
#define RF_2069_GE16_OVR22_ovr_rxrf5g_pwrsw_lna5g_en_SHIFT 5
#define RF_2069_GE16_LNA5G_TUNE_tx_tune_MASK          0xf00
#define RF_2069_GE16_LNA5G_TUNE_tx_tune_SHIFT         8
#define RF_2069_GE16_TXRX5G_CAL_TX_i_cal_pa_atten_5g_MASK 0x18
#define RF_2069_GE16_TXRX5G_CAL_TX_i_cal_pa_atten_5g_SHIFT 3
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_cal_MASK 0x8
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi1_cal_SHIFT 3
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_papdcal_pu_MASK 0x100
#define RF_2069_GE16_TXRX2G_CAL_RX_loopback2g_papdcal_pu_SHIFT 8
#define RF_2069_GE16_OVR5_ovr_testbuf_PU_MASK         0x1000
#define RF_2069_GE16_OVR5_ovr_testbuf_PU_SHIFT        12
#define RF_2069_GE16_OVR25_ovr_tia_DC_Ib1_MASK        0x20
#define RF_2069_GE16_OVR25_ovr_tia_DC_Ib1_SHIFT       5
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_15_13_MASK    0xe000
#define RF_2069_GE16_ADC_RC1_adc_ctl_RC_15_13_SHIFT   13
#define RF_2069_GE16_OVR25_ovr_tia_DC_Ib2_MASK        0x10
#define RF_2069_GE16_OVR25_ovr_tia_DC_Ib2_SHIFT       4
#define RF_2069_GE16_OVR14_ovr_pad2g_bias_filter_bypass_MASK 0x200
#define RF_2069_GE16_OVR14_ovr_pad2g_bias_filter_bypass_SHIFT 9
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_pu_MASK    0x1
#define RF_2069_GE16_LNA2G_RSSI_dig_wrssi1_pu_SHIFT   0
#define RF_2069_GE16_RXMIX2G_CFG1_LO_bias_sel_MASK    0x4
#define RF_2069_GE16_RXMIX2G_CFG1_LO_bias_sel_SHIFT   2
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refhigh_puI_MASK 0x40
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refhigh_puI_SHIFT 6
#define RF_2069_GE16_IQCAL_CFG2_tssi_cm_center_MASK   0x300
#define RF_2069_GE16_IQCAL_CFG2_tssi_cm_center_SHIFT  8
#define RF_2069_GE16_PGA2G_INCAP_pga2g_dac_incap_compen_MASK 0xf
#define RF_2069_GE16_PGA2G_INCAP_pga2g_dac_incap_compen_SHIFT 0
#define RF_2069_GE16_OVR13_ovr_pa5g_bias_cas_pu_MASK  0x2
#define RF_2069_GE16_OVR13_ovr_pa5g_bias_cas_pu_SHIFT 1
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_rc_MASK 0x100
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_bq2_rc_SHIFT 8
#define RF_2069_GE16_TXRX5G_CAL_TX_i_calPath_pa_pu_5g_MASK 0x20
#define RF_2069_GE16_TXRX5G_CAL_TX_i_calPath_pa_pu_5g_SHIFT 5
#define RF_2069_GE16_TXGM_LOFT_COARSE_Q_idac_lo_rf_qn_MASK 0xf0
#define RF_2069_GE16_TXGM_LOFT_COARSE_Q_idac_lo_rf_qn_SHIFT 4
#define RF_2069_GE16_OVR19_ovr_tempsense_sel_Vbe_Vbg_MASK 0x2
#define RF_2069_GE16_OVR19_ovr_tempsense_sel_Vbe_Vbg_SHIFT 1
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_irx_MASK      0x7
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_irx_SHIFT     0
#define RF_2069_GE16_TXMIX5G_CFG1_tune_MASK           0xf00
#define RF_2069_GE16_TXMIX5G_CFG1_tune_SHIFT          8
#define RF_2069_GE16_TXGM_LOFT_COARSE_Q_idac_lo_rf_qp_MASK 0xf
#define RF_2069_GE16_TXGM_LOFT_COARSE_Q_idac_lo_rf_qp_SHIFT 0
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_MASK    0xf000
#define RF_2069_GE16_TRSW5G_CFG1_trsw5g_cntrl_SHIFT   12
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_rx_attn_MASK 0x6
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_rx_attn_SHIFT 1
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_filter_aux_MASK 0xf0
#define RF_2069_GE16_PA5G_CFG2_pa5g_bias_filter_aux_SHIFT 4
#define RF_2069_GE16_OVR21_ovr_txmix2g_gc_load_MASK   0x8
#define RF_2069_GE16_OVR21_ovr_txmix2g_gc_load_SHIFT  3
#define RF_2069_GE16_RXMIX2G_IDAC_main_bias_idac_MASK 0xf
#define RF_2069_GE16_RXMIX2G_IDAC_main_bias_idac_SHIFT 0
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_LowIF_dis_MASK 0x1
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_LowIF_dis_SHIFT 0
#define RF_2069_GE16_ADC_CALCODE24_i_wrf_jtag_afe_iqadc_Qch_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE24_i_wrf_jtag_afe_iqadc_Qch_cal_code_SHIFT 0
#define RF_2069_GE16_TIA_CFG1_sel_5G_2G_MASK          0x10
#define RF_2069_GE16_TIA_CFG1_sel_5G_2G_SHIFT         4
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refhigh_puQ_MASK 0x80
#define RF_2069_GE16_OVR24_ovr_nbrssi_Refhigh_puQ_SHIFT 7
#define RF_2069_GE16_OVR19_ovr_rxmix2g_gm_size_MASK   0x1000
#define RF_2069_GE16_OVR19_ovr_rxmix2g_gm_size_SHIFT  12
#define RF_2069_GE16_ADC_CALCODE16_i_wrf_jtag_afe_iqadc_Ich_cal_code_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE16_i_wrf_jtag_afe_iqadc_Ich_cal_code_SHIFT 0
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_vcm_ctrl_MASK 0x30
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_vcm_ctrl_SHIFT 4
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_lpf_MASK 0x2
#define RF_2069_GE16_LNA5G_RSSI_ANA_lna5g_ana_wrssi1_lpf_SHIFT 1
#define RF_2069_GE16_PAD5G_CFG1_vcas_monitor_sw_MASK  0x4
#define RF_2069_GE16_PAD5G_CFG1_vcas_monitor_sw_SHIFT 2
#define RF_2069_GE16_IQCAL_CFG2_sel_nbw_iqcal_MASK    0x2
#define RF_2069_GE16_IQCAL_CFG2_sel_nbw_iqcal_SHIFT   1
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refhigh_puI_MASK 0x1
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refhigh_puI_SHIFT 0
#define RF_2069_GE16_OVR21_ovr_mix5g_gainboost_MASK   0x2
#define RF_2069_GE16_OVR21_ovr_mix5g_gainboost_SHIFT  1
#define RF_2069_GE16_OVR19_ovr_rx2g_calpath_mix_pu_MASK 0x8000
#define RF_2069_GE16_OVR19_ovr_rx2g_calpath_mix_pu_SHIFT 15
#define RF_2069_GE16_OVR10_ovr_lpf_sw_dac_adc_MASK    0x1
#define RF_2069_GE16_OVR10_ovr_lpf_sw_dac_adc_SHIFT   0
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_pu_MASK 0x80
#define RF_2069_GE16_TXRX5G_CAL_RX_loopback5g_rxiqcal_pu_SHIFT 7
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_gm_size_MASK 0x10
#define RF_2069_GE16_OVR6_ovr_lna2g_lna2_gm_size_SHIFT 4
#define RF_2069_GE16_PGA5G_CFG2_tune_MASK             0xf000
#define RF_2069_GE16_PGA5G_CFG2_tune_SHIFT            12
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_main_MASK 0xf0
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_main_SHIFT 4
#define RF_2069_GE16_LNA5G_CFG1_FB_EN_MASK            0x8
#define RF_2069_GE16_LNA5G_CFG1_FB_EN_SHIFT           3
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_aux_MASK 0xf00
#define RF_2069_GE16_PGA2G_CFG2_pga2g_ptat_slope_aux_SHIFT 8
#define RF_2069_GE16_OVR11_ovr_lpf_sw_tia_bq1_MASK    0x1000
#define RF_2069_GE16_OVR11_ovr_lpf_sw_tia_bq1_SHIFT   12
#define RF_2069_GE16_RXRF5G_CFG1_pwrsw_en_MASK        0x8
#define RF_2069_GE16_RXRF5G_CFG1_pwrsw_en_SHIFT       3
#define RF_2069_GE16_ADC_CFG2_afe_pwrup_clk_MASK      0x80
#define RF_2069_GE16_ADC_CFG2_afe_pwrup_clk_SHIFT     7
#define RF_2069_GE16_LOGEN2G_CFG2_reset_rx_MASK       0x2
#define RF_2069_GE16_LOGEN2G_CFG2_reset_rx_SHIFT      1
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_Refladder_MASK 0xe000
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_ib_Refladder_SHIFT 13
#define RF_2069_GE16_PA5G_INCAP_pa5g_idac_incap_compen_aux_MASK 0xf00
#define RF_2069_GE16_PA5G_INCAP_pa5g_idac_incap_compen_aux_SHIFT 8
#define RF_2069_GE16_OVR22_ovr_trsw2g_cntrl_MASK      0x1000
#define RF_2069_GE16_OVR22_ovr_trsw2g_cntrl_SHIFT     12
#define RF_2069_GE16_LNA2G_TUNE_lna1_avdd_cap_MASK    0xc000
#define RF_2069_GE16_LNA2G_TUNE_lna1_avdd_cap_SHIFT   14
#define RF_2069_GE16_LNA5G_CFG2_ana_wrssi1_pu_MASK    0x2
#define RF_2069_GE16_LNA5G_CFG2_ana_wrssi1_pu_SHIFT   1
#define RF_2069_GE16_TX5G_TSSI_pa5g_tssi_pu_MASK      0x1
#define RF_2069_GE16_TX5G_TSSI_pa5g_tssi_pu_SHIFT     0
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl1_MASK        0x4000
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl1_SHIFT       14
#define RF_2069_GE16_OVR11_ovr_lpf_rc_bw_MASK         0x4
#define RF_2069_GE16_OVR11_ovr_lpf_rc_bw_SHIFT        2
#define RF_2069_GE16_ADC_CFG2_iqadc_reset_ov_det_MASK 0x1
#define RF_2069_GE16_ADC_CFG2_iqadc_reset_ov_det_SHIFT 0
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl2_MASK        0x1000
#define RF_2069_GE16_OVR7_ovr_lna5g_gctl2_SHIFT       12
#define RF_2069_GE16_OVR5_ovr_bias_rx_pu_MASK         0x200
#define RF_2069_GE16_OVR5_ovr_bias_rx_pu_SHIFT        9
#define RF_2069_GE16_OVR19_ovr_rxrf2g_pwrsw_en_MASK   0x20
#define RF_2069_GE16_OVR19_ovr_rxrf2g_pwrsw_en_SHIFT  5
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_calcode_Ich_MASK 0x20
#define RF_2069_GE16_OVR3_ovr_afe_iqadc_flash_calcode_Ich_SHIFT 5
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refhigh_puQ_MASK 0x2
#define RF_2069_GE16_OVR24_ovr_wrssi3_Refhigh_puQ_SHIFT 1
#define RF_2069_GE16_OVR13_ovr_lpf_dc_hold_MASK       0x8000
#define RF_2069_GE16_OVR13_ovr_lpf_dc_hold_SHIFT      15
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_19_18_MASK   0xc
#define RF_2069_GE16_ADC_RC2_adc_ctrl_RC_19_18_SHIFT  2
#define RF_2069_GE16_ADC_CFG4_Ich_run_flashcal_MASK   0x2
#define RF_2069_GE16_ADC_CFG4_Ich_run_flashcal_SHIFT  1
#define RF_2069_GE16_LNA2G_CFG2_lna2_gain_MASK        0x700
#define RF_2069_GE16_LNA2G_CFG2_lna2_gain_SHIFT       8
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_trsw_sub_bis_MASK 0x1e0
#define RF_2069_GE16_TRSW2G_CFG3_trsw2g_trsw_sub_bis_SHIFT 5
#define RF_2069_GE16_PGA5G_IDAC_idac_main_MASK        0x3f
#define RF_2069_GE16_PGA5G_IDAC_idac_main_SHIFT       0
#define RF_2069_GE16_OVR19_ovr_tempsense_swap_amp_MASK 0x1
#define RF_2069_GE16_OVR19_ovr_tempsense_swap_amp_SHIFT 0
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_itx_MASK      0x700
#define RF_2069_GE16_LOGEN5G_IDAC4_idac_itx_SHIFT     8
#define RF_2069_GE16_PGA5G_CFG1_idac_boost_MASK       0xf00
#define RF_2069_GE16_PGA5G_CFG1_idac_boost_SHIFT      8
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_Qch_MASK 0x400
#define RF_2069_GE16_OVR4_ovr_afe_iqadc_reset_Qch_SHIFT 10
#define RF_2069_GE16_TRSW5G_CFG2_trsw5g_trsw_dwell_bis_MASK 0xf0
#define RF_2069_GE16_TRSW5G_CFG2_trsw5g_trsw_dwell_bis_SHIFT 4
#define RF_2069_GE16_RXMIX2G_IDAC_main_bias_ptat_MASK 0x700
#define RF_2069_GE16_RXMIX2G_IDAC_main_bias_ptat_SHIFT 8
#define RF_2069_GE16_PA2G_TSSI_pa2g_ctrl_tssi_MASK    0xf0
#define RF_2069_GE16_PA2G_TSSI_pa2g_ctrl_tssi_SHIFT   4
#define RF_2069_GE16_VBAT_CFG_bw_MASK                 0xf0
#define RF_2069_GE16_VBAT_CFG_bw_SHIFT                4
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_adc_MASK 0x200
#define RF_2069_GE16_LPF_MUX_SWITCHES_lpf_sw_dac_adc_SHIFT 9
#define RF_2069_GE16_ADC_CALCODE8_Qch_flash_calcode_MASK 0xffff
#define RF_2069_GE16_ADC_CALCODE8_Qch_flash_calcode_SHIFT 0
#define RF_2069_GE16_LNA2G_CFG1_bias_reset_MASK       0x10
#define RF_2069_GE16_LNA2G_CFG1_bias_reset_SHIFT      4
#define RF_2069_GE16_OVR4_ovr_afe_rst_clk_MASK        0x10
#define RF_2069_GE16_OVR4_ovr_afe_rst_clk_SHIFT       4
#define RF_2069_GE16_OVR21_ovr_tx5g_calpath_pa_pu_MASK 0x1000
#define RF_2069_GE16_OVR21_ovr_tx5g_calpath_pa_pu_SHIFT 12
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Reflow_puI_MASK 0x10
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Reflow_puI_SHIFT 4
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_ff_mult_MASK 0xc0
#define RF_2069_GE16_LPF_CMFB_AND_FEEDFORWARD_BIAS_MULTIPLIERS_lpf_bias_bq1_ff_mult_SHIFT 6
#define RF_2069_GE16_TX2G_CFG1_pga2g_pu_MASK          0x4
#define RF_2069_GE16_TX2G_CFG1_pga2g_pu_SHIFT         2
#define RF_2069_GE16_RXMIX2G_CFG1_LO_bias_MASK        0x70
#define RF_2069_GE16_RXMIX2G_CFG1_LO_bias_SHIFT       4
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_reset_MASK   0x8
#define RF_2069_GE16_PA5G_CFG1_pa5g_bias_reset_SHIFT  3
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_gain_MASK 0x3000
#define RF_2069_GE16_LNA2G_RSSI_ANA_ana_wrssi2_gain_SHIFT 12
#define RF_2069_GE16_TIA_CFG1_pu_MASK                 0x1
#define RF_2069_GE16_TIA_CFG1_pu_SHIFT                0
#define RF_2069_GE16_PA5G_INCAP_pa5g_idac_incap_compen_main_MASK 0xf
#define RF_2069_GE16_PA5G_INCAP_pa5g_idac_incap_compen_main_SHIFT 0
#define RF_2069_GE16_OVR3_ovr_afe_ctrl_flash17lvl_MASK 0x2
#define RF_2069_GE16_OVR3_ovr_afe_ctrl_flash17lvl_SHIFT 1
#define RF_2069_GE16_ADC_CFG4_iqadc_flash_only_MASK   0x400
#define RF_2069_GE16_ADC_CFG4_iqadc_flash_only_SHIFT  10
#define RF_2069_GE16_DAC_CFG2_DACbuf_Cc_MASK          0x3
#define RF_2069_GE16_DAC_CFG2_DACbuf_Cc_SHIFT         0
#define RF_2069_GE16_OVR25_ovr_lna5g_lna1_bias_idac_MASK 0x1000
#define RF_2069_GE16_OVR25_ovr_lna5g_lna1_bias_idac_SHIFT 12
#define RF_2069_GE16_OVR10_ovr_logen5g_rx_pu_MASK     0x100
#define RF_2069_GE16_OVR10_ovr_logen5g_rx_pu_SHIFT    8
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Reflow_puI_MASK 0x10
#define RF_2069_GE16_WRSSI3_CONFG_wrssi3_Reflow_puI_SHIFT 4
#define RF_2069_GE16_LOGEN2G_CFG2_reset_tx_MASK       0x1
#define RF_2069_GE16_LOGEN2G_CFG2_reset_tx_SHIFT      0
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_bias_ctrl_MASK 0x3000
#define RF_2069_GE16_AUXPGA_CFG1_auxpga_i_bias_ctrl_SHIFT 12
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_sel_test_MASK 0xf00
#define RF_2069_GE16_WRSSI3_TEST_wrssi3_sel_test_SHIFT 8
#define RF_2069_GE16_RXRF2G_CFG2_lna2g_epapd_attn_MASK 0xf
#define RF_2069_GE16_RXRF2G_CFG2_lna2g_epapd_attn_SHIFT 0
#define RF_2069_GE16_OVR21_ovr_txmix2g_pu_MASK        0x4
#define RF_2069_GE16_OVR21_ovr_txmix2g_pu_SHIFT       2
#define RF_2069_GE16_LNA5G_CFG1_lna1_pu_MASK          0x1
#define RF_2069_GE16_LNA5G_CFG1_lna1_pu_SHIFT         0
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_cas_MASK     0xff00
#define RF_2069_GE16_PA2G_CFG2_pa2g_bias_cas_SHIFT    8
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refmid_puI_MASK 0x4
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Refmid_puI_SHIFT 2
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Reflow_puQ_MASK 0x20
#define RF_2069_GE16_NBRSSI_CONFG_nbrssi_Reflow_puQ_SHIFT 5
#define RF_2069_GE16_PLL_VCOCAL5_rfpll_vcocal_delayAfterRefresh_MASK 0xff00
#define RF_2069_GE16_PLL_VCOCAL5_rfpll_vcocal_delayAfterRefresh_SHIFT 8
#define RF_2069_GE16_OVR16_ovr_rfpll_cp_pu_MASK       0x2
#define RF_2069_GE16_OVR16_ovr_rfpll_cp_pu_SHIFT      1
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_div_MASK  0x1000
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_div_SHIFT 12
#define RF_2069_GE16_TOP_SPARE2_spare2_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE2_spare2_SHIFT          0
#define RF_2069_GE16_OVR18_ovr_afediv_pu0_inbuf_MASK  0x8
#define RF_2069_GE16_OVR18_ovr_afediv_pu0_inbuf_SHIFT 3
#define RF_2069_GE16_RCAL_CFG_i_wrf_jtag_rcal_valid_MASK 0x8
#define RF_2069_GE16_RCAL_CFG_i_wrf_jtag_rcal_valid_SHIFT 3
#define RF_2069_GE16_PLL_VCOCAL3_rfpll_vcocal_compLevelIn_MASK 0x3
#define RF_2069_GE16_PLL_VCOCAL3_rfpll_vcocal_compLevelIn_SHIFT 0
#define RF_2069_GE16_RCAL_CFG_div_cntrl_MASK          0x6
#define RF_2069_GE16_RCAL_CFG_div_cntrl_SHIFT         1
#define RF_2069_GE16_CGPAIO_CFG4_cgpaio_tssi_muxsel0_MASK 0xff00
#define RF_2069_GE16_CGPAIO_CFG4_cgpaio_tssi_muxsel0_SHIFT 8
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_stop_mod_MASK 0x1
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_stop_mod_SHIFT 0
#define RF_2069_GE16_CGPAIO_CFG4_cgpaio_tssi_muxsel1_MASK 0xff
#define RF_2069_GE16_CGPAIO_CFG4_cgpaio_tssi_muxsel1_SHIFT 0
#define RF_2069_GE16_LOGEN5G_IDAC2_tc_ptat_MASK       0x7
#define RF_2069_GE16_LOGEN5G_IDAC2_tc_ptat_SHIFT      0
#define RF_2069_GE16_PLL_CP1_rfpll_cp_i_spur_cancel_MASK 0xf
#define RF_2069_GE16_PLL_CP1_rfpll_cp_i_spur_cancel_SHIFT 0
#define RF_2069_GE16_AFEDIV1_afediv_cal_driver_size_MASK 0x380
#define RF_2069_GE16_AFEDIV1_afediv_cal_driver_size_SHIFT 7
#define RF_2069_GE16_PLL_CP4_rfpll_cp_op_int_on_MASK  0x1
#define RF_2069_GE16_PLL_CP4_rfpll_cp_op_int_on_SHIFT 0
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowpwren_VCO_aux_MASK 0x8
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowpwren_VCO_aux_SHIFT 3
#define RF_2069_GE16_PMU_OP_TXldo_bypass_MASK         0x200
#define RF_2069_GE16_PMU_OP_TXldo_bypass_SHIFT        9
#define RF_2069_GE16_PLL_CFG6_rfpll_spare6_MASK       0xff00
#define RF_2069_GE16_PLL_CFG6_rfpll_spare6_SHIFT      8
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_cal_MASK 0x2000
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_cal_SHIFT 13
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_VCO_aux_vout_sel_MASK 0xf0
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_VCO_aux_vout_sel_SHIFT 4
#define RF_2069_GE16_PLL_CFG6_rfpll_spare7_MASK       0xff
#define RF_2069_GE16_PLL_CFG6_rfpll_spare7_SHIFT      0
#define RF_2069_GE16_PMU_CFG3_mancodes_MASK           0xf
#define RF_2069_GE16_PMU_CFG3_mancodes_SHIFT          0
#define RF_2069_GE16_AFEDIV3_afediv_pu3_nfc_MASK      0x1
#define RF_2069_GE16_AFEDIV3_afediv_pu3_nfc_SHIFT     0
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_cvar_extra_MASK 0x1e00
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_cvar_extra_SHIFT 9
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_USE_2p5V_MASK 0x80
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_USE_2p5V_SHIFT 7
#define RF_2069_GE16_PLL_VCOCAL12_rfpll_vcocal_ref_value_MASK 0xffff
#define RF_2069_GE16_PLL_VCOCAL12_rfpll_vcocal_ref_value_SHIFT 0
#define RF_2069_GE16_OVR33_ovr_rfpll_vcoldo_iso_dis_MASK 0x800
#define RF_2069_GE16_OVR33_ovr_rfpll_vcoldo_iso_dis_SHIFT 11
#define RF_2069_GE16_PMU_OP_synth_pwrsw_en_MASK       0x20
#define RF_2069_GE16_PMU_OP_synth_pwrsw_en_SHIFT      5
#define RF_2069_GE16_PMU_OP_vref_select_MASK          0x8000
#define RF_2069_GE16_PMU_OP_vref_select_SHIFT         15
#define RF_2069_GE16_PMU_CFG1_vrefadj_cbuck_MASK      0xf00
#define RF_2069_GE16_PMU_CFG1_vrefadj_cbuck_SHIFT     8
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_BT_MASK        0x100
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_BT_SHIFT       8
#define RF_2069_GE16_LOGEN5G_CFG1_reset_MASK          0x20
#define RF_2069_GE16_LOGEN5G_CFG1_reset_SHIFT         5
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_tc_ptat_MASK 0x7
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_tc_ptat_SHIFT 0
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_RCCAL_MASK 0x7
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_RCCAL_SHIFT 0
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_BT_MASK       0x6000
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_BT_SHIFT      13
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ib_bias_opamp_fastsettle_MASK 0xf
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ib_bias_opamp_fastsettle_SHIFT 0
#define RF_2069_GE16_PLL_CP3_rfpll_cp_idump_offset_up_MASK 0xf
#define RF_2069_GE16_PLL_CP3_rfpll_cp_idump_offset_up_SHIFT 0
#define RF_2069_GE16_PLL_CFG1_rfpll_vco_buf_pu_MASK   0x800
#define RF_2069_GE16_PLL_CFG1_rfpll_vco_buf_pu_SHIFT  11
#define RF_2069_GE16_PLL_VCOCAL11_rfpll_vcocal_ref_value_MASK 0xfff
#define RF_2069_GE16_PLL_VCOCAL11_rfpll_vcocal_ref_value_SHIFT 0
#define RF_2069_GE16_PLL_CFG1_rfpll_synth_pu_MASK     0x200
#define RF_2069_GE16_PLL_CFG1_rfpll_synth_pu_SHIFT    9
#define RF_2069_GE16_LOGEN5G_TUNE1_ctune_mix_MASK     0xf0
#define RF_2069_GE16_LOGEN5G_TUNE1_ctune_mix_SHIFT    4
#define RF_2069_GE16_PLL_VCOCAL4_rfpll_vcocal_delayAfterCloseLoop_MASK 0xff00
#define RF_2069_GE16_PLL_VCOCAL4_rfpll_vcocal_delayAfterCloseLoop_SHIFT 8
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_1p2_MASK 0x7800
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_1p2_SHIFT 11
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_tempco_dcadj_MASK 0x20
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_tempco_dcadj_SHIFT 5
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_EN_DEGEN_ovrVal_MASK 0x1000
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_EN_DEGEN_ovrVal_SHIFT 12
#define RF_2069_GE16_OVR23_ovr_xtal_coresize_pmos_MASK 0x200
#define RF_2069_GE16_OVR23_ovr_xtal_coresize_pmos_SHIFT 9
#define RF_2069_GE16_PLL_VCOCAL14_rfpll_vcocal_dump_count_reg_MASK 0xff
#define RF_2069_GE16_PLL_VCOCAL14_rfpll_vcocal_dump_count_reg_SHIFT 0
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_div_MASK      0x70
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_div_SHIFT     4
#define RF_2069_GE16_PLL_CP3_rfpll_cp_idn_integer_MASK 0x3f0
#define RF_2069_GE16_PLL_CP3_rfpll_cp_idn_integer_SHIFT 4
#define RF_2069_GE16_PLL_MONITOR1_rfpll_monitor_vth_high_MASK 0xf0
#define RF_2069_GE16_PLL_MONITOR1_rfpll_monitor_vth_high_SHIFT 4
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowpwren_VCO_MASK 0x20
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowpwren_VCO_SHIFT 5
#define RF_2069_GE16_LOGEN2G_CFG1_logen_pu_MASK       0x1
#define RF_2069_GE16_LOGEN2G_CFG1_logen_pu_SHIFT      0
#define RF_2069_GE16_PLL_XTAL3_xtal_xtal_swcap_out_MASK 0xf
#define RF_2069_GE16_PLL_XTAL3_xtal_xtal_swcap_out_SHIFT 0
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_mixb_MASK 0x200
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_mixb_SHIFT 9
#define RF_2069_GE16_PMU_OP_ldoref_start_cal_MASK     0x4000
#define RF_2069_GE16_PMU_OP_ldoref_start_cal_SHIFT    14
#define RF_2069_GE16_LOGEN2G_CFG1_ocl_ctrl_MASK       0x80
#define RF_2069_GE16_LOGEN2G_CFG1_ocl_ctrl_SHIFT      7
#define RF_2069_GE16_OVR2_ovr_bg_pulse_MASK           0x20
#define RF_2069_GE16_OVR2_ovr_bg_pulse_SHIFT          5
#define RF_2069_GE16_OVR9_ovr_logen5g_lob2_pu_MASK    0x80
#define RF_2069_GE16_OVR9_ovr_logen5g_lob2_pu_SHIFT   7
#define RF_2069_GE16_PLL_CFG5_rfpll_spare4_MASK       0xff00
#define RF_2069_GE16_PLL_CFG5_rfpll_spare4_SHIFT      8
#define RF_2069_GE16_OVR15_ovr_rfpll_bias_reset_vco_MASK 0x2
#define RF_2069_GE16_OVR15_ovr_rfpll_bias_reset_vco_SHIFT 1
#define RF_2069_GE16_RCCAL_LOGIC2_rccal_adc_code_MASK 0xf
#define RF_2069_GE16_RCCAL_LOGIC2_rccal_adc_code_SHIFT 0
#define RF_2069_GE16_OVR18_ovr_afediv_sel_div_MASK    0x10
#define RF_2069_GE16_OVR18_ovr_afediv_sel_div_SHIFT   4
#define RF_2069_GE16_PLL_VCOCAL7_rfpll_vcocal_force_caps_val_MASK 0x7ff
#define RF_2069_GE16_PLL_VCOCAL7_rfpll_vcocal_force_caps_val_SHIFT 0
#define RF_2069_GE16_PLL_CFG5_rfpll_spare5_MASK       0xff
#define RF_2069_GE16_PLL_CFG5_rfpll_spare5_SHIFT      0
#define RF_2069_GE16_PLL_LF5_rfpll_lf_lf_r3_MASK      0xff
#define RF_2069_GE16_PLL_LF5_rfpll_lf_lf_r3_SHIFT     0
#define RF_2069_GE16_AFEDIV2_afediv_pu_repeater1_MASK 0x200
#define RF_2069_GE16_AFEDIV2_afediv_pu_repeater1_SHIFT 9
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_gm2nd_MASK 0xe00
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_gm2nd_SHIFT 9
#define RF_2069_GE16_AFEDIV2_afediv_pu_repeater2_MASK 0x10
#define RF_2069_GE16_AFEDIV2_afediv_pu_repeater2_SHIFT 4
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idac_op_int_MASK 0xf0
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idac_op_int_SHIFT 4
#define RF_2069_GE16_PMU_STAT_wlpmu_zcode_MASK        0xf0
#define RF_2069_GE16_PMU_STAT_wlpmu_zcode_SHIFT       4
#define RF_2069_GE16_PLL_XTAL4_xtal_doubler_delay_MASK 0xc00
#define RF_2069_GE16_PLL_XTAL4_xtal_doubler_delay_SHIFT 10
#define RF_2069_GE16_PMU_CFG2_VCOldo_adj_MASK         0x700
#define RF_2069_GE16_PMU_CFG2_VCOldo_adj_SHIFT        8
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_EN_DEGEN_ovr_MASK 0x800
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_EN_DEGEN_ovr_SHIFT 11
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_por_MASK     0x8
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_por_SHIFT    3
#define RF_2069_GE16_OVR9_ovr_logen5g_reset_MASK      0x200
#define RF_2069_GE16_OVR9_ovr_logen5g_reset_SHIFT     9
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_gm_MASK 0x7000
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_gm_SHIFT 12
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_en_alc_MASK   0x100
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_en_alc_SHIFT  8
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_vctrl_tp_MASK 0x10
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_vctrl_tp_SHIFT 4
#define RF_2069_GE16_OVR33_reg_wlpmu_txldo_iso_dis_MASK 0x2
#define RF_2069_GE16_OVR33_reg_wlpmu_txldo_iso_dis_SHIFT 1
#define RF_2069_GE16_OVR17_ovr_rfpll_vcocal_rstn_MASK 0x800
#define RF_2069_GE16_OVR17_ovr_rfpll_vcocal_rstn_SHIFT 11
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ALC_ref_ctrl_MASK 0xf00
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ALC_ref_ctrl_SHIFT 8
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_en_dac_p_MASK 0x8
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_en_dac_p_SHIFT 3
#define RF_2069_GE16_PLL_LF3_rfpll_lf_lf_c3_MASK      0xff00
#define RF_2069_GE16_PLL_LF3_rfpll_lf_lf_c3_SHIFT     8
#define RF_2069_GE16_CGPAIO_CFG5_cgpaio_tssi_muxsel2_MASK 0xff00
#define RF_2069_GE16_CGPAIO_CFG5_cgpaio_tssi_muxsel2_SHIFT 8
#define RF_2069_GE16_PLL_LF3_rfpll_lf_lf_c4_MASK      0xff
#define RF_2069_GE16_PLL_LF3_rfpll_lf_lf_c4_SHIFT     0
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_hold_vctrl_ovr_MASK 0x8
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_hold_vctrl_ovr_SHIFT 3
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_ref_val_MASK 0x3
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_ref_val_SHIFT 0
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_spur_cancel_MASK 0x10
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_spur_cancel_SHIFT 4
#define RF_2069_GE16_AFEDIV2_afediv_repeater2_dsize_MASK 0xf
#define RF_2069_GE16_AFEDIV2_afediv_repeater2_dsize_SHIFT 0
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_MASK 0x780
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_ib_ctrl_ptat_SHIFT 7
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_P1_MASK       0xc0
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_P1_SHIFT      6
#define RF_2069_GE16_PLL_XTAL6_xtal_bufstrg_NFC_MASK  0xff
#define RF_2069_GE16_PLL_XTAL6_xtal_bufstrg_NFC_SHIFT 0
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufstrg_MASK   0x3
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufstrg_SHIFT  0
#define RF_2069_GE16_PLL_VCO8_rfpll_vco_vctrl_buf_device_width_MASK 0x38
#define RF_2069_GE16_PLL_VCO8_rfpll_vco_vctrl_buf_device_width_SHIFT 3
#define RF_2069_GE16_LOGEN5G_CFG1_vcobuf_pu_MASK      0x2
#define RF_2069_GE16_LOGEN5G_CFG1_vcobuf_pu_SHIFT     1
#define RF_2069_GE16_CGPAIO_CFG3_sel_16to31_port_MASK 0xffff
#define RF_2069_GE16_CGPAIO_CFG3_sel_16to31_port_SHIFT 0
#define RF_2069_GE16_BG_CFG1_bg_pulse_MASK            0x2
#define RF_2069_GE16_BG_CFG1_bg_pulse_SHIFT           1
#define RF_2069_GE16_OVR23_ovr_xtal_buf_pu_MASK       0x4000
#define RF_2069_GE16_OVR23_ovr_xtal_buf_pu_SHIFT      14
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_VCO_MASK 0x1000
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_VCO_SHIFT 12
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_gm_2nd_MASK 0x4
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_gm_2nd_SHIFT 2
#define RF_2069_GE16_PMU_CFG2_AFEldo_adj_MASK         0x7
#define RF_2069_GE16_PMU_CFG2_AFEldo_adj_SHIFT        0
#define RF_2069_GE16_PLL_CFG2_rfpll_rst_n_MASK        0x1
#define RF_2069_GE16_PLL_CFG2_rfpll_rst_n_SHIFT       0
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_tempco_MASK  0x10
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_tempco_SHIFT 4
#define RF_2069_GE16_PLL_CFG4_rfpll_spare2_MASK       0xff00
#define RF_2069_GE16_PLL_CFG4_rfpll_spare2_SHIFT      8
#define RF_2069_GE16_PLL_CFG4_rfpll_spare3_MASK       0xff
#define RF_2069_GE16_PLL_CFG4_rfpll_spare3_SHIFT      0
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_VCO_vout_sel_MASK 0xf
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_VCO_vout_sel_SHIFT 0
#define RF_2069_GE16_OVR33_ovr_wlpmu_rxldo_iso_dis_MASK 0x100
#define RF_2069_GE16_OVR33_ovr_wlpmu_rxldo_iso_dis_SHIFT 8
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_gm_MASK   0x8
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_gm_SHIFT  3
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_Q1_MASK       0x30
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_Q1_SHIFT      4
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_CP_MASK 0x20
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_CP_SHIFT 5
#define RF_2069_GE16_PMU_OP_LNLDO_bypass_MASK         0x1000
#define RF_2069_GE16_PMU_OP_LNLDO_bypass_SHIFT        12
#define RF_2069_GE16_OVR8_ovr_logen2g_logen_pu_MASK   0x2000
#define RF_2069_GE16_OVR8_ovr_logen2g_logen_pu_SHIFT  13
#define RF_2069_GE16_PLL_XTAL3_xtal_refsel_MASK       0x700
#define RF_2069_GE16_PLL_XTAL3_xtal_refsel_SHIFT      8
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_bufb1_MASK    0x7
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_bufb1_SHIFT   0
#define RF_2069_GE16_BG_TRIM2_afe_vbgtrim_MASK        0x3f
#define RF_2069_GE16_BG_TRIM2_afe_vbgtrim_SHIFT       0
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_CP_byp_MASK 0x8
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_CP_byp_SHIFT 3
#define RF_2069_GE16_OVR16_rfpll_vco_FAST_SETTLE_jtag_ovr_MASK 0x100
#define RF_2069_GE16_OVR16_rfpll_vco_FAST_SETTLE_jtag_ovr_SHIFT 8
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_bufb2_MASK    0x1c0
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_bufb2_SHIFT   6
#define RF_2069_GE16_AFEDIV2_afediv_repeater1_dsize_MASK 0x1e0
#define RF_2069_GE16_AFEDIV2_afediv_repeater1_dsize_SHIFT 5
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_refresh_cap_cnt_MASK 0x380
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_refresh_cap_cnt_SHIFT 7
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_ICAL_MASK     0x1f
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_ICAL_SHIFT    0
#define RF_2069_GE16_VREG_CFG_pup_MASK                0x1
#define RF_2069_GE16_VREG_CFG_pup_SHIFT               0
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_dcadj_1p2_MASK 0x780
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_dcadj_1p2_SHIFT 7
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_byp_MASK 0x4
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_byp_SHIFT 2
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_R1_MASK       0xc
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_R1_SHIFT      2
#define RF_2069_GE16_OVR33_ovr_wlpmu_afeldo_iso_dis_MASK 0x200
#define RF_2069_GE16_OVR33_ovr_wlpmu_afeldo_iso_dis_SHIFT 9
#define RF_2069_GE16_PMU_CFG3_seldiv_MASK             0x30
#define RF_2069_GE16_PMU_CFG3_seldiv_SHIFT            4
#define RF_2069_GE16_PLL_VCO8_rfpll_vco_vctrl_buf_ical_MASK 0x7
#define RF_2069_GE16_PLL_VCO8_rfpll_vco_vctrl_buf_ical_SHIFT 0
#define RF_2069_GE16_VREG_CFG_vout_sel_MASK           0x300
#define RF_2069_GE16_VREG_CFG_vout_sel_SHIFT          8
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_cvar_extra_MASK 0x100
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_cvar_extra_SHIFT 8
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_bias_mode_MASK 0x1000
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_bias_mode_SHIFT 12
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idn_dac_pulse_MASK 0xf
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idn_dac_pulse_SHIFT 0
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cap_mode_MASK 0x10
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cap_mode_SHIFT 4
#define RF_2069_GE16_OVR9_ovr_logen5g_lob1_pu_MASK    0x40
#define RF_2069_GE16_OVR9_ovr_logen5g_lob1_pu_SHIFT   6
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_tempco_MASK   0xf
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_tempco_SHIFT  0
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_VCO_MASK 0x8000
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_VCO_SHIFT 15
#define RF_2069_GE16_PMU_CFG3_tsten_MASK              0x100
#define RF_2069_GE16_PMU_CFG3_tsten_SHIFT             8
#define RF_2069_GE16_LOGEN5G_CFG1_logen_pu_MASK       0x1
#define RF_2069_GE16_LOGEN5G_CFG1_logen_pu_SHIFT      0
#define RF_2069_GE16_LOGEN2G_TUNE_logen2g_buftune_2nd_MASK 0xf0
#define RF_2069_GE16_LOGEN2G_TUNE_logen2g_buftune_2nd_SHIFT 4
#define RF_2069_GE16_PLL_DSPR27_rfpll_dsp_spurFreqSwtch_Rd_MASK 0x8
#define RF_2069_GE16_PLL_DSPR27_rfpll_dsp_spurFreqSwtch_Rd_SHIFT 3
#define RF_2069_GE16_PLL_CFG3_rfpll_spare0_MASK       0xff00
#define RF_2069_GE16_PLL_CFG3_rfpll_spare0_SHIFT      8
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_HSIC_MASK      0x200
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_HSIC_SHIFT     9
#define RF_2069_GE16_PLL_CFG3_rfpll_spare1_MASK       0xff
#define RF_2069_GE16_PLL_CFG3_rfpll_spare1_SHIFT      0
#define RF_2069_GE16_BG_CFG1_rcal_trim_MASK           0xf0
#define RF_2069_GE16_BG_CFG1_rcal_trim_SHIFT          4
#define RF_2069_GE16_PLL_VCOCAL14_rfpll_vcocal_done_cal_MASK 0x100
#define RF_2069_GE16_PLL_VCOCAL14_rfpll_vcocal_done_cal_SHIFT 8
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tmx_mode_MASK 0x3
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tmx_mode_SHIFT 0
#define RF_2069_GE16_PLL_VCOCAL6_rfpll_vcocal_force_caps_MASK 0x1
#define RF_2069_GE16_PLL_VCOCAL6_rfpll_vcocal_force_caps_SHIFT 0
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_kvco_ctrl_MASK 0x60
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_kvco_ctrl_SHIFT 5
#define RF_2069_GE16_AFEDIV1_afediv_pu1_main_MASK     0x1000
#define RF_2069_GE16_AFEDIV1_afediv_pu1_main_SHIFT    12
#define RF_2069_GE16_PLL_XTAL4_xtal_xt_res_bpass_MASK 0x380
#define RF_2069_GE16_PLL_XTAL4_xtal_xt_res_bpass_SHIFT 7
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cal_en_MASK   0x40
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cal_en_SHIFT  6
#define RF_2069_GE16_RCCAL_CFG_pu_MASK                0x1
#define RF_2069_GE16_RCCAL_CFG_pu_SHIFT               0
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_por_MASK      0x10
#define RF_2069_GE16_PLL_VCO4_rfpll_vco_por_SHIFT     4
#define RF_2069_GE16_PLL_LF6_rfpll_lf_cm_pu_MASK      0x1
#define RF_2069_GE16_PLL_LF6_rfpll_lf_cm_pu_SHIFT     0
#define RF_2069_GE16_PMU_CFG1_ana_mux_MASK            0x3
#define RF_2069_GE16_PMU_CFG1_ana_mux_SHIFT           0
#define RF_2069_GE16_CGPAIO_CFG2_sel_0to15_port_MASK  0xffff
#define RF_2069_GE16_CGPAIO_CFG2_sel_0to15_port_SHIFT 0
#define RF_2069_GE16_OVR2_ovr_bg_pu_MASK              0x8
#define RF_2069_GE16_OVR2_ovr_bg_pu_SHIFT             3
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_START_MASK    0x1
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_START_SHIFT   0
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_ALC_ref_ctrl_MASK 0x800
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_ALC_ref_ctrl_SHIFT 11
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_bias_reset_VCO_aux_MASK 0x40
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_bias_reset_VCO_aux_SHIFT 6
#define RF_2069_GE16_OVR16_ovr_rfpll_monitor_pu_MASK  0x2000
#define RF_2069_GE16_OVR16_ovr_rfpll_monitor_pu_SHIFT 13
#define RF_2069_GE16_OVR15_ovr_xtal_core_lp_MASK      0x2000
#define RF_2069_GE16_OVR15_ovr_xtal_core_lp_SHIFT     13
#define RF_2069_GE16_PMU_CFG1_VCOldo_load_MASK        0x4
#define RF_2069_GE16_PMU_CFG1_VCOldo_load_SHIFT       2
#define RF_2069_GE16_OVR18_ovr_afediv_pu_repeater1_MASK 0x2
#define RF_2069_GE16_OVR18_ovr_afediv_pu_repeater1_SHIFT 1
#define RF_2069_GE16_TOP_SPARE9_spare9_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE9_spare9_SHIFT          0
#define RF_2069_GE16_TOP_SPARE10_spare10_MASK         0xffff
#define RF_2069_GE16_TOP_SPARE10_spare10_SHIFT        0
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_CP_MASK 0x100
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_CP_SHIFT 8
#define RF_2069_GE16_RCCAL_CFG_sc_MASK                0x18
#define RF_2069_GE16_RCCAL_CFG_sc_SHIFT               3
#define RF_2069_GE16_OVR18_ovr_afediv_pu_repeater2_MASK 0x1
#define RF_2069_GE16_OVR18_ovr_afediv_pu_repeater2_SHIFT 0
#define RF_2069_GE16_PLL_CP5_rfpll_open_gnd_cp_lf_MASK 0x2
#define RF_2069_GE16_PLL_CP5_rfpll_open_gnd_cp_lf_SHIFT 1
#define RF_2069_GE16_PLL_CP4_rfpll_cp_ioff_MASK       0xff00
#define RF_2069_GE16_PLL_CP4_rfpll_cp_ioff_SHIFT      8
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_en_MASK 0x4
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_en_SHIFT 2
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cal_en_empco_MASK 0x20
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cal_en_empco_SHIFT 5
#define RF_2069_GE16_AFEDIV3_afediv_nfc_sel_div_MASK  0x2
#define RF_2069_GE16_AFEDIV3_afediv_nfc_sel_div_SHIFT 1
#define RF_2069_GE16_AFEDIV1_afediv_pu2_cal_MASK      0x800
#define RF_2069_GE16_AFEDIV1_afediv_pu2_cal_SHIFT     11
#define RF_2069_GE16_PLL_CP4_rfpll_cp_kpd_scale_MASK  0xfe
#define RF_2069_GE16_PLL_CP4_rfpll_cp_kpd_scale_SHIFT 1
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_bbdrv_MASK     0x20
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_bbdrv_SHIFT    5
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_cp_down_MASK 0x20
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_cp_down_SHIFT 5
#define RF_2069_GE16_OVR33_reg_wlpmu_vcoldo_iso_dis_MASK 0x1
#define RF_2069_GE16_OVR33_reg_wlpmu_vcoldo_iso_dis_SHIFT 0
#define RF_2069_GE16_RCCAL_CFG_fixed_cap_MASK         0x2000
#define RF_2069_GE16_RCCAL_CFG_fixed_cap_SHIFT        13
#define RF_2069_GE16_OVR8_ovr_logen2g_pu_MASK         0x4000
#define RF_2069_GE16_OVR8_ovr_logen2g_pu_SHIFT        14
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_load_MASK 0x100
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_load_SHIFT 8
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_noise_cancel_MASK 0x40
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_noise_cancel_SHIFT 6
#define RF_2069_GE16_OVR17_rfpll_vco_FAST_SETTLE_jtag_ovrVal_MASK 0x40
#define RF_2069_GE16_OVR17_rfpll_vco_FAST_SETTLE_jtag_ovrVal_SHIFT 6
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_sel_sglag_MASK 0x2
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_sel_sglag_SHIFT 1
#define RF_2069_GE16_PLL_XTAL6_xtal_bufstrg_GPS_MASK  0xff00
#define RF_2069_GE16_PLL_XTAL6_xtal_bufstrg_GPS_SHIFT 8
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_RCCAL_MASK    0x600
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_RCCAL_SHIFT   9
#define RF_2069_GE16_PLL_LF1_rfpll_lf_extvcbin_MASK   0xf
#define RF_2069_GE16_PLL_LF1_rfpll_lf_extvcbin_SHIFT  0
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r1_MASK    0x1000
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r1_SHIFT   12
#define RF_2069_GE16_TOP_SPARE7_spare7_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE7_spare7_SHIFT          0
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r2_MASK    0x800
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r2_SHIFT   11
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r3_MASK    0x400
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_r3_SHIFT   10
#define RF_2069_GE16_OVR23_ovr_xtal_pu_MASK           0x100
#define RF_2069_GE16_OVR23_ovr_xtal_pu_SHIFT          8
#define RF_2069_GE16_OVR9_ovr_logen5g_vcobuf_pu_MASK  0x100
#define RF_2069_GE16_OVR9_ovr_logen5g_vcobuf_pu_SHIFT 8
#define RF_2069_GE16_OVR33_reg_wlpmu_rxldo_iso_dis_MASK 0x4
#define RF_2069_GE16_OVR33_reg_wlpmu_rxldo_iso_dis_SHIFT 2
#define RF_2069_GE16_LOGEN5G_CFG1_pdet_en_MASK        0xf80
#define RF_2069_GE16_LOGEN5G_CFG1_pdet_en_SHIFT       7
#define RF_2069_GE16_PLL_LF4_rfpll_lf_lf_r1_MASK      0xff00
#define RF_2069_GE16_PLL_LF4_rfpll_lf_lf_r1_SHIFT     8
#define RF_2069_GE16_RCCAL_CFG_sr_MASK                0x1000
#define RF_2069_GE16_RCCAL_CFG_sr_SHIFT               12
#define RF_2069_GE16_OVR8_ovr_logen2g_logen_reset_MASK 0x800
#define RF_2069_GE16_OVR8_ovr_logen2g_logen_reset_SHIFT 11
#define RF_2069_GE16_PLL_LF4_rfpll_lf_lf_r2_MASK      0xff
#define RF_2069_GE16_PLL_LF4_rfpll_lf_lf_r2_SHIFT     0
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_cm_bgfilter_MASK 0x200
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_cm_bgfilter_SHIFT 9
#define RF_2069_GE16_OVR23_ovr_xtal_coresize_nmos_MASK 0x400
#define RF_2069_GE16_OVR23_ovr_xtal_coresize_nmos_SHIFT 10
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cvar_MASK     0xf
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cvar_SHIFT    0
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_RCCAL1_MASK    0x40
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_RCCAL1_SHIFT   6
#define RF_2069_GE16_PMU_CFG2_TXldo_adj_MASK          0x70
#define RF_2069_GE16_PMU_CFG2_TXldo_adj_SHIFT         4
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_corebuf_bb_MASK 0x4
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_corebuf_bb_SHIFT 2
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_refresh_cal_MASK 0x400
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_refresh_cal_SHIFT 10
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_ib_ctrl_MASK  0xf0
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_ib_ctrl_SHIFT 4
#define RF_2069_GE16_PLL_LF2_rfpll_lf_lf_c1_MASK      0xff00
#define RF_2069_GE16_PLL_LF2_rfpll_lf_lf_c1_SHIFT     8
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_dith_sel_MASK 0x8
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_dith_sel_SHIFT 3
#define RF_2069_GE16_PLL_LF2_rfpll_lf_lf_c2_MASK      0xff
#define RF_2069_GE16_PLL_LF2_rfpll_lf_lf_c2_SHIFT     0
#define RF_2069_GE16_TOP_SPARE5_spare5_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE5_spare5_SHIFT          0
#define RF_2069_GE16_RCAL_CFG_i_wrf_jtag_rcal_value_MASK 0x1f0
#define RF_2069_GE16_RCAL_CFG_i_wrf_jtag_rcal_value_SHIFT 4
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_bufb_MASK     0x7000
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_bufb_SHIFT    12
#define RF_2069_GE16_OVR23_ovr_xtal_core_MASK         0x1000
#define RF_2069_GE16_OVR23_ovr_xtal_core_SHIFT        12
#define RF_2069_GE16_PLL_VCOCAL15_rfpll_vcocal_read_cap_val_MASK 0x7ff
#define RF_2069_GE16_PLL_VCOCAL15_rfpll_vcocal_read_cap_val_SHIFT 0
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_vcob_MASK     0x7
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_vcob_SHIFT    0
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufBBstrg_MASK 0x3000
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufBBstrg_SHIFT 12
#define RF_2069_GE16_OVR16_ovr_rfpll_en_vcocal_MASK   0x4000
#define RF_2069_GE16_OVR16_ovr_rfpll_en_vcocal_SHIFT  14
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_VCO_aux_MASK 0x400
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_VCO_aux_SHIFT 10
#define RF_2069_GE16_PMU_OP_AFEldo_pu_MASK            0x4
#define RF_2069_GE16_PMU_OP_AFEldo_pu_SHIFT           2
#define RF_2069_GE16_PLL_VCOCAL15_rfpll_vcocal_refresh_cal_valid_MASK 0x800
#define RF_2069_GE16_PLL_VCOCAL15_rfpll_vcocal_refresh_cal_valid_SHIFT 11
#define RF_2069_GE16_OVR23_ovr_xtal_clock_HSIC_pu_MASK 0x2000
#define RF_2069_GE16_OVR23_ovr_xtal_clock_HSIC_pu_SHIFT 13
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_X1_MASK       0xff00
#define RF_2069_GE16_RCCAL_LOGIC1_rccal_X1_SHIFT      8
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_qb_MASK 0x1c0
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_qb_SHIFT 6
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_artload_MASK 0x80
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_artload_SHIFT 7
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_pu_ldo_VCO_MASK 0x10
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_pu_ldo_VCO_SHIFT 4
#define RF_2069_GE16_LOGEN5G_CFG1_lob2_pu_MASK        0x10
#define RF_2069_GE16_LOGEN5G_CFG1_lob2_pu_SHIFT       4
#define RF_2069_GE16_TOP_SPARE3_spare3_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE3_spare3_SHIFT          0
#define RF_2069_GE16_OVR8_ovr_logen2g_div1p5_pu_MASK  0x1000
#define RF_2069_GE16_OVR8_ovr_logen2g_div1p5_pu_SHIFT 12
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_mash_sel_MASK 0x2
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_mash_sel_SHIFT 1
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_lsb_sel_MASK 0x4
#define RF_2069_GE16_PLL_FRCT1_rfpll_frct_lsb_sel_SHIFT 2
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_2v_vcovdd_MASK 0x40
#define RF_2069_GE16_PLL_LF1_rfpll_lf_en_2v_vcovdd_SHIFT 6
#define RF_2069_GE16_BG_CFG1_bias_lpf_r_MASK          0xf00
#define RF_2069_GE16_BG_CFG1_bias_lpf_r_SHIFT         8
#define RF_2069_GE16_PLL_XTAL1_xtal_coresize_pmos_MASK 0x1f
#define RF_2069_GE16_PLL_XTAL1_xtal_coresize_pmos_SHIFT 0
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_calSetCount_MASK 0x1ffe
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_calSetCount_SHIFT 1
#define RF_2069_GE16_OVR30_ovr_rfpll_vcocal_force_caps_MASK 0x2
#define RF_2069_GE16_OVR30_ovr_rfpll_vcocal_force_caps_SHIFT 1
#define RF_2069_GE16_VREG_CFG_bg_filter_en_MASK       0x10
#define RF_2069_GE16_VREG_CFG_bg_filter_en_SHIFT      4
#define RF_2069_GE16_LOGEN2G_CFG1_div1p5_pu_MASK      0x2
#define RF_2069_GE16_LOGEN2G_CFG1_div1p5_pu_SHIFT     1
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_1_MASK 0x7c00
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_1_SHIFT 10
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_2_MASK 0x1f
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_2_SHIFT 0
#define RF_2069_GE16_OVR33_ovr_rfpll_cpldo_iso_dis_MASK 0x400
#define RF_2069_GE16_OVR33_ovr_rfpll_cpldo_iso_dis_SHIFT 10
#define RF_2069_GE16_OVR16_ovr_xtal_pu_corebuf_pfd_MASK 0x10
#define RF_2069_GE16_OVR16_ovr_xtal_pu_corebuf_pfd_SHIFT 4
#define RF_2069_GE16_PMU_OP_TXldo_pu_MASK             0x8
#define RF_2069_GE16_PMU_OP_TXldo_pu_SHIFT            3
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_qb_MASK   0x2
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_qb_SHIFT  1
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_CP_artload_MASK 0x2
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_CP_artload_SHIFT 1
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf_MASK     0xf
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf_SHIFT    0
#define RF_2069_GE16_TOP_SPARE1_spare1_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE1_spare1_SHIFT          0
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_vcob_MASK 0x800
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_vcob_SHIFT 11
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_mode_MASK 0x100
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_mode_SHIFT 8
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_static_load_CP_MASK 0x2
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_static_load_CP_SHIFT 1
#define RF_2069_GE16_OVR17_ovr_rfpll_vcobuf_pu_MASK   0x100
#define RF_2069_GE16_OVR17_ovr_rfpll_vcobuf_pu_SHIFT  8
#define RF_2069_GE16_RCCAL_CFG_gpio_en_MASK           0x4
#define RF_2069_GE16_RCCAL_CFG_gpio_en_SHIFT          2
#define RF_2069_GE16_OVR2_ovr_otp_rcal_sel_MASK       0x2
#define RF_2069_GE16_OVR2_ovr_otp_rcal_sel_SHIFT      1
#define RF_2069_GE16_LOGEN2G_TUNE_logen2g_buftune_MASK 0xf00
#define RF_2069_GE16_LOGEN2G_TUNE_logen2g_buftune_SHIFT 8
#define RF_2069_GE16_BG_CFG1_ate_rcal_trim_MASK       0xf000
#define RF_2069_GE16_BG_CFG1_ate_rcal_trim_SHIFT      12
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_bias_reset_VCO_MASK 0x100
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_bias_reset_VCO_SHIFT 8
#define RF_2069_GE16_LPO_CFG_gpio_en_MASK             0x2
#define RF_2069_GE16_LPO_CFG_gpio_en_SHIFT            1
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_ib_ctrl_MASK 0x1000
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_ib_ctrl_SHIFT 12
#define RF_2069_GE16_PMU_OP_RXldo_pu_MASK             0x2
#define RF_2069_GE16_PMU_OP_RXldo_pu_SHIFT            1
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_xtl_div_MASK 0x1f
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_xtl_div_SHIFT 0
#define RF_2069_GE16_PLL_CFG2_rfpll_pfd_open_MASK     0x2
#define RF_2069_GE16_PLL_CFG2_rfpll_pfd_open_SHIFT    1
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_mixb_MASK     0x700
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_mixb_SHIFT    8
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_CP_MASK 0x4000
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_bias_reset_CP_SHIFT 14
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_input_sel_MASK 0x4
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_input_sel_SHIFT 2
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cal_en_empco_MASK 0x20
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cal_en_empco_SHIFT 5
#define RF_2069_GE16_LOGEN5G_TUNE1_ctune_vcob_MASK    0xf
#define RF_2069_GE16_LOGEN5G_TUNE1_ctune_vcob_SHIFT   0
#define RF_2069_GE16_PLL_XTAL3_xtal_xtal_swcap_in_MASK 0xf0
#define RF_2069_GE16_PLL_XTAL3_xtal_xtal_swcap_in_SHIFT 4
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_core_MASK      0x8
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_core_SHIFT     3
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_lowquiescenten_MASK 0x20
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_lowquiescenten_SHIFT 5
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_numCapChange_MASK 0x7
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_numCapChange_SHIFT 0
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_en_alc_MASK  0x40
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_en_alc_SHIFT 6
#define RF_2069_GE16_OVR17_rfpll_vco_EN_DEGEN_jtag_ovrVal_MASK 0x80
#define RF_2069_GE16_OVR17_rfpll_vco_EN_DEGEN_jtag_ovrVal_SHIFT 7
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_VCO_aux_MASK 0x200
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_VCO_aux_SHIFT 9
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_fract_mode_MASK 0x80
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_fract_mode_SHIFT 7
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_up_unit_MASK 0x1e00
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_up_unit_SHIFT 9
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cal_en_MASK  0x40
#define RF_2069_GE16_OVR32_ovr_rfpll_vco_cal_en_SHIFT 6
#define RF_2069_GE16_LOGEN5G_CFG1_lob1_pu_MASK        0x8
#define RF_2069_GE16_LOGEN5G_CFG1_lob1_pu_SHIFT       3
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_bias_reset_CP_MASK 0x200
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_bias_reset_CP_SHIFT 9
#define RF_2069_GE16_OVR2_ovr_bg_rcal_trim_MASK       0x4
#define RF_2069_GE16_OVR2_ovr_bg_rcal_trim_SHIFT      2
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_ctl_MASK 0xf80
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_ctl_SHIFT 7
#define RF_2069_GE16_RCCAL_CFG_rccal_mode_MASK        0x2
#define RF_2069_GE16_RCCAL_CFG_rccal_mode_SHIFT       1
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cap_mode_MASK 0x10
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_cap_mode_SHIFT 4
#define RF_2069_GE16_PLL_FRCT2_rfpll_frct_wild_base_MASK 0xffff
#define RF_2069_GE16_PLL_FRCT2_rfpll_frct_wild_base_SHIFT 0
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_gear_4_MASK 0x3e00
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_gear_4_SHIFT 9
#define RF_2069_GE16_PLL_LF7_rfpll_lf_lf_rf_cm_MASK   0xff00
#define RF_2069_GE16_PLL_LF7_rfpll_lf_lf_rf_cm_SHIFT  8
#define RF_2069_GE16_OVR23_ovr_xtal_core_change_MASK  0x800
#define RF_2069_GE16_OVR23_ovr_xtal_core_change_SHIFT 11
#define RF_2069_GE16_OVR16_ovr_xtal_pu_core_MASK      0x8
#define RF_2069_GE16_OVR16_ovr_xtal_pu_core_SHIFT     3
#define RF_2069_GE16_BG_TRIM1_wlan_vbgtrim_MASK       0x3f00
#define RF_2069_GE16_BG_TRIM1_wlan_vbgtrim_SHIFT      8
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_gear_8_MASK 0x1f0
#define RF_2069_GE16_PLL_VCOCAL9_rfpll_vcocal_gear_8_SHIFT 4
#define RF_2069_GE16_PLL_LF7_rfpll_lf_lf_rs_cm_MASK   0xff
#define RF_2069_GE16_PLL_LF7_rfpll_lf_lf_rs_cm_SHIFT  0
#define RF_2069_GE16_LOGEN2G_CFG1_logen_reset_MASK    0x4
#define RF_2069_GE16_LOGEN2G_CFG1_logen_reset_SHIFT   2
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_RCCAL1_MASK   0x1800
#define RF_2069_GE16_PLL_XTAL5_xtal_sel_RCCAL1_SHIFT  11
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_artload_MASK 0x1
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_artload_SHIFT 0
#define RF_2069_GE16_RCAL_CFG_pu_MASK                 0x1
#define RF_2069_GE16_RCAL_CFG_pu_SHIFT                0
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_BT_MASK   0x1c0
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_BT_SHIFT  6
#define RF_2069_GE16_PLL_CFGR2_rfpll_spare2_MASK      0xff00
#define RF_2069_GE16_PLL_CFGR2_rfpll_spare2_SHIFT     8
#define RF_2069_GE16_RCCAL_LOGIC3_rccal_N0_MASK       0x1fff
#define RF_2069_GE16_RCCAL_LOGIC3_rccal_N0_SHIFT      0
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_pll_val_MASK 0x3ffc
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_pll_val_SHIFT 2
#define RF_2069_GE16_PLL_CFGR2_rfpll_spare3_MASK      0xff
#define RF_2069_GE16_PLL_CFGR2_rfpll_spare3_SHIFT     0
#define RF_2069_GE16_OVR2_ovr_vreg_bg_filter_en_MASK  0x10
#define RF_2069_GE16_OVR2_ovr_vreg_bg_filter_en_SHIFT 4
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_RCCAL_MASK     0x80
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_RCCAL_SHIFT    7
#define RF_2069_GE16_OVR33_reg_rfpll_cpldo_iso_dis_MASK 0x10
#define RF_2069_GE16_OVR33_reg_rfpll_cpldo_iso_dis_SHIFT 4
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_CP_MASK 0x2000
#define RF_2069_GE16_OVR17_ovr_ldo_2p5_pu_ldo_CP_SHIFT 13
#define RF_2069_GE16_PMU_CFG3_selavg_MASK             0xc0
#define RF_2069_GE16_PMU_CFG3_selavg_SHIFT            6
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_pull_down_sw_MASK 0x6
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_pull_down_sw_SHIFT 1
#define RF_2069_GE16_OVR33_ovr_wlpmu_vcoldo_iso_dis_MASK 0x40
#define RF_2069_GE16_OVR33_ovr_wlpmu_vcoldo_iso_dis_SHIFT 6
#define RF_2069_GE16_OVR9_ovr_logen5g_lob_pu_MASK     0x20
#define RF_2069_GE16_OVR9_ovr_logen5g_lob_pu_SHIFT    5
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_ib_ctrl_1p2_MASK 0xf
#define RF_2069_GE16_PLL_VCO3_rfpll_vco_ib_ctrl_1p2_SHIFT 0
#define RF_2069_GE16_PLL_DSPR27_rfpll_monitor_vctrl_level_MASK 0x3
#define RF_2069_GE16_PLL_DSPR27_rfpll_monitor_vctrl_level_SHIFT 0
#define RF_2069_GE16_OVR33_reg_rfpll_vcoldo_iso_dis_MASK 0x20
#define RF_2069_GE16_OVR33_reg_rfpll_vcoldo_iso_dis_SHIFT 5
#define RF_2069_GE16_PMU_CFG1_LNLDO_adj_MASK          0x70
#define RF_2069_GE16_PMU_CFG1_LNLDO_adj_SHIFT         4
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_cvar_MASK    0x80
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_cvar_SHIFT   7
#define RF_2069_GE16_PLL_LF1_rfpll_lf_bcon_cm_MASK    0x180
#define RF_2069_GE16_PLL_LF1_rfpll_lf_bcon_cm_SHIFT   7
#define RF_2069_GE16_OVR16_ovr_ldo_1p2_xtalldo1p2_BG_pu_MASK 0x40
#define RF_2069_GE16_OVR16_ovr_ldo_1p2_xtalldo1p2_BG_pu_SHIFT 6
#define RF_2069_GE16_AFEDIV3_afediv_nfc_driver_size_MASK 0x3c
#define RF_2069_GE16_AFEDIV3_afediv_nfc_driver_size_SHIFT 2
#define RF_2069_GE16_AFEDIV1_afediv_sel_div_MASK      0x70
#define RF_2069_GE16_AFEDIV1_afediv_sel_div_SHIFT     4
#define RF_2069_GE16_PLL_XTAL4_xtal_xtbufstrg_MASK    0x70
#define RF_2069_GE16_PLL_XTAL4_xtal_xtbufstrg_SHIFT   4
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf_MASK  0x4000
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf_SHIFT 14
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_pu_ldo_CP_MASK 0x20
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_pu_ldo_CP_SHIFT 5
#define RF_2069_GE16_AFEDIV1_afediv_pu0_inbuf_MASK    0x400
#define RF_2069_GE16_AFEDIV1_afediv_pu0_inbuf_SHIFT   10
#define RF_2069_GE16_PLL_VCOCAL5_rfpll_vcocal_delayBeforeOpenLoop_MASK 0xff
#define RF_2069_GE16_PLL_VCOCAL5_rfpll_vcocal_delayBeforeOpenLoop_SHIFT 0
#define RF_2069_GE16_PMU_CFG2_wlpmu_cntl_MASK         0x8
#define RF_2069_GE16_PMU_CFG2_wlpmu_cntl_SHIFT        3
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufcalstrg_MASK 0xc
#define RF_2069_GE16_PLL_XTAL4_xtal_outbufcalstrg_SHIFT 2
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovr_MASK 0x400
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovr_SHIFT 10
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_static_load_VCO_MASK 0x1
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_static_load_VCO_SHIFT 0
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_cal_rstb_MASK 0x80
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_cal_rstb_SHIFT 7
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_aux_MASK 0x400
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_aux_SHIFT 10
#define RF_2069_GE16_PLL_CFGR1_rfpll_spare0_MASK      0xff00
#define RF_2069_GE16_PLL_CFGR1_rfpll_spare0_SHIFT     8
#define RF_2069_GE16_PMU_STAT_wlpmu_calcode_MASK      0xf00
#define RF_2069_GE16_PMU_STAT_wlpmu_calcode_SHIFT     8
#define RF_2069_GE16_PLL_CFGR1_rfpll_spare1_MASK      0xff
#define RF_2069_GE16_PLL_CFGR1_rfpll_spare1_SHIFT     0
#define RF_2069_GE16_OVR31_ovr_rfpll_cp_ioff_MASK     0x4
#define RF_2069_GE16_OVR31_ovr_rfpll_cp_ioff_SHIFT    2
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf1_MASK 0x20
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf1_SHIFT 5
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_rst_n_MASK 0x40
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_rst_n_SHIFT 6
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf2_MASK 0x80
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_buf2_SHIFT 7
#define RF_2069_GE16_RCCAL_CFG_bi_MASK                0xf80
#define RF_2069_GE16_RCCAL_CFG_bi_SHIFT               7
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ib_bias_opamp_MASK 0xf0
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_ib_bias_opamp_SHIFT 4
#define RF_2069_GE16_PMU_CFG3_reserved_wlpmu_cntl_MASK 0x1e00
#define RF_2069_GE16_PMU_CFG3_reserved_wlpmu_cntl_SHIFT 9
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_ref_timeout_MASK 0xff
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_ref_timeout_SHIFT 0
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_16_MASK 0x3e0
#define RF_2069_GE16_PLL_VCOCAL8_rfpll_vcocal_gear_16_SHIFT 5
#define RF_2069_GE16_CGPAIO_CFG1_cgpaio_pu_MASK       0x1
#define RF_2069_GE16_CGPAIO_CFG1_cgpaio_pu_SHIFT      0
#define RF_2069_GE16_BG_TRIM2_pmu_vbgtrim_MASK        0x3f00
#define RF_2069_GE16_BG_TRIM2_pmu_vbgtrim_SHIFT       8
#define RF_2069_GE16_PLL_CFG1_rfpll_vco_pu_MASK       0x100
#define RF_2069_GE16_PLL_CFG1_rfpll_vco_pu_SHIFT      8
#define RF_2069_GE16_PLL_CFG1_rfpll_monitor_pu_MASK   0x400
#define RF_2069_GE16_PLL_CFG1_rfpll_monitor_pu_SHIFT  10
#define RF_2069_GE16_PMU_OP_VCOldo_pu_MASK            0x10
#define RF_2069_GE16_PMU_OP_VCOldo_pu_SHIFT           4
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_corebuf_pfd_MASK 0x2
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_corebuf_pfd_SHIFT 1
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovrVal_MASK 0x200
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_FAST_SETTLE_ovrVal_SHIFT 9
#define RF_2069_GE16_OVR23_ovr_xtal_buf_low_pow_MASK  0x8000
#define RF_2069_GE16_OVR23_ovr_xtal_buf_low_pow_SHIFT 15
#define RF_2069_GE16_OVR15_ovr_rfpll_bias_reset_cp_MASK 0x4
#define RF_2069_GE16_OVR15_ovr_rfpll_bias_reset_cp_SHIFT 2
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_dac_pulse_MASK 0x100
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_dac_pulse_SHIFT 8
#define RF_2069_GE16_BG_CFG1_pu_MASK                  0x1
#define RF_2069_GE16_BG_CFG1_pu_SHIFT                 0
#define RF_2069_GE16_PLL_FRCT3_rfpll_frct_wild_base_MASK 0xffff
#define RF_2069_GE16_PLL_FRCT3_rfpll_frct_wild_base_SHIFT 0
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb1_MASK 0x10
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb1_SHIFT 4
#define RF_2069_GE16_PMU_OP_VCOldo_bypass_MASK        0x400
#define RF_2069_GE16_PMU_OP_VCOldo_bypass_SHIFT       10
#define RF_2069_GE16_BG_TRIM1_wlan_vptrim_MASK        0x3f
#define RF_2069_GE16_BG_TRIM1_wlan_vptrim_SHIFT       0
#define RF_2069_GE16_PLL_XTAL1_xtal_coresize_nmos_MASK 0x3e0
#define RF_2069_GE16_PLL_XTAL1_xtal_coresize_nmos_SHIFT 5
#define RF_2069_GE16_PLL_VCOCAL6_rfpll_vcocal_enableTimeOut_MASK 0x3e
#define RF_2069_GE16_PLL_VCOCAL6_rfpll_vcocal_enableTimeOut_SHIFT 1
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_rf_cm_MASK 0x100
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_rf_cm_SHIFT 8
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb2_MASK 0x40
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb2_SHIFT 6
#define RF_2069_GE16_PMU_STAT_ldobg_cal_done_MASK     0x1
#define RF_2069_GE16_PMU_STAT_ldobg_cal_done_SHIFT    0
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_offset_cancel_MASK 0x20
#define RF_2069_GE16_PLL_CP1_rfpll_cp_en_offset_cancel_SHIFT 5
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_buf1_MASK     0x38
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_buf1_SHIFT    3
#define RF_2069_GE16_BG_CFG2_m_count_trim_MASK        0x3f
#define RF_2069_GE16_BG_CFG2_m_count_trim_SHIFT       0
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_buf2_MASK     0xe00
#define RF_2069_GE16_LOGEN5G_IDAC3_idac_buf2_SHIFT    9
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_qb_2nd_MASK 0x38
#define RF_2069_GE16_LOGEN2G_IDAC2_logen2g_idac_qb_2nd_SHIFT 3
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idac_op_core_MASK 0xf00
#define RF_2069_GE16_PLL_CP2_rfpll_cp_idac_op_core_SHIFT 8
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_vref_bias_reset_MASK 0x1
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_vref_bias_reset_SHIFT 0
#define RF_2069_GE16_OVR30_ovr_rfpll_vcocal_force_caps_val_MASK 0x1
#define RF_2069_GE16_OVR30_ovr_rfpll_vcocal_force_caps_val_SHIFT 0
#define RF_2069_GE16_LOGEN2G_CFG1_pdet_en_MASK        0x78
#define RF_2069_GE16_LOGEN2G_CFG1_pdet_en_SHIFT       3
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_lowpwren_VCO_MASK 0x40
#define RF_2069_GE16_PLL_HVLDO1_ldo_2p5_lowpwren_VCO_SHIFT 6
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_rs_cm_MASK 0x200
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_rs_cm_SHIFT 9
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_MASK 0x10
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowquiescenten_VCO_SHIFT 4
#define RF_2069_GE16_OVR27_ovr_xtal_outbufBBstrg_MASK 0x4
#define RF_2069_GE16_OVR27_ovr_xtal_outbufBBstrg_SHIFT 2
#define RF_2069_GE16_OVR16_ovr_rfpll_vco_pu_MASK      0x400
#define RF_2069_GE16_OVR16_ovr_rfpll_vco_pu_SHIFT     10
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_buf_MASK      0x800
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_buf_SHIFT     11
#define RF_2069_GE16_AFEDIV1_afediv_main_driver_size_MASK 0xf
#define RF_2069_GE16_AFEDIV1_afediv_main_driver_size_SHIFT 0
#define RF_2069_GE16_PLL_MONITOR1_rfpll_monitor_vth_low_MASK 0xf
#define RF_2069_GE16_PLL_MONITOR1_rfpll_monitor_vth_low_SHIFT 0
#define RF_2069_GE16_OVR16_ovr_xtal_pu_corebuf_bb_MASK 0x20
#define RF_2069_GE16_OVR16_ovr_xtal_pu_corebuf_bb_SHIFT 5
#define RF_2069_GE16_OVR16_ovr_ldo_1p2_xtalldo1p2_vref_bias_reset_MASK 0x80
#define RF_2069_GE16_OVR16_ovr_ldo_1p2_xtalldo1p2_vref_bias_reset_SHIFT 7
#define RF_2069_GE16_RCCAL_LOGIC5_rccal_raw_dacbuf_MASK 0x3e0
#define RF_2069_GE16_RCCAL_LOGIC5_rccal_raw_dacbuf_SHIFT 5
#define RF_2069_GE16_OVR16_ovr_rfpll_vcocal_cal_MASK  0x4
#define RF_2069_GE16_OVR16_ovr_rfpll_vcocal_cal_SHIFT 2
#define RF_2069_GE16_OVR15_ovr_rfpll_afe_pu_MASK      0x10
#define RF_2069_GE16_OVR15_ovr_rfpll_afe_pu_SHIFT     4
#define RF_2069_GE16_PMU_OP_RXldo_bypass_MASK         0x800
#define RF_2069_GE16_PMU_OP_RXldo_bypass_SHIFT        11
#define RF_2069_GE16_RCCAL_TRC_rccal_Trc_MASK         0x1fff
#define RF_2069_GE16_RCCAL_TRC_rccal_Trc_SHIFT        0
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_load_MASK     0x70
#define RF_2069_GE16_LOGEN5G_IDAC2_idac_load_SHIFT    4
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_sel_sglead_MASK 0x1
#define RF_2069_GE16_PLL_MMD1_rfpll_mmd_sel_sglead_SHIFT 0
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c1_MASK    0x80
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c1_SHIFT   7
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_mix_MASK  0x2000
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_mix_SHIFT 13
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_delay_MASK    0x70
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_delay_SHIFT   4
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c2_MASK    0x40
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c2_SHIFT   6
#define RF_2069_GE16_PMU_OP_wlpmu_en_MASK             0x80
#define RF_2069_GE16_PMU_OP_wlpmu_en_SHIFT            7
#define RF_2069_GE16_PMU_OP_wlpmu_ldobg_clk_en_MASK   0x2000
#define RF_2069_GE16_PMU_OP_wlpmu_ldobg_clk_en_SHIFT  13
#define RF_2069_GE16_RCCAL_LOGIC5_rccal_raw_adc1p2_MASK 0x1f
#define RF_2069_GE16_RCCAL_LOGIC5_rccal_raw_adc1p2_SHIFT 0
#define RF_2069_GE16_TOP_SPARE8_spare8_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE8_spare8_SHIFT          0
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_caps_sel_MASK 0xe00
#define RF_2069_GE16_PLL_VCOCAL2_rfpll_vcocal_cal_caps_sel_SHIFT 9
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c3_MASK    0x20
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c3_SHIFT   5
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_pu_ldo_VCO_aux_MASK 0x800
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_pu_ldo_VCO_aux_SHIFT 11
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c4_MASK    0x10
#define RF_2069_GE16_OVR31_ovr_rfpll_lf_lf_c4_SHIFT   4
#define RF_2069_GE16_CGPAIO_CFG5_cgpaio_tssi_muxselgpaio_MASK 0xff
#define RF_2069_GE16_CGPAIO_CFG5_cgpaio_tssi_muxselgpaio_SHIFT 0
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_ICAL_1p2_MASK 0x1f00
#define RF_2069_GE16_PLL_VCO2_rfpll_vco_ICAL_1p2_SHIFT 8
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_vout_gpaio_test_en_MASK 0x4
#define RF_2069_GE16_PLL_HVLDO4_ldo_2p5_vout_gpaio_test_en_SHIFT 2
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_bypass_vctrl_buf_MASK 0x2000
#define RF_2069_GE16_PLL_VCO6_rfpll_vco_bypass_vctrl_buf_SHIFT 13
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_qb_2nd_MASK 0x1
#define RF_2069_GE16_OVR29_ovr_logen2g_idac_qb_2nd_SHIFT 0
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_pfddrv_MASK    0x1
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_pfddrv_SHIFT   0
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_CP_vout_sel_MASK 0xf00
#define RF_2069_GE16_PLL_HVLDO3_ldo_2p5_ldo_CP_vout_sel_SHIFT 8
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_VCO_aux_MASK 0x10
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_VCO_aux_SHIFT 4
#define RF_2069_GE16_OVR16_ovr_rfpll_synth_pu_MASK    0x1
#define RF_2069_GE16_OVR16_ovr_rfpll_synth_pu_SHIFT   0
#define RF_2069_GE16_OVR18_ovr_afediv_pu1_main_MASK   0x4
#define RF_2069_GE16_OVR18_ovr_afediv_pu1_main_SHIFT  2
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_RCCAL1_MASK 0x38
#define RF_2069_GE16_PLL_XTAL5_xtal_bufstrg_RCCAL1_SHIFT 3
#define RF_2069_GE16_LPO_CFG_pu_MASK                  0x1
#define RF_2069_GE16_LPO_CFG_pu_SHIFT                 0
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_bias_mode_MASK 0x400
#define RF_2069_GE16_OVR30_ovr_rfpll_vco_bias_mode_SHIFT 10
#define RF_2069_GE16_OVR33_reg_wlpmu_afeldo_iso_dis_MASK 0x8
#define RF_2069_GE16_OVR33_reg_wlpmu_afeldo_iso_dis_SHIFT 3
#define RF_2069_GE16_PMU_CFG2_RXldo_adj_MASK          0x7000
#define RF_2069_GE16_PMU_CFG2_RXldo_adj_SHIFT         12
#define RF_2069_GE16_BG_CFG2_n_count_trim_MASK        0x3f00
#define RF_2069_GE16_BG_CFG2_n_count_trim_SHIFT       8
#define RF_2069_GE16_TOP_SPARE6_spare6_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE6_spare6_SHIFT          0
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowpwren_VCO_aux_MASK 0x200
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_lowpwren_VCO_aux_SHIFT 9
#define RF_2069_GE16_OVR16_rfpll_vco_EN_DEGEN_jtag_ovr_MASK 0x200
#define RF_2069_GE16_OVR16_rfpll_vco_EN_DEGEN_jtag_ovr_SHIFT 9
#define RF_2069_GE16_OVR27_ovr_ldo_1p2_xtalldo1p2_lowquiescenten_MASK 0x40
#define RF_2069_GE16_OVR27_ovr_ldo_1p2_xtalldo1p2_lowquiescenten_SHIFT 6
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_byp_MASK 0x100
#define RF_2069_GE16_PLL_HVLDO2_ldo_2p5_ldo_VCO_aux_byp_SHIFT 8
#define RF_2069_GE16_OVR27_ovr_xtal_xtbufstrg_MASK    0x2
#define RF_2069_GE16_OVR27_ovr_xtal_xtbufstrg_SHIFT   1
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_ovr_mode_MASK 0x4000
#define RF_2069_GE16_PLL_VCOCAL10_rfpll_vcocal_ovr_mode_SHIFT 14
#define RF_2069_GE16_PLL_CP1_rfpll_cp_pu_MASK         0x2000
#define RF_2069_GE16_PLL_CP1_rfpll_cp_pu_SHIFT        13
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_caldrv_MASK    0x10
#define RF_2069_GE16_PLL_XTAL2_xtal_pu_caldrv_SHIFT   4
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_calSetCountOvr_MASK 0x1
#define RF_2069_GE16_PLL_VCOCAL1_rfpll_vcocal_calSetCountOvr_SHIFT 0
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf1_MASK    0xf0
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf1_SHIFT   4
#define RF_2069_GE16_OVR27_ovr_xtal_outbufstrg_MASK   0x1
#define RF_2069_GE16_OVR27_ovr_xtal_outbufstrg_SHIFT  0
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf2_MASK    0xf00
#define RF_2069_GE16_LOGEN5G_TUNE2_ctune_buf2_SHIFT   8
#define RF_2069_GE16_PMU_OP_LNLDO_pu_MASK             0x1
#define RF_2069_GE16_PMU_OP_LNLDO_pu_SHIFT            0
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_BG_pu_MASK 0x10
#define RF_2069_GE16_PLL_XTALLDO1_ldo_1p2_xtalldo1p2_BG_pu_SHIFT 4
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_dcadj_MASK 0x78
#define RF_2069_GE16_PLL_VCO5_rfpll_vco_tempco_dcadj_SHIFT 3
#define RF_2069_GE16_PLL_CP5_rfpll_cp_ioff_extra_MASK 0x1
#define RF_2069_GE16_PLL_CP5_rfpll_cp_ioff_extra_SHIFT 0
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb_MASK 0x400
#define RF_2069_GE16_OVR29_ovr_logen5g_idac_bufb_SHIFT 10
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_VCO_MASK 0x80
#define RF_2069_GE16_OVR27_ovr_ldo_2p5_lowquiescenten_VCO_SHIFT 7
#define RF_2069_GE16_OVR2_ovr_vreg_pup_MASK           0x1
#define RF_2069_GE16_OVR2_ovr_vreg_pup_SHIFT          0
#define RF_2069_GE16_TOP_SPARE4_spare4_MASK           0xffff
#define RF_2069_GE16_TOP_SPARE4_spare4_SHIFT          0
#define RF_2069_GE16_PLL_VCOCAL3_rfpll_vcocal_clk_gate_ovr_MASK 0x4
#define RF_2069_GE16_PLL_VCOCAL3_rfpll_vcocal_clk_gate_ovr_SHIFT 2
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_mix_MASK      0x80
#define RF_2069_GE16_LOGEN5G_IDAC1_idac_mix_SHIFT     7
#define RF_2069_GE16_PLL_DSPR27_rfpll_monitor_need_refresh_MASK 0x4
#define RF_2069_GE16_PLL_DSPR27_rfpll_monitor_need_refresh_SHIFT 2
#define RF_2069_GE16_RCCAL_LOGIC4_rccal_N1_MASK       0x1fff
#define RF_2069_GE16_RCCAL_LOGIC4_rccal_N1_SHIFT      0
#define RF_2069_GE16_BG_CFG1_bg_ate_rcal_trim_en_MASK 0x4
#define RF_2069_GE16_BG_CFG1_bg_ate_rcal_trim_en_SHIFT 2
#define RF_2069_GE16_LOGEN5G_CFG1_lob_pu_MASK         0x4
#define RF_2069_GE16_LOGEN5G_CFG1_lob_pu_SHIFT        2
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_en_MASK       0xf
#define RF_2069_GE16_PLL_CFG1_rfpll_pfd_en_SHIFT      0
#define RF_2069_GE16_OVR33_ovr_wlpmu_txldo_iso_dis_MASK 0x80
#define RF_2069_GE16_OVR33_ovr_wlpmu_txldo_iso_dis_SHIFT 7
#define RF_2069_GE16_OVR9_ovr_logen5g_pu_MASK         0x400
#define RF_2069_GE16_OVR9_ovr_logen5g_pu_SHIFT        10
#define RF_2069_GE16_PMU_OP_AFEldo_bypass_MASK        0x100
#define RF_2069_GE16_PMU_OP_AFEldo_bypass_SHIFT       8
#define RF_2069_GE16_PLL_VCOCAL4_rfpll_vcocal_delayAfterOpenLoop_MASK 0xff
#define RF_2069_GE16_PLL_VCOCAL4_rfpll_vcocal_delayAfterOpenLoop_SHIFT 0
#define RF_2069_GE16_RCCAL_LOGIC2_rccal_DONE_MASK     0x10
#define RF_2069_GE16_RCCAL_LOGIC2_rccal_DONE_SHIFT    4
#define RF_2069_GE16_OVR16_ovr_rfpll_pll_pu_MASK      0x1000
#define RF_2069_GE16_OVR16_ovr_rfpll_pll_pu_SHIFT     12
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_test_gpaio_MASK 0x20
#define RF_2069_GE16_PLL_VCOCAL13_rfpll_vcocal_test_gpaio_SHIFT 5
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_HDRM_CAL_MASK 0x1e0
#define RF_2069_GE16_PLL_VCO1_rfpll_vco_HDRM_CAL_SHIFT 5
#define RF_2069_GE16_OVR31_ovr_rfpll_cp_kpd_scale_MASK 0x8
#define RF_2069_GE16_OVR31_ovr_rfpll_cp_kpd_scale_SHIFT 3
#define RF_2069_GE16_OVR16_ovr_rfpll_cal_rst_n_MASK   0x8000
#define RF_2069_GE16_OVR16_ovr_rfpll_cal_rst_n_SHIFT  15
#define RF_2069_GE16_OVR16_ovr_rfpll_rst_n_MASK       0x800
#define RF_2069_GE16_OVR16_ovr_rfpll_rst_n_SHIFT      11
#define RF_2069_GE16_LOGEN5G_CFG1_div3_en_MASK        0x40
#define RF_2069_GE16_LOGEN5G_CFG1_div3_en_SHIFT       6
#define RF_2069_GE25_LOGEN5G_IDAC1_idac_mix_MASK      0x780
#define RF_2069_GE25_LOGEN5G_IDAC1_idac_mix_SHIFT     7
#define RF_2069_GE25_LOGEN5G_IDAC1_idac_buf_MASK      0x7800
#define RF_2069_GE25_LOGEN5G_IDAC1_idac_buf_SHIFT     11

/* GE REV32 MASK and SHIFTS deltas over GE16 */
#define RF_2069_GE32_OVR1_ovr_vreg_pup_MASK           0x1
#define RF_2069_GE32_OVR1_ovr_vreg_pup_SHIFT          0
#define RF_2069_GE32_OVR1_ovr_vreg_bg_filter_en_MASK  0x10
#define RF_2069_GE32_OVR1_ovr_vreg_bg_filter_en_SHIFT 4
#define RF_2069_GE32_OVR2_ovr_bg_pulse_MASK           0x20
#define RF_2069_GE32_OVR2_ovr_bg_pulse_SHIFT          5
#define RF_2069_GE32_OVR2_ovr_bg_pu_MASK              0x8
#define RF_2069_GE32_OVR2_ovr_bg_pu_SHIFT             3
#define RF_2069_GE32_OVR2_ovr_bg_rcal_trim_MASK       0x4
#define RF_2069_GE32_OVR2_ovr_bg_rcal_trim_SHIFT      2
#define RF_2069_GE32_OVR2_ovr_otp_rcal_sel_MASK       0x2
#define RF_2069_GE32_OVR2_ovr_otp_rcal_sel_SHIFT      1
#define RF_2069_GE32_BG_CFG1_ate_rcal_trim_MASK       0xf000
#define RF_2069_GE32_BG_CFG1_ate_rcal_trim_SHIFT      12
#define RF_2069_GE32_BG_CFG1_bias_lpf_r_MASK          0xf00
#define RF_2069_GE32_BG_CFG1_bias_lpf_r_SHIFT         8
#define RF_2069_GE32_BG_CFG1_rcal_trim_MASK           0xf0
#define RF_2069_GE32_BG_CFG1_rcal_trim_SHIFT          4
#define RF_2069_GE32_BG_CFG1_bg_ate_rcal_trim_en_MASK 0x4
#define RF_2069_GE32_BG_CFG1_bg_ate_rcal_trim_en_SHIFT 2
#define RF_2069_GE32_BG_CFG1_bg_pulse_MASK            0x2
#define RF_2069_GE32_BG_CFG1_bg_pulse_SHIFT           1
#define RF_2069_GE32_BG_CFG1_pu_MASK                  0x1
#define RF_2069_GE32_BG_CFG1_pu_SHIFT                 0
#define RF_2069_GE32_PMU_OP_vref_select_MASK          0x8000
#define RF_2069_GE32_PMU_OP_vref_select_SHIFT         15
#define RF_2069_GE32_PMU_OP_ldoref_start_cal_MASK     0x4000
#define RF_2069_GE32_PMU_OP_ldoref_start_cal_SHIFT    14
#define RF_2069_GE32_PMU_OP_wlpmu_ldobg_clk_en_MASK   0x2000
#define RF_2069_GE32_PMU_OP_wlpmu_ldobg_clk_en_SHIFT  13
#define RF_2069_GE32_PMU_OP_LNLDO_bypass_MASK         0x1000
#define RF_2069_GE32_PMU_OP_LNLDO_bypass_SHIFT        12
#define RF_2069_GE32_PMU_OP_RXldo_bypass_MASK         0x800
#define RF_2069_GE32_PMU_OP_RXldo_bypass_SHIFT        11
#define RF_2069_GE32_PMU_OP_VCOldo_bypass_MASK        0x400
#define RF_2069_GE32_PMU_OP_VCOldo_bypass_SHIFT       10
#define RF_2069_GE32_PMU_OP_TXldo_bypass_MASK         0x200
#define RF_2069_GE32_PMU_OP_TXldo_bypass_SHIFT        9
#define RF_2069_GE32_PMU_OP_AFEldo_bypass_MASK        0x100
#define RF_2069_GE32_PMU_OP_AFEldo_bypass_SHIFT       8
#define RF_2069_GE32_PMU_OP_wlpmu_en_MASK             0x80
#define RF_2069_GE32_PMU_OP_wlpmu_en_SHIFT            7
#define RF_2069_GE32_PMU_OP_synth_pwrsw_en_MASK       0x20
#define RF_2069_GE32_PMU_OP_synth_pwrsw_en_SHIFT      5
#define RF_2069_GE32_PMU_OP_VCOldo_pu_MASK            0x10
#define RF_2069_GE32_PMU_OP_VCOldo_pu_SHIFT           4
#define RF_2069_GE32_PMU_OP_TXldo_pu_MASK             0x8
#define RF_2069_GE32_PMU_OP_TXldo_pu_SHIFT            3
#define RF_2069_GE32_PMU_OP_AFEldo_pu_MASK            0x4
#define RF_2069_GE32_PMU_OP_AFEldo_pu_SHIFT           2
#define RF_2069_GE32_PMU_OP_RXldo_pu_MASK             0x2
#define RF_2069_GE32_PMU_OP_RXldo_pu_SHIFT            1
#define RF_2069_GE32_PMU_OP_LNLDO_pu_MASK             0x1
#define RF_2069_GE32_PMU_OP_LNLDO_pu_SHIFT            0
#define RF_2069_GE32_PMU_STAT_wlpmu_calcode_MASK      0xf00
#define RF_2069_GE32_PMU_STAT_wlpmu_calcode_SHIFT     8
#define RF_2069_GE32_PMU_STAT_wlpmu_zcode_MASK        0xf0
#define RF_2069_GE32_PMU_STAT_wlpmu_zcode_SHIFT       4
#define RF_2069_GE32_PMU_STAT_ldobg_cal_done_MASK     0x1
#define RF_2069_GE32_PMU_STAT_ldobg_cal_done_SHIFT    0
#define RF_2069_GE32_CGPAIO_CFG1_cgpaio_pu_MASK       0x1
#define RF_2069_GE32_CGPAIO_CFG1_cgpaio_pu_SHIFT      0
#define RF_2069_GE32_PMU_CFG2_AFEldo_adj_MASK         0x7
#define RF_2069_GE32_PMU_CFG2_AFEldo_adj_SHIFT        0
#define RF_2069_GE32_PMU_CFG1_vrefadj_cbuck_MASK      0xf00
#define RF_2069_GE32_PMU_CFG1_vrefadj_cbuck_SHIFT     8
/* New mask defs */

/* Common defines */

#endif	/* _BCM20XX_H */
