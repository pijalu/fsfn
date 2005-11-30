#
# Makefile for the Linux Wireless network device drivers.
#
# using bit's from makefile by Peter Johanson

ifneq ($(KERNELRELEASE),)

obj-m := sony_acpi.o

ifeq ($(PATCHLEVEL),4)
include $(TOPDIR)/Rules.make
endif

else

# KSRC should be set to the path to your sources
# modules are installed into KMISC
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
KMISC := /lib/modules/$(KVER)/kernel/drivers/acpi/
KMISC_INC := /lib/modules/$(KVER)/include

# KSRC_OUTPUT should be overridden if you are using a 2.6 kernel that
# has it's output sent elsewhere via KBUILD_OUTPUT= or O=
KSRC_OUTPUT := $(KSRC)

VERFILE := $(KSRC_OUTPUT)/include/linux/version.h
KERNELRELEASE := $(shell \
	if [ -r $(VERFILE) ]; then \
		(cat $(VERFILE); echo UTS_RELEASE) | \
		$(CC) -I$(KSRC_OUTPUT) $(CFLAGS) -E - | \
		tail -n 1 | \
		xargs echo; \
        else \
		uname -r; \
	fi)

MODPATH := $(DESTDIR)/lib/modules/$(KERNELRELEASE)
PWD := $(shell pwd)

default: modules

modules:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) modules

install:
#install -d $(KMISC)
	install -m 644 -c sony_acpi.ko $(KMISC)
	/sbin/depmod -a

uninstall:
	rm -rf $(addprefix $(KMISC),sony_acpi.ko)
	/sbin/depmod -a

endif

clean:
	rm -rf *.[oas] .*.flags *.ko .*.cmd .*.d .*.tmp *.mod.c .tmp_versions
