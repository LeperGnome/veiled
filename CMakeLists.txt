cmake_minimum_required(VERSION 2.8.4)
project(veiled-bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system program_options filesystem REQUIRED)
find_package(CURL)
find_package(ImageMagick COMPONENTS Magick++)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

include_directories(/usr/lib /usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${ImageMagick_INCLUDE_DIRS})


if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

add_executable(
    veiled-bot 
    src/main.cpp
    src/bot.cpp
    src/utils.cpp
)

target_link_libraries(veiled-bot /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${ImageMagick_LIBRARIES})