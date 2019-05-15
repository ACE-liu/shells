/*
 * main.cpp
 * Copyright (C) 2018 Albert Lv <altair.albert@yahoo.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "tts.h"
#include <string>
#include <unistd.h>
#include <iostream>


int main(int argc, char const* argv[])
{
    std::string path = WORK_DIR;
    TTS tts(path);
    tts.init();
    if (argc >= 3) {
        const std::string dst = path + "/" + argv[2];
        tts.setDesPath(dst.c_str());
        tts.synthesis(argv[1]);
    } else if (argc == 2) {
        tts.synthesis(argv[1]);
    } else {
        std::cout << "Usage: " << argv[0] << " text outfile" << std::endl;
    }
    sleep(1);
    return 0;
}
