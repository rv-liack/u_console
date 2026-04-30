/**
 * @file Console.hpp
 * @brief Main header for the u_console library.
 * 
 * u_console provides a graphical console window that can display messages
 * from various sources, including internal memory, files, and streams.
 */

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define VERSION 1.0.0

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

/**
 * @brief Categories for console messages, determining their visual style and flags.
 */
enum MessageType {
    NORMAL, ///< Standard white text
    INFO,   ///< Informational message (typically Blue)
    WARN,   ///< Warning message (typically Yellow)
    ERROR,  ///< Error message (typically Red)
    SUCCESS ///< Success/Intro message (typically Green)
};

/**
 * @brief Defines the active source of data for the console.
 */
enum OutWay {
    DEFAULT,   ///< Internal message stack
    MEMSTREAM, ///< External memory stream (std::iostream)
    FSTREAM,   ///< External text file
};

/**
 * @brief Internal representation of a single console message.
 * 
 * Handles the raw content and its graphical representation using SFML.
 */
struct ComposedMessage {
    std::string content = "\n"; ///< Raw text of the message.
    MessageType type = NORMAL;  ///< Type of message for styling.
    unsigned int lines = 1;     ///< Number of lines in the message.

    sf::Font* font = nullptr;           ///< Font used for rendering.
    unsigned int font_size = 0;         ///< Size of the font.
    sf::Text composed_message;          ///< SFML representation of the message.

    /**
     * @brief Construct a new Composed Message object.
     * 
     * @param content Raw text of the message.
     * @param font Reference to the font to be used.
     * @param font_size Character size for the text.
     * @param type The type of message (influences color and prefix).
     * @param prompt The vendor prompt string.
     * @param pw Whether to enable pretty printing (colors).
     * @param fw Whether to enable flag writing (prefixes).
     */
    ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type, std::string prompt, bool pw = false, bool fw = false);
};

namespace u_console {

/**
 * @class Console
 * @brief The main console class responsible for rendering and source monitoring.
 */
class Console {
public:
    /**
     * @brief Initialize a new Console window.
     * 
     * @param width Window width in pixels.
     * @param height Window height in pixels.
     * @param title Window title bar text.
     * @param font Font file path.
     */
    Console(unsigned int width, unsigned int height, const std::string& title, const std::string& font);
    
    /**
     * @brief Destructor for the Console. Ensures resources are cleaned up and thread is joined.
     */
    ~Console();

    /**
     * @brief Starts the console in DEFAULT mode (internal stack).
     */
    void run();

    /**
     * @brief Starts the console in FSTREAM mode, tracking a text file.
     * @param filePath Path to the file to monitor.
     */
    void run(const char* filePath);

    /**
     * @brief Starts the console in MEMSTREAM mode, tracking an active stream.
     * @param stream Reference to a std::iostream (e.g., std::stringstream).
     */
    void run(std::iostream& stream);

    /**
     * @brief Stops the console, closes the window, and joins the worker thread.
     */
    void close();

    /**
     * @brief Checks if the console is currently running.
     * @return true if the window loop is active.
     */
    bool is_running() const;

    /**
     * @brief Writes a normal message to the active outway.
     */
    void write(const char*);
    
    /**
     * @brief Writes an INFO message to the active outway.
     */
    void write_info(const char*);
    
    /**
     * @brief Writes an ERROR message to the active outway.
     */
    void write_error(const char*);
    
    /**
     * @brief Writes a WARN message to the active outway.
     */
    void write_warning(const char*);

    /**
     * @brief Writes a SUCCESS message to the active outway.
     */
    void write_success(const char*);

    /**
     * @brief Sets the path for the console font.
     * @param path Absolute or relative path to a .ttf or .otf file.
     */
    void set_font(const std::string& path);

    /**
     * @brief Sets the vendor name to be shown in the prompt.
     * @param vendor The name of the vendor (e.g., "Rafael").
     */
    void set_vendor(const std::string& vendor);

    /**
     * @brief If true, messages will be color-coded based on their type.
     */
  
    bool pretty_writing = false;
        
    /**
     * @brief If true, messages will include a prefix (e.g., "[INFO]: ").
     */
    bool flag_writing = false;

private:
    std::vector<std::unique_ptr<ComposedMessage>> messages; ///< Internal list of all messages.
    sf::RenderWindow m_window; ///< Main SFML window.

    float vertical_top_offset = 0;    ///< Scrolling top offset.
    float vertical_bottom_offset = 0; ///< Scrolling bottom offset (window height).

    std::thread m_windowThread;       ///< Thread for the window loop.
    std::atomic<bool> m_running{false}; ///< Flag indicating if the console is running.
    std::mutex m_mutex;               ///< Mutex for thread-safe access to messages.

    unsigned int m_width, m_height;   ///< Stored window dimensions.
    std::string m_title;              ///< Stored window title.

    /**
     * @brief Main window event and rendering loop.
     */
    void handle_window();

    /**
     * @brief Creates and appends a ComposedMessage to the messages list.
     * @param message Raw text content.
     * @param type The type of message.
     */
    void set_message(const char* message, MessageType type);

    /**
     * @brief Attempts to load the font from m_fontPath..
     * @return true if a font was successfully loaded.
     */
    bool load_font();

    /**
     * @brief Polling method called each frame to check for updates in files/streams.
     */
    void check_outway_state();

    /**
     * @brief Unified writing method that redirects to the appropriate outway.
     */
    void write_to_outway(const char* message, MessageType type);
    
    /**
     * @brief Method to check if a file exists and is a file.
     * @return true if @param path is exists and is a file. 
     */
    bool validate_file(const char* path);

    OutWay out_way = DEFAULT;

    std::string m_filePath;
    std::string m_fontPath;
    std::iostream* m_externalStream = nullptr;
    std::streamoff m_lastReadPos = 0;
    std::string current_vendor = "";

    const std::string DEFAULT_VENDOR = "console";
    const std::string VENDOR_PROMPT = "--> ";

    const float LEFT_MARGIN = 20.0f;
    const float VERTICAL_GAP = 10.0f;
    const unsigned int FONT_SIZE = 16;
    sf::Font font;
};
}
