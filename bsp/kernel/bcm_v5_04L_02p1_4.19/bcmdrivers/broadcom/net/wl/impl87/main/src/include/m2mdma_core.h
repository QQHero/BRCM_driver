/*
 * BCM43XX M2M DMA core hardware definitions.
 *
 * Copyright (C) 2022, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id:m2mdma _core.h 421139 2013-08-30 17:56:15Z kiranm $
 */

#ifndef	_M2MDMA_CORE_H
#define	_M2MDMA_CORE_H
#include <sbhnddma.h>
/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif

/* XXX
 *
 * M2MCORE Twiki: [M2MDmaRev0] provides several memory to memory DMA engines. A
 * memory to memory DMA engine instances. These mem2mem DMA engines are distinct
 * from the mem2mem DMA engines available in the PCIECORE, namely the host2dev,
 * dev2host and implicit-DMA channels.
 *
 *
 * Mem2Mem DMA Engine:
 * -------------------
 * A memory to memory engine is derived by pairing two DMA processors. A DMA
 * processor is defined in Twiki [CurrentDmaProgGuide] and implemented in
 * sbhnddma.h dma64regs_t. A transmit DMA processor is responsible for DMAing
 * data from a specified source memory into a local FIFO (1KBytes to 4KBytes)
 * and the paired receive DMA processor is responsible for DMAing from the local
 * FIFO into a destination memory, effecting a memory to memory transfer.
 * Each mem2mem DMA engine (pair of DMA processors) utilize a pair of <Tx,Rx>
 * descriptor rings. The term DMA channel and DMA engine is used interchangeably
 * with the channel number refering to the DMA engine instance in the M2MCORE.
 * A total of 8 DMA engines may be instantiated in a M2MCORE.
 *
 *
 * Simple Mem2Mem DMA Engine, aka Byte Move Engine:
 * ------------------------------------------------
 * Byte Move Engine (see hndbme.h,c) is an instance of a M2MCORE DMA engine,
 * wherein the driver is not required to program the pair of <transmit,receive>
 * descriptor. Instead, the driver specifies a source and destination memory
 * attributes (Address, AddrExt, Coherency, NotPCIe, etc) and data length via a
 * set of explicit registers. SW does not explicitly program a pair of <Tx,Rx>
 * descriptors and as such there is no notion of CD,AD,LD. [Internally, HW still
 * uses a pair of descriptor rings, with the descriptor ring depth of one. BME
 * may also be referred to as "simple" mem2mem and offers a HW assisted memcpy
 * like programming interface. While DMA_DONE Interrupt is generated by the
 * Transmit and Receive Processor, the driver exposes a synchronous programming
 * model -- caller is required to poll for DMA completion. Caller may initiate
 * a DMA transfer, perform some other cooperative computation in parallel and
 * poll (busy loop) for DMA completion. Due to the synchronous (busy loop poll)
 * programming model, the Byte Move Engines are specifically for short data
 * transfers.
 *
 * A simple mem2mem DMA channel may not be shared across entities:
 * driver on Router Host SoC, driver in WLAN chipset, UCode in PSM and MAC HW.
 * As such, a DMA Engine used explicitly by the WLAN D11 MAC for offloading
 * TxStatus or PhyRxStatus, may not be considered in the pool of simple mem2mem
 * DMA engines governed by BME (see hndbme.c).
 *
 *
 * Traditional Mem2Mem DMA Engine:
 * -------------------------------
 * Unlike the synchronous BME, the "traditional" Mem2Mem offers an asynchronous
 * programming model, by using the traditional pair of descriptor rings for
 * conveying the source and destination memory attributes. The Receive DMA
 * processor's Interrupt along with the advanced LastDescriptor is used to
 * determine the mem2mem DMA transaction that has just completed and resume the
 * calling application (invoke callback and associated context).
 * See hndm2m.h,c
 *
 *
 * Terminology:
 * ------------
 * - HND DMA Processor: sbhnddma.h
 * - Engine ~ Channel => [Transmit, Receive] DMA Processor pair m2mdma_core.h
 * - Simple Mem2Mem ~ Non-Descriptor based Mem2Mem => Byte Move Engine hndbme.h
 * - Traditional Mem2Mem ~ Descriptor based Mem2Mem => hndm2m.h
 *
 * Mem2Mem host2dev, dev2host and iDMA in PCIECORE are distinct from M2MCORE.
 */

/* Maximum number of DMA channels that may be instantiated in a M2MCORE.
 * Actual number of channels instantiated is defined per M2MCORE revision.
 */
#define M2M_CORE_CHANNELS   8
#define M2M_CORE_CH0_OFFSET (0x200)

/* M2MCORE DMA Engine (pair of transmit and receive DMA Processors) Registers */
typedef volatile struct m2m_eng_regs {
	dma64regs_t tx;     /* Transmit DMA Processor */
	uint32      PAD[2];
	dma64regs_t rx;     /* Receive DMA Processor */
	uint32      PAD[2];
} m2m_eng_regs_t;       /* M2M DMA Engine: pair of Tx and Rx DMA Processor */

/* M2MCORE per M2M Channel (DMA Processor pair) IntStatus and IntMask */
typedef volatile struct m2m_int_regs {
	uint32 intstatus;   /* DE=b10, DA=b11, DP=b12, RU=b13; RI=b16, XI=b24 */
	uint32 intmask;
} m2m_int_regs_t;

/* M2MCORE STATUS (TXS and PHYRXS) Channel Registers */
typedef volatile struct m2m_status_eng_regs {
	uint32 cfg;                                  /* 0x0 */
	uint32 ctl;                                  /* 0x4 */
	uint32 sts;                                  /* 0x8 */
	uint32 debug;                                /* 0xC */
	uint32 da_base_l;                            /* 0x10 */
	uint32 da_base_h;                            /* 0x14 */
	uint32 size;                                 /* 0x18 */
	uint32 wridx;                                /* 0x1C */
	uint32 rdidx;                                /* 0x20 */
	uint32 dma_template;                         /* 0x24 */
	uint32 sa_base_l;                            /* 0x28 */
	uint32 sa_base_h;                            /* 0x2C */
} m2m_status_eng_regs_t;

/* M2MCORE registers */
typedef volatile struct m2m_core_regs {
	uint32 control;                              /* 0x0 */
	uint32 capabilities;                         /* 0x4 */
	uint32 intcontrol;                           /* 0x8 */
	uint32 PAD[5];
	uint32 intstatus;                            /* 0x20 */
	uint32 PAD[3];
	m2m_int_regs_t int_regs[M2M_CORE_CHANNELS];  /* 0x30 - 0x6c */
	uint32 PAD[36];
	uint32 intrcvlazy[M2M_CORE_CHANNELS];        /* 0x100 - 0x11c */
	uint32 PAD[48];
	uint32 clockctlstatus;                       /* 0x1e0 */
	uint32 workaround;                           /* 0x1e4 */
	uint32 powercontrol;                         /* 0x1e8 */
	uint32 PAD[5];
	m2m_eng_regs_t eng_regs[M2M_CORE_CHANNELS];  /* 0x200 - 0x3fc */
	uint32 PAD[256];
	m2m_status_eng_regs_t txs_regs;              /* 0x800 - 0x82c */
	uint32 PAD[52];
	m2m_status_eng_regs_t phyrxs_regs;           /* 0x900 - 0x92c */
} m2m_core_regs_t;

/** m2m_core_regs_t::capabilities */
#define M2M_CORE_CAPABILITIES_CHANNELCNT_SHIFT		0
#define M2M_CORE_CAPABILITIES_CHANNELCNT_NBITS		4
#define M2M_CORE_CAPABILITIES_CHANNELCNT_MASK		BCM_MASK(M2M_CORE_CAPABILITIES_CHANNELCNT)

#define M2M_CORE_CAPABILITIES_MAXBURSTLEN_SHIFT		4
#define M2M_CORE_CAPABILITIES_MAXBURSTLEN_NBITS		3
#define M2M_CORE_CAPABILITIES_MAXBURSTLEN_MASK		BCM_MASK(M2M_CORE_CAPABILITIES_MAXBURSTLEN)

#define M2M_CORE_CAPABILITIES_MAXREADSOUTSTANDING_SHIFT 7
#define M2M_CORE_CAPABILITIES_MAXREADSOUTSTANDING_NBITS 3
#define M2M_CORE_CAPABILITIES_MAXREADSOUTSTANDING_MASK		\
	BCM_MASK(M2M_CORE_CAPABILITIES_MAXREADSOUTSTANDING)

#define M2M_CORE_CAPABILITIES_SM2MCNT_SHIFT		10
#define M2M_CORE_CAPABILITIES_SM2MCNT_NBITS		4
#define M2M_CORE_CAPABILITIES_SM2MCNT_MASK		BCM_MASK(M2M_CORE_CAPABILITIES_SM2MCNT)

/** m2m_core_regs_t::intcontrol */
#define M2M_CORE_INTCONTROL_TXSWRINDUPD_INTMASK_SHIFT		30 /**< TxStatus W_IDX update */
#define M2M_CORE_INTCONTROL_TXSWRINDUPD_INTMASK_NBITS		1
#define M2M_CORE_INTCONTROL_TXSWRINDUPD_INTMASK_MASK		\
	BCM_MASK(M2M_CORE_INTCONTROL_TXSWRINDUPD_INTMASK)

#define M2M_CORE_INTCONTROL_PHYRXSWRINDUPD_INTMASK_SHIFT	31 /**< PhyRxStatus W_IDX update */
#define M2M_CORE_INTCONTROL_PHYRXSWRINDUPD_INTMASK_NBITS	1
#define M2M_CORE_INTCONTROL_PHYRXSWRINDUPD_INTMASK_MASK		\
	BCM_MASK(M2M_CORE_INTCONTROL_PHYRXSWRINDUPD_INTMASK)

/** m2m_int_regs_t intstatus, intmask for traditional and simple m2m channels */
/* Ch# <IntStatus,IntMask> DescErr (DE) */
#define M2M_CORE_CH_DE_NBITS        1
#define M2M_CORE_CH_DE_SHIFT        10
#define M2M_CORE_CH_DE_MASK         BCM_MASK(M2M_CORE_CH_DE)
/* Ch# <IntStatus,IntMask> DataErr (DA) */
#define M2M_CORE_CH_DA_NBITS        1
#define M2M_CORE_CH_DA_SHIFT        11
#define M2M_CORE_CH_DA_MASK         BCM_MASK(M2M_CORE_CH_DA)
/* Ch# <IntStatus,IntMask> DescProtoErr (DP) */
#define M2M_CORE_CH_DP_NBITS        1
#define M2M_CORE_CH_DP_SHIFT        12
#define M2M_CORE_CH_DP_MASK         BCM_MASK(M2M_CORE_CH_DP)
/* Ch# <IntStatus,IntMask> RcvDescUf (RU) */
#define M2M_CORE_CH_RU_NBITS        1
#define M2M_CORE_CH_RU_SHIFT        13
#define M2M_CORE_CH_RU_MASK         BCM_MASK(M2M_CORE_CH_RU)
/* Ch# <IntStatus,IntMask> RcvInt (RI) */
#define M2M_CORE_CH_RI_NBITS        1
#define M2M_CORE_CH_RI_SHIFT        16
#define M2M_CORE_CH_RI_MASK         BCM_MASK(M2M_CORE_CH_RI)
/* Ch# <IntStatus,IntMask> XmtInt (XI) */
#define M2M_CORE_CH_XI_NBITS        1
#define M2M_CORE_CH_XI_SHIFT        24
#define M2M_CORE_CH_XI_MASK         BCM_MASK(M2M_CORE_CH_XI)

/** Per M2M Channel: Interrupt Receive Lazy */
#define M2M_CORE_CH_INTRCVLAZY_TO_NBITS 24  /* TimeOut (TO) microsecs */
#define M2M_CORE_CH_INTRCVLAZY_TO_SHIFT 0
#define M2M_CORE_CH_INTRCVLAZY_TO_MASK  BCM_MASK(M2M_CORE_CH_INTRCVLAZY_TO)
#define M2M_CORE_CH_INTRCVLAZY_FC_NBITS 8   /* FrameCntr (FC) */
#define M2M_CORE_CH_INTRCVLAZY_FC_SHIFT 24
#define M2M_CORE_CH_INTRCVLAZY_FC_MASK  BCM_MASK(M2M_CORE_CH_INTRCVLAZY_FC)

#endif	/* _M2MDMA_CORE_H */
