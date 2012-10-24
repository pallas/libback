default: all

.PHONY: all
all: libback.so

PROGS:= segv abrt
TESTS:= $(addprefix test-,$(PROGS))
test: $(TESTS)

$(TESTS): test-%: % libback.so
	echo $(PROGS) $(TESTS)
	env LD_PRELOAD=./libback.so ./$< || true

libback.so: back.c
	$(CC) -std=c99 $(CFLAGS) $(LDFLAGS) -fPIC -shared $< -o $@ -lsupc++

abrt segv : LDFLAGS+= -g -rdynamic
abrt segv : CXXFLAGS+= -rdynamic

.PHONY: clean
clean:
	rm -rf segv abrt libback.so

#