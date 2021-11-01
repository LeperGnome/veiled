#include <sstream>
#include <iostream>
#include <string>

#include "bot.hpp"


VeiledBot::VeiledBot(const std::string& token, const Config& conf)
    : bot_(token)
    , config(conf)
    {
        bot_.getApi().deleteWebhook();
        initActions();
        std::cout << "Bot initialized" << std::endl;
    }

void VeiledBot::initActions(){
    // TODO: logging
    std::cout << "Initialized reply actions" << std::endl;
    
    bot_.getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
        bot_.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot_.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        auto user_document = message->document;
        if (user_document){
            auto caption = message->caption;
            if (caption.empty()){ 
                // TODO: try to exctract text
                bot_.getApi().sendMessage(message->chat->id, 
                    "Документ и текст должны быть в одном сообщении");
                return;
            }
            TgBot::File::Ptr file = bot_.getApi().getFile(user_document->fileId);
            std::string file_content = bot_.getApi().downloadFile(file->filePath);

            // TODO: save to tmp folder & perform operations & send back as InputFile
            // 1. FileTools::write 
            // 2. HideText(...) 
            // 3. .sendDocument(..., TgBot::InputFile::fromFile())

        }
    });
}

std::string VeiledBot::GetInfo() const {
    std::stringstream out;
    out << "Bot username: " << bot_.getApi().getMe()->username;
    return out.str();
}

void VeiledBot::RunLongPoll(){
    try {
        TgBot::TgLongPoll longPoll(bot_);
        while (true) { 
            std::cout << "LongPoll in progress..." << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        // TODO: logging
        std::cerr << e.what() << std::endl;
    }
}

void VeiledBot::RunWebwookServer(){
    /*
        NOT WORKING!
        Issue with TgBot-cpp
        See https://github.com/reo7sp/tgbot-cpp/issues/109
    */
    try {
        if (config.webhookUrl.empty()){
            throw std::invalid_argument("Webhook url is empty.");
        }
        TgBot::TgWebhookTcpServer webhookServer(config.port, bot_);
        std::cout << "Webhook bot starting" << std::endl;
        bot_.getApi().setWebhook(config.webhookUrl);
        webhookServer.start();
    } catch (std::exception& e) {
        // TODO: logging
        std::cerr << e.what() << std::endl;
    }
}