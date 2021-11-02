#include <sstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

#include "bot.hpp"


VeiledBot::VeiledBot(const std::string& token, const Config& conf)
    : bot_(token)
    , config(conf)
    {
        bot_.getApi().deleteWebhook();
        initActions();
        createTmpDir();
        std::cout << "Bot initialized" << std::endl;
    }

void VeiledBot::createTmpDir(){
    boost::filesystem::path dir(tmp_folder);
    if(boost::filesystem::create_directory(dir))
    {
        std::cerr<< "Directory Created: "<<tmp_folder<<std::endl;
    }
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
            TgBot::File::Ptr file = bot_.getApi().getFile(user_document->fileId);
            std::string tmp_filepath = tmp_folder + file->fileId;
            std::string file_content = bot_.getApi().downloadFile(file->filePath);
            FileTools::write(file_content, tmp_filepath);

            std::string caption = message->caption;
            if (caption.empty()){ 
                // TODO: try to exctract text
                bot_.getApi().sendMessage(message->chat->id, 
                    "Документ и текст должны быть в одном сообщении");
                return;
            } else {
                HideText(caption, tmp_filepath);
                auto out_file = TgBot::InputFile::fromFile(tmp_filepath, user_document->mimeType);
                out_file->fileName = user_document->fileName;
                bot_.getApi().sendDocument(message->chat->id, out_file);
            }
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