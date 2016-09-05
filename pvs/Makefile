CURRENT_PATH=$(shell pwd)
BASE_PATH=$(CURRENT_PATH)/../..
include $(BASE_PATH)/build/linux/header.mk

BIN_PATH=$(BASE_PATH)/$(BINTARGET)
LIB_PATH=$(BASE_PATH)/$(LIBTARGET)
TRDLIB_PATH=$(BASE_PATH)/$(TRDLIB)

ifeq ($(RELEASE),1)
BIN_NAME=libpvs.so
else
BIN_NAME=libpvsd.so
endif
CFLAGS+=-DPVS_EXPORTS

INCLUDE=-I$(VION_BASE_INC) -I$(BASE_PATH)/code/include -I$(BASE_PATH)/code/include/linux -I$(BASE_PATH)/code/utils -I$(CURRENT_PATH)

#在原来基础上将debug和release库区分开,debug版本执行时得设置环境变量在链接该目录下的lib_d
ifeq ($(RELEASE),1)	
LIB=-L$(USR_LIB) \
	-L$(BIN_PATH) \
	-L$(LIB_PATH) \
	-L$(VION_BASE_LIB) \
	-lSCImage \
	-lSCCommon \
	-lSCOS \
	-lSCAppCommon -lasound -lini \
	-lz \
    -lSCXml  
#    
else
LIB=-L$(USR_LIB) \
	-L$(LIB_PATH) \
	-L$(BIN_PATH) \
	-lSCImaged \
	-lSCCommond \
	-lSCOSd \
	-lzd \
    -lSCXmld\
    
	#libjpeg.a 
endif

ifeq ($(PLATFORM),TK1)
	ifeq ($(RELEASE),1)	
	LIB+=-lxml2 -liconv  \
	-lSCAppCommon -lasound -lini
	else
	LIB+=-lxml2 -liconv  \
	-lSCAppCommond -lasound -lini 
	endif
else
	ifeq ($(RELEASE),1)	
	LIB+=-lxml2 -liconv -ljpeg
	else
	LIB+=-lxml2d -liconvd
	endif
endif

LIB+=-pthread -ldl -lrt


.PHONY:all pvs prepare clean

all:prepare pvs

pvs:$(OBJS)
	$(PP) $(LDFLAGS) -o $(BIN_PATH)/$(BIN_NAME) $^ $(LIB)

$(CXXOBJTARGET)
#编译优化选项-march会根据处理器架构进行编译优化生成特定的代码（可移植性差），鱼与熊掌不可兼得。
ifeq ($(PLATFORM),TK1)
	$(PP) $(CFLAGS) $< -o $@ $(INCLUDE)
else
	$(PP) $(CFLAGS) -march=i686 $< -o $@ $(INCLUDE)
endif

prepare:
	@if [ ! -e $(INTDIR) ]; then mkdir -p $(INTDIR); fi
	@if [ ! -e $(BIN_PATH) ]; then mkdir -p $(BIN_PATH); fi

clean:
	rm -f $(BIN_PATH)/$(BIN_NAME) $(OBJS)
