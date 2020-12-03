
%.o: %.c
	@echo -e " Compile... $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: save-server

save-server: save.o charif.o save_db.o $(COMMON_OBJ)
	@echo -e " Linking... $<"
	@$(CC) -o ../../$@ save.o charif.o save_db.o $(COMMON_OBJ) $(CLIBS)

save.o: save.c
charif.o: charif.c
save_db.o: save_db.c

clean:
	@rm -f *.o ../../save-server
