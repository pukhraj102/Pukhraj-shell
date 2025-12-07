#pragma once

#include "executor/executor.hpp"
#include <functional>
#include <memory>
#include <optional>

namespace executor {

  using BuiltinFn = std::function<void(redirect::Redirector& redirector, const ArgsList&)>;
  
  class BuiltinExecutor: public Executor {
  private:
    BuiltinFn m_fn;
  public:
    BuiltinExecutor(BuiltinFn fn);
    void exec(redirect::Redirector& redirector, const ArgsList& args) const override;
    void execAsync(redirect::Redirector& redirector, const ArgsList& args) const override;
  };

  std::optional<BuiltinExecutor> getBuiltin(const std::string& command);

  const std::unordered_map<std::string, BuiltinFn>& listBuiltins();

} // namespace executor