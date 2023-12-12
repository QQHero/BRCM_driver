export LD_LIBRARY_PATH=/projects/hnd/tools/bcm/crosstools-arm-gcc-9.2-linux-4.19-glibc-2.30-binutils-2.32/lib:$LD_LIBRARY_PATH
CROSS_CC=/projects/hnd/tools/bcm/crosstools-arm-gcc-9.2-linux-4.19-glibc-2.30-binutils-2.32/usr/bin/arm-buildroot-linux-gnueabi-gcc
LOCAL_CC=gcc

CFLAGS=
# HEADER_DIR=-I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/shared/bcmwifi/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/proto/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/include -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/src/wl/exe -I/home/slimehsiao/product_compile_ax3v2/bsp/kernel/bcm_v5_02L_07p1_4.1.52/bcmdrivers/broadcom/net/wl/impl63/main/components/wlioctl/include
LDFLAGS=-L/projects/hnd/tools/bcm/crosstools-arm-gcc-9.2-linux-4.19-glibc-2.30-binutils-2.32/lib
LIBS= -lrt
ROMFS_PATH=../../../../targets/ax12_pro_nor/romfs
TFTP_PATH=/home/startlab/tftp

OBJS=./debugfs.o

TARGET=debugfs

$(OBJS):%o:%c
	$(CROSS_CC) -c $(CFLAGS) $(HEADER_DIR) $(LDFLAGS) $< -o $@

$(TARGET):$(OBJS)
	$(CROSS_CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

all:$(TARGET)
	cp $(TARGET) $(ROMFS_PATH)/bin
	cp $(TARGET) $(TFTP_PATH)

clean:
	rm -f $(OBJS) $(TARGET) 
