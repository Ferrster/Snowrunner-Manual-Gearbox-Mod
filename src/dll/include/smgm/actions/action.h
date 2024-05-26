#pragma once

#include <functional>

namespace smgm {
class Action {
 public:
  Action() = default;
  virtual bool Trigger() = 0;
};

class BasicAction : public Action {
 public:
  using FunctionSigT = bool();
  BasicAction(const std::function<FunctionSigT>& action) : action(action) {}

  bool Trigger() override {
    if (!action) return false;

    return action();
  }

  std::function<FunctionSigT> action;
};
}  // namespace smgm
