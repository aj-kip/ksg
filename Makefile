CXX = g++
LD = g++
CXXFLAGS = -std=c++14 -g -I./inc -Ilib/common/inc -Wall -pedantic -Werror -DMACRO_PLATFORM_LINUX
SOURCES  = $(shell find src | grep '[.]cpp$$')
OBJECTS_DIR = .debug-build
OBJECTS = $(addprefix $(OBJECTS_DIR)/,$(SOURCES:%.cpp=%.o))

$(OBJECTS_DIR)/%.o: | $(OBJECTS_DIR)/src
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $@

default: $(OBJECTS)
	@echo $(SOURCES)
	ar rvs libksg-d.a $(OBJECTS)

$(OBJECTS_DIR)/src:
	mkdir -p $(OBJECTS_DIR)/src

clean:
	rm -rf $(OBJECTS_DIR)

demos:
	$(CXX) $(CXXFLAGS) -g -L/usr/lib/ -L./demons demons/demo.cpp -lsfml-system -lsfml-graphics -lsfml-window -lcommon-d -lksg-d -o demons/.demo
