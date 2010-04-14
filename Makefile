obj-m = nullderef.o

nullderef.ko: nullderef.c
	$(MAKE) -C '/lib/modules/$(shell uname -r)/source' \
		O='/lib/modules/$(shell uname -r)/build/' \
		"M=$$PWD" modules
