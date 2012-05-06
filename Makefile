CC     = gcc


all: tests

main.c:
	sh make-tests.sh > main.c

tests: main.c texture_atlas.o test_texture_atlas.c CuTest.c main.c chashmap_via_linked_list/linked_list_hashmap.c fixedarraylist/fixed_arraylist.c
	$(CC) -g -o $@ $^
#	./tests

texture_atlas.o: texture_atlas.c 
	$(CC) -g -c -Ichashmap_via_linked_list -Ifixedarraylist -o $@ $^

clean:
	rm -f main.c texture_atlas.o tests
