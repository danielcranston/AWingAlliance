#include "audio/audio_manager.h"

AudioManager::AudioManager(const std::string sounds_path)
  : sounds_path(sounds_path),
    dev_mgr(alure::DeviceManager::getInstance()),
    dev(dev_mgr.openPlayback()),
    ctx(dev.createContext())
{
    alure::Context::MakeCurrent(ctx);
    std::cout << "Opened \"" << dev.getName() << "\"" << std::endl;
}

AudioManager::~AudioManager()
{
    for (auto item : audios)
    {
        item.second.second.destroy();
        ctx.removeBuffer(item.second.first);
    }
    alure::Context::MakeCurrent(nullptr);
    ctx.destroy();
    dev.close();
}

void AudioManager::loadAudio(const std::string& filename, const float gain)
{
    std::cout << "Loading \"" << filename << "\"" << std::endl;

    const std::string full_path = sounds_path + filename;

    auto source = ctx.createSource();
    source.setGain(gain);

    audios.insert(std::make_pair(filename, std::make_pair(ctx.getBuffer(full_path), source)));
};

void AudioManager::play(const std::string& filename)
{
    auto pair = audios.at(filename);
    if (!pair.second.isPlaying())
    {
        pair.second.play(pair.first);
    }
};

void AudioManager::stop(const std::string& filename)
{
    auto pair = audios.at(filename);
    if (pair.second.isPlaying())
    {
        pair.second.stop();
        // pair.second.fadeOutToStop(0.0f, std::chrono::milliseconds(10));
    }
}
