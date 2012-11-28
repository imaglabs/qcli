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
#include <CL/cl.h>

namespace QCLI {

class DeviceManager
{
public:
    ~DeviceManager() { };

    /// Static instance method (thread safe in C++11)
    static DeviceManager& instance() {
        static DeviceManager inst;
        return inst;
    }

    /// Returns true if there was a problem in the constructor listing the devices
    bool initError() const { return _initError; }

    /// Selects the used devices by type
    /// @retval false if there are no devices of this type or if the devices where already selected
    bool selectDevices(cl_device_type type= CL_DEVICE_TYPE_GPU);
    /// Selects the used devices from a list of indices (following the order of clGetDeviceIDs)
    /// @retval false if an index is out of bounds or if the devices where already selected
    bool selectDevices(QList<int> devIds);

    /// Returns the OpenCL platform object
    cl_platform_id platform() const { QMutexLocker l(&_lock); return _platform; }

    /// Returns the vector of selected devices
    QVector<cl_device_id> devices() const { QMutexLocker l(&_lock); return _devs; }

    /// Disable copying
    DeviceManager(const DeviceManager& other) = delete;
    /// Disable assignments
    DeviceManager& operator=(const DeviceManager& other) = delete;

private:
    /// Hide constructor
    DeviceManager();

    /// Returns the devices of the requested type (empty vector on error)
    /// Use CL_DEVICE_TYPE_ALL to get all devices
    QVector<cl_device_id> devicesOfType(cl_device_type type);

    // OpenCL
    cl_platform_id _platform= nullptr;
    // State
    mutable QMutex _lock; // Mutable so it can be used in const getters
    QAtomicInt _initError;
    QAtomicInt _devsSelected;

    /// All the devices in the system
    QVector<cl_device_id> _allDevs;
    /// Devices selected to be used in the context
    QVector<cl_device_id> _devs;
};

/// Global function to access the DeviceManager
inline
DeviceManager& devMgr() { return DeviceManager::instance(); }

} // namespace QCLI

#endif // _QCLI_DEVICEMANAGER_H
