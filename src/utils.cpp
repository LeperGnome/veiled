#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <Magick++.h>

#include "utils.hpp"


const int FF = 255;
const int D9 = 217;
const std::set<std::string> ALLOWED_EXTENSIONS = {
    "PNG", 
    "JPG", 
    "JPEG", 
    "GIF", 
    "TIFF", 
    "HEIC"
};

Config::Config(int p, std::string wu)
    : port(p)
    , webhookUrl(wu)
    {
    }

bool IsAllowedExtension(const std::string filename){
    std::string extension = filename.substr(filename.find_last_of('.') + 1);
    for (auto& c: extension) c = std::toupper(c);
    return ALLOWED_EXTENSIONS.count(extension);
}

std::string SetExtensionToJPEG(const std::string& filename){
    return filename.substr(0, filename.find_last_of('.'))+".jpeg";
}

std::string ConvertToJPEG(const std::string& path){
    Magick::Image img(path);
    img.magick("JPEG");
    std::string new_path = SetExtensionToJPEG(path);
    img.write(new_path);
    return new_path;
}

void RemoveTMPFile(const std::string& path){
    // TODO logging
    if(std::remove(path.c_str()) != 0){
        std::cout << "Error removig file: " << path << std::endl;
    } else {
        std::cout << "File " << path << " successfully deleted" << std::endl;
    }
}


void HideText(const std::string& text, const std::string& image_path){
    std::ofstream img(image_path, std::ios::binary | std::ios::app);
    img << text;
    img.close();
}

std::string ExtractText(const std::string& image_path){
    std::ifstream img(image_path, std::ios::binary);

    // reading image to vector of unsigned char
    std::vector<unsigned char> buffer (std::istreambuf_iterator<char>(img), {});
    std::vector<unsigned char>::iterator it = buffer.begin();

    // finding FFD9 in file
    while (true){
        it = std::find_if(it, buffer.end(), [](auto x){return static_cast<int>(x) == FF;});

        // check EOF
        if (it == buffer.end() || next(it) == buffer.end()){
            break;
        }
        ++it;

        // check D9 after FF found
        if (static_cast<int>(*it) == D9){
            ++it;
            return std::string(it, buffer.end());
        }
    }
    throw std::runtime_error("Message not found.");
}