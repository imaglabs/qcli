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

#ifndef _QCLI_DEVICEMANAGER_H
#define _QCLI_DEVICEMANAGER_H

#include <QtCore>
#include "device.h"

namespace QCLI {

/// Global functions to access the Context instance
DeviceManager& devMgr() { return DeviceManager::instance(); }

class DeviceManager
{
public:
    ~DeviceManager() { };

    /// Static instance method (thread safe in C++11)
    static DeviceManager& instance() {
        static DeviceManager inst;
        return inst;
    }

    /// Disable copies
    DeviceManager(const DeviceManager& other) = delete;
    /// Disable assignments
    DeviceManager& operator=(const DeviceManager& other) = delete;

private:
    /// Hide constructor
    DeviceManager() = default;

    QMutex _lock;

    QVector<Device> _devs;
};

} // namespace QCLI

#endif // _QCLI_DEVICEMANAGER_H
