#pragma once

#include <map>
#include <string>
#include <utility>

#include <AL/alure2.h>

struct AudioManager
{
    AudioManager(const std::string sounds_path);
    ~AudioManager();

    void loadAudio(const std::string& filename, const float gain = 1.0);
    void play(const std::string& filename);
    void stop(const std::string& filename);

    const std::string sounds_path;

    alure::DeviceManager dev_mgr;
    alure::Device dev;
    alure::Context ctx;
    std::map<std::string, std::pair<alure::Buffer, alure::Source>> audios;
};
