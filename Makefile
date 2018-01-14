window.elf: wayland.o draw.o
	$(CXX) -O3 -flto -o "$@" wayland.o draw.o -lwayland-client

draw.o: draw.cpp sl.hpp gl.hpp
	$(CXX) -O3 -flto -std=c++1z -Wall -Wextra -Werror -Wno-non-template-friend -c -o "$@" "$<"

wayland.o: wayland.c
	$(CC) -O3 -flto -std=c11 -Wall -Wextra -Werror -D _GNU_SOURCE -c -o "$@" "$<"

clean:
	rm -f *.o window.elf
