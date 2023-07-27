# ============================================================================
#  Name	 : Icons_scalable_dc.mk
#  Part of  : S60Maps
#
#  Description: This is file for creating .mif file (scalable icon)
# 
# ============================================================================


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\S60Maps_0xED689B88.mif

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	@echo ...Deleting $(ICONTARGETFILENAME)
	del /q /f $(ICONTARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE: if you have JUSTINTIME enabled for your S60 3rd FP1 or newer SDK
# and this command crashes, consider adding "/X" to the command line.
# See <http://forum.nokia.com/document/Forum_Nokia_Technical_Library_v1_35/contents/FNTL/Build_process_fails_at_mif_file_creation_in_S60_3rd_Ed_FP1_SDK.htm>
# ----------------------------------------------------------------------------

RESOURCE : $(ICONTARGETFILENAME)

# ToDo: Make compresed MIF (without /x flag) for Symbian 9.2 and newer
$(ICONTARGETFILENAME) : $(ICONDIR)\qgn_menu_S60Maps.svg
	mifconv $(ICONTARGETFILENAME) \
		/x /c32 $(ICONDIR)\qgn_menu_S60Maps.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

