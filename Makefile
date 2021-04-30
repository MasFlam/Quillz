CFLAGS := -Iinclude/ -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter
LDFLAGS := -lGL -lglfw -lm

.PHONY: all clean clean-all compile help wren-clean
all: compile
compile: quillz
clean-all: clean wren-clean

wren/lib/libwren.a:
	cd wren/projects/make && make wren

quillz: quillz.o wren/lib/libwren.a
	cc -o $@ $^ $(LDFLAGS)

quillz.o: src/quillz.wren.inc src/quillz.c src/logging.c src/foreigns.c
	cc -o $@ $(CFLAGS) -c src/quillz.c

src/quillz.wren.inc: src/quillz.wren gen-wren.py
	python3 gen-wren.py

clean:
	rm -f quillz.o quillz

wren-clean:
	cd wren/projects/make && make clean

help:
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'TARGETS:'
	@echo '	all (default) - equivalent to make wren compile'
	@echo '	clean'
	@echo '	clean-all - equivalent to make clean wren-clean'
	@echo '	compile'
	@echo '	help'
	@echo '	wren-clean - clean the effects of wren compilation'
