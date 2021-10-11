#!/bin/bash -x

gcc -m32 -ldl -lX11 -lXext -lxcb -lxcb-present -lxcb-dri3 -lxcb-xfixes -lX11-xcb $(pkg-config --cflags --libs sdl2) -Iinclude/D3D9 -I /usr/include/d3dadapter SDL_nine.c dri3.c device_wrap.c -shared -fPIC   -g -o nine_sdl.so && \
gcc -ldl -lX11 -lXext -lxcb -lxcb-present -lxcb-dri3 -lxcb-xfixes -lX11-xcb $(pkg-config --cflags --libs sdl2) -Iinclude/D3D9 -I /usr/include/d3dadapter SDL_nine.c dri3.c device_wrap.c -shared -fPIC   -g -o nine_sdl64.so
