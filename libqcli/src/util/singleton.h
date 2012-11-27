/*
 *   Copyright (C) 2012 by the libQCLI authors (see AUTHORS)
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 */

#ifndef _QCLI_SINGLETON_H
#define _QCLI_SINGLETON_H

namespace QCLI {

/// \brief Singleton pattern template.
/// @param T should provide a public default constructor.

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

} // namespace QCLI

#endif // _QCLI_SINGLETON_H
