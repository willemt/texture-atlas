CC     = gcc


all: tests

chashmap:
	mkdir -p chashmap_via_linked_list/.git
	git --git-dir=chashmap_via_linked_list/.git init 
	pushd chashmap_via_linked_list; git pull git@github.com:willemt/CHashMapViaLinkedList.git; popd

fixedarraylist:
	mkdir -p fixedarraylist/.git
	git --git-dir=fixedarraylist/.git init 
	pushd fixedarraylist; git pull git@github.com:willemt/CFixedArraylist.git; popd

download-contrib: chashmap fixedarraylist 

main.c:
	sh make-tests.sh > main.c

tests: main.c texture_atlas.o test_texture_atlas.c CuTest.c main.c chashmap_via_linked_list/linked_list_hashmap.c fixedarraylist/fixed_arraylist.c 
	$(CC) -g -o $@ $^
	./tests

texture_atlas.o: texture_atlas.c 
	$(CC) -g -c -Ichashmap_via_linked_list -Ifixedarraylist -o $@ $^

clean:
	rm -f main.c texture_atlas.o tests
