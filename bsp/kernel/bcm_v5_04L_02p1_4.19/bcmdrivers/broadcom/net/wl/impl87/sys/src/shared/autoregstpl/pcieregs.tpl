#
# pcieregs.tpl  make file - Generates pcie regs files
#
# $ Copyright (C) 2002-2004 Broadcom  $
#
# <<Broadcom-WL-IPTag/Proprietary:>>
#
# $Id: $

FEATURE_BEGIN[pcie_regs] 
uint32	pciecontrol
uint32	pcieiostatus
uint32	capabilities
uint32	biststatus
uint32	gpioselect
uint32	gpioouten
uint32	GpioOut
uint32	intstatus
uint32	intmask
uint32	sbtopciemailbox
uint32	obffcontrol
uint32	obffintstats
uint32	obffdatastats
uint32	errlog
uint32	errlogaddr
uint32	mailboxint
uint32	mailboxintmask
uint32	ltrspacing
uint32	ltrhysteresiscnt
uint32	msivectorassignment
uint32	intmask2
uint32	msivector0
uint32	msiintmask0
uint32	msiintstatus0
uint32	msivector1
uint32	msiintmask1
uint32	msiintstatus1
uint32	msififopendingcntr
uint32	sbtopcietranslation2
uint32	sbtopcietranslation2upper
uint32	sbtopcietranslation3
uint32	sbtopcietranslation3upper
uint32	sbtopcietranslation0
uint32	sbtopcietranslation1
uint32	sbtopcietranslationbigmem
uint32	sbtopcietranslation0upper
uint32	sbtopcietranslation1upper
uint32	configindaddr
uint32	configinddata
uint32	mdiocontrol
uint32	mdiowrdata
uint32	mdiorddata
uint32	hosttodev0doorbell0
uint32	hosttodev0doorbell1
uint32	devtohost0doorbell0
uint32	devtohost0doorbell1
uint32	hosttodev1doorbell0
uint32	hosttodev1doorbell1
uint32	devtohost1doorbell0
uint32	devtohost1doorbell1
uint32	hosttodev2doorbell0
uint32	hosttodev2doorbell1
uint32	devtohost2doorbell0
uint32	devtohost2doorbell1
uint32	hosttodev3doorbell0
uint32	hosttodev3doorbell1
uint32	devtohost3doorbell0
uint32	devtohost3doorbell1
uint32	dataintfunction
uint32	intreclazydevtohost
uint32	intreclazyhosttodev
uint32	dmahosttodevintstat0
uint32	dmahosttodevintmask0
uint32	dmadevtohostintstat0
uint32	dmadevtohostintmask0
uint32	ltrstate
uint32	powerintstatus
uint32	powerintmask
uint32	pmesource
uint32	clockctlstatus
uint32	workaround
uint32	powercontrol
uint32	xmt0ctl
uint32	xmt0ptr
uint32	xmt0addrlow
uint32	xmt0addrhigh
uint32	xmt0stat0
uint32	xmt0stat1
uint32	xmt0fifoctl
uint32	xmt0fifodata
uint32	rcv0ctl
uint32	rcv0ptr
uint32	rcv0addrlow
uint32	rcv0addrhigh
uint32	rcv0stat0
uint32	rcv0stat1
uint32	rcv0fifoctl
uint32	rcv0fifodata
uint32	xmt1ctl
uint32	xmt1ptr
uint32	xmt1addrlow
uint32	xmt1addrhigh
uint32	xmt1stat0
uint32	xmt1stat1
uint32	xmt1fifoctl
uint32	xmt1fifodata
uint32	rcv1ctl
uint32	rcv1ptr
uint32	rcv1addrlow
uint32	rcv1addrhigh
uint32	rcv1stat0
uint32	rcv1stat1
uint32	rcv1fifoctl
uint32	rcv1fifodata
uint32	error_header_reg1
uint32	error_header_reg2
uint32	error_header_reg3
uint32	error_header_reg4
uint32	error_code
uint32	Ch1_XmtCtrl_Inbound
uint32	Ch1_XmtPtr_Inbound
uint32	Ch1_XmtAddrLow_Inbound
uint32	Ch1_XmtAddrHigh_Inbound
uint32	Ch1_XmtStatus0_Inbound
uint32	Ch1_XmtStatus1_Inbound
uint32	Ch1_RcvCtrl_Inbound
uint32	Ch1_RcvPtr_Inbound
uint32	Ch1_RcvAddrLow_Inbound
uint32	Ch1_RcvAddrHigh_Inbound
uint32	Ch1_RcvStatus0_Inbound
uint32	Ch1_RcvStatus1_Inbound

uint32	Ch1_XmtCtrl_Outbound
uint32	Ch1_XmtPtr_Outbound
uint32	Ch1_XmtAddrLow_Outbound
uint32	Ch1_XmtAddrHigh_Outbound
uint32	Ch1_XmtStatus0_Outbound
uint32	Ch1_XmtStatus1_Outbound
uint32	Ch1_RcvCtrl_Outbound
uint32	Ch1_RcvPtr_Outbound
uint32	Ch1_RcvAddrLow_Outbound
uint32	Ch1_RcvAddrHigh_Outbound
uint32	Ch1_RcvStatus0_Outbound
uint32	Ch1_RcvStatus1_Outbound
uint32	Ch1_intRcv0Lazy
uint32	Ch1_intRcv1Lazy

uint32	Ch1_DmaIntStat_Inbound
uint32	Ch1_DmaIntMask_Inbound

uint32	Ch1_DmaIntStat_Outbound
uint32	Ch1_DmaIntMask_Outbound

uint32	Ch2_XmtCtrl_Inbound
uint32	Ch2_XmtPtr_Inbound
uint32	Ch2_XmtAddrLow_Inbound
uint32	Ch2_XmtAddrHigh_Inbound
uint32	Ch2_XmtStatus0_Inbound
uint32	Ch2_XmtStatus1_Inbound
uint32	Ch2_RcvCtrl_Inbound
uint32	Ch2_RcvPtr_Inbound
uint32	Ch2_RcvAddrLow_Inbound
uint32	Ch2_RcvAddrHigh_Inbound
uint32	Ch2_RcvStatus0_Inbound
uint32	Ch2_RcvStatus1_Inbound

uint32	Ch2_XmtCtrl_Outbound
uint32	Ch2_XmtPtr_Outbound
uint32	Ch2_XmtAddrLow_Outbound
uint32	Ch2_XmtAddrHigh_Outbound
uint32	Ch2_XmtStatus0_Outbound
uint32	Ch2_XmtStatus1_Outbound
uint32	Ch2_RcvCtrl_Outbound
uint32	Ch2_RcvPtr_Outbound
uint32	Ch2_RcvAddrLow_Outbound
uint32	Ch2_RcvAddrHigh_Outbound
uint32	Ch2_RcvStatus0_Outbound
uint32	Ch2_RcvStatus1_Outbound

uint32	Ch2_intRcv0Lazy
uint32	Ch2_intRcv1Lazy

uint32	Ch2_DmaIntStat_Inbound
uint32	Ch2_DmaIntMask_Inbound

uint32	Ch2_DmaIntStat_Outbound
uint32	Ch2_DmaIntMask_Outbound

uint32	num_functions_ep
uint32	srom_offset_4
uint32	srom_offset_6
uint32	srom_pme
uint32	srom_pme_func1

uint32	dar_clk_ctl_st
uint32	dar_power_control
uint32	dar_intstatus
uint32	dar_h2d0_doorbell0
uint32	dar_h2d0_doorbell1
uint32	dar_h2d1_doorbell0
uint32	dar_h2d1_doorbell1
uint32	dar_h2d2_doorbell0
uint32	dar_h2d2_doorbell1
uint32	dar_h2d3_doorbell0
uint32	dar_h2d3_doorbell1
uint32	dar_h2d4_doorbell0
uint32	dar_h2d4_doorbell1
uint32	dar_h2d5_doorbell0
uint32	dar_h2d5_doorbell1
uint32	dar_h2d6_doorbell0
uint32	dar_h2d6_doorbell1
uint32	dar_h2d7_doorbell0
uint32	dar_h2d7_doorbell1

uint32	dar_errorlog
uint32	dar_errorlog_addr
uint32	dar_mailboxint
uint32	cpl_to_ctrl
uint32	write_to_host_int_mask
uint32	write_to_host_int_vector
uint32	write_to_host_int_addr_low
uint32	write_to_host_int_addr_high
uint32	idma_flow_mgr_ctrl
uint32	idma_mask_addr
uint32	idma_mask_data
uint32	idma_priority_array_addr_dev0
uint32	idma_priority_array_data_dev0
uint32	idma_priority_array_addr_dev1
uint32	idma_priority_array_data_dev1
uint32	idma_diff_vec_dev0
uint32	idma_svd_tid_vec_dev0
uint32	idma_svd_diff_vec_dev0
uint32	idma_diff_vec_dev1
uint32	idma_svd_tid_vec_dev1
uint32	idma_svd_diff_vec_dev1
uint32	idma_int_status_dev0
uint32	idma_int_status2_dev0
uint32	idma_int_status3_dev0
uint32	idma_int_status1_dev0
uint32	idma_int_mask_dev0
uint32	idma_int_status_dev1
uint32	idma_int_mask_dev1
uint32	fis
uint32	dar_control
uint32	enum_err
uint32	hmapwindow0_baseaddrlower
uint32	hmapwindow0_baseaddrupper
uint32	hmapwindow0_windowlength

uint32	hmapwindow1_baseaddrlower
uint32	hmapwindow1_baseaddrupper
uint32	hmapwindow1_windowlength

uint32	hmapwindow2_baseaddrlower
uint32	hmapwindow2_baseaddrupper
uint32	hmapwindow2_windowlength

uint32	hmapwindow3_baseaddrlower
uint32	hmapwindow3_baseaddrupper
uint32	hmapwindow3_windowlength

uint32	hmapwindow4_baseaddrlower
uint32	hmapwindow4_baseaddrupper
uint32	hmapwindow4_windowlength

uint32	hmapwindow5_baseaddrlower
uint32	hmapwindow5_baseaddrupper
uint32	hmapwindow5_windowlength

uint32	hmapwindow6_baseaddrlower
uint32	hmapwindow6_baseaddrupper
uint32	hmapwindow6_windowlength

uint32	hmapwindow7_baseaddrlower
uint32	hmapwindow7_baseaddrupper
uint32	hmapwindow7_windowlength

uint32	hmapviolation_erroraddrlower
uint32	hmapviolation_erroraddrupper
uint32	hmapviolation_errorinfo
uint32	hmapwindowconfig
uint32	hwastatus
uint32	msivector
uint32	msiintmask
uint32	msiintstatus
uint32	dmafuncmap
uint32	doorbellfuncmap
uint32	dmacapability
uint32	idma_ctrl
uint32	Ch2_DmaFRGIntStat_Inbound
uint32	Ch2_DmaFRGIntMask_Inbound
uint32	idma_flow_mgr_ctrl1
uint32	idma_flow_mgr_ctrl2
uint32	functioncontrol
uint32	functioniostatus
FEATURE_END
