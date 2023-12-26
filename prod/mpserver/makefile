include $(TOP_PATH)/rootpath.mk
include $(TOP_PATH)/tsfile.mk
include $(TOP_PATH)/targets/$(TARGETS_FILE)/makefile.common
include $(TOP_PATH)/targets/$(TARGETS_FILE)/config.mk

VPATH = ./

LDFLAGS += $(LDFLAGS_UGW) -lcommonprod -lrt -lwifibase
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=199309L
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/src/shared/bcmwifi/include
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/src/wl/td_wifibase/
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/components/proto/include 
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/src/include 
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/src/wl/exe 
CFLAGS += -I$(KERNEL_DIR)/bcmdrivers/broadcom/net/wl/impl63/main/components/wlioctl/include 
CFLAGS += -I./


OBJS=mp_server.o cJSON.o tcp_cJSON.o TENDA_HAL/ap_core.o mp_util.o mp_udpclient.o mp_deca.o mp_core.o router_server.o debug_log.o 

EXEC=mp_server

all:$(EXEC)
	cp $(EXEC) $(ROMFS_PATH)/bin
#$(OBJS):%o:%c
#	$(CROSS_CC) -c $(CFLAGS) $(HEADER_DIR) $(LDFLAGS) $< -o $@

$(EXEC):$(OBJS)
	#$(CROSS_CC) -o $(EXEC) $(OBJS) $(LDFLAGS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(CFLAGS) 

clean:
	rm -f $(OBJS) $(EXEC) 
