
%.o: %.c
	@echo -e " Compile... $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: login-server

login-server: login.o clif.o intif.o $(COMMON_OBJ)
	@echo -e " Linking... $<"
	@$(CC) -o ../../$@ login.o clif.o intif.o $(COMMON_OBJ) $(CLIBS)

login.o: login.c login.h clif.h zlib.h $(COMMON_H)
clif.o: clif.c login.h clif.h intif.h $(COMMON_H)
intif.o: intif.c intif.h $(COMMON_H)
clean:
	@rm -f *.o ../../login-server
