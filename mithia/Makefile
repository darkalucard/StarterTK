#!make
# Yuri Makefile
#################

CC = gcc -pipe -w -g
MAKE = make -s
CFLAGS = -fno-stack-protector -ffast-math -Wall -Wno-sign-compare -DFD_SETSIZE=1024 -DNO_MEMMGR -DLOGGING_ENABLED -DUSE_MYSQL -I../common -I/usr/include/mysql -I/usr/include/lua5.1 -I/usr/local/zlib/include
CLIBS = -L../lib -L/usr/local/lib -L/usr/lib  -L/usr/local/mysql/lib -lcrypt -lmysqlclient -lm -lz -ldl -llua5.1

COMMON_OBJ = ../common/core.o ../common/socket.o ../common/timer.o ../common/crypt.o ../common/db.o ../common/malloc.o  ../common/db_mysql.o ../common/md5calc.o ../common/ers.o ../common/strlib.o ../common/showmsg.o ../common/rndm.o
COMMON_H = ../common/core.h ../common/socket.h ../common/timer.h ../common/crypt.h ../common/db.h ../common/mmo.h ../common/version.h ../common/malloc.h ../common/db_mysql.h ../common/md5calc.h ../common/ers.h ../common/cbasetypes.h ../common/strlib.h ../common/showmsg.h ../common/rndm.h

MKDEF = CC="$(CC)" CFLAGS="$(CFLAGS)" CLIBS="$(CLIBS)" COMMON_OBJ="$(COMMON_OBJ)" COMMON_H="$(COMMON_H)"
METADEF = CC="$(CC)" CFLAGS="$(CFLAGS)" CLIBS="$(CLIBS)" COMMON_OBJ="../common/db.o ../common/malloc.o ../common/db_mysql.o ../common/timer.o ../common/strlib.o ../common/showmsg.o ../common/ers.o ../common/md5calc.o" COMMON_H="../common/db.h ../common/malloc.h ../common/db_mysql.h ../common/timer.h ../common/md5calc.h"

all: title common login char map save metan decrypt

title:
	@echo "Hyul [Make]"
	@echo "-----------"

common:
	@echo "Common:"
	@$(MAKE) $(MKDEF) -C src/common/

login: common
	@echo "Login Server:"
	@$(MAKE) $(MKDEF) -C src/login/ clean
	@$(MAKE) $(MKDEF) -C src/login/

char: common
	@echo "Char Server:"
	@$(MAKE) $(MKDEF) -C src/char/ clean
	@$(MAKE) $(MKDEF) -C src/char/

map: common
	@echo "Map Server:"
	@$(MAKE) $(MKDEF) -C src/map/ clean
	@$(MAKE) $(MKDEF) -C src/map/

save: common
	@echo "Save Server:"
	@$(MAKE) $(MKDEF) -C src/save/ clean
	@$(MAKE) $(MKDEF) -C src/save/

metan: common
	@echo "Meta Creator:"
	@$(MAKE) $(METADEF) -C src/metan/ clean
	@$(MAKE) $(METADEF) -C src/metan/

#sqlclear: common
#	@echo "Sql Clear Tool:"
#	@$(MAKE) $(METADEF) -C src/sqlclear/ clean
#	@$(MAKE) $(METADEF) -C src/sqlclear/

decrypt: common
	@echo "Decrypt Packet Tool: "
	@$(MAKE) $(METADEF) -C src/decrypt/ clean
	@$(MAKE) $(METADEF) -C src/decrypt/

clean:
	@$(MAKE) $(MKDEF) $@ -C src/common/
	@$(MAKE) $(MKDEF) $@ -C src/login/
	@$(MAKE) $(MKDEF) $@ -C src/char/
	@$(MAKE) $(MKDEF) $@ -C src/map/
	@$(MAKE) $(MKDEF) $@ -C src/metan/
#	@$(MAKE) $(MKDEF) $@ -C src/sqlclear/
	@$(MAKE) $(MKDEF) $@ -C src/decrypt/