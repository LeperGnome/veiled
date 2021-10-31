#pragma once

#include <string>
#include <boost/program_options.hpp>
#include <tgbot/tgbot.h>

#include "utils.hpp"

namespace po = boost::program_options;


class VeiledBot {
public:
    VeiledBot (const std::string& token, const Config& c);
    void RunWebwookServer ();
    std::string GetInfo () const;

private:
    TgBot::Bot bot_;
    const Config config;
    void initActions ();
};