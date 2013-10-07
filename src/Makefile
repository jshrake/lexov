lib_dirs=-L./../dep/glfw/lib -L./../dep/glew/lib
include_dirs=-I./../dep/glfw/include -I./../dep/glew/include -I./../dep/mogl/include -I./../dep/

CC=clang++
CC_OPTIONS=-Wall -Werror -g -std=c++11 -stdlib=libc++

OBJ=main.o camera.o chunk_generator.o chunk_manager.o chunk_renderer.o game.o lexov.o

all: lexov

lexov: $(OBJ) 
	$(CC) $(CC_OPTIONS) -DGLEW_STATIC  $(lib_dirs) -framework Cocoa -framework OpenGL -framework IOkit -lglew -lglfw3 $(OBJ) -o lexov.bin

main.o: main.cpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c main.cpp

camera.o: camera.cpp camera.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c camera.cpp

chunk_generator.o: chunk_generator.cpp chunk_generator.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c chunk_generator.cpp

chunk_manager.o: chunk_manager.cpp chunk_manager.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c chunk_manager.cpp

chunk_renderer.o: chunk_renderer.cpp chunk_renderer.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c chunk_renderer.cpp

game.o: game.cpp game.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c game.cpp

lexov.o: lexov.cpp lexov.hpp
	$(CC) $(CC_OPTIONS) -DMOGL_DEBUG  $(include_dirs) -c lexov.cpp
clean: 
	rm -f metaverse.bin *.o
