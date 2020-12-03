
%.o: %.c
	@echo -e " Compile... $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: map-server

map-server: map.o clif.o intif.o class_db.o itemdb.o recipedb.o pc.o npc.o script.o command.o mob.o magic.o board_db.o clan_db.o sl.o creation.o guide.o $(COMMON_OBJ)
	@echo -e " Linking... $<"
	@$(CC) -o ../../$@ map.o clif.o intif.o class_db.o itemdb.o recipedb.o pc.o npc.o script.o command.o mob.o magic.o board_db.o clan_db.o sl.o creation.o guide.o $(COMMON_OBJ) $(CLIBS)

map.o: map.c map.h $(COMMON_H)
clif.o: clif.c clif.h $(COMMON_H)
intif.o: intif.c intif.h $(COMMON_H)
class_db.o: class_db.c class_db.h $(COMMON_H)
itemdb.o: itemdb.c itemdb.h $(COMMON_H)
recipedb.o: recipedb.c recipedb.h $(COMMON_H)
pc.o: pc.c pc.h $(COMMON_H)
npc.o: npc.c npc.h $(COMMON_H)
script.o: script.c script.h $(COMMON_H)
command.o: command.c command.h $(COMMON_H)
mob.o: mob.c mob.h $(COMMON_H)
magic.o: magic.c magic.h $(COMMON_H)
board_db.o: board_db.c board_db.h $(COMMON_H)
clan_db.o: clan_db.c clan_db.h $(COMMON_H)
sl.o: sl.c sl.h $(COMMON_H)
creation.o: creation.c creation.h $(COMMON_H)
guide.o: guide.c guide.h $(COMMON_H)
clean:
	@rm -f *.o ../../map-server
