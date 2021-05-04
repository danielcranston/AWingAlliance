#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace resources
{
struct TextureData
{
    TextureData(const std::string& name,
                const std::vector<unsigned char>&& data,
                const int width,
                const int height,
                const int comp)
      : name(name), data(std::move(data)), width(width), height(height), comp(comp)
    {
    }

    const std::string name;
    const std::vector<unsigned char> data;
    const int width;
    const int height;
    const int comp;
};
}  // namespace resources