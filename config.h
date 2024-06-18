#ifndef __CORO_CONFIG_H__
#define __CORO_CONFIG_H__

#include <functional>
#include <memory>

namespace coro {

class ConfigVarBase {
   public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    const std::string& getName() const { return m_name; }

    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;

    virtual bool fromString(const std::string& val) = 0;

    virtual std::string getTypeName() const = 0;

   protected:
    std::string m_name;
    std::string m_description;
};

class ConfigVar : public ConfigVarBase {
   public:
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void(const T& old_value, const T& new_value)>
        on_change_cb;
};
}  // namespace coro

#endif