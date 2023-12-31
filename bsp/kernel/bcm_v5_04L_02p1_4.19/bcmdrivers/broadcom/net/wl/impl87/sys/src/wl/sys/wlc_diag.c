/*
 * Diagnostic routines for Broadcom 802.11 Networking Driver.
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
 * $Id: wlc_diag.c 802692 2021-09-03 10:19:13Z $
 */

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <pcicfg.h>
#include <bcmsrom.h>
#include <wlioctl.h>

#include <sbhndpio.h>
#include <sbhnddma.h>
#include <hnddma.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_channel.h>
#include <wlc_pub.h>
#include <wlc_bsscfg.h>
#include <bcmendian.h>
#include <wlc_pcb.h>  // for loopback test
#include <wlc.h>
#include <wlc_hw_priv.h>
#include <wlc_phy_hal.h>
#include <wlc_diag.h>
#include <wlc_bmac.h>
#include <wl_export.h>
#include <wlc_tx.h>
#include <phy_radio_api.h>

#define DIAG_LED_SLEEP		80000	/* led blink gap in us */
#define DIAG_LED_BLINK_TIMES	3   /* # of times led should blink */

#define WLC_ACTIVITY_LED	1	/*  activity LED is GPIO bit 0 */
#define WLC_RADIO_LED_B		2	/*  radio LED from GPIO bit 1 */
#define WLC_RADIO_LED_A		4	/*  radio LED from GPIO bit 2 */
#define WLC_RADIO_LEDS		6	/*  all radio LEDs: GPIO bit 1 and bit 2 */

#define C_DIAG_BMCLPB_NBIT (1<<0)

#define MAX_LOOPBACK_PKTS 128

static wlc_txq_info_t *qi = NULL; /**< loopback test, points at ibss tx queue */

static uint32 wlc_diag_queue_tx_packet(wlc_info_t *wlc);
static void wlc_diag_loopback_tx_complete(wlc_info_t *wlc, uint txstatus, void *arg);

static int n_lb_pkts;

/** TODO: this needs to be updated when gpiocontrol is transferred from dot11 to cc */
static void
wlc_diag_led_toggle(wlc_hw_info_t *wlc_hw, uint led_bits, bool want)
{
	if (want == ON) {
		si_gpioout(wlc_hw->sih, led_bits, led_bits, GPIO_DRV_PRIORITY);
	} else {
		si_gpioout(wlc_hw->sih, led_bits, 0, GPIO_DRV_PRIORITY);
	}
}

static uint32
wlc_diag_srom(wlc_hw_info_t *wlc_hw)
{
	int nw;
	uint32 macintmask;
	srom_rw_t *s;
	uint32 ssize;
	uint32 status = WL_DIAGERR_SUCCESS;
	uint8 crc;
	int crc_range;

	WL_INFORM(("wlc_diag_srom():\n"));

	/* space for the entire srom */
	ssize = sizeof(srom_rw_t) + SROM_MAX;
	s = (srom_rw_t *)MALLOC(wlc_hw->osh, ssize);

	if (s == NULL) {
		status = WL_DIAGERR_NOMEM;
		return status;
	}

	if (BUSTYPE(wlc_hw->sih->bustype) == PCMCIA_BUS) {
		crc_range = SROM_MAX;
	}
#if defined(BCMUSBDEV) || defined(BCMSDIODEV)
	else {
		crc_range = srom_size(wlc_hw->sih, wlc_hw->osh);
	}
#else /* def BCMUSBDEV || def BCMSDIODEV */
	else {
		crc_range = (SROM8_SIGN+1) * 2;	/* must big enough for SROM8 */
	}
#endif /* def BCMUSBDEV || def BCMSDIODEV */

	WL_INFORM(("crc_range = %d\n", crc_range));

	/* intrs off */
	macintmask = wl_intrsoff(wlc_hw->wlc->wl);

	nw = crc_range / 2;

	GCC_DIAGNOSTIC_PUSH_SUPPRESS_CAST();
	/* read first small number words from srom, then adjust the length, read all */
	if (srom_read(wlc_hw->sih, wlc_hw->sih->bustype, (void *)wlc_hw->regs,
		wlc_hw->osh, 0, crc_range, s->buf, FALSE)) {
		goto out;
	}
	GCC_DIAGNOSTIC_POP();

	if ((s->buf[SROM4_SIGN] == SROM4_SIGNATURE) ||
	    (s->buf[SROM8_SIGN] == SROM4_SIGNATURE)) {
		nw = SROM4_WORDS;
		crc_range = nw * 2;
		GCC_DIAGNOSTIC_PUSH_SUPPRESS_CAST();
		if (srom_read(wlc_hw->sih, wlc_hw->sih->bustype, (void *)wlc_hw->regs,
			wlc_hw->osh, 0, crc_range, s->buf, FALSE)) {
			goto out;
		}
		GCC_DIAGNOSTIC_POP();

	} else if (s->buf[SROM10_SIGN] == SROM10_SIGNATURE) {
		nw = SROM10_WORDS;
		crc_range = nw * 2;
		GCC_DIAGNOSTIC_PUSH_SUPPRESS_CAST();
		if (srom_read(wlc_hw->sih, wlc_hw->sih->bustype, (void *)wlc_hw->regs,
			wlc_hw->osh, 0, crc_range, s->buf, FALSE)) {
			goto out;
		}
		GCC_DIAGNOSTIC_POP();

	} else if (s->buf[SROM11_SIGN] == SROM11_SIGNATURE) {
		nw = SROM11_WORDS;
		crc_range = nw * 2;
		GCC_DIAGNOSTIC_PUSH_SUPPRESS_CAST();
		if (srom_read(wlc_hw->sih, wlc_hw->sih->bustype, (void *)wlc_hw->regs,
			wlc_hw->osh, 0, crc_range, s->buf, FALSE)) {
			goto out;
		}
		GCC_DIAGNOSTIC_POP();

	} else {
		/* Assert that we have already read enough for sromrev 2 */
		ASSERT(crc_range >= SROM_WORDS * 2);
		nw = SROM_WORDS;
		crc_range = nw * 2;
	}

	crc = ~hndcrc8((uint8 *)s->buf, nw * 2 - 1, CRC8_INIT_VALUE);
	WL_INFORM(("crc = 0x%x, s->buf[%d-1]=0x%x\n", crc, nw, s->buf[nw-1]));

	if ((s->buf[nw - 1] >> 8) != crc)
		status = WL_DIAGERR_SROM_BADCRC;

out:
	/* restore intrs */
	wl_intrsrestore(wlc_hw->wlc->wl, macintmask);

	if (s)
		MFREE(wlc_hw->osh, s, ssize);
	return status;
}

static void
wlc_diag_led_blink(wlc_hw_info_t *wlc_hw, uint leds)
{
	int i;

	for (i = 0; i < DIAG_LED_BLINK_TIMES; i++) {
		wlc_diag_led_toggle(wlc_hw, leds, ON);
		OSL_DELAY(DIAG_LED_SLEEP);
		wlc_diag_led_toggle(wlc_hw, leds, OFF);
		OSL_DELAY(DIAG_LED_SLEEP);
	}

}

static uint32
wlc_diag_led(wlc_hw_info_t *wlc_hw)
{
	uint32 ledoe = WLC_ACTIVITY_LED | WLC_RADIO_LEDS;
	uint32 orig_ledoe;

	/* save led outen */
	orig_ledoe = si_gpioouten(wlc_hw->sih, 0, 0, GPIO_DRV_PRIORITY);

	/* Enable the first 3 gpios */
	si_gpioouten(wlc_hw->sih, ledoe, ledoe, GPIO_DRV_PRIORITY);

	wlc_diag_led_toggle(wlc_hw, WLC_ACTIVITY_LED | WLC_RADIO_LEDS, OFF);

	wlc_diag_led_blink(wlc_hw, WLC_ACTIVITY_LED);

	if (NBANDS_HW(wlc_hw) == 2) {
		/* DELL request to blink individual lines first */
		wlc_diag_led_blink(wlc_hw, WLC_RADIO_LED_B);

		wlc_diag_led_blink(wlc_hw, WLC_RADIO_LED_A);
	}

	wlc_diag_led_blink(wlc_hw, WLC_RADIO_LEDS);

	wlc_diag_led_blink(wlc_hw, WLC_ACTIVITY_LED | WLC_RADIO_LEDS);

	wlc_diag_led_toggle(wlc_hw, WLC_ACTIVITY_LED | WLC_RADIO_LEDS, OFF);

	/* restore original led outen */
	si_gpioouten(wlc_hw->sih, orig_ledoe, orig_ledoe, GPIO_DRV_PRIORITY);

	return WL_DIAGERR_SUCCESS;
}

static uint32
wlc_diag_reg(wlc_hw_info_t *wlc_hw)
{
	if (!wlc_bmac_validate_chip_access(wlc_hw))
		return WL_DIAGERR_REG_FAIL;

	return WL_DIAGERR_SUCCESS;
}

/* Run bist on current core. core may have specific requirements and bist hazards */
static uint32
wlc_diag_runbist(wlc_info_t *wlc)
{
	wlc_hw_info_t *wlc_hw = (wlc_hw_info_t*)wlc->hw;
	uint32 status = WL_DIAGERR_SUCCESS;
	uint32 cflags, sflags;
	uint32 biststatus = 0, biststatus2 = 0;
	uint16 bw;

	/* only supports d11 for now */
	ASSERT(si_coreid(wlc_hw->sih) == D11_CORE_ID);

	if (!wlc_hw->sbclk)
		return status;

	/* pre processing */
	bw = phy_utils_get_bwstate(wlc_hw->band->pi);
	if (WLCISNPHY(wlc_hw->band)) {
		/* BIST can be run only in 80Mhz(20Mhz channel), not 160Mhz/40Mhz phy clock
		 * save the current phy_bw, force it to 20Mhz channel, restore after finishing bist
		 */
		phy_utils_set_bwstate(wlc_hw->band->pi, WL_CHANSPEC_BW_20);
		wlc_bmac_phy_reset(wlc_hw);
		phy_radio_runbist_config(wlc_hw->band->pi, ON);
	}

	cflags = si_core_cflags(wlc_hw->sih, 0, 0);

	sflags = 0;

	/* run the bist */

	si_core_cflags(wlc_hw->sih, ~0, (cflags | SICF_BIST_EN | SICF_FGC | SICF_PCLKE));

	/* Wait for bist done */
	SPINWAIT((((sflags = si_core_sflags(wlc_hw->sih, 0, 0)) & SISF_BIST_DONE) == 0), 40000);

	if (!(sflags & SISF_BIST_DONE)) {
		status = WL_DIAGERR_MEMORY_TIMEOUT;
		goto done;
	}

	if (sflags & SISF_BIST_ERROR) {
		biststatus = R_REG(wlc_hw->osh, D11_biststatus(wlc_hw));
		biststatus2 = R_REG(wlc_hw->osh, D11_syncSlowTimeStamp(wlc_hw));
		WL_ERROR(("wl%d: wlc_diag_runbist, sflags 0x%x status 0x%x, status2 0x%x",
			wlc_hw->unit, sflags, biststatus, biststatus2));
		BCM_REFERENCE(biststatus);
		BCM_REFERENCE(biststatus2);
		status = WL_DIAGERR_MEMORY_FAIL;
	}

done:
	si_core_cflags(wlc_hw->sih, ~0, cflags);
	wlc_hw->ucode_loaded = FALSE;

	/* post processing */
	if (WLCISNPHY(wlc_hw->band)) {
		phy_radio_runbist_config(wlc_hw->band->pi, OFF);
		phy_utils_set_bwstate(wlc_hw->band->pi, bw);
		wlc_bmac_phy_reset(wlc_hw);
	}

	return WL_DIAGERR_SUCCESS;
}

/** selects RXBMMAP pass-through path, meaning the block will behave like it does not exist. */
static void
_diag_lb_rxbmmap_disable(wlc_hw_info_t *wlc_hw, uint32 *save)
{
	uint32 bmccmd1;

	if (D11REV_LT(wlc_hw->corerev, 48))
		return;

	/* Normal RXBMMAP operation if 0.clear device interrupts
	 */
	*save = R_REG(wlc_hw->osh, D11_BMCCmd1(wlc_hw));
	bmccmd1 = *save | (1 << BMCCmd1_RXMapPassThru_SHIFT);
	W_REG(wlc_hw->osh, D11_BMCCmd1(wlc_hw), bmccmd1);
}

static void
_diag_lb_rxbmmap_restore(wlc_hw_info_t *wlc_hw, uint32 save)
{
	if (D11REV_LT(wlc_hw->corerev, 48))
		return;

	W_REG(wlc_hw->osh, D11_BMCCmd1(wlc_hw), save);
}

static void
_diag_lb_fifo_sel(wlc_hw_info_t *wlc_hw, uint32 *save)
{
	uint32 rcv_ctl;

	/* fifoSel field of RXE RCV_CTL */
	*save = R_REG(wlc_hw->osh, D11_RCV_CTL(wlc_hw));
	rcv_ctl = ((*save & ~RX_CTL_FIFOSEL_MASK) |
		((RX_FIFO << RX_CTL_FIFOSEL_SHIFT) & RX_CTL_FIFOSEL_MASK));
	W_REG(wlc_hw->osh, D11_RCV_CTL(wlc_hw), rcv_ctl);
}

static void
_diag_lb_fifo_sel_restore(wlc_hw_info_t *wlc_hw, uint32 save)
{
	W_REG(wlc_hw->osh, D11_RCV_CTL(wlc_hw), save);
}

/**
 * Performs a simple loopback test of one frame on one particular tx and one particular rx FIFO.
 * Disables d11 core interrupts and polls instead.
 *
 * @param lb_int  WL_DIAG_INTERRUPT (only test irq) or WL_DIAG_LOOPBACK (validate rx'ed data)
 */
static uint32
wlc_diag_lb_int(wlc_info_t *wlc, uint32 lb_int)
{
#define WAIT_COUNT_XI	100	/* wait count for tx interrupt */

	wlc_hw_info_t *wlc_hw = wlc->hw;
	osl_t *osh;
	void *p;
	void *rx_p;
	uint patterns[] = {
		0x00000000, 0xffffffff, 0x55555555, 0xaaaaaaaa,
		0x33333333, 0xcccccccc, 0x66666666, 0x99999999
	};
	uint patternlen;
	uint8 *data;
	uint8 *rx_data;
	uint32 intstatus;
	uint32 macintstatus;
	uint32 rxbmmap_save = 0;
	uint32 fifosel_save;
	int i;
	uint32 status = WL_DIAGERR_SUCCESS;
#ifdef BULKRX_PKTLIST
	void *sts_list = NULL;
#endif /* BULKRX_PKTLIST */

	ASSERT((lb_int == WL_DIAG_INTERRUPT) || (lb_int == WL_DIAG_LOOPBACK));

	osh = wlc_hw->osh;

	/* init tx and rx dma channels */
	dma_txinit(wlc->hw->di[TX_DATA_FIFO]); /* == TX_AC_BE_FIFO */
	dma_rxinit(wlc->hw->di[RX_FIFO]);

	/* post receive buffers */
	wlc_bmac_dma_rxfill(wlc_hw, RX_FIFO);

	/* disable rxbmmap */
	_diag_lb_rxbmmap_disable(wlc->hw, &rxbmmap_save);

	/* fifo selection */
	_diag_lb_fifo_sel(wlc->hw, &fifosel_save);

	/* enable fifo-level loopback */
	dma_fifoloopbackenable(wlc->hw->di[TX_DATA_FIFO]);

	/* alloc one tx packet */
	if ((p = PKTGET(osh, sizeof(patterns), TRUE)) == NULL) {
		WL_ERROR(("wl%d: %s: tx PKTGET error\n", wlc_hw->unit, __FUNCTION__));
		status = WL_DIAGERR_NOMEM;
		goto cleanup;
	}
	data = (uint8 *)PKTDATA(osh, p);
	ASSERT(ISALIGNED(data, sizeof(uint32)));

	bcopy((uint8 *)patterns, data, sizeof(patterns));

	/* clear device interrupts */
	macintstatus = R_REG(osh, D11_MACINTSTATUS(wlc));
	W_REG(osh, D11_MACINTSTATUS(wlc), macintstatus);
	intstatus = R_REG(osh, &(D11Reggrp_intctrlregs(wlc_hw, TX_DATA_FIFO)->intstatus));
	W_REG(osh, &(D11Reggrp_intctrlregs(wlc_hw, TX_DATA_FIFO)->intstatus), intstatus);

	wlc_bmac_txfifo(wlc_hw, TX_DATA_FIFO, p, TRUE, INVALIDFID, 1); // queues one tx frame

	/* expect tx interrupt */
	for (i = 0; i < WAIT_COUNT_XI; i++) {
		OSL_DELAY(10);	/* wait for fifo loopback to finish */
		intstatus = R_REG(osh, &(D11Reggrp_intctrlregs(wlc_hw, TX_DATA_FIFO)->intstatus));

		if (intstatus & (I_PC | I_PD | I_DE | I_RU | I_RO | I_XU)) {
			status = (lb_int == WL_DIAG_INTERRUPT) ? WL_DIAGERR_INTERRUPT_FAIL :
			    WL_DIAGERR_LOOPBACK_FAIL;

			macintstatus = R_REG(osh, D11_MACINTSTATUS(wlc));
			WL_ERROR(("wl%d: %s: error int: macintstatus 0x%x intstatus[%d] 0x%x\n",
				wlc_hw->unit, __FUNCTION__, macintstatus, TX_DATA_FIFO,
				intstatus));
			goto cleanup;
		}
		if (intstatus & I_XI)
			break;
	}
	if (!(intstatus & I_XI)) {
		status = (lb_int == WL_DIAG_INTERRUPT) ? WL_DIAGERR_INTERRUPT_FAIL :
		    WL_DIAGERR_LOOPBACK_FAIL;
		macintstatus = R_REG(osh, D11_MACINTSTATUS(wlc));
		WL_ERROR(("wl%d: %s: timeout waiting tx int; macintstatus 0x%x intstatus[%d]"
			" 0x%x\n",
			wlc_hw->unit, __FUNCTION__, macintstatus, TX_DATA_FIFO, intstatus));
		goto cleanup;
	}

	/* rx */
#ifdef BULKRX_PKTLIST
	SPINWAIT((rx_p = dma_rx(wlc_hw->di[RX_FIFO], &sts_list, 1)) == NULL, 5000);
	if (sts_list != NULL) {
		STSBUF_FREE(sts_list, &wlc_hw->sts_mempool);
	}
#else
	SPINWAIT((rx_p = dma_rx(wlc->hw->di[RX_FIFO])) == NULL, 5000);
#endif /* BULKRX_PKTLIST */

	/* no packet received */
	if (rx_p == NULL) {
		status = (lb_int == WL_DIAG_INTERRUPT) ? WL_DIAGERR_INTERRUPT_FAIL :
		    WL_DIAGERR_LOOPBACK_FAIL;
		WL_ERROR(("wl%d: %s: no packet looped back\n", wlc_hw->unit, __FUNCTION__));
		goto cleanup;
	}

	/* done if choice is interrupt diag
	 * Since rx interrupt is not asserted for fifo loopback, the surest way
	 * to ensure flushing is to have dma rx done.
	 */
	if (lb_int == WL_DIAG_INTERRUPT) {
		status = WL_DIAGERR_SUCCESS;
	} else {
		/* compare */
		rx_data = (uint8 *)PKTDATA(wlc->osh, rx_p) + wlc->hwrxoff;
		patternlen = sizeof(patterns);
		if (bcmp(data, rx_data, patternlen) == 0)
			status = WL_DIAGERR_SUCCESS;
		else {
			status = WL_DIAGERR_LOOPBACK_FAIL;
			WL_ERROR(("wl%d: %s: data miscompare\n", wlc_hw->unit, __FUNCTION__));
		}
	}

	PKTFREE(wlc->osh, rx_p, FALSE);

cleanup:
	/* restore rxbbmap */
	_diag_lb_rxbmmap_restore(wlc->hw, rxbmmap_save);

	/* restore rcv_ctl */
	_diag_lb_fifo_sel_restore(wlc->hw, fifosel_save);

	/* reset tx and rx dma channels; also disable fifo loopback */
	dma_txreset(wlc->hw->di[TX_DATA_FIFO]);
	wlc_upd_suspended_fifos_clear(wlc->hw, TX_DATA_FIFO);
	dma_rxreset(wlc->hw->di[RX_FIFO]);

	/* free posted tx and rx packets */
	dma_txreclaim(wlc->hw->di[TX_DATA_FIFO], HNDDMA_RANGE_ALL);
	dma_rxreclaim(wlc->hw->di[RX_FIFO]);

	return status;
}

static unsigned long _seed = 1;

static void
_srand(unsigned long l)
{
	_seed = l;
}

static unsigned long
_rand(void)
{
	long x, hi, lo, t;

	x = _seed;
	hi = x / 127773;
	lo = x % 127773;
	t = 16807 * lo - 2836 * hi;
	if (t <= 0) t += 0x7fffffff;
	_seed = t;
	return t;
}

struct buf_info {
	void *pkt;
	uint32 off;
	uint32 len;
} tx_bi[MAX_LOOPBACK_PKTS], rx_bi[MAX_LOOPBACK_PKTS];

/**
 * Enhanced loopback test. Performs a loopback test of one frame
 * on one particular tx and one particular rx FIFO.
 * Enhancements:
 * - better test coverage by introducing a factor of randomness
 *
 * @param lb_int   WL_DIAG_LOOPBACK_EXT
 */
static uint32
wlc_diag_lb_int_ext(wlc_info_t *wlc, uint32 lb_int)
{
	wlc_hw_info_t *wlc_hw = wlc->hw;
	osl_t *osh = wlc_hw->osh;
	void *p;
	void *rx_p;
	uint patterns[] = {
		0x00000000, 0xffffffff, 0x55555555, 0xaaaaaaaa,
		0x33333333, 0xcccccccc, 0x66666666, 0x99999999
	};
	uint patternlen = sizeof(patterns);
	uint8 *data;
	uint8 *rx_data;
	int i;
	uint32 status = WL_DIAGERR_SUCCESS;
	uint32 txcnt, rxcnt;
	int txavail;
	uint32 txlen = 0, mrrs, pfctl, tx_bl, err_inj;
	uint32 rxbmmap_save = 0;
	uint32 fifosel_save;
	hnddma_t *di;
	int maxlen;
	uint16 rxoffset, rxbufsize;

	ASSERT(lb_int == WL_DIAG_LOOPBACK_EXT);

	txcnt = 0;
	rxcnt = 0;
	p = NULL;

	_srand(OSL_SYSUPTIME());

	if (_rand() % 2) {
		mrrs = 1024;
		tx_bl = DMA_BL_1024;
	} else {
		mrrs = 512;
		tx_bl = DMA_BL_512;
	}

	pfctl = (_rand() % 3) + DMA_PC_4;
	err_inj = _rand() % 3;

	printf("mrrs %d, pfctl %d, errinj %d\n", mrrs, pfctl, err_inj);

	si_pcie_set_request_size(wlc->hw->sih, (uint16)mrrs);
	/* nop for BCMDBG builds and dongle builds */
	si_pcie_set_error_injection(wlc->hw->sih, err_inj);

	/* turn on aspm */
	si_pcielcreg(wlc_hw->sih, 3, 3);
	/* turn on clkreq */
	si_pcieclkreq(wlc_hw->sih, 1, 1);

	for (i = 0; i < 2; i ++) {
		if (i == 0)
			di = wlc->hw->di[TX_DATA_FIFO];
		else
			di = wlc->hw->di[RX_FIFO];

		dma_param_set(di, HNDDMA_PID_TX_MULTI_OUTSTD_RD, DMA_MR_2);
		dma_param_set(di, HNDDMA_PID_TX_PREFETCH_CTL, (uint16)pfctl);
		dma_param_set(di, HNDDMA_PID_TX_PREFETCH_THRESH, DMA_PT_8);
		dma_param_set(di, HNDDMA_PID_TX_BURSTLEN, (uint16)tx_bl);
		dma_param_set(di, HNDDMA_PID_RX_PREFETCH_CTL, (uint16)pfctl);
		dma_param_set(di, HNDDMA_PID_RX_PREFETCH_THRESH, DMA_PT_8);
		dma_param_set(di, HNDDMA_PID_RX_BURSTLEN, DMA_BL_256);
	}

	/* init tx and rx dma channels */
	dma_txinit(wlc->hw->di[TX_DATA_FIFO]);
	dma_rxinit(wlc->hw->di[RX_FIFO]);

	/* disable rxbmmap */
	_diag_lb_rxbmmap_disable(wlc->hw, &rxbmmap_save);

	/* fifo selection */
	_diag_lb_fifo_sel(wlc->hw, &fifosel_save);

	/* enable fifo-level loopback */
	dma_fifoloopbackenable(wlc->hw->di[TX_DATA_FIFO]);

	dma_rxparam_get(wlc->hw->di[RX_FIFO], &rxoffset, &rxbufsize);
	maxlen = rxbufsize - rxoffset;

	while (txcnt < MAX_LOOPBACK_PKTS || rxcnt < MAX_LOOPBACK_PKTS) {
		/* tx */
		while (txcnt < MAX_LOOPBACK_PKTS) {
			/* alloc tx packet */
			if (p == NULL) {
				txlen = (_rand() % maxlen) + 1;
				if ((p = PKTGET(osh, txlen, TRUE)) == NULL) {
					WL_ERROR(("wl%d: %s: tx PKTGET error\n",
						wlc_hw->unit, __FUNCTION__));
					status = WL_DIAGERR_NOMEM;
					break;
				}
				data = (uint8 *)PKTDATA(osh, p);
				ASSERT(ISALIGNED(data, sizeof(uint32)));

				for (i = 0; i < (int)(txlen / patternlen); i ++)
					bcopy((uint8 *)patterns, data + i * patternlen, patternlen);

				data[0] = (uint8)txcnt;
			}

			txavail = TXAVAIL(wlc, TX_DATA_FIFO);

			if (txavail < 255) {
				while (wlc_bmac_dma_getnexttxp(wlc, TX_DATA_FIFO,
					HNDDMA_RANGE_TRANSFERED));
			}

			txavail = TXAVAIL(wlc, TX_DATA_FIFO);

			/* tx */
			if (txavail == 0)
				break;

			if (wlc_bmac_dma_txfast(wlc, TX_DATA_FIFO, p, TRUE)) {
				PKTFREE(wlc->osh, p, TRUE);
			}

			tx_bi[txcnt].pkt = p;
			tx_bi[txcnt].off = 0;
			tx_bi[txcnt].len = txlen;

			p = NULL;
			txcnt ++;
		}

		/* Send reads/writes(benign register) randomly in middle of Loopback test */
		switch (_rand() % 4) {
		case 0:
			si_corereg(wlc_hw->sih, 0, 0, 0, 0);
			break;
		case 1:
			si_corereg(wlc_hw->sih, 1, 0x120, 0, 0);
			break;
		case 2:
			si_corereg(wlc_hw->sih, 0, 0xd0, ~0, 0x5a5a5a5a);
			break;
		case 3:
			/* do nothing */
			break;
		default:
			break;
		}

		if (dma_peeknextrxp(wlc->hw->di[RX_FIFO]) == NULL) {
			if (_rand() % 2)
				wlc_bmac_dma_rxfill(wlc_hw, RX_FIFO);
		}

		/* rx */
		while (rxcnt < MAX_LOOPBACK_PKTS) {
#ifdef BULKRX_PKTLIST
			void *sts_list = NULL;
			rx_p = dma_rx(wlc_hw->di[RX_FIFO], &sts_list, 1);
			if (sts_list != NULL) {
				STSBUF_FREE(sts_list, &wlc_hw->sts_mempool);
			}
#else
			rx_p = dma_rx(wlc->hw->di[RX_FIFO]);
#endif /* BULKRX_PKTLIST */
			if (rx_p == NULL)
				break;

			rx_data = (uint8 *)PKTDATA(osh, rx_p) + wlc->hwrxoff;

			rx_bi[rxcnt].pkt = rx_p;
			rx_bi[rxcnt].off = wlc->hwrxoff;
			rx_bi[rxcnt].len = PKTLEN(osh, rx_p);

			rxcnt ++;
		}
	}

	/* reset tx and rx dma channels; also disable fifo loopback */
	dma_txreset(wlc->hw->di[TX_DATA_FIFO]);
	wlc_upd_suspended_fifos_clear(wlc->hw, TX_DATA_FIFO);
	dma_rxreset(wlc->hw->di[RX_FIFO]);

	while (wlc_bmac_dma_getnexttxp(wlc, TX_DATA_FIFO, HNDDMA_RANGE_ALL));

	dma_rxreclaim(wlc->hw->di[RX_FIFO]);

	for (i = 0; i < MAX_LOOPBACK_PKTS; i ++) {
		if (tx_bi[i].len != rx_bi[i].len - rx_bi[i].off) {
			WL_ERROR(("wl%d: %s: length mismatch %d %d %d\n",
				wlc_hw->unit, __FUNCTION__, i, tx_bi[i].len, rx_bi[i].len));
			status = WL_DIAGERR_LOOPBACK_FAIL;
			break;
		}

		data = (uint8 *)PKTDATA(osh, tx_bi[i].pkt);
		rx_data = (uint8 *)PKTDATA(osh, rx_bi[i].pkt) + rx_bi[i].off;

		if (bcmp(data, rx_data, tx_bi[i].len) != 0) {
			WL_ERROR(("wl%d: %s: data mismatch %d %02x %02x\n",
				wlc_hw->unit, __FUNCTION__, i, data[0], rx_data[0]));
			status = WL_DIAGERR_LOOPBACK_FAIL;
			break;
		}
	}

	for (i = 0; i < MAX_LOOPBACK_PKTS; i ++) {
		PKTFREE(osh, tx_bi[i].pkt, FALSE);
		PKTFREE(osh, rx_bi[i].pkt, FALSE);
	}

	/* restore rxbbmap */
	_diag_lb_rxbmmap_restore(wlc->hw, rxbmmap_save);

	/* restore rcv_ctl */
	_diag_lb_fifo_sel_restore(wlc->hw, fifosel_save);

	return status;
}

static void
wlc_diag_lb_ucode_enter_testmode(wlc_info_t *wlc)
{
	wlc_hw_info_t *wlc_hw = wlc->hw;

	wlc_txflowcontrol(wlc, qi, ON, ALLPRIO); // avoids tx packets from host interfere with test
	wlc_sync_txfifo_all(wlc, qi, FLUSHFIFO_FLUSHID);
	wlc_bmac_suspend_mac_and_wait(wlc_hw);

	//_diag_lb_rxbmmap_disable(wlc_hw, &rxbmmap_save); RVo required ?

	wlc_bmac_set_deaf(wlc_hw, TRUE); // avoids rx packets from air to interfere with test
	wlc_bmac_write_shm(wlc_hw, M_DIAG_FLAGS(wlc), C_DIAG_BMCLPB_NBIT);
	wlc->ucode_loopback_test = TRUE;
	wlc_bmac_enable_mac(wlc_hw);
	n_lb_pkts = 0;
}

static void
wlc_diag_lb_ucode_exit_testmode(wlc_info_t *wlc)
{
	wlc_hw_info_t *wlc_hw = wlc->hw;

	printf("%s\n", __FUNCTION__);
	wlc_bmac_suspend_mac_and_wait(wlc_hw);
	wlc_bmac_write_shm(wlc_hw, M_DIAG_FLAGS(wlc), 0);
//	wlc_bmac_clear_deaf(wlc_hw, TRUE); crashes with this line in place
	wlc->ucode_loopback_test = FALSE;
	wlc_bmac_enable_mac(wlc_hw);
	wlc_txflowcontrol(wlc, qi, OFF, ALLPRIO);
}

/**
 * Firmware generates 802.3 packets, d11 core loops tx packets back to rx. Called via an ioctl.
 * Preconditions: requires DIAG ucode and IBSS operation.
 */
static uint32
wlc_diag_lb_ucode(wlc_info_t *wlc)
{
	uint32 status = WL_DIAGERR_SUCCESS;
	int32 idx;
	wlc_bsscfg_t *bsscfg;
	wlc_bsscfg_t *ibsscfg = NULL;

	if (!wlc->pub->associated)
		return WL_DIAGERR_NOT_SUPPORTED; // IBSS association required

	FOREACH_IBSS(wlc, idx, bsscfg) {
		if (ibsscfg != NULL) {
			printf("Error: more than 1 IBSS configured\n");
			return WL_DIAGERR_NOT_SUPPORTED;
		}
		ibsscfg = bsscfg;
	}

	if (ibsscfg == NULL) {
		printf("Error: no IBSS configured\n");
		return WL_DIAGERR_NOT_SUPPORTED;
	}

	qi = ibsscfg->wlcif->qi;
	if (qi != wlc->active_queue) {
		printf("Error: excursion in progress\n");
		return WL_DIAGERR_NOT_SUPPORTED;
	}

	wlc_diag_lb_ucode_enter_testmode(wlc);
	status = wlc_diag_queue_tx_packet(wlc);

	return status;
} /* wlc_diag_lb_ucode */

/**
 * For ucode assisted loopback, the payload of 802.3 tx packets has to be defined, so the looped
 * backed packets can be verified on their contents (in wlc_diag_verify_rx_packet()).
 *
 * @param p   points at the payload (so directly following the ethernet header)
 */
static void
wlc_diag_fill_tx_packet(char *p, int payload_len)
{
	while (payload_len--) {
		*p++ = (payload_len & 0xFF); // writes a data pattern
	}
}

/**
 * checks payload of a looped back 802.3 packet
 */
static void
wlc_diag_verify_rx_packet(osl_t *osh, void *pkt)
{
	char expected;
	int i = 0;
	unsigned char *p = PKTDATA(osh, pkt);
	int payload_len = PKTLEN(osh, pkt) - ETHER_HDR_LEN - 2 - ETHER_CRC_LEN;
	int err_cnt = 0;
	p += ETHER_HDR_LEN; // skip 802.3 header

	while (payload_len-- > 0) {
		expected = (payload_len & 0xFF);
		if (*p != expected) {
			printf("%d: verify error exp:%02X got:%02X\n", i, expected, *p);
			if (++err_cnt > 5) {
				break; // avoids console being flooded with errors
			}
		}
		p++; i++;
	}

	if (err_cnt == 0) {
		printf("looped back packet is OK\n");
	}
}

/**
 * For ucode assisted loopback, tx packets have to be created by the driver and forwarded to ucode.
 * Precondition : ucode is not suspended.
 */
static uint32
wlc_diag_queue_tx_packet(wlc_info_t *wlc)
{
	int msdulen = DOT11_A3_HDR_LEN; /**< d11 header length with A3 */
	struct ether_addr *sa = &wlc->pub->cur_etheraddr; // 00:10:18:f8:f1:0f ?
	wlc_bsscfg_t *bsscfg = wlc_bsscfg_find_by_wlcif(wlc, NULL); // obtains primary bsscfg
	void *pkt;
	struct ether_header *eth;
	int payload_len = 100;		/** in [bytes] */
	int err = WL_DIAGERR_FAIL_TO_RUN;

	msdulen += payload_len;
	if ((pkt = PKTGET(wlc->osh, wlc->txhroff + msdulen, TRUE)) == NULL) {
		goto exit;
	}

	PKTPULL(wlc->osh, pkt, wlc->txhroff);

	eth = (struct ether_header *) PKTDATA(wlc->osh, pkt);
	PKTSETLEN(wlc->osh, pkt, msdulen);
	memcpy(eth->ether_dhost, sa, ETHER_ADDR_LEN); // destination
	memcpy(eth->ether_shost, sa, ETHER_ADDR_LEN); // source
	eth->ether_type = ntoh16(msdulen);

	wlc_diag_fill_tx_packet((char*)eth + sizeof(struct ether_header), payload_len);

	err = wlc_pcb_fn_register(wlc->pcb, wlc_diag_loopback_tx_complete, bsscfg, pkt);
	if (err != BCME_OK) {
		goto exit;
	}

	printf("%s: sending packet...\n", __FUNCTION__);
	if (wlc_sendpkt(wlc, pkt, bsscfg->wlcif) == TRUE) { // sends 802.3 packet
		err = BCME_NOMEM;
		goto exit;
	}

	err = WL_DIAGERR_SUCCESS;

exit:
	return err;
} /* wlc_diag_queue_tx_packet */

static int n_lb_pkts;

/** Called when a looped back frame 802.3 frame is received by firmware */
void
wlc_diag_recv(wlc_info_t *wlc, void *p)
{
	printf("%s i_pkt=%d/%d\n", __FUNCTION__, n_lb_pkts, MAX_LOOPBACK_PKTS);
	wlc_diag_verify_rx_packet(wlc->osh, p);

	if (++n_lb_pkts != MAX_LOOPBACK_PKTS) {
		wlc_diag_queue_tx_packet(wlc); // transmit another packet
	} else {
		wlc_diag_lb_ucode_exit_testmode(wlc);
	}
}

/**
 * Ucode in loopback mode does not generate txstatus, thus no MI_TFS irq. Firmware however requires
 * a txstatus for correct transmit operation. Workaround is to 'fake' a txstatus on a d11 receive
 * interrupt (irq caused by reception of a looped back packet).
 */
void
wlc_diag_lb_ucode_fake_txstatus(wlc_info_t *wlc)
{
	tx_status_t txs = {0};
	uint32 s2 = 0;
	uint queue = TX_DATA_FIFO;
	void *p;
	wlc_txh_info_t txh_info;

	while ((p = wlc_bmac_dmatx_peeknexttxp(wlc, queue)), p != NULL) {
		wlc_get_txh_info(wlc, p, &txh_info);
		txs.framelen = txh_info.d11FrameSize;
		txs.frameid = txh_info.TxFrameID;
		txs.sequence = txh_info.seq;
		txs.status.was_acked = TRUE;
		wlc_bmac_dotxstatus(wlc->hw, &txs, s2); // consumes head packet of tx DMA queue
	}
}

/**
 * This function is called when a tx packet has completed processing, either by firmware or by the
 * ucode. Ideally when ucode reported a txstatus back to firmware.
 */
static void
wlc_diag_loopback_tx_complete(wlc_info_t *wlc, uint txstatus, void *arg)
{
	printf("%s txs=0x%08X\n", __FUNCTION__, txstatus);
}

/*
 * diagnostics entry:
 *   return code only means if the test is conducted
 *   *result is the detailed test result, non-zero means failed
 */
int
wlc_diag(wlc_info_t *wlc, uint32 diagtype, uint32 *result)
{
	wlc_hw_info_t *wlc_hw = wlc->hw;
	*result = WL_DIAGERR_FAIL_TO_RUN;

	switch (diagtype) {
	case WL_DIAG_MEMORY:
	case WL_DIAG_LED:
	case WL_DIAG_SROM:
	case WL_DIAG_INTERRUPT:
	case WL_DIAG_LOOPBACK:
	case WL_DIAG_LOOPBACK_EXT:
		if (wlc_hw->up)
			return BCME_NOTDOWN;
		break;
	}

	if (!wlc_hw->clk)
		return BCME_NOCLK;

	/* initialize result to success, each testcase will override it for fail reason */
	*result = 0;

	switch (diagtype) {
	case WL_DIAG_MEMORY:
		*result = wlc_diag_runbist(wlc);
		break;
	case WL_DIAG_LED:
		*result = wlc_diag_led(wlc_hw);
		break;

	case WL_DIAG_REG:
		*result = wlc_diag_reg(wlc_hw);
		break;

	case WL_DIAG_SROM:
		*result = wlc_diag_srom(wlc_hw);
		break;

	case WL_DIAG_INTERRUPT:
	case WL_DIAG_LOOPBACK:
		*result = wlc_diag_lb_int(wlc, diagtype);
		break;

	case WL_DIAG_LOOPBACK_EXT:
		*result = wlc_diag_lb_int_ext(wlc, diagtype);
		break;

	case WL_DIAG_LOOPBACK_UCODE:
		*result = wlc_diag_lb_ucode(wlc);
		break;
	default:
		*result = WL_DIAGERR_NOT_SUPPORTED; /* request diag type is not supported */
		break;
	} /* end of switch (diagtype) */

	return 0;
}
