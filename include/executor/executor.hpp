#pragma once

#include "redirect/redirector.hpp"

#include <string>
#include <vector>

namespace executor {

  using ArgsList = std::vector<std::string>;

  class Executor {
  public:
    virtual ~Executor() = default;
    virtual void exec(redirect::Redirector& redirector, const ArgsList& args) const = 0;
    virtual void execAsync(redirect::Redirector& redirector, const ArgsList& args) const = 0;
  };

} // namespace executor
