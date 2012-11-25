#ifndef _QCLI_SINGLETON_H
#define _QCLI_SINGLETON_H

// Singleton pattern template

template<class T>
class Singleton
{
public:
    // Disable copy constructor
    Singleton(const Singleton&) = delete;
    // Disable singleton asignment
    void operator=(const Singleton&) = delete;

    // Get instance (thread safe in C++11)
    static T& instance() {
        static T inst;
        return inst;
    }

private:
    Singleton() = default;
};

#endif // _QCLI_SINGLETON_H
