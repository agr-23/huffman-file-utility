CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

SRC_DIR = .
FM_DIR = file_manager
COMP_DIR = compression
ENC_DIR = encryption

SOURCES = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/$(FM_DIR)/file_manager.cpp \
    $(SRC_DIR)/$(COMP_DIR)/bitstream_utils.cpp \
    $(SRC_DIR)/$(COMP_DIR)/huffman.cpp \
    $(SRC_DIR)/$(ENC_DIR)/encryption.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = huffenc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

# ======================================
cleanTests:
	@echo "Limpiando archivos generados por pruebas..."
	rm -f tests/out1 tests/out1_dec
	rm -f tests/out2 tests/out2_dec
	rm -rf tests/out_dir1 tests/out_dir1_rec tests/out_dir2
	rm -f tests/*.huf
	rm -f tests/out*
	@echo "Limpieza de pruebas completada."

.PHONY: all clean cleanTests