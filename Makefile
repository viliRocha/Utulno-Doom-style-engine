CC = g++
CFLAGS = -Wall -std=c++14

SRC = $(wildcard src/*.cpp)
OUT = Utulno

ifeq ($(OS), Windows_NT) 
	INCLUDE = -IC:/raylib/raylib/src 
	LIBS = -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm 
	OUT := $(OUT).exe
	RM = del
else ifeq ($(UNAME_S), Linux) 
	INCLUDE = -I/usr/local/include 
	LIBS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 
	RM = rm -f 
else ifeq ($(UNAME_S), Darwin) #	Mac
	INCLUDE = -I/usr/local/include 
	LIBS = -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo 
	RM = rm -f 
endif

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(INCLUDE) $(LIBS)

run: $(OUT)
	./$(OUT)

#	Depending on the OS, the clean command changes
clean:
	$(RM) $(OUT)