/*
 * AdvancedFactory.h
 *
 *  Created on: 21.05.2015
 *      Author: richard
 */

#ifndef CLASSES_ADVANCEDFACTORY_H_
#define CLASSES_ADVANCEDFACTORY_H_

#include <functional>
#include <vector>
#include <algorithm>
#include <memory>
#include <exception>
//#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

template<typename Sig>
class AdvancedFactory;

template<typename Ret, typename ...Args>
class AdvancedFactory<Ret(Args...)> {
public:
  static_assert(std::is_constructible<bool, Ret>::value, "the return value have to be explicit convertible to bool");

  using CreatingFunc = Ret(*)(Args...); // optional: std::function<T(Args&&...)>;
  using ReturnType = Ret;
  using CallingArgs = std::tuple<Args...>;

  void registerCanidate(CreatingFunc creating_func/*, int priority*/)
  {
    registered_canidates_.push_back(Entry{creating_func/*, priority*/});
  }

  template<typename U>
  void registerCanidate()
  {
    registerCanidate(U::create /*, T::priority*/);
  }

  ReturnType create(Args... args)
  {
    for (auto&& entry : registered_canidates_) {
      ReturnType result = entry.creating_func(args...);
      if (result) return std::move(result);
    }

    return ReturnType();
  }

  template<typename Exception = std::invalid_argument>
  ReturnType createOrThrow(Args... args)
  {
    return createOr(args..., []() -> ReturnType {
      throw Exception("invalid argument to factory");
    });
  }

  ReturnType createOrDefault(Args... args, const ReturnType& dflt)
  {
    return createOr(args..., [&]() -> ReturnType {
      return dflt;
    });
  }

  template<typename Func>
  ReturnType createOr(Args... args, Func func)
  {
    for (auto&& entry : registered_canidates_) {
      auto&& canidate = entry.creating_func(args...);
      if (canidate)
        return std::move(canidate);
    }

    return func();
  }

  /*ReturnType create(Args&&... args)
  {
    using namespace boost::adaptors;

    auto iter = boost::max_element(
        registered_canidates_ |
        filtered([](const Entry& entry) -> bool { return entry.matching_func(args...); }),
        [](const Entry& entry) -> int { return entry.priority; }
    );

    if (iter == registered_canidates_.end()) {
      // throw ...
      return T();
    }

    return (*iter).creating_func(args...);
  }*/

private:
  struct Entry {
    CreatingFunc creating_func;
    //int priority;
  };

  std::vector<Entry> registered_canidates_;
};

// static factory
//
// >> factory.h:
// struct my_factory_tag { };
// typedef StaticAdvancedFactory<my_factory_tag, std::shared_ptr<Object>, const std::string&> MyFactory;
//
// >> factory_canidate1.h
// class FC1 : public Object, private FactoryCanidate<MyFactory, FC1> {
// public:
//   static bool match(const std::string& input) {
//     return input.starts_with("fc1://");
//   }
//
//   static std::shared_ptr<Object> create(const std::string& input) {
//     return make_shared<FC1>(input);
//   }
//
//   FC1(const std::string&);
// }
//
// >> main.cpp
// int main(int argc, const char** argv) {
//   std::shared_ptr<Object> obj = MyFactory::create(argv[1]);
//   return obj->id();
// }

template<typename FactoryTag, typename Sig>
class StaticAdvancedFactory;

template<typename FactoryTag, typename Ret, typename ...Args>
class StaticAdvancedFactory<FactoryTag, Ret(Args...)> {
  StaticAdvancedFactory() { }
public:
  using CreatingFunc = typename AdvancedFactory<Ret(Args...)>::CreatingFunc;

  static void registerCanidate(CreatingFunc creating_func/*, int priority*/)
  {
    get().registerCanidate(creating_func/*, priority*/);
  }

  template<typename U>
  static void registerCanidate()
  {
    get().registerCanidate(U::create /*, T::priority*/);
  }

  template<typename ...FuncArgs>
  Ret create(FuncArgs&&... args)
  {
    return get().create(std::forward<FuncArgs>(args)...);
  }

  static AdvancedFactory<Ret(Args...)>& get() {
    static AdvancedFactory<Ret(Args...)> instance;
    return instance;
  }
};

template<typename SAF, typename Class>
class FactoryCanidate {
private:
  static void registerMe() {
     SAF::get().registerCanidate(&Class::match, &Class::create /*, Class::priority*/);
  }

  struct init_tag_t {
    init_tag_t() {
      registerMe();
    }
  };
  static init_tag_t init_tag;

  // CRIP
  Class& self() { return *static_cast<Class*>(std::addressof(this)); }
  const Class& self() const { return *static_cast<const Class*>(std::addressof(this)); }
  const Class& cself() const { return self(); }
};

template<typename SAF, typename Class>
typename FactoryCanidate<SAF,Class>::init_tag_t FactoryCanidate<SAF,Class>::init_tag;

#endif /* CLASSES_ADVANCEDFACTORY_H_ */
