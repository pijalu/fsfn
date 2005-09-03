ifneq ($(KERNELRELEASE),)

obj-m := sony_acpi.o

ifeq ($(PATCHLEVEL),4)
include $(TOPDIR)/Rules.make
endif

else

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

endif

install:
	cp sony_acpi.ko /lib/modules/$(shell uname -r)/kernel/drivers/acpi/
	depmod -a

clean:
	rm -rf *.[oas] .*.flags *.ko .*.cmd .*.d .*.tmp *.mod.c .tmp_versions
