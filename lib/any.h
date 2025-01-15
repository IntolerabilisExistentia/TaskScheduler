#pragma once

#include <memory>

namespace my {

class TypeInfo {
private:
  const void* typeId;

  explicit TypeInfo(const void* id) : typeId(id) {}

public:
  bool operator==(const TypeInfo& other) const {
    return typeId == other.typeId;
  }

  bool operator!=(const TypeInfo& other) const {
    return typeId != other.typeId;
  }

  template <class T>
  static TypeInfo get() {
    static const int id = 0;
    return TypeInfo(&id);
  }
};

class any {
public:
  any() = default;

  template<class ValueType>
  any(const ValueType& other) {
    content_ = std::unique_ptr<Wrapper>(new TypedWrapper<ValueType>(other));
  }

  any(const any& other) {
    content_ = std::unique_ptr<Wrapper>(other.content_->clone());
  }

  template<class ValueType>
  any& operator=(const ValueType& other) {
    content_ = std::make_unique<TypedWrapper<ValueType>>(other);
    return *this;
  }

  any& operator=(any other) {
    content_.swap(other.content_);
    return *this;
  }

  ~any() {
    content_ = nullptr;
  }

  template<class ValueType>
  ValueType& get() {
    if (TypeInfo::get<ValueType>() == content_->type()) {
      return static_cast<TypedWrapper<ValueType>*>(content_.get())->cont_;
    }
    throw std::bad_cast();
  }

  const TypeInfo& type() const {
    return content_->type();
  }

private:
  class Wrapper {
  public:
    virtual ~Wrapper() = default;
    virtual const TypeInfo& type() const = 0;
    virtual Wrapper* clone() = 0;
  };

  template<class T>
  class TypedWrapper : public Wrapper {
  public:
    TypedWrapper(const T& value) : cont_(value) {}

    const TypeInfo& type() const override {
      return type_;
    }

    Wrapper* clone() override {
      return new TypedWrapper(cont_);
    }

    T cont_;
    TypeInfo type_ = TypeInfo::get<T>();
  };

  std::unique_ptr<Wrapper> content_ = nullptr;
};

} // namespace my
