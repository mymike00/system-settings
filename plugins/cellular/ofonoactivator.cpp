/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ofonoactivator.h"

#include"nm_manager_proxy.h"
#include"nm_settings_proxy.h"
#include"nm_settings_connection_proxy.h"
#include<QCoreApplication>

typedef QMap<QString,QVariantMap> Vardict;
Q_DECLARE_METATYPE(Vardict)

namespace {

QString nmService("org.freedesktop.NetworkManager");
QString nmSettingsPath("/org/freedesktop/NetworkManager/Settings");
QString nmPath("/org/freedesktop/NetworkManager");

QDBusObjectPath detectConnection(const QString &ofonoContext, const QString imsi) {
    auto ofonoContextBase = ofonoContext.split('/').back();
    auto target = "/" + imsi + "/" + ofonoContextBase;

    OrgFreedesktopNetworkManagerSettingsInterface settings(nmService, nmSettingsPath,
            QDBusConnection::systemBus());
    auto reply = settings.ListConnections();
    reply.waitForFinished();
    if(!reply.isValid()) {
        qWarning() << "Error getting connection list: " << reply.error().message() << "\n";
    }
    auto connections = reply.value(); // Empty list if failed.

    for(const auto &c : connections) {
        OrgFreedesktopNetworkManagerSettingsConnectionInterface connProxy(nmService,
                c.path(), QDBusConnection::systemBus());
        auto reply2 = connProxy.GetSettings();
        reply2.waitForFinished();
        if(!reply2.isValid()) {
            qWarning() << "Error getting property: " << reply2.error().message() << "\n";
            continue;
        }
        auto settings = reply2.value();
        auto id = settings["connection"]["id"].toString();
        if(id == target) {
            return c;
        }
    }
    return QDBusObjectPath("");
}

QDBusObjectPath detectDevice(const QString &modemPath) {
    OrgFreedesktopNetworkManagerInterface nm(nmService, nmPath, QDBusConnection::systemBus());
    auto reply = nm.GetDevices();
    reply.waitForFinished();
    auto devices = reply.value();

    for(const auto &device : devices) {
        QDBusInterface iface(nmService, device.path(), "org.freedesktop.DBus.Properties",
                QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> ifaceReply = iface.call("Get",
                "org.freedesktop.NetworkManager.Device", "Interface");
        if(!ifaceReply.isValid()) {
            qWarning() << "Error getting property: " << ifaceReply.error().message() << "\n";
            continue;
        }
        auto devIface = ifaceReply.value().variant().toString();
        if(devIface == modemPath) {
            return device;
        }
    }
    return QDBusObjectPath("");
}
}

void activateOfono(QDBusObjectPath connection, QDBusObjectPath device)
{
    OrgFreedesktopNetworkManagerInterface nm(nmService, nmPath, QDBusConnection::systemBus());


    /// @bug https://bugs.launchpad.net/ubuntu/+source/network-manager/+bug/1378102
    /// This is a big and fat workaround for the above bug
    // this is probably racy as well as we are not tracking the ActiveConnection to know
    // when the Device has actually disconnected.
    QDBusInterface dev_iface("org.freedesktop.NetworkManager",
                             device.path(),
                             "org.freedesktop.NetworkManager.Device",
                             nm.connection());
    dev_iface.call("Disconnect");


    nm.ActivateConnection(connection, device, QDBusObjectPath("/"));
}

OfonoActivator::OfonoActivator(QObject *parent) : QObject(parent) {
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<Vardict>();
        isRegistered = true;
    }
}

Q_INVOKABLE bool OfonoActivator::activate(const QString ofonoContext, const QString imsi, const QString modemPath)
{
    auto dev = detectDevice(modemPath);
    if(dev.path() == "") {
        qWarning() << "Could not detect device object to use for Ofono activation.\n";
        return false;
    }
    auto conn = detectConnection(ofonoContext, imsi);
    if(conn.path() == "") {
        qWarning() << "Could not detect connection object to use for Ofono activation.\n";
        return false;
    }
    activateOfono(conn, dev);
    return true;
}