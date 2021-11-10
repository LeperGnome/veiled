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
        std::stringstream start_message;
        start_message << "*Прячу текст в картинки и извлекаю его.*\n"
                      << "\nЕсли хочешь _спрятать_, напиши сообщение которое хочешь спрятать и прикрепи документ в одном сообщении."
                      << "\nЕсли хочешь _извлечь_, то просто пришли картинку, как документ. Я сделаю, что смогу."
                      << "\n\n P.S. я в бете, не всегда работаю.";
        bot_.getApi().sendMessage(
            message->chat->id, 
            start_message.str(), 
            false, 0, {}, 
            "Markdown"
        );
    });
    bot_.getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        // TODO: refactor
        auto user_document = message->document;
        std::string user_fullname = message->from->firstName + ' ' + message->from->lastName;
        if (user_document){
            if (!IsAllowedExtension(user_document->fileName)){
                bot_.getApi().sendMessage(message->chat->id, "Расширение не поддерживается");
                return;
            }
            TgBot::File::Ptr file = bot_.getApi().getFile(user_document->fileId);
            std::string tmp_filepath = tmp_folder + file->fileId;
            std::string file_content = bot_.getApi().downloadFile(file->filePath);
            FileTools::write(file_content, tmp_filepath);

            std::string caption = message->caption;
            if (caption.empty()){ 
                std::string hidden_text = ExtractText(tmp_filepath);
                std::string reply_msg = "Текст не найден =(";
                if (!hidden_text.empty()){
                    reply_msg = "Найден текст: \"" + hidden_text + "\"";
                }
                bot_.getApi().sendMessage(message->chat->id, reply_msg);
            } else {
                std::string jpg_image_path = ConvertToJPEG(tmp_filepath);
                HideText(caption, jpg_image_path);
                LogMessage(user_fullname, caption);

                auto out_file = TgBot::InputFile::fromFile(jpg_image_path, user_document->mimeType);
                out_file->fileName = SetExtensionToJPEG(user_document->fileName);
                bot_.getApi().sendDocument(message->chat->id, out_file);

                RemoveTMPFile(jpg_image_path);
            }
            RemoveTMPFile(tmp_filepath);
        }
    });
}

std::string VeiledBot::GetInfo() const {
    std::stringstream out;
    out << "Bot username: " << bot_.getApi().getMe()->username;
    return out.str();
}

void VeiledBot::RunLongPoll(){
    TgBot::TgLongPoll longPoll(bot_);
    std::cout << "LongPoll in progress..." << std::endl;
    while (true) { 
        try {
            longPoll.start();
        } catch (std::exception& e) {
            // TODO: logging
            std::cerr << e.what() << std::endl;
        }
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
        std::cerr << e.what() << std::endl;
    }
}