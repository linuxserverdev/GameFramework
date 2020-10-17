#pragma once

#include "NonCopyable.h"
#include <vector>
#include <memory>
#include <type_traits>

template <typename T>
class ObjectPool : public NonCopyable,
                   public std::enable_shared_from_this<ObjectPool<T>>
{
  public:
    std::shared_ptr<T> getObject()
    {
        static_assert(!std::is_pointer<T>::value,
                      "The parameter type of the ObjectPool template can't be "
                      "pointer type");
        T *p{nullptr};
        {
            if (!objs_.empty())
            {
                p = objs_.back();
                objs_.pop_back();
            }
        }

        if (p == nullptr)
        {
            p = new T;
        }

        assert(p);
        std::weak_ptr<ObjectPool<T>> weakPtr = this->shared_from_this();
        auto obj = std::shared_ptr<T>(p, [weakPtr](T *ptr) {
            auto self = weakPtr.lock();
            if (self)
            {
                self->objs_.push_back(ptr);
            }
            else
            {
                delete ptr;
            }
        });
        return obj;
    }

  private:
    std::vector<T *> objs_;
};