#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <Classes/AdvancedFactory.h>
#include <string>
#include <memory>
#include <boost/optional.hpp>
#include "../extentions.h"

namespace {

class Resource {
public:
  Resource(const std::string& token) :
    token_(token)
  { }

  std::string getToken() const { return token_; }

private:
  std::string token_;
};

using Uri = std::string;

std::unique_ptr<Resource> createExHttpResource(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "http://"))
    return std::unique_ptr<Resource>(new Resource("HTTP"));
  else
    return std::unique_ptr<Resource>();
}

std::unique_ptr<Resource> createExFtpResource(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "ftp://"))
    return std::unique_ptr<Resource>(new Resource("FTP"));
  else
    return std::unique_ptr<Resource>();
}

int createHTTP_ID(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "http://"))
    return 0x4779;
  else
    return 0;
}

int createFTP_ID(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "ftp://"))
    return 0xF79;
  else
    return 0;
}

}

BOOST_AUTO_TEST_SUITE(AdvancedFactoryTests)

BOOST_AUTO_TEST_CASE(test_create) {
  AdvancedFactory<std::unique_ptr<Resource>(const Uri&)> factory;
  factory.registerCanidate(&createExHttpResource);
  factory.registerCanidate(&createExFtpResource);

  auto res = factory.create("http://sdfghjkl");
  BOOST_REQUIRE(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "HTTP");

  res = factory.create("ftp://sdfghjkl");
  BOOST_REQUIRE(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "FTP");

  BOOST_CHECK(factory.create("dfghj:asdfghjkl") == nullptr);

  // create or throw
  BOOST_CHECK_NO_THROW(res = factory.createOrThrow("http://sdfghjkl"));
  BOOST_REQUIRE(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "HTTP");
  BOOST_CHECK_THROW(factory.createOrThrow("dfghj:asdfghjkl") == nullptr, std::invalid_argument);
  BOOST_CHECK_THROW(factory.createOrThrow<std::logic_error>("dfghj:asdfghjkl") == nullptr, std::logic_error);

  // create or fallback
  BOOST_CHECK_NO_THROW(res = factory.createOr("http://sdfghjkl", []() -> std::unique_ptr<Resource> {
    BOOST_ERROR("Fallback function should not be called when result is found.");
    return std::unique_ptr<Resource>();
  }));
  BOOST_REQUIRE(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "HTTP");
  BOOST_CHECK(factory.createOr("dfghj:asdfghjkl", []() -> std::unique_ptr<Resource> {
    return std::unique_ptr<Resource>(nullptr);
  }) == nullptr);
}

BOOST_AUTO_TEST_CASE(test_create_int) {
  AdvancedFactory<int(const Uri&)> factory;
  factory.registerCanidate(&createHTTP_ID);
  factory.registerCanidate(&createFTP_ID);

  BOOST_CHECK_EQUAL(factory.create("http://sdfghjkl"), 0x4779);
  BOOST_CHECK_EQUAL(factory.create("ftp://sdfghjkl"), 0xF79);
  BOOST_CHECK_EQUAL(factory.create("dfghj:asdfghjkl"), 0);

  // create or throw
  int res;
  BOOST_CHECK_NO_THROW(res = factory.createOrThrow("http://sdfghjkl"));
  BOOST_CHECK_EQUAL(res, 0x4779);
  BOOST_CHECK_THROW(factory.createOrThrow("dfghj:asdfghjkl"), std::invalid_argument);
  BOOST_CHECK_THROW(factory.createOrThrow<std::logic_error>("dfghj:asdfghjkl"), std::logic_error);

  // create or default
  BOOST_CHECK_EQUAL(factory.createOrDefault("http://sdfghjkl", 0xABC), 0x4779);
  BOOST_CHECK_EQUAL(factory.createOrDefault("//4567olkjhgf://sdfghjkl", 0xABC), 0xABC);

  // create or fallback
  BOOST_CHECK_EQUAL(factory.createOr("dfghj:asdfghjkl", []() -> int {
    return 666;
  }), 666);
}

BOOST_AUTO_TEST_SUITE_END()
