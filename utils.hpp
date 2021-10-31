#pragma once

#include <string>

struct Config {
    const int port; 
    const std::string webhookUrl;

    Config(int p, std::string wu);
};

void HideText(const std::string& text, const std::string& image_path);
std::string ExtractText(const std::string& image_path);