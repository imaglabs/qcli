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

#include "devicemanager.h"

#include <cassert>
#include "util/utils.h"
#include "opencl/context.h"

namespace QCLI {

DeviceManager::DeviceManager()
{
    qDebug() << "Init start";
    // Get the platform information
    cl_int err= clGetPlatformIDs(1, &_platform, nullptr);
    if(checkCLError(err, "clGetPlatformIDs")) {
        _platform= nullptr;
        _initError.ref();
        return;
    }

    // List all the available devices
    _allDevs= devicesOfType(CL_DEVICE_TYPE_ALL);
    if(_allDevs.empty())
        _initError.ref();
    qDebug() << "Init OK";
}

DeviceManager::~DeviceManager()
{
    foreach(const auto& queue, _queues)
        clReleaseCommandQueue(queue);
}

QVector<cl_device_id> DeviceManager::devicesOfType(cl_device_type type)
{
    auto ret= QVector<cl_device_id>();
    if(_initError)
        return ret;

    QMutexLocker locker(&_lock);
    cl_int err;

    // Query number of devices of type type
    const int maxDevices= 32;
    cl_uint devCount;
    err= clGetDeviceIDs(_platform, type, maxDevices, nullptr, &devCount);
    if(checkCLError(err, "clGetDeviceIDs"))
        return ret;

    // Get devices IDs
    ret.resize(devCount);
    err= clGetDeviceIDs(_platform, CL_DEVICE_TYPE_ALL, maxDevices, ret.data(), &devCount);
    if(checkCLError(err, "clGetDeviceIDs"))
        ret.clear();

    return ret;
}

bool DeviceManager::selectDevices(cl_device_type type)
{
    if(_initError or _devsSelected)
        return false;

    // Get list of devices of this type (devicesOfType is already thread-safe)
    auto devsOfType= devicesOfType(type);
    // Find the indices of these devices
    QList<int> indices;
    foreach(const auto& dev, devsOfType) {
        const int index= _allDevs.indexOf(dev);
        if(index==-1) return false;
        indices << index;
    }

    // Select those devices
    return selectDevices(indices);
}

bool DeviceManager::selectDevices(QList<int> devIds)
{
    if(_initError or _devsSelected or devIds.empty())
        return false;

    QMutexLocker locker(&_lock);

    _devs.resize(devIds.count());
    for(int i=0; i<devIds.count(); i++) {
        // For each requested id...
        const int id= devIds[i];
        // ...make sure it is valid
        if(id >= _allDevs.count() or devIds.count(id)>1)
            return false;
        // Store the corresponding cl_device_id in _devs
        _devs[i]= _allDevs[id];
    }
    // Context will now call setQueues and only then the devices are marked as
    // selected (with _devsSelected)
    return true;
}

void DeviceManager::setQueues(QVector<cl_command_queue> queues)
{
    QMutexLocker locker(&_lock);
    assert(!_queues.count());
    assert(queues.count() == _devs.count());
    _queues= queues;
    // Now store the number of selected devices in an atomic int
    _devsSelected= _devs.count();
}


} // namespace QCLI
