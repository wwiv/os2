Add this to config.sys to use the Watcom linker.
This will let -Zomf work from gcc (and therefore debugging):

SET EMXOMFLD_LINKER=wl.exe
SET EMXOMFLD_TYPE=WLINK
SET EMXOMFLD_RC=wrc.exe
SET EMXOMFLD_RC_TYPE=WRC


