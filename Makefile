BINARYNAME = main
BUILDDIR = buildobj

OPTFLAG = -O0

ARCHDIR = arch/stm32mp1xx
LINKSCR = build/stm32mp157axx/stm32mp15xx_ca7_app.ld

#Same?
# MCU =  -mcpu=cortex-a7 -mfpu=vfpv4 -mfloat-abi=hard

# MCFLAGS = 	$(MCU) \
# 			-fno-math-errno -funroll-loops \
# 			-fgraphite-identity \
# 			-ffunction-sections \
# 			-fdata-sections \
# 			-ffat-lto-objects \
# 			-ftree-vectorize \

# 157C vs 157A matters?
# =1 or just defined matters?
# ARCH_CFLAGS = -DUSE_FULL_LL_DRIVER \
# 			   -DSTM32MP157Cxx \
# 			   -DSTM32MP1 \
# 			   -DCORE_CA7 \
# 			   $(EXTRA_ARCH_CFLAGS)

EXTRA_ARCH_CFLAGS = -D"NDEBUG"=1 -D"CPUSTYLE_STM32MP1"=1 -D"EGL_STATIC_LIBRARY"=1 -D"STM32MP157Axx"=1 
EXTRA_ARCH_CFLAGS += -D"USE_HAL_DRIVER"=1 
EXTRA_ARCH_CFLAGS += -D"CORE_CA7"=1 -D"USE_FULL_LL_DRIVER"=1

# CFLAGS = 	-std=gnu99 -Wstrict-prototypes \
# 			$(MCFLAGS) \
# 			-gdwarf-2 -fomit-frame-pointer -Wall \
# 			$(EXTRA_ARCH_CFLAGS) \

EXTRA_AFLAGS = -D__ASSEMBLY__=1

EXTRALDFLAGS = -lm
# LFLAGS = $(MCFLAGS) $(OPTFLAG) \
# 		 -nostartfiles \
# 		 -Xlinker --gc-sections \
# 		 -T$(LINKSCR) \
# 		 -Wl,-Map=$(BUILDDIR)/$(BINARYNAME).map,--cref \
# 		 -lm

#LINK_STDLIB = 


CMSISDIR = CMSIS_5/CMSIS
ARCHDIR = arch/stm32mp1xx

EXTLIBDIR = lib
STM32HALDIR = $(EXTLIBDIR)/Drivers/STM32MP1xx_HAL_Driver
USBLIBDIR = $(EXTLIBDIR)/Middlewares/ST/STM32_USB_Device_Library
USBCLASSDIR = $(EXTLIBDIR)
USBXDIR = src/hal

SOURCES = src/crt_CortexA.S \
		  src/clocks.c \
		  src/formats.c \
		  src/gpio.c \
		  src/hardware.c \
		  src/irq.c \
		  src/irq_ctrl_gic_forward.c \
		  src/main2.c \
		  src/serial.c \
		  src/usb/usbd_desc.c \
		  src/usb/usbd_msc_storage.c \
		  src/hal/usb_device.c \
		  src/hal/usbd_conf.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_pcd.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_pcd_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_ll_usb.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_gpio.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_hal_rcc_ex.c \
		  $(STM32HALDIR)/Src/stm32mp1xx_ll_rcc.c \
		  $(USBCLASSDIR)/Class/MSC/Src/usbd_msc_bot.c \
		  $(USBCLASSDIR)/Class/MSC/Src/usbd_msc_data.c \
		  $(USBCLASSDIR)/Class/MSC/Src/usbd_msc_scsi.c \
		  $(USBCLASSDIR)/Class/MSC/Src/usbd_msc.c \
		  $(USBLIBDIR)/Core/Src/usbd_core.c \
		  $(USBLIBDIR)/Core/Src/usbd_ctlreq.c \
		  $(USBLIBDIR)/Core/Src/usbd_ioreq.c \
		  
INCLUDES = -I. \
		   -Iinc \
		   -I$(ARCHDIR) \
		   -I$(STM32HALDIR)/Inc \
		   -I$(USBLIBDIR)/Core/Inc \
		   -I$(USBCLASSDIR)/Class/MSC/Inc \
		   -I$(USBXDIR) \
		   -I$(CMSISDIR)/Core_A/Include \
		   -I$(CMSISDIR)/Core_A/Source \

#CFLAGS += $(INCLUDES)
LINK_STDLIB =

include makefile-common.mk

