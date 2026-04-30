#include <u_console/Console.hpp>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <atomic>
#include <functional>
#include <sstream>

std::atomic<bool> running(true);

void producer(u_console::Console& console) {
    std::vector<std::string> messages = {
        "Initializing system core...",
        "Loading module: Graphics",
        "Loading module: Audio",
        "Warning: Low memory detected",
        "Error: Failed to connect to server",
        "Info: User 'admin' logged in",
        "Warning: Connection latency high",
        "Error: Disk write failure",
        "Info: Background task completed",
        "System ready.",
        "Processing data batch #104",
        "Heartbeat signal received",
        "Backup started: /mnt/backup",
        "Alert: Unusual network activity",
        "Service 'logger' restarted"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> msg_dist(0, messages.size() - 1);
    std::uniform_int_distribution<> type_dist(0, 2);


    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        if (!running) break;

        int type = type_dist(gen);
        const char* msg = messages[msg_dist(gen)].c_str();

        switch (type) {
            case 0: console.write_info(msg); break;
            case 1: console.write_warning(msg); break;
            case 2: console.write_error(msg); break;
        }
    }
}

int main() {
    u_console::Console console(800, 600, "u_console Test Application", "/usr/share/fonts/liberation-mono-fonts/LiberationMono-Bold.ttf");
    
    // Enable features
    console.pretty_writing = true;
    console.flag_writing = true;

    console.set_vendor("API_consumer");
    
    // Start producer thread
    std::thread t(producer, std::ref(console));
    
    // Run console with memory stream
    // Note: In a real scenario, run() blocks, so the producer 
    // would be writing to the 'ss' through the 'console' object.

    console.run();

    // Wait while console is running
    while (console.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Stop producer thread
    running = false;
    if (t.joinable()) {
        t.join();
    }
    
    return 0;
}
