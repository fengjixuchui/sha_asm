# C compiler
CC     := cc
CFLAGS := -Werror -std=gnu99 -O1 -funroll-loops

# Library object files
LIB_OBJ := lib/sha1.o lib/sha1_amd64.o
LIBNAME := lib/libsha.a

# Applictions
APPS := app/sha1sum

all: $(APPS)

lib/libsha.a: $(LIB_OBJ)
	$(AR) -cr $@ $(LIB_OBJ)

app/%: app/%.c lib/libsha.a
	$(CC) $(CFLAGS) -Ilib -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.asm
	nasm -f elf64 $^ -o $@

clean:
	rm -f $(APPS) $(LIB_OBJ) lib/libsha.a
