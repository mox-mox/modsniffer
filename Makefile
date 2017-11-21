BINARY_NAME := modsniffer

$(BINARY_NAME): main.cpp
	g++ --std=c++17 -Wall -Wextra -Wpedantic -Werror -Wfatal-errors $< -o $@ -lX11 -lXi -lstdc++fs
clean:
	rm --force $(BINARY_NAME)

