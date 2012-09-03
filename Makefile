#
# Makefile v0.4
#

OUT = ./everborn
DUMP_DIR = DUMP/
CFLAGS = -g `sdl-config --cflags` -Wno-write-strings
#LIBMIXER = /usr/lib/libSDL_mixer.a -lvorbisfile -lmikmod -lsmpeg
LIBMIXER = -lSDL_mixer
LIBS = -lGL -lm -lpng `sdl-config --libs` $(LIBMIXER)

default: flags $(OUT)

flags:
	@echo CFLAGS = $(CFLAGS)
	@echo LIBS = $(LIBS)



#==============================================================================

MAIN_SRC = $(wildcard *.cpp)
MAIN_OBJ = $(patsubst %.cpp, %.o, $(MAIN_SRC))
#$(DUMP_DIR)%.o: %.cpp Makefile
$(DUMP_DIR)%.o: %.cpp
	@echo  " "$<
	@$(CXX) $(CFLAGS) $< -c -o $@

SRC = $(MAIN_SRC)
OBJ = $(addprefix $(DUMP_DIR), $(MAIN_OBJ))
DEPS = $(patsubst %.o, %.d, $(OBJ))

$(DUMP_DIR)%.d: %.cpp
	@mkdir -p $(DUMP_DIR)
	@$(CXX) -MM $< -MF $@ -MT $(patsubst %.d, %.o, $@)



#==============================================================================

.PHONY: $(OUT) clean


$(OUT): $(OBJ) $(DEPS)
	@echo linking $(OUT)
	@$(CXX) $(CFLAGS) -o $(OUT) $(OBJ) $(LIBS)

clean:
	-rm -r $(DUMP_DIR)

uninstall: clean
	-rm $(OUT)


-include $(DEPS)

#EOF===========================================================================
