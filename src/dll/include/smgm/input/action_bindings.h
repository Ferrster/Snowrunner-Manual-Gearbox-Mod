#pragma once

#include "smgm/input/actions.h"

namespace smgm::input {

std::function<void()> GetCallbackForAction(InputAction action);

}
