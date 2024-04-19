CXX_FLAGS := -O2 -std=c++17 -ggdb
PRE_FLAGS := -MMD -MP

SRC := src
MAINFILE := $(SRC)/main.cpp

TARGET := main
BUILD := build

INC_DIRS := $(INC) $(shell find $(SRC) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

SRCS := $(shell find $(SRC) -name *.cpp)
OBJS := $(subst $(SRC)/,$(BUILD)/,$(addsuffix .o,$(basename $(SRCS))))
DEPS := $(OBJS:.o=.d)

build: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@

$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(PRE_FLAGS) $(INC_FLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf build

-include $(DEPS)