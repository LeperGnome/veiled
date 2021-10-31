#include <iostream>
#include <boost/program_options.hpp>

#include "bot.hpp"
#include "utils.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[]){
    // Parsing command line args
    po::options_description desc("Usage");
    desc.add_options()
        ("token", po::value<std::string>(), "Bot token.")
        ("port" , po::value<int>()->default_value(8080), "Port on which to run bot server.")
        ("webhook", po::value<std::string>(), "Webhook url which would be registered via Telegram API.")
    ;

    po::variables_map opts;
    po::store(po::parse_command_line(argc, argv, desc), opts);

    try {
        po::notify(opts);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    signal(SIGINT, [](int s) {
        std::cerr << "SIGINT got" << std::endl;
        exit(0);
    });

    // creating and running bot
    Config conf(opts["port"].as<int>(), opts["webhook"].as<std::string>());
    VeiledBot bot(opts["token"].as<std::string>(), conf);

    std::cout << bot.GetInfo() << std::endl;
    bot.RunWebwookServer();

    return 0;
}
