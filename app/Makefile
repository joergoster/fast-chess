#Compiler
CXX              := g++

TARGET           := fastchess
BINARY_PATH	  	 := .

# Source, Includes
SRCDIR           := src
TESTDIR          := tests
BUILDDIR         := tmp
INCDIR           := src

#Flags, Libraries and Includes
CXXFLAGS         := -O3 -std=c++17 -Wall -Wextra -DNDEBUG
CXXFLAGS_TEST	 := -O2 -std=c++17 -Wall -Wextra -pedantic -Wuninitialized -g3 -fno-omit-frame-pointer
CXXFLAGS_COV	 := -O0 --coverage -g -std=c++17 -Wall -Wextra -pedantic -Wuninitialized -g3 -fno-omit-frame-pointer
INC              := -I$(INCDIR) -Ithird_party

SRC_FILES        := \
	$(SRCDIR)/book/opening_book.cpp \
	$(SRCDIR)/book/epd_reader.cpp \
	$(SRCDIR)/book/pgn_reader.cpp \
	$(SRCDIR)/cli/cli.cpp \
	$(SRCDIR)/cli/sanitize.cpp \
	$(SRCDIR)/elo/elo_wdl.cpp \
	$(SRCDIR)/elo/elo_pentanomial.cpp \
	$(SRCDIR)/engine/uci_engine.cpp \
	$(SRCDIR)/globals/globals.cpp \
	$(SRCDIR)/matchmaking/match/match.cpp \
	$(SRCDIR)/matchmaking/sprt/sprt.cpp \
	$(SRCDIR)/matchmaking/tournament/base/tournament.cpp \
	$(SRCDIR)/matchmaking/tournament/roundrobin/roundrobin.cpp \
	$(SRCDIR)/matchmaking/tournament/tournament_manager.cpp \
	$(SRCDIR)/pgn/pgn_builder.cpp \
	$(SRCDIR)/timecontrol/timecontrol.cpp \
	$(SRCDIR)/util/logger/logger.cpp \
	$(SRCDIR)/printing/printing.cpp \
	$(SRCDIR)/main.cpp

SRC_FILES_TEST   := \
	$(TESTDIR)/elo_test.cpp \
	$(TESTDIR)/epd_builder_test.cpp \
	$(TESTDIR)/functions_test.cpp \
	$(TESTDIR)/hash_test.cpp \
	$(TESTDIR)/main.cpp \
	$(TESTDIR)/opening_test.cpp \
	$(TESTDIR)/cli_test.cpp \
	$(TESTDIR)/pgn_builder_test.cpp \
	$(TESTDIR)/pgn_reader_test.cpp \
	$(TESTDIR)/player.cpp \
	$(TESTDIR)/scoreboard_test.cpp \
	$(TESTDIR)/sprt_test.cpp \
	$(TESTDIR)/timecontrol_test.cpp \
	$(TESTDIR)/uci_engine_test.cpp \
	$(TESTDIR)/config_test.cpp \
	$(TESTDIR)/epd_reader_test.cpp \
	$(TESTDIR)/fd_limit_test.cpp \
	$(TESTDIR)/option_test.cpp \
	$(TESTDIR)/output_factory_test.cpp \
	$(TESTDIR)/match_test.cpp

# Only used for formatting
HEADERS := $(shell find $(SRCDIR) -name "*.hpp") $(shell find $(TESTDIR) -maxdepth 1 -name "*.hpp")

# Windows file extension
SUFFIX           := .exe

NATIVE 	         := -march=native

ifeq ($(MAKECMDGOALS),$(TESTDIR))
	CXXFLAGS  := $(CXXFLAGS_TEST)
	SRC_FILES := $(filter-out src/main.cpp, $(SRC_FILES)) $(SRC_FILES_TEST)
	TARGET    := $(TARGET)-tests
	NATIVE    := 
endif

ifeq ($(MAKECMDGOALS), coverage)
	CXXFLAGS  := $(CXXFLAGS_COV)
	SRC_FILES := $(filter-out src/main.cpp, $(SRC_FILES)) $(SRC_FILES_TEST)
	TARGET    := $(TARGET)-tests
	NATIVE    := 
endif

ifeq ($(scan), yes)
	CXX := scan-build-19 clang++-19
endif

TARGET   := $(BINARY_PATH)/$(TARGET)

DEPFLAGS  := -MMD -MP
MKDIR	  := mkdir -p

ifeq ($(OS), Windows_NT)
	uname_S  := Windows
else
ifeq ($(COMP), MINGW)
	uname_S  := Windows
else
	SUFFIX  :=
	LDFLAGS := -pthread -lstdc++fs
	uname_S := $(shell uname -s)
endif
endif

ifeq ($(build), debug)
	CXXFLAGS := -O2 -std=c++17 -Wall -Wextra -pedantic -Wuninitialized -g3
endif

ifeq ($(build), release)
	LDFLAGS  :=
	NATIVE   := -march=x86-64
	CXXFLAGS += -DRELEASE

	ifneq (,$(findstring clang,$(CXX)))
	else ifneq (,$(findstring g++,$(CXX)))
		LDFLAGS += -Wl,--no-as-needed -lstdc++fs 
	endif

	LDFLAGS += -lpthread -static -static-libgcc -static-libstdc++
endif

# Different native flag for macOS
ifeq ($(uname_S), Darwin)
	NATIVE =
	LDFLAGS =
endif

# Compile with address sanitizer
ifeq ($(san), asan)
	LDFLAGS += -fsanitize=address
endif

# Compile with memory sanitizer
ifeq ($(san), memory)
	LDFLAGS += -fsanitize=memory -fPIE -pie
endif

# Compile with undefined behavior sanitizer
ifeq ($(san), undefined)
	LDFLAGS += -fsanitize=undefined
endif

# Compile with thread sanitizer
ifeq ($(san), thread)
	LDFLAGS += -fsanitize=thread
endif

# Versioning
GIT_SHA = $(shell git rev-parse --short HEAD 2>/dev/null)
ifneq ($(GIT_SHA), )
	CXXFLAGS += -DGIT_SHA=\"$(GIT_SHA)\"
endif

GIT_DATE = $(shell git show -s --date=format:'%Y%m%d' --format=%cd HEAD 2>/dev/null)
ifneq ($(GIT_DATE), )
	CXXFLAGS += -DGIT_DATE=\"$(GIT_DATE)\"
endif

COMPILE_MSG=
ifneq ($(COMPILE_MSG),)
	CXXFLAGS += -DCOMPILE_MSG="\"$(COMPILE_MSG)\""
endif

# Compile with Cutechess output support
ifeq ($(USE_CUTE), true)
	CXXFLAGS += -DUSE_CUTE
endif

ifeq ($(ZLIB), true)
	CXXFLAGS += -DUSE_ZLIB
	SRC_FILES += third_party/gzip/gzstream.cpp
	LDFLAGS += -lz
endif

OBJECTS   := $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRC_FILES))
DEPENDS   := $(patsubst %.cpp,$(BUILDDIR)/%.d,$(SRC_FILES))

.PHONY: clean all tests FORCE coverage

all: $(TARGET)

coverage: tests
	cd .. && ./fastchess-tests
	mkdir -p coverage && cd coverage &&  gcovr --html-details coverage.html --gcov-ignore-errors=no_working_dir_found --gcov-ignore-parse-errors -f "../src" -r ../. ..

tests: $(TARGET) compile-random-mover
	$(CXX) $(CXXFLAGS) ./tests/mock/engine/dummy_engine.cpp -o ./tests/mock/engine/dummy_engine$(SUFFIX) $(LDFLAGS)

format: $(SRC_FILES) $(HEADERS)
	clang-format -i $^

tidy: $(SRC_FILES) $(HEADERS)
	clang-tidy $^

compile-random-mover: $(TARGET)
	$(CXX) -O3 -std=c++17 ./tests/mock/engine/random_mover.cpp -o ./tests/mock/engine/random_mover

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(NATIVE) $(INC) $(DEPFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: %.cpp | build_directories
	$(CXX) $(CXXFLAGS) $(NATIVE) $(INC) $(DEPFLAGS) -c $< -o $@

build_directories:
	@$(shell mkdir -p $(dir $(OBJECTS)))

clean:
	rm -rf $(BUILDDIR) $(TARGET) $(TARGET)-tests $(TARGET)-tests.exe $(TARGET).exe ../random_mover ./tests/mock/engine/random_mover

-include $(DEPENDS)
