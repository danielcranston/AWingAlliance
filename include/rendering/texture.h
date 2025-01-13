#pragma once

#include <memory>
#include <string>
#include <vector>

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

    Texture(const std::string& uri, const Type = Type::TEXTURE);

    // Disallow copy constructor and copy assignment
    Texture(Texture&) = delete;
    Texture(const Texture&) = delete;
    Texture& operator=(Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move constructor transfers ownership to avoid early deallocation of the texture_id. Relevant:
    // https://www.khronos.org/opengl/wiki/Common_Mistakes#RAII_and_hidden_destructor_calls
    // Const versions are possible thanks to is_owning being mutable. Relevant:
    // https://www.reddit.com/r/cpp/comments/8wbeom/coding_guideline_avoid_const_member_variables/
    Texture(Texture&&);
    Texture& operator=(Texture&&);

    // TODO: Implement if needed
    Texture(const Texture&&) = delete;
    Texture& operator=(const Texture&&) = delete;

    ~Texture();

    std::string uri;
    Type type;

    int width;
    int height;
    int comp;
    unsigned int texture_id = 0;

  private:
    mutable bool is_owning = true;
};
}  // namespace rendering
