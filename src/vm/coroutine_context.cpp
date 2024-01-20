#include "coroutine_context.h"

bytes FraktalVMContext::run() {
  bytes ret;
  bool done = false;
  while(!done) {
    ret = CallContext::run();

    // TODO: Break on non-stoptoken error
    done = nextCoroutine();
  }

  return ret;
}
