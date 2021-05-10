MSVC_HOME	= C:\MSVC
INCLUDE 	= $(MSVC_HOME)\INCLUDE
LIB		= $(MSVC_HOME)\LIB
# Note: /G3 for 386 compile caused this to crash.
CPPFLAGS	= /nologo -I $(INCLUDE) -L $(LIB) /W4 /G0
LDFLAGS		= 
BINDIR		= bin\


all:		$(BINDIR)\PIPECLNT.EXE $(BINDIR)\FOSSPIPE.EXE \
		$(BINDIR)\UTILTEST.EXE

clean:
	-@DEL/N $(BINDIR)\*.*

.cpp{$(BINDIR)}.obj:
    $(MSVC_HOME)\BIN\CL.EXE $(CPPFLAGS) /Fo$@ /c $<

PIPE_CLNT_OBJS	= $(BINDIR)\PIPECLNT.OBJ \
		  $(BINDIR)\UTIL.OBJ \
		  $(BINDIR)\PIPE.OBJ

$(BINDIR)\PIPECLNT.EXE::   $(PIPE_CLNT_OBJS)
	-@MKDIR $(BINDIR)
	LINK /NOLOGO $(LDFLAGS) @<<$*.CRF
$(PIPE_CLNT_OBJS: =+^
)
$@
$*.MAP


<<

FOSSPIPE_OBJS	= $(BINDIR)\FOSSPIPE.OBJ \
		  $(BINDIR)\FOSSIL.OBJ \
		  $(BINDIR)\UTIL.OBJ \
		  $(BINDIR)\PIPE.OBJ

$(BINDIR)\FOSSPIPE.EXE::   $(FOSSPIPE_OBJS)
	-@MKDIR $(BINDIR)
	LINK /NOLOGO $(LDFLAGS) @<<$*.CRF
$(FOSSPIPE_OBJS: =+^
)
$@
$*.MAP


<<


UTILTEST_OBJS	= $(BINDIR)\UTILTEST.OBJ \
		  $(BINDIR)\UTIL.OBJ


$(BINDIR)\UTILTEST.EXE::   $(UTILTEST_OBJS)
	-@MKDIR $(BINDIR)
	LINK /NOLOGO $(LDFLAGS) @<<$*.CRF
$(UTILTEST_OBJS: =+^
)
$@
$*.MAP


<<







