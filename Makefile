MODULE_NAME := rwProcMemDemo
RESMAN_CORE_OBJS:=sys.o
RESMAN_GLUE_OBJS:=
ifneq ($(KERNELRELEASE),)    
	$(MODULE_NAME)-objs:=$(RESMAN_GLUE_OBJS) $(RESMAN_CORE_OBJS)
	obj-m := rwProcMemDemo.o
else
	KDIR := $(PWD)/../out
all:
	make -C $(KDIR) \
	ARCH=arm64 \
	CC=/home/shocker/proton-clang/bin/clang \
	LD=/home/shocker/proton-clang/bin/ld.lld \
	CROSS_COMPILE=/home/shocker/proton-clang/bin/aarch64-linux-gnu- \
	LLVM_NM=/home/shocker/proton-clang/bin/llvm-nm \
	CXX=/home/shocker/proton-clang/bin/clang++ \
	M=$(PWD) modules 
clean:    
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  
endif