CONTRIB_DIR = ..
HASHMAP_DIR = $(CONTRIB_DIR)/CHashMapViaLinkedList

GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
SHELL  = /bin/bash
CC     = gcc
CCFLAGS = -g -O2 -Wall -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS) -I$(HASHMAP_DIR)

all: tests

chashmap:
	mkdir -p $(HASHMAP_DIR)/.git
	git --git-dir=$(HASHMAP_DIR)/.git init 
	pushd $(HASHMAP_DIR); git pull git@github.com:willemt/CHashMapViaLinkedList.git; popd

download-contrib: chashmap

main.c:
	if test -d $(HASHMAP_DIR); \
	then echo have contribs; \
	else make download-contrib; \
	fi
	sh make-tests.sh > main.c

tests: main.c texture_atlas.o test_texture_atlas.c CuTest.c main.c $(HASHMAP_DIR)/linked_list_hashmap.c 
	$(CC) $(CCFLAGS) -o $@ $^
	./tests
	gcov main.c test_texture_atlas.c texture_atlas.c

texture_atlas.o: texture_atlas.c 
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c texture_atlas.o tests $(GCOV_OUTPUT)
