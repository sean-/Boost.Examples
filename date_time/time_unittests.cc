#include <ctime>
#include <iostream>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "boost/date_time/microsec_time_clock.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/format.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using boost::format;



BOOST_AUTO_TEST_CASE( simple ) {
  using ::boost::posix_time::ptime;
  using ::boost::posix_time::time_duration;
  typedef ::boost::date_time::microsec_clock< ptime > msecc_t;

  ptime t = msecc_t::universal_time();
  ptime cur_time = boost::posix_time::from_time_t(::time(0));
  time_duration td = cur_time - t;

  BOOST_CHECK_EQUAL(td.seconds(), 0);
}



BOOST_AUTO_TEST_CASE( epoc ) {
  using ::boost::posix_time::ptime;
  using ::boost::posix_time::time_duration;
  typedef ::boost::date_time::microsec_clock< ptime > msecc_t;

  ptime t = msecc_t::universal_time();
  ptime time_t_epoc = boost::posix_time::from_time_t(0);
  ptime cur_time = boost::posix_time::from_time_t(::time(0));

  time_duration td1 = t - time_t_epoc;
  time_duration td2 = cur_time - time_t_epoc;
  BOOST_CHECK_EQUAL(td1.total_seconds(), td2.total_seconds());
  BOOST_CHECK_EQUAL(td1.total_seconds(), ::time(0));
  std::cout << format("epoc: %1%.%2%") % td1.total_seconds() % td1.fractional_seconds() << std::endl;
}



BOOST_AUTO_TEST_CASE( epoc_date ) {
  using ::boost::posix_time::ptime;
  using ::boost::posix_time::time_duration;
  typedef ::boost::date_time::microsec_clock< ptime > msecc_t;

  ptime t = msecc_t::universal_time();
  ptime time_t_epoc = boost::posix_time::from_time_t(0);
  ptime friday_time = boost::posix_time::from_time_t(1311376273);

  time_duration td = friday_time - time_t_epoc;
  BOOST_CHECK_EQUAL(td.total_seconds() / 86400, 15177);
  std::cout << format("days from epoc to time(1311376273): %1%")
      % (td.total_seconds() / 86400)
            << std::endl;
}



BOOST_AUTO_TEST_CASE( epoc_date_duration ) {
  using namespace ::boost::posix_time;
  using namespace ::boost::gregorian;
  using ::boost::posix_time::ptime;
  typedef ::boost::date_time::microsec_clock< ptime > msecc_t;

  ptime t = msecc_t::universal_time();
  date epoc_day(from_time_t(0).date());
  date friday_day(boost::posix_time::from_time_t(1311376273).date());

  date_duration day_diff = friday_day - epoc_day;
  BOOST_CHECK_EQUAL(day_diff.days(), 15177);
  std::cout << format("days from epoc to time(1311376273): %1%")
      % day_diff.days() << std::endl;
}
