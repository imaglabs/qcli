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
    // Context will call some protected functions
    friend class Context;
public:
    ~DeviceManager();

    /// Static instance method (thread safe in C++11)
    static DeviceManager& instance() {
        static DeviceManager inst;
        return inst;
    }

    /// Returns true if there was a problem in the constructor listing the devices
    bool initError() const { return _initError; }

    /// Returns the vector of selected devices
    QVector<cl_device_id> devices() const { QMutexLocker l(&_lock); return _devs; }
    /// Returns the number of selected devices
    int devCount() const { return _devsSelected; }
    /// Returns true if the index is a valid selected device index
    bool validId(int i) const { return i>=0 and i<_devsSelected; }

    /// Returns selected device, nullptr if the index is invalid
    cl_device_id device(int i) const { QMutexLocker l(&_lock); return validId(i) ? _devs[i] : nullptr; }
    /// Returns the queue of a selected device, nullptr if the index is invalid
    cl_command_queue queue(int i) const { QMutexLocker l(&_lock); return validId(i) ? _queues[i] : nullptr; }

    /// Returns the OpenCL platform object
    cl_platform_id platform() const { QMutexLocker l(&_lock); return _platform; }

    /// Disable copying
    DeviceManager(const DeviceManager& other) = delete;
    /// Disable assignments
    DeviceManager& operator=(const DeviceManager& other) = delete;

protected:
    /// Used by Context to selects the devices by type
    /// @retval false if there are no devices of this type or if the devices where already selected
    bool selectDevices(cl_device_type type= CL_DEVICE_TYPE_GPU);
    /// Used by Context to selects the devices by index (following the order of clGetDeviceIDs)
    /// @retval false if an index is out of bounds or if the devices where already selected
    bool selectDevices(QList<int> devIds);
    /// Used by Context to set the device queues after being initialized
    void setQueues(QVector<cl_command_queue> queues);

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
    QAtomicInt _devsSelected; // Number of selected devices, 0 when devs where not selected yet

    /// All the devices in the system
    QVector<cl_device_id> _allDevs;
    /// Devices selected to be used in the context
    QVector<cl_device_id> _devs;

    /// Device queues
    QVector<cl_command_queue> _queues;
};

/// Global function to access the DeviceManager
inline
DeviceManager& devMgr() { return DeviceManager::instance(); }

} // namespace QCLI

#endif // _QCLI_DEVICEMANAGER_H
