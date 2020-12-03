
%.o: %.c
	@echo -e " Compile... $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: metan

metan: metan.o itemdb.o guide.o class_db.o $(COMMON_OBJ)
	@echo -e " Linking... $<"
	@$(CC) -o ../../$@ metan.o itemdb.o guide.o class_db.o $(COMMON_OBJ) $(CLIBS)

metan.o: metan.c metan.h $(COMMON_H)
itemdb.o: itemdb.c itemdb.h $(COMMON_H)
guide.o: guide.c guide.h $(COMMON_H)
class_db.o: class_db.c class_db.h $(COMMON_H)
clean:
	@rm -f *.o ../../metan

