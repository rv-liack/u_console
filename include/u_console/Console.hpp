/**
 * @file Console.hpp
 * @brief Main header for the u_console library.
 * 
 * u_console provides a graphical console window that can display messages
 * from various sources, including internal memory, files, and streams.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * @brief Categories for console messages, determining their visual style and flags.
 */
enum MessageType {
    NORMAL, ///< Standard white text
    INFO,   ///< Informational message (typically Blue)
    WARN,   ///< Warning message (typically Yellow)
    ERROR   ///< Error message (typically Red)
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
    std::string content = "\n";
    MessageType type = NORMAL;

    sf::Font* font = nullptr;
    unsigned int font_size = 0;
    sf::Text composed_message;

    /**
     * @brief Construct a new Composed Message object.
     * 
     * @param content Raw text of the message.
     * @param font Reference to the font to be used.
     * @param font_size Character size for the text.
     * @param type The type of message (influences color and prefix).
     * @param pw Whether to enable pretty printing (colors).
     * @param fw Whether to enable flag writing (prefixes).
     */
    ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type, bool pw = false, bool fw = false);
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
     */
    Console(unsigned int width, unsigned int height, const std::string& title);
    
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
     * @brief Sets the path for the console font.
     * @param path Absolute or relative path to a .ttf or .otf file.
     */
    void set_font(const std::string& path);

    /**
     * @brief If true, messages will be color-coded based on their type.
     */
    bool pretty_writing = false;
        
    /**
     * @brief If true, messages will include a prefix (e.g., "[INFO]: ").
     */
    bool flag_writing = false;

    const unsigned int FONT_SIZE = 14;
    sf::Font font;

private:
    std::vector<ComposedMessage*>* messages = new std::vector<ComposedMessage*>();
    sf::RenderWindow m_window;

    unsigned int vertical_top_offset = 0;
    unsigned int vertical_bottom_offset = 0;

    void handleWindow();
    void set_message(const char*, MessageType);

    /**
     * @brief Attempts to load the font from m_fontPath or common system fallbacks.
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

    OutWay out_way = DEFAULT;
    std::string m_filePath;
    std::string m_fontPath;
    std::iostream* m_externalStream = nullptr;
    std::streamoff m_lastReadPos = 0;
};
}
