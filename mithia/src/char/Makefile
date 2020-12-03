
%.o: %.c
	@echo -e " Compile... $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: char-server

char-server: char.o logif.o mapif.o saveif.o char_db.o $(COMMON_OBJ)
	@echo -e " Linking... $<"
	@$(CC) -o ../../$@ char.o logif.o mapif.o saveif.o char_db.o $(COMMON_OBJ) $(CLIBS)

char.o: char.c
logif.o: logif.c
mapif.o: mapif.c
saveif.o: saveif.c
char_db.o: char_db.c

clean:
	@rm -f *.o ../../char-server
