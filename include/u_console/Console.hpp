#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/*
    An enum that containes the group of message types.
*/
enum MessageType {
    NORMAL,
    INFO,
    WARN,
    ERROR
};

/*
    An enum that containes the group of diferent types of out ways.
*/
enum OutWay {
    DEFAULT,
    MEMSTREAM,
    FSTREAM,
};

/*
    A struct for the dinamical message representation.
*/
struct ComposedMessage{
    const char* content = "\n";
    MessageType type = NORMAL;

    sf::Font* font;
    unsigned int font_size;
    sf::Text composed_message = sf::Text(*font,content,font_size);

    ComposedMessage(const char*,sf::Font&,unsigned int,MessageType);
    ComposedMessage() = default;
};

namespace u_console {
    
    /*
        Base class for building a console.
    */
class Console {
public:
    Console(unsigned int width, unsigned int height, const std::string& title);
    
    /*
        Launchs the console graphic interface and start showing messages 
        from the console default defined out way.
    */
    void run();

    /*
        Launchs the console graphic interface and start showing messages 
        from a given file as out way.
    */
    void run(const char*);

    /*
        Launchs the console graphic interface and start showing messages 
        from a given stream as out way.
    */
    void run(std::fstream&);

    /*
        Writes a single message in the defined out way.
    */
    void write(const char*);
    /*
        Writes a styled message indicating information. The raw message is written in the defined out way.
    */
    void write_info(const char*);
    /*
        Writes a styled message indicating an error. The raw message is written in the defined out way.
    */
    void write_error(const char*);
    /*
        Writes a styled message indicating a warning. The raw message is written in the defined out way.
    */
    void write_warning(const char*);

    
    /*
        
    */
    bool pretty_printing = false;
    /*
        
    */
    bool flag_printing = false;

    /*
    
    */
    const unsigned int FONT_SIZE = 14;
    sf::Font font;

private:
    /*
        Represents the default out way for the console, being also the messages stack.
    */
    std::vector<ComposedMessage*>* messages = new std::vector<ComposedMessage*>();

    /*
        The sfml window.
    */
    sf::RenderWindow m_window;

    /*
        Adds a new message in the messages stack.
    */

    unsigned int vertical_top_offset = 0;
    unsigned int vertical_bottom_offset = 0;


    void handleWindow();
    void set_message(const char*, MessageType);

    OutWay out_way = DEFAULT;
};
}
