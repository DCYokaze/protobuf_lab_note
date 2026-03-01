#!/usr/bin/env python3
"""
Python side: Sends commands to C++ and receives status updates
"""
import time
import messages_pb2

def create_train_command(train_id: str, command_type: int, speed: float) -> bytes:
    """Create and serialize a train command"""
    command = messages_pb2.TrainCommand()
    command.train_id = train_id
    command.command = command_type
    command.target_speed = speed
    command.timestamp = int(time.time() * 1000)
    
    return command.SerializeToString()

def parse_train_status(data: bytes) -> dict:
    """Parse status message from C++"""
    status = messages_pb2.TrainStatus()
    status.ParseFromString(data)
    
    return {
        'train_id': status.train_id,
        'speed': status.current_speed,
        'position': status.position,
        'status': messages_pb2.TrainStatus.StatusCode.Name(status.status),
        'timestamp': status.timestamp,
        'warnings': list(status.warnings)
    }

def main():
    # Example: Send accelerate command
    print("=== Python Sender ===")
    
    # Create command
    command_data = create_train_command(
        train_id="TRAIN-001",
        command_type=messages_pb2.TrainCommand.ACCELERATE,
        speed=80.0
    )
    
    print(f"Serialized command size: {len(command_data)} bytes")
    print(f"Command data (hex): {command_data.hex()}")
    
    # Save to file for C++ to read
    with open('/tmp/command.bin', 'wb') as f:
        f.write(command_data)
    print("Command written to /tmp/command.bin")
    
    # Wait for C++ response
    print("\nWaiting for C++ to write status...")
    time.sleep(2)
    
    # Read status from C++
    try:
        with open('/tmp/status.bin', 'rb') as f:
            status_data = f.read()
        
        status = parse_train_status(status_data)
        print("\nReceived status from C++:")
        print(f"  Train ID: {status['train_id']}")
        print(f"  Speed: {status['speed']} km/h")
        print(f"  Position: {status['position']} m")
        print(f"  Status: {status['status']}")
        print(f"  Warnings: {status['warnings']}")
    except FileNotFoundError:
        print("No status file yet - run C++ receiver")

if __name__ == '__main__':
    main()
