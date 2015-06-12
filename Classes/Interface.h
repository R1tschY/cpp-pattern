#ifndef CLASSES_INTERFACE_H_
#define CLASSES_INTERFACE_H_

class Interface {
public:
  // optional:
  template<typename Impl>
  using ImplicitImpl = InterfaceImply<Impl>;

  virtual ~Interface() { }

  virtual void one() = 0;
  virtual int two() = 0;
};

// Implementation
class Impl : public Interface {
public:
  void one() override { }
  int two() override { return 42; }
};

// optional:

// implicit implementation
template<typename Class>
class InterfaceImpl : public Interface {
public:
  using Impl = Class;

  InterfaceImpl(Class& impl) :
    impl_(impl)
  { }

  void one() override { impl_.one(); }
  int two() override { return impl_.two(); }

private:
  Class& impl_;
};

template<typename Interface, typename Impl, std::enable_if<!std::is_base_class<Interface, Impl>::value>::type = false>
Interface* GetInterface(Impl& impl)
{
  return &Interface::ImplicitImpl<Impl>(impl);
}

template<typename Interface, typename Impl, std::enable_if<std::is_base_class<Interface, Impl>::value>::type = false >
Interface* GetInterface(Impl& impl)
{
  return static_cast<Interface*>(&impl);
}

#endif /* CLASSES_INTERFACE_H_ */
