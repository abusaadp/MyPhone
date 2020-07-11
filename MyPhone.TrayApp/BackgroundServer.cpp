#include "pch.h"
#include "BackgroundServer.h"

static DeviceInformation DeviceInfo{nullptr};
static PhoneLineTransportDevice CallDevice{nullptr};
static BluetoothDevice BthDevice{nullptr};

static DeviceState _State = DeviceState::Disconnected;

task<bool> BackgroundServer::ConnectTo(DeviceInformation deviceInfo)
{
    CallDevice = PhoneLineTransportDevice::FromId(deviceInfo.Id());
    if (CallDevice == nullptr)
    {
        co_return false;
    }

    DeviceAccessStatus status = co_await CallDevice.RequestAccessAsync();
    if (status != DeviceAccessStatus::Allowed)
    {
        co_return false;
    }

    BthDevice = co_await BluetoothDevice::FromIdAsync(deviceInfo.Id());
    DeviceInfo = deviceInfo;
    if (!CallDevice.IsRegistered())
    {
        CallDevice.RegisterApp();
    }
    _State = DeviceState::Registered;

    bool success = co_await CallDevice.ConnectAsync();
    if (success)
    {
        _State = DeviceState::Connected;
    }

    co_return true;
}

enum DeviceState BackgroundServer::CurrentState()
{
    return _State;
}
