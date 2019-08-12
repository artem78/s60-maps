# ============================================================================
#  Name	 : build_help.mk
#  Part of  : S60Maps
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : S60Maps
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : S60Maps_0xED689B88.hlp
S60Maps_0xED689B88.hlp : S60Maps.xml S60Maps.cshlp Custom.xml
	cshlpcmp S60Maps.cshlp
ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
	md $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
	copy S60Maps_0xED689B88.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del S60Maps_0xED689B88.hlp
	del S60Maps_0xED689B88.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo S60Maps_0xED689B88.hlp

FINAL : do_nothing
