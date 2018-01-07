/// \file serviceregistry.h

#ifndef SERVICEREGISTRY_H_
#define SERVICEREGISTRY_H_

#include <string>
#include <map>
#include <memory>
#include <typeindex>

namespace Engine {

/// \brief
class ServiceRegistry
{
public:
  ServiceRegistry();

  template<class ServiceInterface>
  void registerService(const std::shared_ptr<ServiceInterface>& service);

  template<class ServiceInterface>
  std::shared_ptr<ServiceInterface> getService();

  // TODO: std::shared_ptr<ServiceInterface> findService(); // return value maybe null
  // TODO: hasService();
  // TODO: removeService();
  // TODO: getWeakServiceRef();

private:
  std::map<std::type_index, std::shared_ptr<void>> registry_;
};

template<class ServiceInterface>
void ServiceRegistry::registerService(const std::shared_ptr<ServiceInterface>& service)
{
  if (service)
    registry_[std::type_index(typeid(ServiceInterface))] = service;
  else
    registry_.erase(std::type_index(typeid(ServiceInterface)));
}

template<class ServiceInterface>
std::shared_ptr<ServiceInterface> ServiceRegistry::getService()
{
  // TODO: not found exception
  return std::static_pointer_cast<ServiceInterface>(registry_[std::type_index(typeid(ServiceInterface))]);
}

class ServiceFactory
{
public:
  ServiceFactory() { }

  template<class ServiceInterface>
  void registerService(std::function<std::shared_ptr<ServiceInterface>()> generator);

  template<class ServiceInterface>
  std::shared_ptr<ServiceInterface> getService();

private:
  struct Entry {
    std::shared_ptr<void> ptr;
    std::function<std::shared_ptr<void>()> generator;
  };

  std::map<std::type_index, std::shared_ptr<void>> registry_;
};

template<class ServiceInterface>
void ServiceFactory::registerService(
  std::function<std::shared_ptr<ServiceInterface>()> generator)
{
  if (!generator)
  {
    registry_.erase(std::type_index(typeid(ServiceInterface)));
    return;
  }

  auto& e = registry_[std::type_index(typeid(ServiceInterface))];
  e.generator = []() { return std::shared_ptr<void>(generator()); };
  if (e.ptr)
  {
    e.ptr = e.generator();
  }
}

template<class ServiceInterface>
std::shared_ptr<ServiceInterface> ServiceFactory::getService()
{
  // TODO: not found exception
  auto& e = registry_[std::type_index(typeid(ServiceInterface))];
  if (!e.ptr)
  {
    e.ptr = e.generator();
  }

  return std::static_pointer_cast<ServiceInterface>(e.ptr);
}

} // namespace Engine

#endif /* SERVICEREGISTRY_H_ */
