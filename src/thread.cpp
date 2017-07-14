#include <memory>
#include <functional>
#include <vector>
#include <utility>
#include <uv.h>

#include "thread.h"
#include "message.h"

using std::function;
using std::unique_ptr;
using std::vector;
using std::move;

void thread_callback_helper(void *arg)
{
  function<void()> *bound_fn = (std::function<void()>*) arg;
  (*bound_fn)();
}

void Thread::run()
{
  int err;

  err = uv_thread_create(&uv_handle, thread_callback_helper, &work_fn);
  report_uv_error(err);
}

void Thread::send(Message &&message)
{
  if (!is_healthy()) return;
  in.enqueue(move(message));
  wake();
}

unique_ptr<vector<Message>> Thread::receive_all()
{
  if (!is_healthy()) return nullptr;
  return out.accept_all();
}

void Thread::emit(Message &&message)
{
  if (!is_healthy()) return;
  out.enqueue(move(message));
}

unique_ptr<vector<Message>> Thread::process_all()
{
  if (!is_healthy()) return nullptr;
  return in.accept_all();
}

void Thread::wake()
{
  //
}