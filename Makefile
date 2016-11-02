PROGRAM = main
EM_PROGRAM = em_main.js

BUILD_DIR := build
OUTPUT_DIR := bin
SOURCE_DIR := src

SOURCES_BASE := $(wildcard $(SOURCE_DIR)/akari/*.cpp)\
    $(wildcard $(SOURCE_DIR)/masyu/*.cpp) $(wildcard $(SOURCE_DIR)/nurikabe/*.cpp) \
    $(wildcard $(SOURCE_DIR)/slitherlink/*.cpp) $(wildcard $(SOURCE_DIR)/yajilin/*.cpp) \
	$(wildcard $(SOURCE_DIR)/kakuro/*.cpp) \
    $(wildcard $(SOURCE_DIR)/common/*.cpp) $(wildcard $(SOURCE_DIR)/test/*.cpp)
    
SOURCES := $(SOURCES_BASE)
SOURCES_EM := $(wildcard $(SOURCE_DIR)/em_support/*.cpp) $(SOURCES_BASE)
SOURCES_ALL := $(SOURCES) $(SOURCE_DIR)/main.cpp $(SOURCE_DIR)/frontend_slitherlink_generator.cpp
SOURCE_WITHOUT_SRC_DIR := $(SOURCES:$(SOURCE_DIR)/%=%)
SOURCE_ALL_WITHOUT_SRC_DIR := $(SOURCES_ALL:$(SOURCE_DIR)/%=%)
OBJS := $(addprefix $(BUILD_DIR)/,$(SOURCE_WITHOUT_SRC_DIR:.cpp=.o))
DEPENDS := $(addprefix $(BUILD_DIR)/,$(SOURCE_ALL_WITHOUT_SRC_DIR:.cpp=.d))

CXX = g++
CPPFLAGS = -std=c++11 -O2

EMCC = emcc
EMCCFLAGS = -std=c++11 -O2 --bind --memory-init-file 0

all: main slitherlink-generator

-include $(DEPENDS)

main: $(OUTPUT_DIR)/main
slitherlink-generator: $(OUTPUT_DIR)/slitherlink-generator

$(OUTPUT_DIR)/main: $(OBJS) $(BUILD_DIR)/main.o
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) $(CPPFLAGS) -o $@ $^ -pthread
$(OUTPUT_DIR)/slitherlink-generator: $(OBJS) $(BUILD_DIR)/frontend_slitherlink_generator.o
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) $(CPPFLAGS) -o $@ $^ -pthread

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) $(CPPFLAGS) -MMD -MF $(@:.o=.d) -o $@ -c $<

js: $(BUILD_DIR)/$(EM_PROGRAM)

$(BUILD_DIR)/$(EM_PROGRAM): $(SOURCES_EM)
	$(EMCC) $(EMCCFLAGS) $^ -o $(BUILD_DIR)/$(EM_PROGRAM)
	
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all main slitherlink-generator js clean
