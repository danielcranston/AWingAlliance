#include "audio/audio.h"

#include <memory>
#include <iostream>

namespace audio
{
AudioContextManager::AudioContextManager()
  : dev_mgr(alure::DeviceManager::getInstance()),
    dev(dev_mgr.openPlayback()),
    ctx(dev.createContext())
{
    alure::Context::MakeCurrent(ctx);
}

AudioContextManager::~AudioContextManager()
{
    alure::Context::MakeCurrent(nullptr);
    ctx.destroy();
    dev.close();
}

alure::Context& AudioContextManager::get_context()
{
    if (!audio::impl::instance)
    {
        audio::impl::instance = std::make_unique<AudioContextManager>();
    }
    return audio::impl::instance->ctx;
}

AudioSource::AudioSource(const float gain, const bool looping)
  : data(AudioContextManager::get_context().createSource())
{
    data.set3DSpatialize(alure::Spatialize::On);
    data.setGain(gain);
    data.setLooping(looping);
}

AudioSource::~AudioSource()
{
    data.destroy();
}

void AudioSource::play(const AudioBuffer& buffer)
{
    data.play(buffer.data);
}

void AudioSource::stop()
{
    data.stop();
}

bool AudioSource::is_playing() const
{
    return data.isPlaying();
}

void AudioSource::set_position(const float x, const float y, const float z)
{
    data.setPosition({ x, y, z });
}

AudioBuffer::AudioBuffer(const std::string& name)
  : data(AudioContextManager::get_context().getBuffer(name))
{
}

AudioBuffer::~AudioBuffer()
{
    AudioContextManager::get_context().removeBuffer(data);
}
}  // namespace audio
