#pragma once

#include <string>
#include <set>

struct Config {
    const int port; 
    const std::string webhookUrl;

    Config(int p, std::string wu);
};

bool IsAllowedExtension(const std::string filename);
std::string SetExtensionToJPEG(const std::string& filename);
std::string ConvertToJPEG(const std::string& path); // returns path to converted image
void RemoveTMPFile(const std::string& path);

void HideText(const std::string& text, const std::string& image_path);
std::string ExtractText(const std::string& image_path);