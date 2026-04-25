# u_console

A lightweight, multi-source console implementation using SFML. `u_console` allows you to create a graphical console window that can track internal messages, external files, or memory streams in real-time.

## Features

- **Multi-Source Support**: Run the console using internal memory, a text file, or a custom `std::iostream`.
- **Real-Time Monitoring**: Automatically tracks and displays new content added to files or streams.
- **Message Formatting**: Support for `INFO`, `WARN`, and `ERROR` message types with customizable colors and prefixes.
- **Pretty Printing**: Optional color-coded output for different message severities.
- **Flag Writing**: Toggleable prefixes (e.g., `[INFO]:`) for explicit message categorization.
- **Interactive UI**: Supports scrolling and handles window focus efficiently.

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

int main() {
    u_console::Console console(800, 600, "My Console");
    
    // Enable features
    console.pretty_writing = true;
    console.flag_writing = true;

    // Option 1: Internal console (default)
    console.write_info("System started.");
    console.run();

    // Option 2: Track a file
    // console.run("app.log");

    // Option 3: Track a memory stream
    // std::stringstream ss;
    // console.run(ss);

    return 0;
}
```

## How it Works

The console uses an internal monitoring system (`check_outway_state`) that periodically polls the active "outway" (file or stream). It maintains a read position to ensure only new content is processed, and it can parse prefixes to reconstruct the original `MessageType` for appropriate styling.

## License

This project is licensed under the MIT License - see the LICENSE file for details (or add your own).
