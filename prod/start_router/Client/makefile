CROSS_CC=gcc
CFLAGS=
HEADER_DIR= 
LDFLAGS=
LIBS=

OBJS=cJSON.o tcp_cJSON.o client.o

TARGET=client

$(OBJS):%o:%c
	$(CROSS_CC) -c $(CFLAGS) $(HEADER_DIR) $(LDFLAGS) $< -o $@

$(TARGET):$(OBJS)
	$(CROSS_CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

all:$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) 
