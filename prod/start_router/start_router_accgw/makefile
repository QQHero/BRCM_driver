export LD_LIBRARY_PATH=/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/lib:$LD_LIBRARY_PATH
CROSS_CC=/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/usr/bin/arm-buildroot-linux-gnueabi-gcc
LOCAL_CC=gcc

CFLAGS=
HEADER_DIR=-I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/shared/bcmwifi/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/proto/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/wl/exe -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/wlioctl/include
LDFLAGS=-L/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/lib
LIBS= -lrt

OBJS=./mp_server.o ./cJSON.o ./tcp_cJSON.o ./TENDA_HAL/ap_core.o ./mp_util.o ./mp_udpclient.o ./mp_core.o 

TARGET=mp_server

$(OBJS):%o:%c
	$(CROSS_CC) -c $(CFLAGS) $(HEADER_DIR) $(LDFLAGS) $< -o $@

$(TARGET):$(OBJS)
	$(CROSS_CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

all:$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) 
