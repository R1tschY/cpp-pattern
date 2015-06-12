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

template<typename T, typename Sig>
class AdvancedFactory;

template<typename T, typename Ret, typename ...Args>
class AdvancedFactory<T, Ret(Args...)> {
public:
  static_assert(std::is_constructible<T, Ret>::value, "");
  static_assert(std::is_constructible<bool, Ret>::value, "");

  using ValueType = T;
  using CreatingFunc = Ret(*)(Args...); // optional: std::function<T(Args&&...)>;

  void registerCanidate(CreatingFunc creating_func/*, int priority*/)
  {
    registered_canidates_.push_back(Entry{creating_func/*, priority*/});
  }

  template<typename U>
  void registerCanidate()
  {
    registerCanidate(U::create /*, T::priority*/);
  }

  T create(Args... args)
  {
    for (auto&& entry : registered_canidates_) {
      auto&& canidate = entry.creating_func(args...);
      if (canidate) {
        return T(std::move(canidate));
      }
    }

    return T();
  }
  
  T createOrThrow(Args... args)
  {
    for (auto&& entry : registered_canidates_) {
      auto&& canidate = entry.creating_func(args...);
      if (canidate) {
        return T(std::move(canidate));
      }
    }

    throw std::runtime_error();
  }
  
  T createOrDefault(Args... args, T dflt)
  {
    for (auto&& entry : registered_canidates_) {
      auto&& canidate = entry.creating_func(args...);
      if (canidate) {
        return T(std::move(canidate));
      }
    }

    return dflt;
  }
  
  template<typename Func>
  T createOrFallback(Args... args, Func func)
  {
    for (auto&& entry : registered_canidates_) {
      auto&& canidate = entry.creating_func(args...);
      if (canidate) {
        return T(std::move(canidate));
      }
    }

    return func();
  }

  /*T create(Args&&... args)
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


template<typename FactoryTag, typename T, typename ...Args>
class StaticAdvancedFactory {
  StaticAdvancedFactory() { }
public:
  using CreatingFunc = typename AdvancedFactory<T, Args...>::CreatingFunc;

  static void registerCanidate(CreatingFunc creating_func/*, int priority*/)
  {
    get().registerCanidate(creating_func/*, priority*/);
  }

  template<typename U>
  static void registerCanidate()
  {
    get().registerCanidate(U::create /*, T::priority*/);
  }

  T create(Args&&... args)
  {
    return get().create(std::forward<Args>(args)...);
  }

  static AdvancedFactory<T, Args...>& get() {
    static AdvancedFactory<T, Args...> instance;
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
