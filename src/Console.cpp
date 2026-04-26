#include <u_console/Console.hpp>
#include <iostream>
#include <string>

namespace u_console {

Console::Console(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({width, height}), title) {
    std::cout << "u_console initialized." << std::endl;

    // Default font path (common on many Linux distros)
    m_fontPath = "/usr/share/fonts/aajohan-comfortaa-fonts/Comfortaa-Regular.otf";
    
    this->vertical_bottom_offset = height;
}

bool Console::load_font() {
    // Try primary font path
    if (this->font.openFromFile(m_fontPath)) {
        return true;
    }

    // Fallback list of common system fonts
    std::vector<std::string> fallbacks = {
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        "/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf",
        "/usr/share/fonts/gnu-free/FreeSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };

    for (const auto& path : fallbacks) {
        if (this->font.openFromFile(path)) {
            std::cout << "u_console: Loaded fallback font from " << path << std::endl;
            return true;
        }
    }

    std::cerr << "u_console Error: Could not load any usable font." << std::endl;
    return false;
}

void Console::set_font(const std::string& path) {
    m_fontPath = path;
}

void Console::run() {
    load_font();
    out_way = DEFAULT;
    handleWindow();
}

void Console::run(const char* filePath) {
    load_font();
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

    handleWindow();
}

void Console::run(std::iostream& stream) {
    load_font();
    out_way = MEMSTREAM;
    m_externalStream = &stream;
    
    m_externalStream->clear();
    m_externalStream->seekg(0, std::ios::end);
    m_externalStream->seekp(0, std::ios::end);
    m_lastReadPos = m_externalStream->tellg();

    handleWindow();
}

void Console::set_message(const char* message, MessageType type){
    ComposedMessage* m = new ComposedMessage(message, this->font, this->FONT_SIZE, type, pretty_writing, flag_writing);
    this->messages->push_back(m);
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

void Console::handleWindow(){
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    while (m_window.isOpen()) {
        
        check_outway_state();

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
                
                if( e->delta < 0 && vertical_top_offset <= 0){
                    continue;
                }

                vertical_top_offset += e->delta * FONT_SIZE;
                vertical_bottom_offset += e->delta * FONT_SIZE;
            }
        }

        m_window.clear(sf::Color(41, 45, 51));
        
        int index = 0;
        for(auto m : *messages){
            auto& cpm = m->composed_message;
            cpm.setPosition({LEFT_MARGIN, static_cast<float>(index * FONT_SIZE) + (index + 1) * VERTICAL_GAP});

            if(cpm.getPosition().y >= vertical_top_offset && cpm.getPosition().y <= vertical_bottom_offset){
                cpm.setPosition({LEFT_MARGIN, cpm.getPosition().y - vertical_top_offset});
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

} // namespace u_console

ComposedMessage::ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type, bool pw, bool fw)
    : content(content), composed_message(font, content, font_size) {
    this->type = type;
    this->font = &font;
    this->font_size = font_size;

    switch (type)
    {
    case ERROR:
        if(pw) composed_message.setFillColor(sf::Color(166, 36, 36));
        if(fw) composed_message.setString(std::string("[ERROR]: ").append(content));
        break;
        
    case WARN:
        if(pw) composed_message.setFillColor(sf::Color(10, 53, 110));
        if(fw) composed_message.setString(std::string("[WARN]: ").append(content));
        break;

    case INFO:
        if(pw) composed_message.setFillColor(sf::Color(196, 201, 42));
        if(fw) composed_message.setString(std::string("[INFO]: ").append(content));
        break;
    default:
        break;
    }
}
