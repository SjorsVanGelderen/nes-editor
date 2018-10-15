CC=g++
CFLAGS=-c -Wall -std=c++17
LDFLAGS=-framework OpenGL -lglfw -lGLEW
SOURCES=           \
main.cpp           \
app.cpp            \
media.cpp          \
debug.cpp          \
palette.cpp        \
samples.cpp        \
character.cpp      
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=app

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
