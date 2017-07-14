GCC=g++
LINK=-lGLEW -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor
LINKSOIL=-lSOIL -limage_helper -limage_DXT -lstb_image_aug

FLAGS=-std=c++11

main: main.cpp
	$(GCC) $(FLAGS) $< -o $@ $(LINK) $(LINKSOIL)

test: clean main
	./main

clean:
	rm -rf *.o main
