SOURCES := $(shell find src -type f -name "*.c")
HEADERS := $(shell find include -type f -name "*.h")
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.c=.o))
LIBS := -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm

main: $(OBJECTS)
	gcc $(OBJECTS) $(LIBS) -o $@

obj/%.o: src/%.c $(HEADERS)
	@mkdir -p obj
	gcc -c -o $@ $< -Wall -gdwarf-3 -Iinclude
#	gcc -c -o $@ $< -Wall -O3 -Iinclude

debug: main
	gdb main

run: main
	./main

clean:
	rm -rf obj/*.o
	rm -rf main

.PHONY: clean
.PHONY: install
