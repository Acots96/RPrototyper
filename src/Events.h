#pragma once

#include <functional>
#include "Utils.h"


namespace Events
{
	using EventInt = std::function<void(int)>;
	using EventBool = std::function<void(bool)>;
	using EventMOId = std::function<void(Utils::MapObjectId)>;
	using EventProps = std::function<void(Utils::MapObjectProperties)>;
}

