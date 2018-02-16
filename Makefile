#NAME: Clark Minor
#EMAIL: clarkminor@ucla.edu
#ID: 104812434

CC= gcc
CFLAGS= -Wall -Wextra

make:
	$(CC) $(CFLAGS) -pthread -lrt -o lab2_add lab2_add.c

dist: lab2a-104812434.tar.gz
sources = *.png lab2_add.c SortedList.h SortedList.c lab2_list.c Makefile README
lab2a-104812434.tar.gz: $(sources)
	tar -zcvf lab2a-104812434.tar.gz $(sources)

clean:
	rm -f lab2_add lab2_list lab2a-104812434.tar.gz

test:
	-./lab2_add  --threads=16 --iterations=1000 --yield=
