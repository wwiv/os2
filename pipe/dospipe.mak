PROJ		= DOSPIPE
MSVC_HOME	= C:\MSVC
INCLUDE 	= $(MSVC_HOME)\INCLUDE
LIB		= $(MSVC_HOME)\LIB
CPPFLAGS	= /nologo -I $(INCLUDE) -L $(LIB) /W4 /G3
LDFLAGS		= 
BINDIR		= bin\

all:		$(BINDIR)\$(PROJ).EXE

clean:
	-@DEL/N $(BINDIR)\*.*

.cpp{$(BINDIR)}.obj:
    $(CC) $(CPPFLAGS) /Fo$@ /C $<

OBJS		= $(BINDIR)\DOSPIPE.OBJ
	
$(BINDIR)\$(PROJ).EXE::   $(OBJS)
	-@MKDIR $(BINDIR)
	LINK /NOLOGO $(LDFLAGS) @<<$(PROJ).CRF
$(OBJS: =+^
)
$(BINDIR)\$(PROJ).EXE
$(PROJ).MAP


<<

