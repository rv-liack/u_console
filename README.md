# u_console

A lightweight, multi-source console implementation using SFML. `u_console` allows you to create a graphical console window that can track internal messages, external files, or memory streams in real-time.

## Features

- **Multi-Source Support**: Run the console using internal memory, a text file, or a custom `std::iostream`.
- **Real-Time Monitoring**: Automatically tracks and displays new content added to files or streams.
- **Message Formatting**: Support for `INFO`, `WARN`, and `ERROR` message types with customizable colors and prefixes.
- **Pretty Printing**: Optional color-coded output for different message severities.
- **Flag Writing**: Toggleable prefixes (e.g., `[INFO]:`) for explicit message categorization.
- **Interactive UI**: Supports scrolling and handles window focus efficiently.
- **Asynchronous Execution**: The console window runs in its own thread, allowing you to continue writing messages from your main loop without blocking.

## Directory Structure

```text
u_console/
├── include/
│   └── u_console/
│       └── Console.hpp  # Public API and definitions
├── src/
│   └── Console.cpp      # Implementation
├── tests/
│   └── main.cpp         # Test application and examples
├── CMakeLists.txt       # Build configuration
└── README.md            # Project documentation
```

## Getting Started

### Prerequisites

- **SFML 3.1+**: Ensure SFML is installed on your system.
- **C++20**: The project uses modern C++ features.
- **CMake**: Minimum version 3.24.

### Building

```bash
mkdir build
cd build
cmake ..
make
```

### Basic Usage

```cpp
#include <u_console/Console.hpp>
#include <thread>
#include <chrono>

int main() {
    u_console::Console console(800, 600, "My Console", "path/to/font.ttf");
    
    // Enable features
    console.pretty_writing = true;
    console.flag_writing = true;

    // Start the console (Non-blocking)
    console.run();

    // Write messages from the main thread
    console.write_info("System started.");
    
    // Keep the main thread alive if necessary
    while (console.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
```

## How it Works

The console uses an internal monitoring system (`check_outway_state`) that periodically polls the active "outway" (file or stream). It maintains a read position to ensure only new content is processed, and it can parse prefixes to reconstruct the original `MessageType` for appropriate styling. The window loop runs in a separate thread, protected by a mutex to ensure thread-safe writing operations.

## License

This project is licensed under the MIT License.
