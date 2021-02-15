add_llvm_library( LLVMHookFuncsIdentify MODULE
  hook_fn.cpp

  PLUGIN_TOOL
  opt
)
