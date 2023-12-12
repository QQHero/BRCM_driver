export LD_LIBRARY_PATH=/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/lib:$LD_LIBRARY_PATH
CROSS_CC=/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/usr/bin/arm-buildroot-linux-gnueabi-gcc
LOCAL_CC=gcc

CFLAGS= -std=c99 -D_POSIX_C_SOURCE=199309L
HEADER_DIR=-I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/shared/bcmwifi/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/proto/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/wl/exe -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/wlioctl/include -I./
LDFLAGS=-L/projects/hnd/tools/bcm/crosstools-arm-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/lib
LIBS= -lrt

OBJS=guard.c

TARGET=mp_guard

$(OBJS):%o:%c
	$(CROSS_CC) -c $(CFLAGS) $(HEADER_DIR) $(LDFLAGS) $< -o $@

$(TARGET):$(OBJS)
	$(CROSS_CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

all:$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
