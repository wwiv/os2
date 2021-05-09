PROJ		= DOSPIPE
MSVC_HOME	= C:\MSVC
INCLUDE 	= $(MSVC_HOME)\INCLUDE
LIB		= $(MSVC_HOME)\LIB
# Note: /G3 for 386 compile caused this to crash.
CPPFLAGS	= /nologo -I $(INCLUDE) -L $(LIB) /W4 /G0
LDFLAGS		= 
BINDIR		= bin\


all:		$(BINDIR)\$(PROJ).EXE

clean:
	-@DEL/N $(BINDIR)\*.*

.cpp{$(BINDIR)}.obj:
    $(MSVC_HOME)\BIN\CL.EXE $(CPPFLAGS) /Fo$@ /c $<

OBJS		= $(BINDIR)\DOSPIPE.OBJ \
		  $(BINDIR)\FOSSIL.OBJ \
		  $(BINDIR)\PIPE.OBJ
	
$(BINDIR)\$(PROJ).EXE::   $(OBJS)
	-@MKDIR $(BINDIR)
	LINK /NOLOGO $(LDFLAGS) @<<$(PROJ).CRF
$(OBJS: =+^
)
$(BINDIR)\$(PROJ).EXE
$(PROJ).MAP


<<







