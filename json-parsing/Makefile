# You can put your build options here
-include config.mk

strtojson: strtojson.c jsmn.h
	$(CC) $(LDFLAGS) $< -c

test: test_default test_strict test_links test_strict_links test_strtojson
test_default: test/tests.c jsmn.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict: test/tests.c jsmn.h
	$(CC) -DJSMN_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_links: test/tests.c jsmn.h
	$(CC) -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict_links: test/tests.c jsmn.h
	$(CC) -DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
strtojsontest.o: test/strtojsontest.c jsmn.h
	$(CC) $(LDFLAGS) $< -c
test_strtojson: strtojson.o strtojsontest.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o test/$@
	./test/$@

simple_example: example/simple.c jsmn.h
	$(CC) $(LDFLAGS) $< -o $@

jsondump: example/jsondump.c jsmn.h
	$(CC) $(LDFLAGS) $< -o $@

fmt:
	clang-format -i jsmn.h test/*.[ch] example/*.[ch]

lint:
	clang-tidy jsmn.h --checks='*'

clean:
	rm -f *.o example/*.o
	rm -f simple_example
	rm -f jsondump

.PHONY: clean test

