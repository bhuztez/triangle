window.elf: wayland.o draw.o
	g++ -O3 -flto -o "$@" wayland.o draw.o -lwayland-client

draw.o: draw.cpp sl.hpp gl.hpp
	g++ -O3 -flto -std=gnu++14 -Wall -Wextra -Werror -c -o "$@" "$<"

wayland.o: wayland.c
	gcc -O3 -flto -std=gnu11 -Wall -Wextra -Werror -c -o "$@" "$<"

clean:
	rm -f *.o window
