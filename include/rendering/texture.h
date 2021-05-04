#pragma once

#include <memory>
#include <string>
#include <vector>

#include "resources/texture_data.h"

namespace rendering
{
class Texture
{
  public:
    enum class Type
    {
        TEXTURE,
        CUBEMAP
    };

    Texture(const resources::TextureData& data);
    Texture(const std::string& name, const std::array<resources::TextureData, 6>& data_vec);
    Texture(const std::string& name,
            const std::vector<unsigned char>& data,
            const int width,
            const int height,
            const int comp);
    ~Texture();

    // Disallow copy constructor and copy assignment
    Texture(Texture&) = delete;
    Texture(const Texture&) = delete;
    Texture& operator=(Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move constructor/assignment operators that correctly transferring ownership to avoid
    // premature deallocation. Relevant:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    // Also allow const versions, possible thanks to is_owning being mutable. Relevant:
    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    Texture(const Texture&&);
    Texture& operator=(const Texture&&);

    std::string name;

    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    int width;
    int height;
    int comp;
    unsigned int texture_id = 0;
    Type type;

  private:
    mutable bool is_owning = true;
};
}  // namespace rendering
