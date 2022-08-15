#pragma once

#include <AL/alure2.h>

namespace audio
{
class AudioContextManager
{
  public:
    friend std::unique_ptr<AudioContextManager> std::make_unique<AudioContextManager>();

    alure::DeviceManager dev_mgr;
    alure::Device dev;
    alure::Context ctx;

    static alure::Context& get_context();
    static void set_listener_position(const float x, const float y, const float z);

    ~AudioContextManager();

  private:
    AudioContextManager();
};

namespace impl
{
static std::unique_ptr<AudioContextManager> instance = nullptr;
}

class AudioBuffer;

class AudioSource
{
  public:
    AudioSource(const float gain = 1.0f, const bool looping = false);
    ~AudioSource();

    AudioSource(AudioSource&) = delete;
    AudioSource(const AudioSource&) = delete;
    AudioSource& operator=(AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;
    AudioSource(AudioSource&&) = delete;

    void play(const AudioBuffer& buffer);
    void stop();
    bool is_playing() const;

    void set_position(const float x, const float y, const float z);

  private:
    alure::Source data;
};

struct AudioBuffer
{
  public:
    friend AudioSource;
    AudioBuffer(const std::string& name);
    ~AudioBuffer();

    AudioBuffer(AudioBuffer&) = delete;
    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;
    AudioBuffer(AudioBuffer&&) = delete;

  private:
    alure::Buffer data;
};
}  // namespace audio
