#pragma once

#include <map>

#include "resources/description_data.h"

namespace resources
{
std::map<std::string, ActorDescription> load_descriptions();
}  // namespace resources