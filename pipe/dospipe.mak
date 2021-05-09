PROJ		= DOSPIPE
MSVC_HOME	= C:\MSVC
INCLUDE 	= $(MSVC_HOME)\INCLUDE
LIB		= $(MSVC_HOME)\LIB
CPPFLAGS	= /nologo -I $(INCLUDE) -L $(LIB) /W4
LDFLAGS		= 

all:		$(PROJ).EXE

clean:
	-DEL $(PROJ).EXE
	-DEL $(PROJ).obj

OBJS		= DOSPIPE.OBJ
	
$(PROJ).EXE::   $(PROJ).OBJ $(OBJS)
	LINK /NOLOGO $(LDFLAGS) @<<$(PROJ).CRF
$(OBJS: =+^
)
$(PROJ).EXE
$(PROJ).MAP


<<KEEP

