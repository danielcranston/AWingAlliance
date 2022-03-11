#pragma once

#include "urdf/parsing_utils.h"
#include "urdf/fighter_model.h"

namespace urdf
{
FighterModel parse_fighter_urdf(const std::string& filename);
}
