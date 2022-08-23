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

void AudioContextManager::set_listener_pose(const Eigen::Matrix4f& pose)
{
    auto listener = get_context().getListener();

    const auto& pos = pose.col(3).head<3>();
    listener.setPosition(alure::Vector3(pos.x(), pos.y(), pos.z()));

    const Eigen::Vector3f& fwd = -pose.col(2).head<3>();
    const Eigen::Vector3f& up = pose.col(1).head<3>();
    listener.setOrientation(std::make_pair(alure::Vector3(fwd.x(), fwd.y(), fwd.z()),
                                           alure::Vector3(up.x(), up.y(), up.z())));
}

void AudioContextManager::update()
{
    get_context().update();
}

AudioSource::AudioSource(const float gain, const bool looping)
  : data(AudioContextManager::get_context().createSource())
{
    data.set3DSpatialize(alure::Spatialize::On);
    data.setRelative(false);
    data.setGain(gain);
    data.setLooping(looping);
    data.setDistanceRange(1.0f, 2000.0f);
    data.setRadius(1.0f);
    data.setRolloffFactors(0.5);
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

void AudioSource::set_pose(const Eigen::Matrix4f& pose)
{
    const auto& pos = pose.col(3).head<3>();
    data.setPosition(alure::Vector3(pos.x(), pos.y(), pos.z()));

    const Eigen::Vector3f& fwd = -pose.col(2).head<3>();
    const Eigen::Vector3f& up = pose.col(1).head<3>();
    data.setOrientation(std::make_pair(alure::Vector3(fwd.x(), fwd.y(), fwd.z()),
                                       alure::Vector3(up.x(), up.y(), up.z())));
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
