#include "ble.h"

namespace
{
    QBluetoothUuid wifiUuid()
    {
        return QUuid("f9eb3fae-947a-4e5b-ab7c-c799e91ed780");
    }
    QBluetoothUuid wifiPublishedUuid()
    {
        return QUuid("f9eb3fae-947a-4e5b-ab7c-c799e91ed781");
    }
    QBluetoothUuid wifiRequestUuid()
    {
        return QUuid("f9eb3fae-947a-4e5b-ab7c-c799e91ed782");
    }
}

/// default constructor
Ble::Ble()
{
    // five second discovery timeout
    search_.setLowEnergyDiscoveryTimeout(3000);
    QObject::connect(&search_, &QBluetoothDeviceDiscoveryAgent::finished, this, &Ble::searchComplete);
}

/// destructor
Ble::~Ble()
{
    search_.stop();
    disconnectFromProbe();
}

/// initiates ble search
void Ble::search()
{
    if (probe_)
        disconnectFromProbe();

    search_.stop();
    search_.start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/// called when ble search has completed
void Ble::searchComplete()
{
    const auto devs = search_.discoveredDevices();
    probes_.clear();
    QStringList probes;

    for (auto d : devs)
    {
        if (d.name().startsWith(QStringLiteral("CUS")))
        {
            auto name = d.name().remove(QStringLiteral("CUS-"));
            probes_.push_back(qMakePair(name, d));
            probes.push_back(name);
        }
    }

    emit devices(probes);
}

/// tries to connect to a probe over bluetooth
/// @param[in] name the name/serial of the probe to connect to
/// @return the success of the connection attempt
bool Ble::connectToProbe(const QString& name)
{
    if (name.isEmpty())
        return false;

    QBluetoothDeviceInfo dev;
    for (auto p : probes_)
    {
        if (p.first == name)
        {
            dev = p.second;
            break;
        }
    }

    if (!dev.isValid())
        return false;

    // don't try to connect again if already attempting
    if (probe_ && probe_->state() != QLowEnergyController::UnconnectedState)
        return false;

    emit powerReady(false);
    emit wifiReady(false);

    probe_.reset(QLowEnergyController::createCentral(dev, this));
    QObject::connect(probe_.get(), &QLowEnergyController::connected, this, &Ble::onConnected);
    QObject::connect(probe_.get(), &QLowEnergyController::serviceDiscovered, this, &Ble::onService);
    QObject::connect(probe_.get(), &QLowEnergyController::discoveryFinished, this, &Ble::onDiscoveryFinished);
    probe_->connectToDevice();
    return true;
}

/// disconnect from a connected probe
/// @return success of the call
bool Ble::disconnectFromProbe()
{
    if (!probe_ || probe_->state() != QLowEnergyController::ConnectedState)
        return false;

    probe_->disconnectFromDevice();
    return true;
}

/// called once the probe is connected
void Ble::onConnected()
{
    if (probe_)
    {
        emit powerReady(false);
        emit wifiReady(false);
        probe_->discoverServices();
    }
}

/// called when a new service is discovered
void Ble::onService(const QBluetoothUuid& u)
{
    if (!probe_)
        return;

    if (u == QBluetoothUuid::ImmediateAlert)
    {
        power_.reset(probe_->createServiceObject(u));
        QObject::connect(power_.get(), &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState s)
        {
            if (s == QLowEnergyService::ServiceDiscovered)
                emit powerReady(true);
        });
    }
    else if (u == wifiUuid())
    {
        wifi_.reset(probe_->createServiceObject(u));
        QObject::connect(wifi_.get(), &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState s)
        {
            auto emitWifi = [this](const QLowEnergyCharacteristic& ch, const QByteArray& v)
            {
                auto uid = ch.uuid();
                if (uid == wifiPublishedUuid() && !v.isEmpty())
                    emit wifiInfo(QString::fromUtf8(v));
            };
            QObject::connect(wifi_.get(), &QLowEnergyService::characteristicChanged, this, [emitWifi](const QLowEnergyCharacteristic& ch, const QByteArray& v)
            {
                emitWifi(ch, v);
            });
            QObject::connect(wifi_.get(), &QLowEnergyService::characteristicRead, this, [emitWifi](const QLowEnergyCharacteristic& ch, const QByteArray& v)
            {
                emitWifi(ch, v);
            });
            if (s == QLowEnergyService::ServiceDiscovered)
            {
                emit wifiReady(true);

                auto ch = wifi_->characteristic(wifiPublishedUuid());
                if (ch.isValid())
                {
                    // subscribe to notifications
                    auto de = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                    if (de.isValid())
                        wifi_->writeDescriptor(de, QByteArray::fromHex("0100"));
                    // engage a first read of the wifi info
                    wifi_->readCharacteristic(ch);
                }
            }
        });
    }
}

/// called when all services have been discovered
void Ble::onDiscoveryFinished()
{
    // silly workaround for windows, see: https://bugreports.qt.io/browse/QTBUG-78488
    QTimer::singleShot(0, [=] ()
    {
        if (power_)
            power_->discoverDetails();
    });
    QTimer::singleShot(0, [=] ()
    {
        if (wifi_)
            wifi_->discoverDetails();
    });
}

/// powers the probe on or off
/// @param[in] en set to true to power on, false to power off
bool Ble::power(bool en)
{
    if (!power_ || power_->state() != QLowEnergyService::ServiceDiscovered)
        return false;

    auto ch = power_->characteristic(QBluetoothUuid::AlertLevel);
    if (!ch.isValid())
        return false;

    power_->writeCharacteristic(ch, QByteArray::fromHex(en ? "02" : "01"), QLowEnergyService::WriteWithoutResponse);
    return true;
}

/// makes a wifi network change request
/// @param[in] info the wifi information
/// @return success of the request
bool Ble::requestWifi(const QString& info)
{
    if (!wifi_ || wifi_->state() != QLowEnergyService::ServiceDiscovered || info.isEmpty())
        return false;

    auto ch = wifi_->characteristic(wifiRequestUuid());
    if (!ch.isValid())
        return false;

    wifi_->writeCharacteristic(ch, QByteArray(info.toUtf8()));
    return true;
}