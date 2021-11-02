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
    void RunLongPoll ();
    std::string GetInfo () const;


private:
    TgBot::Bot bot_;
    const Config config;
    const std::string tmp_folder = "veiled_static_tmp/";
    void initActions ();
    void createTmpDir ();
};