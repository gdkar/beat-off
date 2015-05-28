CC = clang
CXX = clang++

# Files to include
C_SRC  = $(wildcard core/*.c)
C_SRC += $(wildcard filters/*.c)
C_SRC += $(wildcard hits/*.c)
C_SRC += $(wildcard lib/*.c)
C_SRC += $(wildcard midi/*.c)
C_SRC += $(wildcard output/*.c)
C_SRC += $(wildcard patterns/*.c)
C_SRC += $(wildcard signals/*.c)
C_SRC += $(wildcard timebase/*.c)
C_SRC += $(wildcard waveform/*.c)
C_SRC += $(wildcard ui/*.c)
C_SRC += $(wildcard util/*.c)
C_SRC += $(wildcard lib/lux/src/*.c)
CPP_SRC += $(wildcard filters/*.cpp)

C_INC += $(wildcard filters/*.h)
C_INC += $(wildcard lib/*.h)
C_INC += $(wildcard midi/*.h)
C_INC += $(wildcard output/*.h)
C_INC += $(wildcard patterns/*.h)
C_INC += $(wildcard signals/*.h)
C_INC += $(wildcard timebase/*.h)
C_INC += $(wildcard waveform/*.h)
C_INC += $(wildcard ui/*.h)
C_INC += $(wildcard util/*.h)
C_INC += $(wildcard hits/*.h)
C_INC += $(wildcard lib/lux/inc/*.h)
CPP_INC += $(wildcard core/*.hpp)

OBJECTS = $(patsubst %.c,.obj/%.o,$(C_SRC))
OBJECTS += $(patsubst %.cpp,.obj/%.o,$(CPP_SRC))
DEPS = $(OBJECTS:.o=.d)

INC  = -I. -Ilib/lux/inc -Ilib 
LIB  = -lm -lSDL -lSDL_ttf -lSDL_gfx -lpthread -lportaudio -lvamp-hostsdk -lportmidi #-lporttime

# Assembler, compiler, and linker flags
OPTFLAGS	= -g -ggdb -O3 -Wall -Wextra -Werror -Wno-missing-field-initializers
CXXFLAGS  = $(OPTFLAGS) $(INC)  -std=gnu++14 -Wno-c++11-narrowing
CFLAGS    = $(OPTFLAGS) $(INC) -std=gnu11
LFLAGS    = $(OPTFLAGS) $(INC)

-include $(DEPS)
%.d : %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
%.d : %.cpp
	@$(CXX) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@
.obj/%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@
.obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
.obj/%.o: %.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

C_SRC_DIRS   = $(addprefix .obj/, $(dir $(C_SRC)))
CPP_SRC_DIRS = $(addprefix .obj/, $(dir $(CPP_SRC)))
dirs:
	mkdir -p  $(C_SRC_DIRS) $(CPP_SRC_DIRS) bin

# Targets
.PHONY: all
all: dirs beat-off

.PHONY: clean
clean:
	-rm -f $(OBJECTS) $(DEPS) beat-off 
	-rm -f -r .obj bin

beat-off: dirs $(OBJECTS)
	$(CXX) $(LFLAGS) -g -o bin/beat-off $(OBJECTS) $(LIB)


#%.o: %.c $(C_INC)
#	gcc $(CFLAGS) -std=c99 -c -o $@ $<
#%.o: %.cpp
#	g++ $(CFLAGS) -c -o $@ $<

.DEFAULT_GOAL := beat-off
