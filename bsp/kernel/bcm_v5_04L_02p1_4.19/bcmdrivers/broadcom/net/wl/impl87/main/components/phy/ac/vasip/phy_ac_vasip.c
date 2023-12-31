/*
 * ACPHY VASIP modules implementation
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
 * $Id: phy_ac_vasip.c 804642 2021-11-05 08:16:04Z $
 */

#include <phy_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <phy_mem.h>
#include <phy_vasip.h>
#include <phy_vasip_api.h>
#include <phy_ac.h>
#include <phy_ac_info.h>
#include <phy_ac_vasip.h>
#include <wlc_phy_int.h>
/* ************************ */
/* Modules used by this module */
/* ************************ */
#include <wlc_phyreg_ac.h>

#include <phy_utils_reg.h>
#include <phy_utils_var.h>

/* module private states */
struct phy_ac_vasip_info {
	phy_info_t *pi;
	phy_ac_info_t *aci;
	phy_vasip_info_t *cmn_info;
	uint8	vasipver;
};

typedef struct _svmp_list {
	uint32 addr;
	uint16 cnt;
} svmp_list_t;

typedef struct _d11phyregs_list {
	uint16 addr;
	uint16 cnt;
} d11phyregs_list_t;

CONST d11phyregs_list_t PHYLISTDATA[] = {
	{0x1049, 10}, /* MACVASIP_F_MBOX */
	{0x10EC,  4}, /* VASIP_WDOG */
	{0x1110,  9}, /* SVMP_ADDR_MON */
	{0x11C0, 16}, /* TX_MISC_1 */
	{0x1290, 77}, /* TxbfBfdMuMimo */
	{0x1400,  4}, /* SMC_CONTROL_STATUS part1 */
	{0x1405, 17}, /* SMC_CONTROL_STATUS part2 */
	{0x39f,   2}, /* rxmacphy part1 */
	{0x1368,  6}, /* rxmacphy part2 */
	{0x55c,   3}, /* ImpBfeStatus0Reg */
	{0x439,  22}, /* BfeStatus */
	{0x1094, 12}  /* mailbox, m2v_msg */
};

/*
 * Return vasip version, -1 if not present.
 */
static uint8 phy_ac_vasip_get_ver(phy_type_vasip_ctx_t *ctx);
/*
 * reset/activate vasip.
 */
static void phy_ac_vasip_reset_proc(phy_type_vasip_ctx_t *ctx, int reset);
static void phy_ac_vasip_set_clk(phy_type_vasip_ctx_t *ctx, bool val);
static void phy_ac_vasip_write_bin(phy_type_vasip_ctx_t *ctx, const uint32 vasip_code[],
	const uint nbytes);
#ifdef VASIP_SPECTRUM_ANALYSIS
static void phy_ac_vasip_write_spectrum_tbl(phy_type_vasip_ctx_t *ctx,
	const uint32 vasip_spectrum_tbl[], const uint nbytes_tbl);
#endif
static void phy_ac_vasip_write_svmp(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 val);
static void phy_ac_vasip_read_svmp(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 *val);
static void phy_ac_vasip_write_svmp_blk(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 len,
	uint16 *val);
static void phy_ac_vasip_read_svmp_blk(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 len,
	uint16 *val);
#if defined(BCMDBG)
static void phy_ac_dump_bfd_status(phy_type_vasip_ctx_t *ctx, struct bcmstrbuf *b);
static void phy_ac_dump_svmp(phy_type_vasip_ctx_t *ctx, struct bcmstrbuf *b);
#endif

/* register phy type specific implementation */
phy_ac_vasip_info_t *
BCMATTACHFN(phy_ac_vasip_register_impl)(phy_info_t *pi, phy_ac_info_t *aci,
	phy_vasip_info_t *cmn_info)
{
	phy_ac_vasip_info_t *vasip_info;
	phy_type_vasip_fns_t fns;

	PHY_TRACE(("%s\n", __FUNCTION__));

	/* allocate all storage together */
	if ((vasip_info = phy_malloc(pi, sizeof(phy_ac_vasip_info_t))) == NULL) {
		PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
		goto fail;
	}
	vasip_info->pi = pi;
	vasip_info->aci = aci;
	vasip_info->cmn_info = cmn_info;

	/* register PHY type specific implementation */
	bzero(&fns, sizeof(fns));
	fns.ctx = vasip_info;
	fns.get_ver = phy_ac_vasip_get_ver;
	fns.reset_proc = phy_ac_vasip_reset_proc;
	fns.set_clk = phy_ac_vasip_set_clk;
	fns.write_bin = phy_ac_vasip_write_bin;
#ifdef VASIP_SPECTRUM_ANALYSIS
	fns.write_spectrum_tbl = phy_ac_vasip_write_spectrum_tbl;
#endif
	fns.write_svmp = phy_ac_vasip_write_svmp;
	fns.read_svmp = phy_ac_vasip_read_svmp;
	fns.write_svmp_blk = phy_ac_vasip_write_svmp_blk;
	fns.read_svmp_blk = phy_ac_vasip_read_svmp_blk;
#if defined(BCMDBG)
	fns.dump_bfd_status = phy_ac_dump_bfd_status;
	fns.dump_svmp = phy_ac_dump_svmp;
#endif

	if (ACMAJORREV_GE32(pi->pubpi->phy_rev)) {
		vasip_info->vasipver = READ_PHYREGFLD(pi, PhyCapability2, vasipPresent) ?
			READ_PHYREGFLD(pi, MinorVersion, vasipversion) : VASIP_NOVERSION;
	} else {
		vasip_info->vasipver = VASIP_NOVERSION;
	}

	phy_vasip_register_impl(cmn_info, &fns);

	return vasip_info;

	/* error handling */
fail:
	if (vasip_info != NULL)
		phy_mfree(pi, vasip_info, sizeof(phy_ac_vasip_info_t));
	return NULL;
}

void
BCMATTACHFN(phy_ac_vasip_unregister_impl)(phy_ac_vasip_info_t *vasip_info)
{
	phy_info_t *pi;
	phy_vasip_info_t *cmn_info;

	ASSERT(vasip_info);
	pi = vasip_info->pi;
	cmn_info = vasip_info->cmn_info;

	PHY_TRACE(("%s\n", __FUNCTION__));

	/* unregister from common */
	phy_vasip_unregister_impl(cmn_info);

	phy_mfree(pi, vasip_info, sizeof(phy_ac_vasip_info_t));
}

/*
 * Return vasip version, -1 if not present.
 */
static uint8
phy_ac_vasip_get_ver(phy_type_vasip_ctx_t *ctx)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;

	return info->vasipver;
}

/*
 * reset/activate vasip.
 */
static void
phy_ac_vasip_reset_proc(phy_type_vasip_ctx_t *ctx, int reset)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint32 reset_val = 1;
	uint8  stall_val;
	uint16 vasipregisters_reset_offset, vasipregisters_set_offset;

	vasipregisters_reset_offset = VASIPREGISTERS_RESET;
	vasipregisters_set_offset = VASIPREGISTERS_SET;

	if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
		vasipregisters_reset_offset >>= 2;
		vasipregisters_set_offset >>= 2;
	}

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	if (reset) {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_VASIPREGISTERS,
			1, vasipregisters_reset_offset, 32, &reset_val);
	} else {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_VASIPREGISTERS,
			1, vasipregisters_set_offset, 32, &reset_val);
		pi->vasipi->active = 1;
	}

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

void
phy_ac_vasip_clk_enable(si_t *sih, bool val)
{
	uint32 acphyidx = si_findcoreidx(sih, ACPHY_CORE_ID, 0);

	if (val) {
		/* enter reset */
		si_core_wrapperreg(sih, acphyidx,
			OFFSETOF(aidmp_t, resetctrl), ~0, 0x1);
		/* enable vasip clock */
		si_core_wrapperreg(sih, acphyidx,
			OFFSETOF(aidmp_t, ioctrl), ~0, 0x1);
		/* exit reset */
		si_core_wrapperreg(sih, acphyidx,
			OFFSETOF(aidmp_t, resetctrl), ~0, 0x0);
	} else {
		/* disable vasip clock */
		si_core_wrapperreg(sih, acphyidx,
			OFFSETOF(aidmp_t, ioctrl), ~0, 0x0);
	}
}

static void
phy_ac_vasip_set_clk(phy_type_vasip_ctx_t *ctx, bool val)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;

	MOD_PHYREG(pi, dacClkCtrl, vasipClkEn, val);
}

static void
phy_ac_vasip_write_bin(phy_type_vasip_ctx_t *ctx, const uint32 vasip_code[], const uint nbytes)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint8	stall_val;
	uint32	count, mem_id;
	uint32 svmp_addr = 0x0;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	mem_id = 0;
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);

	count = (nbytes/sizeof(uint32));
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, count, svmp_addr, 32, &vasip_code[0]);

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

#ifdef VASIP_SPECTRUM_ANALYSIS
static void
phy_ac_vasip_write_spectrum_tbl(phy_type_vasip_ctx_t *ctx,
        const uint32 vasip_tbl_code[], const uint nbytes_tbl)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint8  stall_val;
	uint32 count_tbl, mem_id_tbl;
	uint32 svmp_tbl_addr = 0x3400; // (0x26800-0x8000*4)>>1

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	mem_id_tbl = 4;
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id_tbl);

	count_tbl = (nbytes_tbl/sizeof(uint32));
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, count_tbl, svmp_tbl_addr, 32,
		&vasip_tbl_code[0]);

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}
#endif /* VASIP_SPECTRUM_ANALYSIS */

static void
phy_ac_vasip_write_svmp(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 val)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint32 tbl_val, mem_id;
	uint8  stall_val, odd_even;

	mem_id = offset/0x8000;
	offset = offset%0x8000;

	odd_even = offset%2;
	offset = offset >> 1;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);
	wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
	if (odd_even) {
		tbl_val = tbl_val & 0xffff;
		tbl_val = tbl_val | (uint32) (val << NBITS(uint16));
	} else {
		tbl_val = tbl_val & (0xffff << NBITS(uint16));
		tbl_val = tbl_val | (uint32) (val);
	}
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

static void
phy_ac_vasip_read_svmp(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 *val)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint32 tbl_val, mem_id;
	uint8 stall_val, odd_even;

	mem_id = offset/0x8000;
	offset = offset%0x8000;

	odd_even = offset%2;
	offset = offset >> 1;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);
	wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);

	*val = odd_even ? ((tbl_val>> NBITS(uint16)) & 0xffff): (tbl_val & 0xffff);
}

static void
phy_ac_vasip_write_svmp_blk(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 len, uint16 *val)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint32 tbl_val, mem_id;
	uint8  stall_val;
	uint16 n, odd_start, odd_end;

	mem_id = offset / 0x8000;
	offset = offset % 0x8000;

	odd_start = offset % 2;
	odd_end = (offset + len) % 2;

	offset = offset >> 1;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);

	if (odd_start == 1) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		tbl_val &= 0xffff;
		tbl_val |= ((uint32)val[0] << NBITS(uint16));
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		offset += 1;
	}
	for (n = odd_start; n < (len-odd_start-odd_end); n += 2) {
		tbl_val  = ((uint32)val[n+1] << NBITS(uint16));
		tbl_val |= ((uint32)val[n]);
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		offset += 1;
	}
	if (odd_end == 1) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		tbl_val &= (0xffff << NBITS(uint16));
		tbl_val |= ((uint32)val[len-1]);
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
	}

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

static void
phy_ac_vasip_read_svmp_blk(phy_type_vasip_ctx_t *ctx, uint32 offset, uint16 len, uint16 *val)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint32 tbl_val, mem_id;
	uint8 stall_val;
	uint16 n, odd_start, odd_end;

	mem_id = offset / 0x8000;
	offset = offset % 0x8000;

	odd_start = offset % 2;
	odd_end = (offset + len) % 2;

	offset = offset >> 1;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);

	if (odd_start == 1) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		val[0] = ((tbl_val >> NBITS(uint16)) & 0xffff);
		offset += 1;
	}
	for (n = odd_start; n < (len-odd_start-odd_end); n += 2) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		val[n]   = (tbl_val & 0xffff);
		val[n+1] = ((tbl_val >> NBITS(uint16)) & 0xffff);
		offset += 1;
	}
	if (odd_end == 1) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, offset, 32, &tbl_val);
		val[len-1] = (tbl_val & 0xffff);
	}

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

#if defined(BCMDBG)
/* for 43684, 63178 */
static void phy_ac_dump_bfd_status_txv_mem(struct bcmstrbuf *b, phy_info_t *pi)
{
	uint32 tmp_val, tbl_val[4];
	uint16 m, n, usedSlot, slot_count;

	bcm_bprintf(b, "\n===== TXV MEMORY USAGE =====\n");
	wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 4, 224, 32, &tbl_val[0]);
	usedSlot = 0;
	for (m = 0; m < 4; m++) {
		tmp_val = tbl_val[m];
		while (tmp_val > 0) {
			if ((tmp_val & 1) == 0)
				usedSlot ++;
			tmp_val  = tmp_val >> 1;
		}
	}
	bcm_bprintf(b, "----- %d slots are used (total 128 slots) ----- \n", usedSlot);
	for (m = 0; m < 64; m++) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 2, 2*m, 32, &tbl_val[0]);
		slot_count = tbl_val[0] & 0x7;
		if (slot_count > 0) {
			bcm_bprintf(b, "txv index %d has %d slots:   ", m, slot_count);
			for (n = 0; n < slot_count; n++) {
				if (n < 4)
					tmp_val = (tbl_val[0] >> (3+ n*7)) & 0x7f;
				if (n == 4)
					tmp_val = ((tbl_val[0] >> 31) & 0x1)
						+ (tbl_val[1] & 0x3f) * 2;
				if (n > 4)
					tmp_val = (tbl_val[1] >> (6+ (n-5)*7)) & 0x7f;
				bcm_bprintf(b, "%03d ", tmp_val);
			}
			bcm_bprintf(b, "\n");
		}
	}
}

/* for 6710 or later with NEW_MEMORY_MANAGER */
static void phy_ac_dump_bfd_status_txv_mem_new(struct bcmstrbuf *b, phy_info_t *pi)
{
	uint32 tmp_val, tbl_val[8];
	uint16 m, n, usedSlot, slotIndex[8];

	bcm_bprintf(b, "\n===== TXV MEMORY USAGE =====\n");
	wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 8, 224, 32, &tbl_val[0]);
	tbl_val[0] |= 0x1; // slot0 is invalid, mark as free slot
	usedSlot = 0;
	for (m = 0; m < 8; m++) {
		tmp_val = ~tbl_val[m];
		for (n = 0; n < 32; n++) {
			usedSlot += (tmp_val & 1);
			tmp_val >>= 1;
		}
	}
	bcm_bprintf(b, "----- %d slots are used (total 256 slots) ----- \n", usedSlot);
	for (m = 0; m < 64; m++) {  // valid txv: 0~63
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 2, 2*m, 32, &tbl_val[0]);
		for (n = 0; n < 4; n++) {
			slotIndex[0+n] = tbl_val[0] & 0xff;
			slotIndex[4+n] = tbl_val[1] & 0xff;
			tbl_val[0] >>= 8;
			tbl_val[1] >>= 8;
		}
		if (slotIndex[0] > 0) {
			bcm_bprintf(b, "txv index %d:   %03d", m, slotIndex[0]);
			n = 1;
			while ((slotIndex[n] > 0) && (n < 8)) {
				bcm_bprintf(b, "%03d ", slotIndex[n]);
				n++;
			}
			bcm_bprintf(b, "\n");
		}
	}
}

static void phy_ac_dump_bfd_status(phy_type_vasip_ctx_t *ctx, struct bcmstrbuf *b)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint8 stall_val;
	uint32 commandIndex, command;
	uint16 bfdrRunning, bfdrDone, bfdrFailed, rptDestIndex, bfdsLogAddr, bfdrCommandIndex;
	uint16 val0, val1, rptFailed, m, bfdsCommandIndex;
	uint16 frameDone, steeringDone, steeringRunning, steeringCommandBusy, badReport, badCommand;
	int local_commandIndex;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	bfdsLogAddr = READ_PHYREG(pi, bfdsLogAddr);
	/* BFDR STATUS */
	rptDestIndex = READ_PHYREGFLD(pi, bfdrStatus1, rptDestIndex);
	bfdrCommandIndex = READ_PHYREGFLD(pi, bfdrStatus1, rptCommandIndex);
	bfdrRunning = READ_PHYREG(pi, bfdrRunning);
	bfdrDone = READ_PHYREG(pi, bfdrDone);
	bfdrFailed = READ_PHYREG(pi, bfdrFailed);
	bcm_bprintf(b, "===== BFDR STATUS =====\n");
	bcm_bprintf(b, "Destination index of most recently loaded report = %d \n", rptDestIndex);
	bcm_bprintf(b, "In below, each bit repsents a fifo:\n");
	bcm_bprintf(b, "bitmapping of BFDR Running     = 0x%04x \n", bfdrRunning);
	bcm_bprintf(b, "bitmapping of BFDR Done        = 0x%04x \n", bfdrDone);
	bcm_bprintf(b, "bitmapping of BFDR Failed is   = 0x%04x \n", bfdrFailed);

	rptFailed = READ_PHYREGFLD(pi, bfdrStatus0, rptFailed);
	if (rptFailed) {
		commandIndex = READ_PHYREGFLD(pi, bfdrStatus2, rptFailedCommandIndex);
		commandIndex = commandIndex*2+ 512 + bfdsLogAddr*16;
		phy_ac_vasip_read_svmp(ctx, commandIndex, &val0);
		phy_ac_vasip_read_svmp(ctx, commandIndex+1, &val1);
		command = val0 + (val1 << 16);
		bcm_bprintf(b, "command of the failed  report  = 0x%08x \n", command);
	}

	/* BFDS STATUS */
	frameDone = READ_PHYREGFLD(pi, bfdsStatus0, frameDone);
	steeringDone = READ_PHYREGFLD(pi, bfdsStatus0, steeringDone);
	steeringRunning = READ_PHYREGFLD(pi, bfdsStatus0, steeringRunning);
	steeringCommandBusy = READ_PHYREGFLD(pi, bfdsStatus0, steeringCommandBusy);
	bfdsCommandIndex = READ_PHYREGFLD(pi, bfdsStatus0, steeringCommandIndex);
	bcm_bprintf(b, "\n===== BFDS STATUS =====\n");
	bcm_bprintf(b, "frameDone                      = %d \n", frameDone);
	bcm_bprintf(b, "steeringDone                   = %d \n", steeringDone);
	bcm_bprintf(b, "steeringrunning                = %d \n", steeringRunning);
	bcm_bprintf(b, "steeringCommandBusy            = %d \n", steeringCommandBusy);

	badReport = READ_PHYREGFLD(pi, bfdsStatus0, illegalRecord);
	badCommand = READ_PHYREGFLD(pi, bfdsStatus0, illegalCommand);
	if (badReport) {
		commandIndex = READ_PHYREGFLD(pi, bfdsStatus1, illegalRecordIndex);
		commandIndex = commandIndex*2 + bfdsLogAddr*16;
		phy_ac_vasip_read_svmp(ctx, commandIndex, &val0);
		phy_ac_vasip_read_svmp(ctx, commandIndex+1, &val1);
		command = val0 + (val1 << 16);
		bcm_bprintf(b, "command containing bad report = 0x%08x \n", command);
	}

	if (badCommand) {
		commandIndex = READ_PHYREGFLD(pi, bfdsStatus1, illegalCommandIndex);
		commandIndex = commandIndex*2 + bfdsLogAddr*16;
		phy_ac_vasip_read_svmp(ctx, commandIndex, &val0);
		phy_ac_vasip_read_svmp(ctx, commandIndex+1, &val1);
		command = val0 + (val1 << 16);
		bcm_bprintf(b, "the last illegal bfds command  = 0x%08x \n", command);
	}

	/* Dump BFDR/BFDS Commands */
	bcm_bprintf(b, "\n===== DUMPING BFD COMMANDS =====\n");
	bcm_bprintf(b, "Last 16 BFDR commands are: \n");
	for (m = 0; m < 16; m++) {
		local_commandIndex = bfdrCommandIndex -m;
		local_commandIndex = (local_commandIndex < 0) ? (local_commandIndex + 256)
			: local_commandIndex;
		commandIndex = local_commandIndex*2+ 512 + bfdsLogAddr*16;
		phy_ac_vasip_read_svmp(ctx, commandIndex, &val0);
		phy_ac_vasip_read_svmp(ctx, commandIndex+1, &val1);
		command = val0 + (val1 << 16);
		bcm_bprintf(b, "0x%08x \n", command);
	}

	bcm_bprintf(b, "Last 16 BFDs commands are: \n");
	for (m = 0; m < 16; m++) {
		local_commandIndex = bfdsCommandIndex -m;
		local_commandIndex = (local_commandIndex < 0) ? (local_commandIndex + 256)
			: local_commandIndex;
		commandIndex = local_commandIndex*2 + bfdsLogAddr*16;
		phy_ac_vasip_read_svmp(ctx, commandIndex, &val0);
		phy_ac_vasip_read_svmp(ctx, commandIndex+1, &val1);
		command = val0 + (val1 << 16);
		bcm_bprintf(b, "0x%08x \n", command);
	}

	/* Print TXV status */
	if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
		phy_ac_dump_bfd_status_txv_mem_new(b, pi);
	} else {
		phy_ac_dump_bfd_status_txv_mem(b, pi);
	}

	/* clear BFDr/BFDs status */
	WRITE_PHYREG(pi, bfdrCtrl1, 0xffff);
	MOD_PHYREG(pi, bfdsConfig, clrBfdsStatus, 1);

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

static void phy_ac_dump_svmp(phy_type_vasip_ctx_t *ctx, struct bcmstrbuf *b)
{
	phy_ac_vasip_info_t *info = (phy_ac_vasip_info_t *)ctx;
	phy_info_t *pi = info->pi;

	uint8 stall_val;
	uint m, n, idx, step, svmp_len, offset_size;
	uint16 svmp_list_len, svmp_block_len;
	uint16 reg_val, svmp_val[32], phyreg_list_len, phyblock_len = 0;
	uint16 vasipreg_list_len, vasipreg_val;
	uint32 svmp_addr;
	uint32 tbl_val[32];
	uint64 tbl_val_64[32];
	uint8 bfm_busy, smc_busy, steer_running;
	uint32 compr_rpt_start = 0, compr_rpt_end = 0, compr_rpt_step = 0;
	uint32 cqi_rpt_start = 0, cqi_rpt_end = 0;

	static const svmp_list_t svmpmems_4x4[] = {
		{0x40000, 0x80},        /* interrupt & FW info/config/knob */
		{0x40080, 0x80},        /* m2v_buf0 */
		{0x40100, 0x80},        /* m2v_buf1 */
		{0x38000, 0x200},       /* legacy grouping */
		{0x40180, 0x80},        /* legacy precoding */
		{0x38600, 0x80},        /* on-the-fly grouping */
		{0x2bb20, 0x20},        /* pwr_per_ant & pwr_per_ant */
		{0x40280, 0x400},       /* bfds_log_buffer */
		{0x40c00, 0x200},       /* hwsch_log_buffer */
		{0x40e00, 0x200},       /* hwsch_v2m_buffer */
		{0x30000, 0x20},        /* txv_decompressed_report 0 */
		{0x30900, 0x20},        /* txv_decompressed_report 1 */
		{0x31200, 0x20},        /* txv_decompressed_report 2 */
		{0x31b00, 0x20},        /* txv_decompressed_report 3 */
		{0x3c010, 0x20},        /* steering_mcs & recommend_mcs */
		{0x383f0, 0x110},       /* txv_header_idx, txv_header_list */
		{0x46000, 0x180},       /* ru allocation - ru_alloc_buf & v2m_ru_alloc_buf & ... */
		{0x46200, 0x40},        /* ru allocation - cqi_rpt_buf & v2m_buf_cqi */
		{0x47380, 0x30},        /* ru allocation - cqi_ready */
		{0x46240, 0x400},       /* ru allocation - cqi_thpt_gain */
		{0x46800, 0x400},       /* ru allocation - cqi_ru_index */
		{0x46dc0, 0x400},       /* ru allocation - cqi_ru_mask */
		{0x47ba0, 0x90},        /* ru allocation - dbg0 */
		{0xe000,  0x2000},      /* CQI report */
		{0x10000, 0x8000},      /* TXV report IDX =  0 ~ 31 */
		{0x18000, 0x8000},      /* TXV report IDX = 32 ~ 63 */
		{0x20000, 0x20},        /* MU precoder IDX = 64 */
		{0x22000, 0x20},        /* MU precoder IDX = 65 */
		{0x24000, 0x20},        /* MU precoder IDX = 66 */
		{0x26000, 0x20}         /* MU precoder IDX = 67 */
	};
	static const svmp_list_t svmpmems_rev51[] = {
		{0x18000, 0x80},        /* interrupt -- err_code_curr */
		{0x18080, 0x80},        /* m2v_buf0 */
		{0x18100, 0x80},        /* m2v_buf1 */
		{0x16000, 0x200},       /* legacy grouping */
		{0x18180, 0x80},        /* legacy precoding */
		{0x16500, 0x80},        /* on-the-fly grouping */
		{0x18280, 0x400},       /* bfds_log_buffer */
		{0x13120, 0x20},        /* pwr_per_ant, param, precoder_buf */
		{0x0b000, 0x20},        /* txv_decompressed_report (txvheadr) */
		{0x0b500, 0x20},        /* txv_decompressed_report (txvheadr) */
		{0x17010, 0x20},        /* steering_mcs,recommend_mcs */
		{0x162f0, 0x110},       /* txv_header_idx, txv_header_list */
		{0x1e000, 0x180},       /* ru allocation - ru_alloc_buf & v2m_ru_alloc_buf & ... */
		{0x1e200, 0x40},        /* ru allocation - cqi_rpt_buf & v2m_buf_cqi */
		{0x1f380, 0x30},        /* ru allocation - cqi_ready */
		{0x1e240, 0x400},       /* ru allocation - cqi_thpt_gain */
		{0x1e800, 0x400},       /* ru allocation - cqi_ru_index */
		{0x1edc0, 0x400},       /* ru allocation - cqi_ru_mask */
		{0x1fba0, 0x90},        /* ru allocation - dbg0 */
		{0xe800,  0x1000},      /* CQI report */
		{0x10000, 0x2000},      /* BFD compressed report */
		{0x9000, 0x20},         /* TXV_IDX = 64 */
		{0x9800, 0x20},         /* TXV_IDX = 65 */
		{0xa000, 0x20},         /* TXV_IDX = 66 */
		{0xa800, 0x20}          /* TXV_IDX = 67 */
	};
	static const svmp_list_t svmpmems_rev131[] = {
		{0x18000, 0x80},        /* interrupt -- err_code_curr */
		{0x18080, 0x80},        /* m2v_buf0 */
		{0x18100, 0x80},        /* m2v_buf1 */
		{0x1d000, 0x200},       /* legacy grouping */
		{0x18180, 0x80},        /* legacy precoding */
		{0x1d500, 0x80},        /* on-the-fly grouping */
		{0x18280, 0x400},       /* bfds_log_buffer */
		{0x1a120, 0x20},        /* pwr_per_ant, param, precoder_buf */
		{0x0ab80, 0x20},        /* txv_decompressed_report (txvheadr) */
		{0x0b580, 0x20},        /* txv_decompressed_report (txvheadr) */
		{0x18910, 0x20},        /* steering_mcs,recommend_mcs */
		{0x1d2f0, 0x110},       /* txv_header_idx, txv_header_list */
		{0x1e000, 0x180},       /* ru allocation - ru_alloc_buf & v2m_ru_alloc_buf & ... */
		{0x1e200, 0x40},        /* ru allocation - cqi_rpt_buf & v2m_buf_cqi */
		{0x1f380, 0x30},        /* ru allocation - cqi_ready */
		{0x1e240, 0x400},       /* ru allocation - cqi_thpt_gain */
		{0x1e800, 0x400},       /* ru allocation - cqi_ru_index */
		{0x1edc0, 0x400},       /* ru allocation - cqi_ru_mask */
		{0x1fba0, 0x90},        /* ru allocation - dbg0 */
		{0x14000, 0x1800},      /* CQI report */
		{0x10000, 0x4000},      /* BFD compressed report */
		{0xc000, 0x20},         /* TXV_IDX = 64 */
		{0xd000, 0x20},         /* TXV_IDX = 65 */
		{0xe000, 0x20},         /* TXV_IDX = 66 */
		{0xf000, 0x20}          /* TXV_IDX = 67 */
	};
	static const svmp_list_t svmpmems_3x3[] = {
		{0x20000, 0x80},        /* interrupt -- err_code_curr */
		{0x20080, 0x80},        /* m2v_buf0 */
		{0x20100, 0x80},        /* m2v_buf1 */
		{0x1d000, 0x200},       /* legacy grouping */
		{0x20180, 0x80},        /* legacy precoding */
		{0x1d500, 0x80},        /* on-the-fly grouping */
		{0x20280, 0x400},       /* bfds_log_buffer */
		{0x1a500, 0x20},        /* pwr_per_ant, param, precoder_buf */
		{0xdc00,  0x20},        /* txv_decompressed_report (txvheadr) */
		{0xe300,  0x20},        /* txv_decompressed_report (txvheadr) */
		{0xea00,  0x20},        /* txv_decompressed_report (txvheadr) */
		{0x1d510, 0x20},        /* steering_mcs,recommend_mcs */
		{0x1d2f0, 0x110},       /* txv_header_idx, txv_header_list */
		{0x26000, 0x180},       /* ru allocation - ru_alloc_buf & v2m_ru_alloc_buf & ... */
		{0x26200, 0x40},        /* ru allocation - cqi_rpt_buf & v2m_buf_cqi */
		{0x27380, 0x20},        /* ru allocation - cqi_ready */
		{0x26240, 0x400},       /* ru allocation - cqi_thpt_gain */
		{0x26800, 0x400},       /* ru allocation - cqi_ru_index */
		{0x26dc0, 0x400},       /* ru allocation - cqi_ru_mask */
		{0x27ba0, 0x90},        /* ru allocation - dbg0 */
		{0x1e800, 0x1000},      /* CQI report */
		{0x10000, 0x8000},      /* BFD compressed report */
		{0x9400, 0x20},         /* TXV_IDX = 64 */
		{0xa600, 0x20},         /* TXV_IDX = 65 */
		{0xb800, 0x20},         /* TXV_IDX = 66 */
		{0xca00, 0x20}          /* TXV_IDX = 67 */
	};

	const svmp_list_t *svmpmems = NULL;

	static const uint16 phyreg_list_4x4[] = {0x1301, 0x1311, 0x1f0, 0x1f1, 0x60d, 0x80d,
			0xa0d, 0xc0d, 0x601, 0x801, 0xa01, 0xc01, 0x3ec, 0x3, 0x107d};
	static const d11phyregs_list_t phyregblock_r130[] = {{0x1380, 26}};
	static const uint16 phyreg_list_3x3[] = {0x1301, 0x1311, 0x1f0, 0x1f1, 0x60d, 0x80d,
			0xa0d, 0x601, 0x801, 0xa01, 0x3ec, 0x3, 0x107d};
	static const uint16 phyreg_list_2x2[] = {0x1301, 0x1311, 0x1f0, 0x1f1, 0x60d, 0x80d,
			0x601, 0x801, 0x3ec, 0x3, 0x107d};
	const d11phyregs_list_t *phyblock_list = NULL;
	const uint16 *phyreg_list = NULL;

	static const uint16 vasipreg_list[] = {0x2c, 0x30, 0xd0, 0xd4, 0xe0, 0xe4};

	if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
		phyblock_list = phyregblock_r130;
		phyblock_len = ARRAYSIZE(phyregblock_r130);
	}

	if (ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
		svmpmems = svmpmems_4x4;
		svmp_list_len = ARRAYSIZE(svmpmems_4x4);
		phyreg_list = phyreg_list_4x4;
		phyreg_list_len = ARRAYSIZE(phyreg_list_4x4);
		compr_rpt_start = 0x10000;
		compr_rpt_end = 0x20000;
		compr_rpt_step = 16;
		cqi_rpt_start = 0xe000;
		cqi_rpt_end = 0x10000;
	} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
		svmpmems = svmpmems_rev51;
		svmp_list_len = ARRAYSIZE(svmpmems_rev51);
		phyreg_list = phyreg_list_2x2;
		phyreg_list_len = ARRAYSIZE(phyreg_list_2x2);
		compr_rpt_start = 0x10000;
		compr_rpt_end = 0x12000;
		compr_rpt_step = 2;
		cqi_rpt_start = 0xe800;
		cqi_rpt_end = 0x10000;
	} else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
		svmpmems = svmpmems_rev131;
		svmp_list_len = ARRAYSIZE(svmpmems_rev131);
		phyreg_list = phyreg_list_2x2;
		phyreg_list_len = ARRAYSIZE(phyreg_list_2x2);
		compr_rpt_start = 0x10000;
		compr_rpt_end = 0x14000;
		compr_rpt_step = 4;
		cqi_rpt_start = 0x14000;
		cqi_rpt_end = 0x15800;
	} else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
		svmpmems = svmpmems_3x3;
		svmp_list_len = ARRAYSIZE(svmpmems_3x3);
		phyreg_list = phyreg_list_3x3;
		phyreg_list_len = ARRAYSIZE(phyreg_list_3x3);
		compr_rpt_start = 0x10000;
		compr_rpt_end = 0x18000;
		compr_rpt_step = 4;
		cqi_rpt_start = 0xe800;
		cqi_rpt_end = 0x10000;
	} else {
		bcm_bprintf(b, "No SVMP dump support for PHYREV %u\n", pi->pubpi->phy_rev);
		return;
	}
	vasipreg_list_len = ARRAYSIZE(vasipreg_list);

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);

	bcm_bprintf(b, "===== Important Register =====\n");
	for (m = 0; m < phyreg_list_len; m++) {
		reg_val = phy_utils_read_phyreg(pi, phyreg_list[m]);
		bcm_bprintf(b, "0x%-4x   0x%-4x\n", phyreg_list[m], reg_val);
	}

	for (m = 0; m < sizeof(PHYLISTDATA)/sizeof(d11phyregs_list_t); m++) {
		for (n = 0; n < PHYLISTDATA[m].cnt; n++) {
			reg_val = phy_utils_read_phyreg(pi, PHYLISTDATA[m].addr+n);
			bcm_bprintf(b, "0x%-4x   0x%-4x\n", PHYLISTDATA[m].addr+n, reg_val);
		}
	}

	for (m = 0; m < phyblock_len; m++) {
		for (n = 0; n < phyblock_list[m].cnt; n++) {
			reg_val = phy_utils_read_phyreg(pi, phyblock_list[m].addr+n);
			bcm_bprintf(b, "0x%-4x   0x%-4x\n", phyblock_list[m].addr+n, reg_val);
		}
	}

	bcm_bprintf(b, "\n===== Important TABLE =====\n");
	/* ACPHY_TBL_ID_AXMACPHYIFTBL has 128 entries and each has 64 bits */
	for (m = 0; m < 4; m++) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_AXMACPHYIFTBL, 32, m*32, 64,
			&tbl_val_64[0]);
		for (n = 0; n < 32; n++) {
			bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x   0x%-8x\n",
				ACPHY_TBL_ID_AXMACPHYIFTBL, n+32*m, (uint32) tbl_val_64[n],
				(uint32) (tbl_val_64[n] >> 32));
		}
	}

	/* dump vasipregister */
	for (m = 0; m < vasipreg_list_len; m++) {
		vasipreg_val = vasipreg_list[m] >> 2;
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_VASIPREGISTERS, 1,
			vasipreg_val, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_VASIPREGISTERS,
			vasipreg_val, tbl_val[0]);
	}

	/* dump vasippchistory 16 index */
	wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_VASIPPCHISTORY, 16, 0, 32, &tbl_val[0]);
	for (m = 0; m < 16; m++) {
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_VASIPPCHISTORY,
			m, tbl_val[m]);
	}

	/* dump smctable */
	for (m = 0; m < 2; m++) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTBL, 1, 0x34, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTBL,
			0x34, tbl_val[0] & 0xffff);
	}

	for (m = 0; m < 2; m++) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTBL, 1, 0x40, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTBL,
			0x40, tbl_val[0] & 0xffff);
	}

	for (m = 0; m < 2; m++) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTBL, 1, 0x44, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTBL,
		0x44, tbl_val[0] & 0xffff);
	}

	smc_busy = READ_PHYREGFLD(pi, SMCStatus, mac2smc_SMC_busy);
	if (!smc_busy || 1) {
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 2, 0x901, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
			0x901, tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
			0x902, tbl_val[1]);

		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 2, 0x9DA, 32, &tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
			0x9DA, tbl_val[0]);
		bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
			0x9DB, tbl_val[1]);

		/* SMC M0 table */
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 32, 0, 32, &tbl_val[0]);
		for (m = 0; m < 32; m++) {
			bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
				m, tbl_val[m]);
		}

		/* SMC M1 table */
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 32, 0x800, 32, &tbl_val[0]);
		for (m = 0; m < 32; m++) {
			bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_SVMPTINYTBL,
				m+0x800, tbl_val[m]);
		}
	} else {
		bcm_bprintf(b, "SMC is running, skip SMC table dump\n");
	}

	bfm_busy = READ_PHYREGFLD(pi, BfmMuStatus0Reg, bfmMuRunning);
	if (!bfm_busy || !ACMAJORREV_131(pi->pubpi->phy_rev)) {
		/* dump BFM header */
		wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFMSTEERMEM, 8, 0, 32,
			&tbl_val[0]);
		for (m = 0; m < 8; m++) {
			bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n", ACPHY_TBL_ID_BFMSTEERMEM,
				m, tbl_val[m]);
		}
	} else {
		bcm_bprintf(b, "BFM is running, skip BFM table dump\n");
	}

	steer_running = READ_PHYREGFLD(pi, bfdsStatus0, steeringRunning);
	if (!steer_running || !ACMAJORREV_131(pi->pubpi->phy_rev)) {
		/* dump bfdIndexLUT 128+32+64 index */
		for (m = 0; m < 7; m++) {
			wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 32, m*32, 32,
				&tbl_val[0]);
			for (n = 0; n < 32; n++) {
				bcm_bprintf(b, "0x%-2x   0x%-4x   0x%-8x\n",
					ACPHY_TBL_ID_BFDINDEXLUT, n+32*m, tbl_val[n]);
			}
		}
	} else {
		bcm_bprintf(b, "BFD is running, skip BFDINDEXLUT dump\n");
	}

	// XXX: check for steering is running for now, May need additional check to skip svmp dump
	if (!steer_running || !ACMAJORREV_131(pi->pubpi->phy_rev)) {
		bcm_bprintf(b, "\n===== Important SVMP =====\n");
		for (m = 0; m < svmp_list_len; m++) {
			svmp_block_len = svmpmems[m].cnt/32;
			svmp_addr = svmpmems[m].addr;

			if ((svmp_addr >= compr_rpt_start) && (svmp_addr < compr_rpt_end)) {
				step = compr_rpt_step, svmp_len = 16, offset_size = 32;
			} else if ((svmp_addr >= cqi_rpt_start) && (svmp_addr < cqi_rpt_end)) {
				step = 11, svmp_len = 176, offset_size = 16;
			} else {
				step = 1, svmp_len = 32, offset_size = 32;
			}

			for (n = 0; n < svmp_block_len; n += step) {
				phy_ac_vasip_read_svmp_blk(ctx, svmp_addr + offset_size*n, svmp_len,
					&svmp_val[0]);
				for (idx = 0; idx < svmp_len; idx++)
					bcm_bprintf(b, "svmp   0x%-4x   0x%-4x\n",
						svmp_addr + offset_size*n + idx, svmp_val[idx]);
			}
		}
	} else {
		bcm_bprintf(b, "BFD is running, skip svmp dump\n");
	}

	/* restore stall value */
	ACPHY_ENABLE_STALL(pi, stall_val);
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);
}

#endif
