#include <u_console/Console.hpp>
#include <iostream>
#include <string>
#include <stdio.h>

namespace u_console {

Console::Console(unsigned int width, unsigned int height, const std::string& title, const std::string& font)
    : m_width(width), m_height(height), m_title(title) {
    std::cout << "u_console initialized." << std::endl;
    
    this->vertical_bottom_offset = height;
    m_fontPath = font;

    load_font();
    
    // Intro message
    set_message(std::string("Uconsole. Version: ").append(STR(VERSION)).c_str(), SUCCESS);
}

Console::~Console() {
    close();
}

void Console::close() {
    m_running = false;
    if (m_windowThread.joinable()) {
        m_windowThread.join();
    }
}

bool Console::is_running() const {
    return m_running;
}

bool Console::load_font() {

    if(!validate_file(m_fontPath.c_str())){
        std::cerr << "u_console Error: Not valid file path provided." << m_fontPath << std::endl;
        return false;
    }

    if (this->font.openFromFile(m_fontPath)) {
        return true;
    }

    std::cerr << "u_console Error: Could not load any usable font." << std::endl;
    return false;
}

void Console::set_font(const std::string& path) {
    m_fontPath = path;
    load_font();
}

void Console::run() {
    if (m_running) return;
    if (m_windowThread.joinable()) m_windowThread.join();
    out_way = DEFAULT;
    m_running = true;
    m_windowThread = std::thread(&Console::handle_window, this);
}

void Console::run(const char* filePath) {
    if (m_running) return;
    if (m_windowThread.joinable()) m_windowThread.join();
    out_way = FSTREAM;
    m_filePath = filePath;
    
    std::ifstream file(m_filePath, std::ios::ate | std::ios::binary);
    if (file.is_open()) {
        m_lastReadPos = file.tellg();
        file.close();
    } else {
        std::ofstream create(m_filePath);
        create.close();
        m_lastReadPos = 0;
    }

    m_running = true;
    m_windowThread = std::thread(&Console::handle_window, this);
}

void Console::run(std::iostream& stream) {
    if (m_running) return;
    if (m_windowThread.joinable()) m_windowThread.join();
    out_way = MEMSTREAM;
    m_externalStream = &stream;
    
    m_externalStream->clear();
    m_externalStream->seekg(0, std::ios::end);
    m_externalStream->seekp(0, std::ios::end);
    m_lastReadPos = m_externalStream->tellg();

    m_running = true;
    m_windowThread = std::thread(&Console::handle_window, this);
}

void Console::set_message(const char* message, MessageType type){

    std::string def_vendor = DEFAULT_VENDOR;

    if(!current_vendor.empty()){
        def_vendor = current_vendor;
    }

    def_vendor.append(VENDOR_PROMPT);

    std::lock_guard<std::mutex> lock(m_mutex);
    messages.push_back(std::make_unique<ComposedMessage>(message, this->font, this->FONT_SIZE, type, def_vendor, pretty_writing, flag_writing));
}


void Console::write_to_outway(const char* message, MessageType type) {
    if (out_way == DEFAULT) {
        set_message(message, type);
        return;
    }

    std::ostream* os = nullptr;
    std::ofstream file;

    if (out_way == FSTREAM) {
        file.open(m_filePath, std::ios::app | std::ios::binary);
        if (file.is_open()) {
            os = &file;
        }
    } else if (out_way == MEMSTREAM) {
        os = m_externalStream;
        os->clear();
        os->seekp(0, std::ios::end);
    }

    if (os) {
        if (flag_writing) {
            switch (type) {
                case INFO:  *os << "[INFO]: ";  break;
                case WARN:  *os << "[WARN]: ";  break;
                case ERROR: *os << "[ERROR]: "; break;
                case SUCCESS: *os << "[SUCCESS]: "; break;
                default: break;
            }
        }
        *os << message << std::endl;
        os->flush();
        if (out_way == FSTREAM) {
            file.close();
        }
    }
}

void Console::write(const char* message){
    write_to_outway(message, NORMAL);
}

void Console::write_info(const char* message){
    write_to_outway(message, INFO);
}

void Console::write_warning(const char* message){
    write_to_outway(message, WARN);
}

void Console::write_error(const char* message){
    write_to_outway(message, ERROR);
}

void Console::write_success(const char* message){
    write_to_outway(message, SUCCESS);
}

void Console::check_outway_state() {
    if (out_way == DEFAULT) return;

    std::istream* is = nullptr;
    std::ifstream file;

    if (out_way == FSTREAM) {
        file.open(m_filePath, std::ios::in | std::ios::binary);
        if (file.is_open()) {
            is = &file;
        }
    } else if (out_way == MEMSTREAM) {
        is = m_externalStream;
    }

    if (!is) return;

    is->clear();
    is->seekg(0, std::ios::end);
    std::streamoff currentSize = is->tellg();

    if (currentSize > m_lastReadPos) {
        is->seekg(m_lastReadPos);
        std::string line;
        while (std::getline(*is, line)) {
            std::streamoff nextPos = is->tellg();
            
            MessageType type = NORMAL;
            std::string content = line;

            if (line.compare(0, 8, "[INFO]: ") == 0) {
                type = INFO;
                content = line.substr(8);
            } else if (line.compare(0, 8, "[WARN]: ") == 0) {
                type = WARN;
                content = line.substr(8);
            } else if (line.compare(0, 9, "[ERROR]: ") == 0) {
                type = ERROR;
                content = line.substr(9);
            } else if (line.compare(0, 11, "[SUCCESS]: ") == 0) {
                type = SUCCESS;
                content = line.substr(11);
            }

            set_message(content.c_str(), type);
            
            if (nextPos != -1) {
                m_lastReadPos = nextPos;
            } else {
                m_lastReadPos = currentSize;
                break;
            }
        }
    }
}

void Console::handle_window(){
    m_window.create(sf::VideoMode({m_width, m_height}), m_title);
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    while (m_window.isOpen() && m_running) {
        
        check_outway_state();

        while (const std::optional event = m_window.pollEvent()){
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }

            if(event->is<sf::Event::MouseWheelScrolled>()){
                auto e = event->getIf<sf::Event::MouseWheelScrolled>();
                
                if( e->delta < 0 && vertical_top_offset <= 0){
                    continue;
                }

                vertical_top_offset += e->delta * FONT_SIZE;
                vertical_bottom_offset += e->delta * FONT_SIZE;
            }
        }

        if(!m_window.hasFocus()){
            m_window.display();
            continue;
        }

        m_window.clear(sf::Color(41, 45, 51));
        
        float current_y = VERTICAL_GAP;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for(const auto& m : messages){
                auto& cpm = m->composed_message;
                cpm.setPosition({LEFT_MARGIN, current_y});

                if(cpm.getPosition().y + (m->lines * FONT_SIZE) >= vertical_top_offset && cpm.getPosition().y <= vertical_bottom_offset){
                    cpm.setPosition({LEFT_MARGIN, cpm.getPosition().y - vertical_top_offset});
                    m_window.draw(cpm);
                }
                current_y += (m->lines * FONT_SIZE) + VERTICAL_GAP;
            }
        }
        m_window.display();
    }
    m_running = false;
}

void Console::set_vendor(const std::string& vendor){
    this->current_vendor = vendor;
}

bool Console::validate_file(const char* path){
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

} // namespace u_console

ComposedMessage::ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type, std::string prompt, bool pw, bool fw)
    : content(content), type(type), font(&font), font_size(font_size), composed_message(font) {
    
    // Count lines
    this->lines = 1;
    std::string content_str(content);
    for (char c : content_str) {
        if (c == '\n') this->lines++;
    }

    std::string full_text;
    if (fw) {
        full_text += prompt;
        switch (type) {
            case INFO:  full_text += "[INFO]: ";  break;
            case WARN:  full_text += "[WARN]: ";  break;
            case ERROR: full_text += "[ERROR]: "; break;
            case SUCCESS: full_text += "[SUCCESS]: "; break;
            default: break;
        }
    }
    full_text += content;

    composed_message.setFont(font);
    composed_message.setCharacterSize(font_size);
    composed_message.setString(sf::String::fromUtf8(full_text.begin(), full_text.end()));

    if (pw || type == SUCCESS) {
        switch (type) {
            case INFO:  composed_message.setFillColor(sf::Color(32, 83, 250));  break;
            case WARN:  composed_message.setFillColor(sf::Color(250, 203, 32)); break;
            case ERROR: composed_message.setFillColor(sf::Color(250, 32, 32));  break;
            case SUCCESS: composed_message.setFillColor(sf::Color(34, 177, 76)); break;
            default: break;
        }
    }
}
