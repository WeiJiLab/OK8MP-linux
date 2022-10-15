TARGET1 = fltest_Sobel
OBJS = Sobel
#CXX = aarch64-poky-linux-g++ --sysroot=/work/lzb/imx8m/sdk/sysroots/aarch64-poky-linux
#CFLAGS += -I opencv4 -L../../../OK8MP-linux-fs/rootfs/usr/lib -lopencv_core -lopencv_objdetect -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_imgproc -lOpenCL -lpthread -lrt
CFLAGS += -I opencv4 -lopencv_core -lopencv_objdetect -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lOpenCL -lpthread -lrt -std=gnu++11

all:
	@$(CXX)  $(OBJS).cpp -o $(TARGET1) $(CFLAGS) 

	
clean:
	rm -rf *.o
	rm -rf $(TARGET1) 

install:
	cp $(TARGET1)  ../../../OK8MP-linux-fs/rootfs/usr/bin/

