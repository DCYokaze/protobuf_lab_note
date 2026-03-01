#include <iostream>
#include <fstream>
#include <chrono>
#include "messages.pb.h"

// Read command from Python
railway::TrainCommand read_command(const std::string& filename) {
    railway::TrainCommand command;
    
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open command file");
    }
    
    if (!command.ParseFromIstream(&input)) {
        throw std::runtime_error("Failed to parse command");
    }
    
    return command;
}

// Send status to Python
void send_status(const std::string& filename, const railway::TrainStatus& status) {
    std::ofstream output(filename, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Cannot open status file");
    }
    
    if (!status.SerializeToOstream(&output)) {
        throw std::runtime_error("Failed to serialize status");
    }
}

// Create status response
railway::TrainStatus create_status(const std::string& train_id, 
                                   double speed, 
                                   double position) {
    railway::TrainStatus status;
    status.set_train_id(train_id);
    status.set_current_speed(speed);
    status.set_position(position);
    status.set_status(railway::TrainStatus::NORMAL);
    
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    status.set_timestamp(timestamp);
    
    status.add_warnings("Low battery");
    
    return status;
}

int main() {
    // Initialize protobuf library
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    std::cout << "=== C++ Receiver ===" << std::endl;
    
    try {
        // Read command from Python
        std::cout << "Reading command from /tmp/command.bin..." << std::endl;
        auto command = read_command("/tmp/command.bin");
        
        std::cout << "\nReceived command from Python:" << std::endl;
        std::cout << "  Train ID: " << command.train_id() << std::endl;
        std::cout << "  Command: " << railway::TrainCommand::CommandType_Name(command.command()) << std::endl;
        std::cout << "  Target Speed: " << command.target_speed() << " km/h" << std::endl;
        std::cout << "  Timestamp: " << command.timestamp() << std::endl;
        
        // Process command and create response
        std::cout << "\nProcessing command..." << std::endl;
        auto status = create_status(command.train_id(), 75.5, 1234.5);
        
        // Send status to Python
        send_status("/tmp/status.bin", status);
        std::cout << "Status written to /tmp/status.bin" << std::endl;
        
        std::cout << "\nSent status:" << std::endl;
        std::cout << "  Speed: " << status.current_speed() << " km/h" << std::endl;
        std::cout << "  Position: " << status.position() << " m" << std::endl;
        std::cout << "  Status: " << railway::TrainStatus::StatusCode_Name(status.status()) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Clean up protobuf library
    google::protobuf::ShutdownProtobufLibrary();
    
    return 0;
}
