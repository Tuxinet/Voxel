include .env

CFLAGS = -std=c++17 -I${TINYOBJ_PATH}
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp
	g++ $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

# make shaders targets
%.spv: %
	/usr/bin/glslc $< -o $@


.PHONY: test clean

test: a.out
	mangohud ./a.out

clean:
	rm -f a.out
	rm -f shaders/*.spv