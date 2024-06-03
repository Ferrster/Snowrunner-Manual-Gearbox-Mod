#pragma once

#include <functional>
#include <string>

namespace smgm {
class Action {
 public:
  virtual bool Trigger() = 0;
  const std::string& GetDisplayName() const noexcept { return display_name_; }

 protected:
  Action() = default;
  Action(const std::string& name) : display_name_(name) {}

  std::string display_name_;
};

class BasicAction : public Action {
 public:
  using FunctionSigT = bool();
  BasicAction(const std::function<FunctionSigT>& action,
              const std::string& name = "unnamed")
      : Action(name), action(action) {}

  bool Trigger() override {
    if (!action) return false;

    return action();
  }

  std::function<FunctionSigT> action;
};
}  // namespace smgm
