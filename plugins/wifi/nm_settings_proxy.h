/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -N -p nm_settings_proxy.h -v nm-settings-introspection.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef NM_SETTINGS_PROXY_H_1402663916
#define NM_SETTINGS_PROXY_H_1402663916

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

typedef QMap<QString,QVariantMap> unholyVariantFuck;
Q_DECLARE_METATYPE(unholyVariantFuck)

/*
 * Proxy class for interface org.freedesktop.NetworkManager.Settings
 */
class OrgFreedesktopNetworkManagerSettingsInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.NetworkManager.Settings"; }

public:
    OrgFreedesktopNetworkManagerSettingsInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0)
        : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
        qDBusRegisterMetaType<unholyVariantFuck>();
    }

    ~OrgFreedesktopNetworkManagerSettingsInterface()
    {}

    Q_PROPERTY(bool CanModify READ canModify)
    inline bool canModify() const
    { return qvariant_cast< bool >(property("CanModify")); }

    Q_PROPERTY(QString Hostname READ hostname)
    inline QString hostname() const
    { return qvariant_cast< QString >(property("Hostname")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QDBusObjectPath> AddConnection(const QMap<QString, QVariantMap> &connection)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(connection);
        return asyncCallWithArgumentList(QLatin1String("AddConnection"), argumentList);
    }

    inline QDBusPendingReply<QDBusObjectPath> GetConnectionByUuid(const QString &uuid)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(uuid);
        return asyncCallWithArgumentList(QLatin1String("GetConnectionByUuid"), argumentList);
    }

    inline QDBusPendingReply<QList<QDBusObjectPath> > ListConnections()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("ListConnections"), argumentList);
    }

    inline QDBusPendingReply<> SaveHostname(const QString &hostname)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(hostname);
        return asyncCallWithArgumentList(QLatin1String("SaveHostname"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void NewConnection(const QDBusObjectPath &in0);
    void PropertiesChanged(const QVariantMap &in0);
};

#endif


