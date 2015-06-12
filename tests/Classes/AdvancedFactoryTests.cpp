#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <Classes/AdvancedFactory.h>
#include <string>
#include <memory>
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

std::unique_ptr<Resource> createHttpResource(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "http://"))
    return std::unique_ptr<Resource>{ new Resource("HTTP") };
  else
    return std::unique_ptr<Resource>{ nullptr };
}

Resource* createExHttpResource(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "http://"))
    return new Resource("HTTP");
  else
    return nullptr;
}

Resource* createExFtpResource(const Uri& uri)
{
  if (boost::algorithm::starts_with(uri, "ftp://"))
    return new Resource("FTP");
  else
    return nullptr;
}

}

BOOST_AUTO_TEST_SUITE(AdvancedFactoryTests)

BOOST_AUTO_TEST_CASE(test_simple) {
  AdvancedFactory<std::unique_ptr<Resource>, std::unique_ptr<Resource>(const Uri&)> factory;

  factory.registerCanidate(&createHttpResource);

  auto res = factory.create("http://sdfghjkl");
  BOOST_CHECK(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "HTTP");

  BOOST_CHECK(factory.create("dfghj://") == nullptr);
}

BOOST_AUTO_TEST_CASE(test_tricky) {
  AdvancedFactory<std::unique_ptr<Resource>, Resource*(const Uri&)> factory;
  factory.registerCanidate(&createExHttpResource);
  factory.registerCanidate(&createExFtpResource);

  auto res = factory.create("http://sdfghjkl");
  BOOST_CHECK(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "HTTP");

  res = factory.create("ftp://sdfghjkl");
  BOOST_CHECK(res != nullptr);
  BOOST_CHECK_EQUAL(res->getToken(), "FTP");

  BOOST_CHECK(factory.create("dfghj:asdfghjkl") == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
