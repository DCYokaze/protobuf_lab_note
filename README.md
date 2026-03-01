# Protocol Buffers Python ↔ C++ Example

## Setup

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
# Install protobuf compiler and libraries
sudo apt-get update
sudo apt-get install -y protobuf-compiler libprotobuf-dev python3-protobuf

# Or install via pip for Python
pip install protobuf
```

### 2. Generate Code from .proto

```bash
# Generate Python code
protoc --python_out=. messages.proto

# Generate C++ code
protoc --cpp_out=. messages.proto
```

This creates:
- `messages_pb2.py` (Python)
- `messages.pb.h` and `messages.pb.cc` (C++)

### 3. Compile C++ Program

```bash
g++ -std=c++11 cpp_receiver.cpp messages.pb.cc \
    -o cpp_receiver \
    -lprotobuf -lpthread
```

## Running the Example

### Option 1: Python → C++

```bash
# Terminal 1: Run Python sender
python3 python_sender.py

# Terminal 2: Run C++ receiver
./cpp_receiver
```

### Option 2: Test Both Directions

```bash
# Send command from Python
python3 python_sender.py

# C++ receives and responds
./cpp_receiver

# Python can read the response
python3 -c "
import messages_pb2
with open('/tmp/status.bin', 'rb') as f:
    status = messages_pb2.TrainStatus()
    status.ParseFromString(f.read())
    print(f'Speed: {status.current_speed} km/h')
"
```

## Key Points

1. **Same .proto file** - Both languages use the same schema
2. **Binary format** - Compact, fast, cross-language
3. **Type safety** - Enum values, field types enforced
4. **Versioning** - Can add fields without breaking compatibility

## File Communication

This example uses files for simplicity:
- `/tmp/command.bin` - Python → C++
- `/tmp/status.bin` - C++ → Python

In production, you'd use:
- Network sockets (TCP/UDP)
- Message queues (ZeroMQ, RabbitMQ)
- gRPC (built on protobuf)
- Shared memory

## Benefits for Railway Work

- **Fast**: Binary format much faster than JSON/XML
- **Typed**: Catches errors at compile time
- **Cross-language**: Works with Python (automation), C++ (real-time control)
- **Versionable**: Can evolve protocol without breaking existing systems
- **Small**: Messages are compact (important for embedded systems)