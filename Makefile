.PHONY: all clean generate compile run-python run-cpp test

# Protobuf compiler
PROTOC = protoc

# C++ compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lprotobuf -lpthread

# Files
PROTO_FILE = messages.proto
PROTO_PY = messages_pb2.py
PROTO_CC = messages.pb.cc
PROTO_H = messages.pb.h
CPP_EXEC = cpp_receiver

all: generate compile

# Generate protobuf code
generate:
	@echo "Generating protobuf code..."
	$(PROTOC) --python_out=. $(PROTO_FILE)
	$(PROTOC) --cpp_out=. $(PROTO_FILE)
	@echo "Generated: $(PROTO_PY), $(PROTO_CC), $(PROTO_H)"

# Compile C++ program
compile: generate
	@echo "Compiling C++ program..."
	$(CXX) $(CXXFLAGS) cpp_receiver.cpp $(PROTO_CC) -o $(CPP_EXEC) $(LDFLAGS)
	@echo "Compiled: $(CPP_EXEC)"

# Run Python sender
run-python:
	python3 python_sender.py

# Run C++ receiver
run-cpp: compile
	./$(CPP_EXEC)

# Full test: Python sends, C++ receives and responds
test: compile
	@echo "Running full test..."
	@rm -f /tmp/command.bin /tmp/status.bin
	python3 python_sender.py
	./$(CPP_EXEC)
	@echo "\n=== Test Complete ==="

# Clean generated files
clean:
	rm -f $(PROTO_PY) $(PROTO_CC) $(PROTO_H)
	rm -f $(CPP_EXEC)
	rm -f /tmp/command.bin /tmp/status.bin
	rm -rf __pycache__
	@echo "Cleaned all generated files"

# Help
help:
	@echo "Available targets:"
	@echo "  make all        - Generate protobuf code and compile C++"
	@echo "  make generate   - Generate Python and C++ code from .proto"
	@echo "  make compile    - Compile C++ program"
	@echo "  make run-python - Run Python sender"
	@echo "  make run-cpp    - Run C++ receiver"
	@echo "  make test       - Run full roundtrip test"
	@echo "  make clean      - Remove generated files"
