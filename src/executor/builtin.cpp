#include "executor/builtin.hpp"

namespace executor {
  using namespace executor;
  BuiltinExecutor::BuiltinExecutor(BuiltinFn fn): m_fn{fn} {};

  void BuiltinExecutor::exec(redirect::Redirector& redirector, const ArgsList& args) const {
    execAsync(redirector, args);
    redirector.Close();
  }

  void BuiltinExecutor::execAsync(redirect::Redirector& redirector, const ArgsList& args) const {
    m_fn(redirector, args);
  }

  extern void exitFn(redirect::Redirector& redirector, const ArgsList& args);
  extern void echoFn(redirect::Redirector& redirector, const ArgsList& args);
  extern void typeFn(redirect::Redirector& redirector, const ArgsList& args);
  extern void pwdFn(redirect::Redirector& redirector, const ArgsList& args);
  extern void cdFn(redirect::Redirector& redirector, const ArgsList& args);
  extern void historyFn(redirect::Redirector& redirect, const ArgsList& args);

  static const std::unordered_map<std::string, BuiltinFn> registry = {
    {"exit", exitFn},
    {"echo", echoFn},
    {"type", typeFn},
    {"pwd", pwdFn},
    {"cd", cdFn},
    {"history", historyFn}
  };

  std::optional<BuiltinExecutor> getBuiltin(const std::string& command) {
    auto it = registry.find(command);
    if (it == registry.end()) {
      return std::nullopt;
    }
    return it->second;
  }

  const std::unordered_map<std::string, BuiltinFn>& listBuiltins() {
    return registry;
  }

} // namespace executor