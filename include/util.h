#pragma once

template <typename Ret, typename... Param0>
class Callback {
 public:
  virtual Ret invoke(Param0... param0) = 0;
};

template <typename Ret, typename... Param0>
class StaticFunctionCallback : public Callback<Ret, Param0...> {
 private:
  Ret (*func_)(Param0...);

 public:
  StaticFunctionCallback(Ret (*func)(Param0...)) : func_(func) {}

  virtual Ret invoke(Param0... param0) { return (*func_)(param0...); }
};

template <typename Ret, typename T, typename Method, typename... Param0>
class MethodCallback : public Callback<Ret, Param0...> {
 private:
  void *object_;
  Method method_;

 public:
  MethodCallback(void *object, Method method)
      : object_(object), method_(method) {}

  virtual Ret invoke(Param0... param0) {
    T *obj = static_cast<T *>(object_);
    return static_cast<void *>((obj->*method_)(param0...));
  }
};

template <typename Ret, typename... Param0>
class Delegate {
 private:
  Callback<Ret, Param0...> *callback_;

 public:
  explicit Delegate(Ret (*func)(Param0...))
      : callback_(new StaticFunctionCallback<Ret, Param0...>(func)) {}

  template <typename T, typename Method>
  Delegate(T *object, Method method)
      : callback_(
            new MethodCallback<Ret, T, Method, Param0...>(object, method)) {}

  ~Delegate() { delete callback_; }

  Ret operator()(Param0... param0) { return callback_->invoke(param0...); }
};
