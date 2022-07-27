
D3DADAPTER9_LOCATION="/usr/local/lib64/d3d/d3dadapter9.so"
CC=gcc -g
CXX=g++ -g

# 32 bits
#D3DADAPTER9_LOCATION="/usr/lib32/d3d/d3dadapter9.so"
#CC=gcc -g -m32
#CXX=g++ -g -m32

D3DADAPTER9_WITHDRI2=1

XNINE=Xnine.o dri3.o
XNINE_LINK=-ldl -lX11 -lXext -lxcb -lxcb-present -lxcb-dri3 -lxcb-xfixes -lX11-xcb

ifneq ($(D3DADAPTER9_WITHDRI2), 0)
 XNINE_LINK+=-lEGL -lGL
endif

SDLNINE=SDL_nine.o dri3.o
SDLNINE_LINK=-lSDL2 $(XNINE_LINK)

EXEC=triangle_c triangle_cpp triangle_SDL

all: $(EXEC)

triangle_c: $(XNINE) triangle.c
	$(CC) triangle.c $(XNINE) $(XNINE_LINK) -I include/D3D9 -o triangle_c 

triangle_cpp: $(XNINE) triangle.cpp
	$(CXX) triangle.cpp $(XNINE) $(XNINE_LINK) -I include/D3D9 -o triangle_cpp

triangle_SDL: $(SDLNINE) triangle_SDL.cpp
	$(CXX) triangle_SDL.cpp $(SDLNINE) $(SDLNINE_LINK) -I include/D3D9 -I /usr/include/SDL2 -o triangle_SDL

Xnine.o: Xnine.c
	$(CC) -c Xnine.c -o Xnine.o -I include/D3D9 -I include -DD3DADAPTERPATH='$(D3DADAPTER9_LOCATION)' -DD3DADAPTER9_WITHDRI2=$(D3DADAPTER9_WITHDRI2)
SDL_nine.o:  SDL_nine.c
	$(CC) -c SDL_nine.c -o SDL_nine.o -I /usr/include/SDL2 -I include/D3D9 -I include -DD3DADAPTERPATH='$(D3DADAPTER9_LOCATION)' -DD3DADAPTER9_WITHDRI2=$(D3DADAPTER9_WITHDRI2)
dri3.o: dri3.c
	$(CC) -c dri3.c -o dri3.o -I include/D3D9 -DD3DADAPTER9_WITHDRI2=$(D3DADAPTER9_WITHDRI2)

clean:
	rm -f $(XNINE) $(SDLNINE)
	rm -f $(EXEC)

