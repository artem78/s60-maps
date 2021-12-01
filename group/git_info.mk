# ============================================================================
#  Name	 : git_info.mk
#  Part of  : S60Maps
#
#  Description: Writes information about current git branch and commit
#  to project files.
# 
# ============================================================================

HEADERSDIR=..\inc
TARGETFILENAME=$(HEADERSDIR)\GitInfo.h

COMMIT := $(shell git rev-parse --short HEAD 2>nul || ECHO unknown)
BRANCH := $(shell git symbolic-ref --short HEAD 2>nul || ECHO unknown)

do_nothing :
	@rem do_nothing

MAKMAKE : $(TARGETFILENAME)

BLD : do_nothing

CLEAN :
	@echo ...Deleting $(TARGETFILENAME)
	del /q /f $(TARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

$(TARGETFILENAME) : CLEAN
	echo Start making of $(TARGETFILENAME)
	echo // Information about current git branch and commit> $(TARGETFILENAME)
	echo // for display in About dialog>> $(TARGETFILENAME)
	echo.>> $(TARGETFILENAME)
	echo // *** Do not edit! This file generates automatically and all of your changes will be destroyed. ***>> $(TARGETFILENAME)
	echo.>> $(TARGETFILENAME)
	echo #ifndef GITINFO_H_>> $(TARGETFILENAME)
	echo #define GITINFO_H_>> $(TARGETFILENAME)
	echo.>> $(TARGETFILENAME)
	echo _LIT(KGITCommit, "$(COMMIT)");>> $(TARGETFILENAME)
	echo _LIT(KGITBranch, "$(BRANCH)");>> $(TARGETFILENAME)
	echo.>> $(TARGETFILENAME)
	echo #endif /* GITINFO_H_ */>> $(TARGETFILENAME)

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing

