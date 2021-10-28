#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <exception>

using ios = std::ios;
const int FF = 255;
const int D9 = 217;

void HideText(const std::string& text, const std::string& image_path){
    std::ofstream img(image_path, ios::binary | ios::app);
    img << text;
    img.close();
}

std::string ExtractText(const std::string& image_path){
    std::ifstream img(image_path, ios::binary);

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


int main(){
    // HideText("Sample message", "./test.jpg");
    std::cout << ExtractText("test.jpg");
    return 0;
}
