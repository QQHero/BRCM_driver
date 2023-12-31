/*
   Copyright (c) 2015 Broadcom
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

#include "rdp_common.h"
#include "drivers_common_ag.h"
#include "ten_g_gearbox_ag.h"

bdmf_error_t ag_drv_ten_g_gearbox_gearbox_set(const ten_g_gearbox_gearbox *gearbox)
{
    uint32_t reg_gearbox=0;

#ifdef VALIDATE_PARMS
    if(!gearbox)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
    if((gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off >= _3BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off_ld >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_tx2rx_loop_en >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_rx_data_end >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_clk_en >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_tx_gbox_rstn >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_rx_gbox_rstn >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_tx_cgen_rstn >= _1BITS_MAX_VAL_) ||
       (gearbox->cfg_sgb_pon_10g_epon_rx_cgen_rstn >= _1BITS_MAX_VAL_))
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_RANGE), BDMF_ERR_RANGE);
        return BDMF_ERR_RANGE;
    }
#endif

    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_FIFO_OFF, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_FIFO_OFF_LD, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off_ld);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX2RX_LOOP_EN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_tx2rx_loop_en);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_DATA_END, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_rx_data_end);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_CLK_EN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_clk_en);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_GBOX_RSTN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_tx_gbox_rstn);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_GBOX_RSTN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_rx_gbox_rstn);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_CGEN_RSTN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_tx_cgen_rstn);
    reg_gearbox = RU_FIELD_SET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_CGEN_RSTN, reg_gearbox, gearbox->cfg_sgb_pon_10g_epon_rx_cgen_rstn);

    RU_REG_WRITE(0, TEN_G_GEARBOX, GEARBOX, reg_gearbox);

    return BDMF_ERR_OK;
}

bdmf_error_t ag_drv_ten_g_gearbox_gearbox_get(ten_g_gearbox_gearbox *gearbox)
{
    uint32_t reg_gearbox;

#ifdef VALIDATE_PARMS
    if(!gearbox)
    {
        bdmf_trace("ERROR driver %s:%u| err=%s (%d)\n", __FILE__, __LINE__, bdmf_strerror(BDMF_ERR_PARM), BDMF_ERR_PARM);
        return BDMF_ERR_PARM;
    }
#endif

    RU_REG_READ(0, TEN_G_GEARBOX, GEARBOX, reg_gearbox);

    gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_FIFO_OFF, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_tx_fifo_off_ld = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_FIFO_OFF_LD, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_tx2rx_loop_en = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX2RX_LOOP_EN, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_rx_data_end = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_DATA_END, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_clk_en = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_CLK_EN, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_tx_gbox_rstn = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_GBOX_RSTN, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_rx_gbox_rstn = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_GBOX_RSTN, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_tx_cgen_rstn = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_TX_CGEN_RSTN, reg_gearbox);
    gearbox->cfg_sgb_pon_10g_epon_rx_cgen_rstn = RU_FIELD_GET(0, TEN_G_GEARBOX, GEARBOX, CFG_SGB_PON_10G_EPON_RX_CGEN_RSTN, reg_gearbox);

    return BDMF_ERR_OK;
}

#ifdef USE_BDMF_SHELL
typedef enum
{
    bdmf_address_gearbox,
}
bdmf_address;

static int bcm_ten_g_gearbox_cli_set(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err = BDMF_ERR_OK;

    switch(parm[0].value.unumber)
    {
    case cli_ten_g_gearbox_gearbox:
    {
        ten_g_gearbox_gearbox gearbox = { .cfg_sgb_pon_10g_epon_tx_fifo_off=parm[1].value.unumber, .cfg_sgb_pon_10g_epon_tx_fifo_off_ld=parm[2].value.unumber, .cfg_sgb_pon_10g_epon_tx2rx_loop_en=parm[3].value.unumber, .cfg_sgb_pon_10g_epon_rx_data_end=parm[4].value.unumber, .cfg_sgb_pon_10g_epon_clk_en=parm[5].value.unumber, .cfg_sgb_pon_10g_epon_tx_gbox_rstn=parm[6].value.unumber, .cfg_sgb_pon_10g_epon_rx_gbox_rstn=parm[7].value.unumber, .cfg_sgb_pon_10g_epon_tx_cgen_rstn=parm[8].value.unumber, .cfg_sgb_pon_10g_epon_rx_cgen_rstn=parm[9].value.unumber};
        err = ag_drv_ten_g_gearbox_gearbox_set(&gearbox);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

int bcm_ten_g_gearbox_cli_get(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_error_t err = BDMF_ERR_OK;

    switch(parm[0].value.unumber)
    {
    case cli_ten_g_gearbox_gearbox:
    {
        ten_g_gearbox_gearbox gearbox;
        err = ag_drv_ten_g_gearbox_gearbox_get(&gearbox);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_tx_fifo_off = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off, gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_tx_fifo_off_ld = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off_ld, gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off_ld);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_tx2rx_loop_en = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_tx2rx_loop_en, gearbox.cfg_sgb_pon_10g_epon_tx2rx_loop_en);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_rx_data_end = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_rx_data_end, gearbox.cfg_sgb_pon_10g_epon_rx_data_end);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_clk_en = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_clk_en, gearbox.cfg_sgb_pon_10g_epon_clk_en);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_tx_gbox_rstn = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_tx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_tx_gbox_rstn);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_rx_gbox_rstn = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_rx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_gbox_rstn);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_tx_cgen_rstn = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_tx_cgen_rstn, gearbox.cfg_sgb_pon_10g_epon_tx_cgen_rstn);
        bdmf_session_print(session, "cfg_sgb_pon_10g_epon_rx_cgen_rstn = %u (0x%x)\n", gearbox.cfg_sgb_pon_10g_epon_rx_cgen_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_cgen_rstn);
        break;
    }
    default:
        err = BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

static int bcm_ten_g_gearbox_cli_test(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    bdmf_test_method m = parm[0].value.unumber;
    bdmf_error_t err = BDMF_ERR_OK;

    {
        ten_g_gearbox_gearbox gearbox = {.cfg_sgb_pon_10g_epon_tx_fifo_off=gtmv(m, 3), .cfg_sgb_pon_10g_epon_tx_fifo_off_ld=gtmv(m, 1), .cfg_sgb_pon_10g_epon_tx2rx_loop_en=gtmv(m, 1), .cfg_sgb_pon_10g_epon_rx_data_end=gtmv(m, 1), .cfg_sgb_pon_10g_epon_clk_en=gtmv(m, 1), .cfg_sgb_pon_10g_epon_tx_gbox_rstn=gtmv(m, 1), .cfg_sgb_pon_10g_epon_rx_gbox_rstn=gtmv(m, 1), .cfg_sgb_pon_10g_epon_tx_cgen_rstn=gtmv(m, 1), .cfg_sgb_pon_10g_epon_rx_cgen_rstn=gtmv(m, 1)};
        if(!err) bdmf_session_print(session, "ag_drv_ten_g_gearbox_gearbox_set( %u %u %u %u %u %u %u %u %u)\n", gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off, gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off_ld, gearbox.cfg_sgb_pon_10g_epon_tx2rx_loop_en, gearbox.cfg_sgb_pon_10g_epon_rx_data_end, gearbox.cfg_sgb_pon_10g_epon_clk_en, gearbox.cfg_sgb_pon_10g_epon_tx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_tx_cgen_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_cgen_rstn);
        if(!err) ag_drv_ten_g_gearbox_gearbox_set(&gearbox);
        if(!err) ag_drv_ten_g_gearbox_gearbox_get( &gearbox);
        if(!err) bdmf_session_print(session, "ag_drv_ten_g_gearbox_gearbox_get( %u %u %u %u %u %u %u %u %u)\n", gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off, gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off_ld, gearbox.cfg_sgb_pon_10g_epon_tx2rx_loop_en, gearbox.cfg_sgb_pon_10g_epon_rx_data_end, gearbox.cfg_sgb_pon_10g_epon_clk_en, gearbox.cfg_sgb_pon_10g_epon_tx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_gbox_rstn, gearbox.cfg_sgb_pon_10g_epon_tx_cgen_rstn, gearbox.cfg_sgb_pon_10g_epon_rx_cgen_rstn);
        if(err || gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off!=gtmv(m, 3) || gearbox.cfg_sgb_pon_10g_epon_tx_fifo_off_ld!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_tx2rx_loop_en!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_rx_data_end!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_clk_en!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_tx_gbox_rstn!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_rx_gbox_rstn!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_tx_cgen_rstn!=gtmv(m, 1) || gearbox.cfg_sgb_pon_10g_epon_rx_cgen_rstn!=gtmv(m, 1))
            return err ? err : BDMF_ERR_IO;
    }
    return err;
}

static int bcm_ten_g_gearbox_cli_address(bdmf_session_handle session, const bdmfmon_cmd_parm_t parm[], uint16_t n_parms)
{
    uint32_t i;
    uint32_t j;
    uint32_t index1_start=0;
    uint32_t index1_stop;
    uint32_t index2_start=0;
    uint32_t index2_stop;
    bdmfmon_cmd_parm_t * bdmf_parm;
    const ru_reg_rec * reg;
    const ru_block_rec * blk;
    const char * enum_string = bdmfmon_enum_parm_stringval(session, 0, parm[0].value.unumber);

    if(!enum_string)
        return BDMF_ERR_INTERNAL;

    switch (parm[0].value.unumber)
    {
    case bdmf_address_gearbox : reg = &RU_REG(TEN_G_GEARBOX, GEARBOX); blk = &RU_BLK(TEN_G_GEARBOX); break;
    default :
        return BDMF_ERR_NOT_SUPPORTED;
        break;
    }
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index1")))
    {
        index1_start = bdmf_parm->value.unumber;
        index1_stop = index1_start + 1;
    }
    else
        index1_stop = blk->addr_count;
    if((bdmf_parm = bdmfmon_find_named_parm(session,"index2")))
    {
        index2_start = bdmf_parm->value.unumber;
        index2_stop = index2_start + 1;
    }
    else
        index2_stop = reg->ram_count + 1;
    if(index1_stop > blk->addr_count)
    {
        bdmf_session_print(session, "index1 (%u) is out of range (%u).\n", index1_stop, blk->addr_count);
        return BDMF_ERR_RANGE;
    }
    if(index2_stop > (reg->ram_count + 1))
    {
        bdmf_session_print(session, "index2 (%u) is out of range (%u).\n", index2_stop, reg->ram_count + 1);
        return BDMF_ERR_RANGE;
    }
    if(reg->ram_count)
        for (i = index1_start; i < index1_stop; i++)
        {
            bdmf_session_print(session, "index1 = %u\n", i);
            for (j = index2_start; j < index2_stop; j++)
                bdmf_session_print(session, 	 "(%5u) 0x%lX\n", j, (blk->addr[i] + reg->addr + j));
        }
    else
        for (i = index1_start; i < index1_stop; i++)
            bdmf_session_print(session, "(%3u) 0x%lX\n", i, blk->addr[i]+reg->addr);
    return 0;
}

bdmfmon_handle_t ag_drv_ten_g_gearbox_cli_init(bdmfmon_handle_t driver_dir)
{
    bdmfmon_handle_t dir;

    if ((dir = bdmfmon_dir_find(driver_dir, "ten_g_gearbox"))!=NULL)
        return dir;
    dir = bdmfmon_dir_add(driver_dir, "ten_g_gearbox", "ten_g_gearbox", BDMF_ACCESS_ADMIN, NULL);

    {
        static bdmfmon_cmd_parm_t set_gearbox[]={
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_tx_fifo_off", "cfg_sgb_pon_10g_epon_tx_fifo_off", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_tx_fifo_off_ld", "cfg_sgb_pon_10g_epon_tx_fifo_off_ld", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_tx2rx_loop_en", "cfg_sgb_pon_10g_epon_tx2rx_loop_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_rx_data_end", "cfg_sgb_pon_10g_epon_rx_data_end", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_clk_en", "cfg_sgb_pon_10g_epon_clk_en", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_tx_gbox_rstn", "cfg_sgb_pon_10g_epon_tx_gbox_rstn", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_rx_gbox_rstn", "cfg_sgb_pon_10g_epon_rx_gbox_rstn", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_tx_cgen_rstn", "cfg_sgb_pon_10g_epon_tx_cgen_rstn", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_MAKE_PARM("cfg_sgb_pon_10g_epon_rx_cgen_rstn", "cfg_sgb_pon_10g_epon_rx_cgen_rstn", BDMFMON_PARM_NUMBER, 0),
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="gearbox", .val=cli_ten_g_gearbox_gearbox, .parms=set_gearbox },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "set", "set", bcm_ten_g_gearbox_cli_set,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_cmd_parm_t set_default[]={
            BDMFMON_PARM_LIST_TERMINATOR
        };
        static bdmfmon_enum_val_t selector_table[] = {
            { .name="gearbox", .val=cli_ten_g_gearbox_gearbox, .parms=set_default },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "get", "get", bcm_ten_g_gearbox_cli_get,
            BDMFMON_MAKE_PARM_SELECTOR("purpose", "purpose", selector_table, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_test_method[] = {
            { .name="low" , .val=bdmf_test_method_low },
            { .name="mid" , .val=bdmf_test_method_mid },
            { .name="high" , .val=bdmf_test_method_high },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "test", "test", bcm_ten_g_gearbox_cli_test,
            BDMFMON_MAKE_PARM_ENUM("method", "low: 0000, mid: 1000, high: 1111", enum_table_test_method, 0));
    }
    {
        static bdmfmon_enum_val_t enum_table_address[] = {
            { .name="GEARBOX" , .val=bdmf_address_gearbox },
            BDMFMON_ENUM_LAST
        };
        BDMFMON_MAKE_CMD(dir, "address", "address", bcm_ten_g_gearbox_cli_address,
            BDMFMON_MAKE_PARM_ENUM("method", "method", enum_table_address, 0),
            BDMFMON_MAKE_PARM("index2", "onu_id/alloc_id/port_id/etc...", BDMFMON_PARM_NUMBER, BDMFMON_PARM_FLAG_OPTIONAL));
    }
    return dir;
}
#endif /* USE_BDMF_SHELL */

