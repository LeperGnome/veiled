#include <sstream>
#include <iostream>
#include <string>

#include "bot.hpp"


VeiledBot::VeiledBot(const std::string& token, const Config& conf)
    : bot_(token)
    , config(conf)
    {
        initActions();
    }

void VeiledBot::initActions(){
    // TODO: logging
    std::cout << "Initialized reply actions" << std::endl;
    
    // TODO: events are not registering?
    bot_.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        this->bot_.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot_.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        std::cout << "User wrote: " << message->text << std::endl;
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        this->bot_.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

}

std::string VeiledBot::GetInfo() const {
    std::stringstream out;
    out << "Bot username: " << bot_.getApi().getMe()->username;
    return out.str();
}

void VeiledBot::RunWebwookServer(){
    try {
        TgBot::TgWebhookTcpServer webhookServer(config.port, bot_);
        std::cout << "Server starting" << std::endl;
        bot_.getApi().setWebhook(config.webhookUrl);
        webhookServer.start();
    } catch (std::exception& e) {
        // TODO: logging
        std::cerr << e.what() << std::endl;
    }
}