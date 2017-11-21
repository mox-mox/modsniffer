BINARY_NAME := modsniffer
DESTDIR     := /usr


$(BINARY_NAME): main.cpp
	g++ --std=c++17 -Wall -Wextra -Wpedantic -Werror -Wfatal-errors $< -o $@ -lX11 -lXi -lstdc++fs

PHONY: install

install: $(BINARY_NAME)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(BINARY_NAME)


clean:
	rm --force $(BINARY_NAME)

