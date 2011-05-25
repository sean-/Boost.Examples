#include <signal.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

// Simple heartbeat example with two timers being serviced by a thread pool
class heartbeat {
 public:
  heartbeat(boost::asio::io_service& io, uint32_t deadline, uint32_t heartbeat_sleep)
      : strand_(io), deadline_(io), heartbeat_(io), count_(0)
  {
    // Set the deadline timer in the future
    deadline_.expires_from_now(boost::posix_time::seconds(deadline));
    deadline_.async_wait(strand_.wrap(boost::bind(
        &heartbeat::deadline_expired, this,
        boost::asio::placeholders::error)));

    // And setup the heartbeat timer
    heartbeat_.expires_from_now(boost::posix_time::seconds(heartbeat_sleep));
    heartbeat_.async_wait(strand_.wrap(boost::bind(
        &heartbeat::heartbeat_check, this,
        boost::asio::placeholders::error,
        heartbeat_sleep)));
  }

  ~heartbeat() {
    std::cout << "Final heartbeat count: " << count_ << "\n";
  }

  // The deadline timer only fires when it's canceled by heartbeat_check() or
  // when it actually expires. Pretend that the deadline timer is a really
  // slow IO operation that can be canceled (e.g. imagine an async_read() or
  // async_write() timing out).
  void deadline_expired(const boost::system::error_code& e) {
    std::cout << "Deadline expired! ";
    if (e == boost::asio::error::operation_aborted) {
      std::cout << "Heartbeat canceled the deadline timer\n";
    } else {
      std::cout << "Deadline timer timed out. Canceling heartbeat\n";
      heartbeat_.cancel();
    }
  }

  // The heartbeat_check() runs every heartbeat_sleep seconds. If we've been
  // canceled by the deadline_ timer, then we quietly slink off in to the
  // night. If see that the deadline_ timer has expired but the deadline_
  // timer hasn't fired (for whatever goofy reason that won't happen in this
  // example), cancel the deadline_ timer. Or, schedule another heartbeat for
  // our fantastic little heartbeat_ self.
  void heartbeat_check(const boost::system::error_code& e, uint32_t heartbeat_sleep) {
    // If we were canceled by the deadline timer, do nothing
    if (e == boost::asio::error::operation_aborted) {
      std::cout << "Heartbeat check was canceled by the deadline timer\n";
      return;
    }

    // The heartbeat noticed the deadline timer has expired.
    if (deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
      std::cout << "Heartbeat says the deadline timer expired! Canceling deadline timer.\n";
      deadline_.cancel();

      // Don't schedule any more events so that io_service::run() returns and
      // threads join.
      return;
    }

    // This is neato to see how the thread pool operates on the io_service
    std::cout << "Heartbeat check #" << count_ << " from thread " << boost::this_thread::get_id() << "\n";
    ++count_;

    // Schedule another heartbeat
    heartbeat_.expires_from_now(boost::posix_time::seconds(heartbeat_sleep));
    heartbeat_.async_wait(strand_.wrap(boost::bind(
        &heartbeat::heartbeat_check, this,
        boost::asio::placeholders::error,
        heartbeat_sleep)));
  }

 private:
  boost::asio::strand strand_;
  boost::asio::deadline_timer deadline_;
  boost::asio::deadline_timer heartbeat_;
  int count_;
};

int
main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "timer <deadline> <heartbeat_interval>\n";
    return -1;
  }

  uint32_t deadline  = boost::lexical_cast<uint32_t>(argv[1]);
  uint32_t interval = boost::lexical_cast<uint32_t>(argv[2]);

  boost::asio::io_service io;
  heartbeat h(io, deadline, interval);

  std::cout << "Main thread has ID " << boost::this_thread::get_id() << std::endl;
  boost::thread_group threads;

  // Mask all signals. New threads inherit from their parent.
  sigset_t filled_mask, old_mask;
  sigfillset(&filled_mask);
  if (pthread_sigmask(SIG_SETMASK, &filled_mask, &old_mask) != 0) {
    std::cerr << "pthread_sigmask(2) failed:" << errno << "\n";
    return -1;
  }

  // Kick off 5 threads
  for (size_t i = 0; i < 5; ++i) {
    boost::thread* t = threads.create_thread(boost::bind(&boost::asio::io_service::run, &io));
    std::cout << "Creating thread " << i << " with id " << t->get_id() << std::endl;
  }

  // Restore the original signal mask.
  if (pthread_sigmask(SIG_SETMASK, &old_mask, NULL) != 0) {
    std::cerr << "pthread_sigmask(2) failed: " << errno << "\n";
    return -1;
  }

  // Don't run the io_service from the main thread.
#ifdef SERVER_MODE
  // Use a sane signal mask
  sigset_t wait_mask;
  sigemptyset(&wait_mask);
  sigaddset(&wait_mask, SIGINT);
  sigaddset(&wait_mask, SIGQUIT);
  sigaddset(&wait_mask, SIGTERM);
  sigaddset(&wait_mask, SIGTSTP);
  if (pthread_sigmask(SIG_SETMASK, &wait_mask, &old_mask) != 0) {
    std::cerr << "pthread_sigmask(2) failed: " << errno << "\n";
    return -1;
  }

  int sig = 0;
  if (::sigwait(&filled_mask, &sig) != 0)
    std::cerr << "sigwait error: " << errno << "\n";
  std::cout << "Caught signal #" << sig << ", waiting for threads to join.\n";

  // Restore the original signal mask in case another signal comes through.
  if (pthread_sigmask(SIG_SETMASK, &old_mask, NULL) != 0) {
    std::cerr << "pthread_sigmask(2) failed: " << errno << "\n";
    return -1;
  }

#else /* CLIENT MODE */
  // Clients terminate when they finish the task at hand.
  io.run();
#endif

  // Attempt to shutdown anyway
  io.stop();

  // Wait for all threads to join
  threads.join_all();

  return 0;
}
