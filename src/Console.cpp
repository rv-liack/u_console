#include <u_console/Console.hpp>
#include <iostream>
#include "string"

namespace u_console {
Console::Console(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({width, height}), title) {
    std::cout << "u_console initialized." << std::endl;

    if(!this->font.openFromFile("/usr/share/fonts/aajohan-comfortaa-fonts/Comfortaa-Regular.otf")){
        return;
    }

    this->vertical_bottom_offset = height;
}

void Console::run() {
    handleWindow();
}

void Console::run(const char* filePath) {
    handleWindow();
}

void Console::run(std::fstream& stream) {
    handleWindow();
}

void Console::set_message(const char* message, MessageType type){
    ComposedMessage* m = new ComposedMessage(message,this->font, this->FONT_SIZE, type,pretty_writing,flag_writing);
    this->messages->push_back(m);
}

void Console::write(const char* message){
    set_message(message, NORMAL);
}

void Console::write_info(const char* message){
    set_message(message, INFO);
}

void Console::write_warning(const char* message){
    set_message(message, WARN);
}

void Console::write_error(const char* message){
    set_message(message, ERROR);
}

void Console::handleWindow(){
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    while (m_window.isOpen()) {

        if(!m_window.hasFocus()){
            m_window.display();
            continue;
        }

        while (const std::optional event = m_window.pollEvent()){
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }

            if(event->is<sf::Event::MouseWheelScrolled>()){
                auto e = event->getIf<sf::Event::MouseWheelScrolled>();
                
                if( e->delta < 0 && vertical_top_offset <=0){
                    continue;
                }

                vertical_top_offset += e->delta * FONT_SIZE;
                vertical_bottom_offset += e->delta * FONT_SIZE;
            }
        }

        m_window.clear();
        
        int index = 0;
        for(auto m : *messages){
            
            auto& cpm = m->composed_message;
            cpm.setPosition({0, static_cast<float>(index * FONT_SIZE)});

            if(cpm.getPosition().y >= vertical_top_offset && cpm.getPosition().y <= vertical_bottom_offset){
                m->composed_message.setPosition({0.0f, static_cast<float>(cpm.getPosition().y - vertical_top_offset)});
                m_window.draw(cpm);
            }
            
            index++;
        }
        m_window.display();
    }

    for (auto m : *messages) {
        delete m;
    }
    delete messages;
}
}


ComposedMessage::ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type, bool pw, bool fw)
    : composed_message(font, content, font_size) {
    this->content = content;
    this->type = type;

    this->font = &font;
    this->font_size = font_size;

    switch (type)
    {
    case ERROR:
        if(pw){
            composed_message.setFillColor(sf::Color::Red);
        }
        if(fw){
            composed_message.setString(std::string("[ERROR]: ").append(content));
        }
        break;
        
    case WARN:
        if(pw){
            composed_message.setFillColor(sf::Color::Yellow);
        }
        if(fw){
            composed_message.setString(std::string("[WARN]: ").append(content));
        }
        break;

    case INFO:
        if(pw){
            composed_message.setFillColor(sf::Color::Blue);
        }
        if(fw){
            composed_message.setString(std::string("[INFO]: ").append(content));
        }
        break;
    default:
        break;
    }
}
