ifneq (${KERNELRELEASE},) 
	obj-m := pcie_anonymouse.o
else	
	# KER_DIR ?= /home/liuguangsheng/Linux_for_Tegra/kernel/linux-headers-4.9.253-tegra-ubuntu18.04_aarch64/kernel-4.9
	KER_DIR ?= /home/liuguangsheng/Linux_for_Tegra/kernel/linux-headers-4.9.253-tegra-linux_x86_64/kernel-4.9
	PWD := $(shell pwd)

all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C ${KER_DIR} M=${PWD} modules
endif

clean:
	make -C ${KER_DIR} M=${PWD} clean
