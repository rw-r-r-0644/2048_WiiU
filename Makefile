BASEDIR	:= $(dir $(firstword $(MAKEFILE_LIST)))
VPATH	:= $(BASEDIR)

PKGCONF			:=	$(DEVKITPRO)/portlibs/ppc/bin/powerpc-eabi-pkg-config
PKGCONF_WIIU	:=	$(DEVKITPRO)/portlibs/wiiu/bin/powerpc-eabi-pkg-config

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
# ROMFS is a folder to generate app's romfs
#---------------------------------------------------------------------------------
TARGET		:=	2048
SOURCES		:=	source
INCLUDES	:=	
ROMFS		:=	romfs

#---------------------------------------------------------------------------------
# libraries
#---------------------------------------------------------------------------------
CFLAGS		+=	`$(PKGCONF_WIIU) --cflags SDL2_gfx SDL2_image SDL2_mixer SDL2_ttf sdl2`
CXXFLAGS	+=	`$(PKGCONF_WIIU) --cflags SDL2_gfx SDL2_image SDL2_mixer SDL2_ttf sdl2`
LDFLAGS		+=	`$(PKGCONF_WIIU) --libs SDL2_gfx SDL2_image SDL2_mixer SDL2_ttf sdl2` \
				`$(PKGCONF) --libs freetype2 zlib libpng libjpeg`

#---------------------------------------------------------------------------------
# wut libraries
#---------------------------------------------------------------------------------
LDFLAGS		+=	$(WUT_NEWLIB_LDFLAGS) $(WUT_STDCPP_LDFLAGS) $(WUT_DEVOPTAB_LDFLAGS) \
				-lcoreinit -lvpad -lsndcore2 -lnsysnet -lsysapp -lproc_ui -lgx2 -lgfd -lwhb

#---------------------------------------------------------------------------------
# romfs
#---------------------------------------------------------------------------------
include $(DEVKITPRO)/portlibs/wiiu/share/romfs-wiiu.mk
CFLAGS		+=	$(ROMFS_CFLAGS)
CXXFLAGS	+=	$(ROMFS_CFLAGS)
LDFLAGS		+=	$(ROMFS_LDFLAGS)
OBJECTS		+=	$(ROMFS_TARGET)


#---------------------------------------------------------------------------------
# includes
#---------------------------------------------------------------------------------
CFLAGS		+=	$(foreach dir,$(INCLUDES),-I$(dir))
CXXFLAGS	+=	$(foreach dir,$(INCLUDES),-I$(dir))

#---------------------------------------------------------------------------------
# generate a list of objects
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
SFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.S))
OBJECTS		+=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

#---------------------------------------------------------------------------------
# targets
#---------------------------------------------------------------------------------
$(TARGET).rpx: $(OBJECTS)

clean:
	$(info clean ...)
	@rm -rf $(TARGET).rpx $(OBJECTS) $(OBJECTS:.o=.d)

.PHONY: clean

#---------------------------------------------------------------------------------
# wut
#---------------------------------------------------------------------------------
include $(WUT_ROOT)/share/wut.mk
