CFLAGS := -Werror -std=gnu99 -O2 -funroll-loops

sha1sum: sha1sum.o sha1.o sha1_amd64_acc.o
	cc $(LDFLAGS) $^ -o $@

%.o: %.c
	cc $(CFLAGS) -c $^ -o $@
%.o: %.asm
	nasm -f elf64 $^ -o $@

clean:
	rm -f *.o sha1sum
