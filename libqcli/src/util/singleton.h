#ifndef _QCLI_SINGLETON_H
#define _QCLI_SINGLETON_H

/// \brief Singleton pattern template
/// @param T should provide a default constructor.

template<class T>
class Singleton
{
public:
    /// Copying is disabled
    Singleton(const Singleton&) = delete;
    /// Assignment is disabled
    void operator=(const Singleton&) = delete;

    /// Get instance (thread safe in C++11)
    static T& instance() {
        static T inst;
        return inst;
    }

private:
    /// Prevent Singleton to be instantiated
    Singleton() = default;
};

#endif // _QCLI_SINGLETON_H
