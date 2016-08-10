PROGRAM = main

BUILD_DIR := build
SOURCE_DIR := src

SOURCES := $(SOURCE_DIR)/main.cpp $(wildcard $(SOURCE_DIR)/akari/*.cpp)\
    $(wildcard $(SOURCE_DIR)/masyu/*.cpp) $(wildcard $(SOURCE_DIR)/nurikabe/*.cpp) \
    $(wildcard $(SOURCE_DIR)/slitherlink/*.cpp) $(wildcard $(SOURCE_DIR)/yajilin/*.cpp) \
    $(wildcard $(SOURCE_DIR)/common/*.cpp) $(wildcard $(SOURCE_DIR)/test/*.cpp)
SOURCE_WITHOUT_SRC_DIR := $(SOURCES:$(SOURCE_DIR)/%=%)
OBJS := $(addprefix $(BUILD_DIR)/,$(SOURCE_WITHOUT_SRC_DIR:.cpp=.o))
DEPENDS := $(addprefix $(BUILD_DIR)/,$(SOURCE_WITHOUT_SRC_DIR:.cpp=.d))

CXX = g++
CPPFLAGS = -std=c++11 -O2

all: $(BUILD_DIR)/$(PROGRAM)

-include $(DEPENDS)

$(BUILD_DIR)/$(PROGRAM): $(OBJS)
	$(CXX) -o $@ $^

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) $(CPPFLAGS) -MMD -MF $(@:.o=.d) -o $@ -c $<
	
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
