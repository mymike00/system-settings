/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <click.h>
#include <gio/gdesktopappinfo.h>
#include <gio/gio.h>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtGui/QIcon>

#include "click_applications_model.h"

#define LEGACY_PUSH_HELPER_DIR "/usr/lib/ubuntu-push-client/legacy-helpers/"
#define GSETTINGS_SCHEMA_ID "com.ubuntu.notifications.settings"
#define GSETTINGS_BASE_PATH "/com/ubuntu/NotificationSettings/"
#define GSETTINGS_ENABLE_NOTIFICATIONS_KEY "enable-notifications"
#define GSETTINGS_SOUNDS_NOTIFY_KEY "use-sounds-notifications"
#define GSETTINGS_VIBRATIONS_NOTIFY_KEY "use-vibrations-notifications"
#define GSETTINGS_BUBBLES_NOTIFY_KEY "use-bubbles-notifications"
#define GSETTINGS_LIST_NOTIFY_KEY "use-list-notifications"

ClickApplicationsModel::ClickApplicationsModel(QObject* parent)
    : QAbstractListModel(parent)
{
    populateFromLegacyHelpersDir();
    populateFromClickDatabase();
}

ClickApplicationsModel::~ClickApplicationsModel()
{
}

QHash<int, QByteArray> ClickApplicationsModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[DisplayName] = "displayName";
        roles[Icon] = "icon";
        roles[EnableNotifications] = "enableNotifications";
        roles[SoundsNotify] = "soundsNotify";
        roles[VibrationsNotify] = "vibrationsNotify";
        roles[BubblesNotify] = "bubblesNotify";
        roles[ListNotify] = "listNotify";
    }
    return roles;
}

int ClickApplicationsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_entries.count();
}

QVariant ClickApplicationsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const ClickApplicationEntry& entry = m_entries.at(index.row());
    switch (role) {
    case DisplayName:
        return entry.displayName;
    case Icon:
        return entry.icon;
    case EnableNotifications:
        return entry.enableNotifications;
    case SoundsNotify:
        return entry.soundsNotify;
    case VibrationsNotify:
        return entry.vibrationsNotify;
    case BubblesNotify:
        return entry.bubblesNotify;
    case ListNotify:
        return entry.listNotify;
    default:
        return QVariant();
    }
}

bool ClickApplicationsModel::setNotifyEnabled(int role, int idx, bool enabled)
{
    if (idx < 0 || idx >= rowCount()) {
        return false;
    }

    if (!saveNotifyEnabled(m_entries[idx], role, enabled)) {
        return false;
    }

    QVector<int> roles;
    roles << role;

    if (role != EnableNotifications) {
        if (!m_entries[idx].soundsNotify &&
            !m_entries[idx].vibrationsNotify &&
            !m_entries[idx].bubblesNotify &&
            !m_entries[idx].listNotify) {

            if (saveNotifyEnabled(m_entries[idx], EnableNotifications, false)) {
                roles << EnableNotifications;
            }
        }
    }

    Q_EMIT dataChanged(this->index(idx, 0), this->index(idx, 0), roles);
    return true;
}

bool ClickApplicationsModel::saveNotifyEnabled(ClickApplicationEntry& entry, int role, bool enabled)
{
    QString path = GSETTINGS_BASE_PATH;
 
    if (entry.appName.isEmpty()) {
        path = path + "dpkg/" + entry.pkgName + "/";
    } else {
        path = path + entry.pkgName + "/" + entry.appName + "/";
    }

    GSettings *settings = g_settings_new_with_path(GSETTINGS_SCHEMA_ID, path.toUtf8().constData());

    QString key;
    if (role == EnableNotifications) {
        if (entry.enableNotifications == enabled) {
            return false;
        }

        entry.enableNotifications = enabled;
        key = GSETTINGS_ENABLE_NOTIFICATIONS_KEY;

    } else if (role == SoundsNotify) {
        if (entry.soundsNotify == enabled) {
            return false;
        }

        entry.soundsNotify = enabled;
        key = GSETTINGS_SOUNDS_NOTIFY_KEY;

    } else if (role == VibrationsNotify) {
        if (entry.vibrationsNotify == enabled) {
            return false;
        }

        entry.vibrationsNotify = enabled;
        key = GSETTINGS_VIBRATIONS_NOTIFY_KEY;

    } else if (role == BubblesNotify) {
        if (entry.bubblesNotify == enabled) {
            return false;
        }

        entry.bubblesNotify = enabled;
        key = GSETTINGS_BUBBLES_NOTIFY_KEY;
    
    } else if (role == ListNotify) {
        if (entry.listNotify == enabled) {
            return false;
        }

        entry.listNotify = enabled;
        key = GSETTINGS_LIST_NOTIFY_KEY;

    } else {
        return false;
    }

    if (!g_settings_set_boolean(settings, key.toUtf8().constData(), enabled)) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to store notification settings";
        return false;
    }

    g_object_unref(settings);
    return true;
}

void ClickApplicationsModel::addClickApplicationEntry(const ClickApplicationEntry& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << entry;
    endInsertRows();
}

void ClickApplicationsModel::getApplicationDataFromDesktopFile(ClickApplicationEntry& entry)
{
    QString desktopFile = entry.pkgName + ".desktop";
    if (!entry.appName.isEmpty() && !entry.version.isEmpty()) {
        desktopFile = entry.pkgName + "_" + entry.appName + "_" + entry.version + ".desktop";
    }

    GAppInfo* appInfo = (GAppInfo*)g_desktop_app_info_new(desktopFile.toUtf8().constData());
    if (appInfo == nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to get desktop file:" << desktopFile;
        return;
    }

    entry.displayName = g_strdup(g_app_info_get_display_name(appInfo));
    GIcon* icon = g_app_info_get_icon(appInfo);
    if (icon != nullptr) {
        QString iconPath = g_icon_to_string(icon);
        entry.icon = iconPath;
    }

    g_object_unref(appInfo);
}

void ClickApplicationsModel::getNotificationsSettings(ClickApplicationEntry& entry)
{
    QString path = GSETTINGS_BASE_PATH;
 
    if (entry.appName.isEmpty()) {
        path = path + "dpkg/" + entry.pkgName + "/";
    } else {
        path = path + entry.pkgName + "/" + entry.appName + "/";
    }

    GSettings *settings = g_settings_new_with_path(GSETTINGS_SCHEMA_ID, path.toUtf8().constData());

    entry.enableNotifications = g_settings_get_boolean(settings, GSETTINGS_ENABLE_NOTIFICATIONS_KEY);
    entry.soundsNotify = g_settings_get_boolean(settings, GSETTINGS_SOUNDS_NOTIFY_KEY);
    entry.vibrationsNotify = g_settings_get_boolean(settings, GSETTINGS_VIBRATIONS_NOTIFY_KEY);
    entry.bubblesNotify = g_settings_get_boolean(settings, GSETTINGS_BUBBLES_NOTIFY_KEY);
    entry.listNotify = g_settings_get_boolean(settings, GSETTINGS_LIST_NOTIFY_KEY);
    
    g_object_unref(settings);
}

void ClickApplicationsModel::populateFromLegacyHelpersDir()
{
    QDirIterator it(LEGACY_PUSH_HELPER_DIR, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        QFileInfo fileInfo(it.next());

        ClickApplicationEntry entry;
        entry.pkgName = fileInfo.fileName();

        getApplicationDataFromDesktopFile(entry);
        getNotificationsSettings(entry);
        addClickApplicationEntry(entry);
    }
}

bool ClickApplicationsModel::clickManifestHasPushHelperHook(const QVariantMap& manifest)
{
    QVariant hooksVar(manifest.value("hooks"));
    if (!hooksVar.isValid()) {
        return false;
    }

    QVariantMap hooksMap(hooksVar.toMap());
    QMapIterator<QString, QVariant> it(hooksMap);
    while (it.hasNext()) {
        it.next();
        QVariant hookVar(it.value());

        if (hookVar.isValid()) {
            QVariantMap hookMap(hookVar.toMap());
            if (hookMap.keys().contains("push-helper")) {
                return true;
            }
        }
    }

    return false;
}

QString ClickApplicationsModel::getApplicationNameFromDesktopHook(const QVariantMap& manifest)
{
    QVariant hooksVar(manifest.value("hooks"));
    if (!hooksVar.isValid()) {
        return QString();
    }

    QVariantMap hooksMap(hooksVar.toMap());
    QMapIterator<QString, QVariant> it(hooksMap);
    while (it.hasNext()) {
        it.next();
        QVariant hookVar(it.value());

        if (hookVar.isValid()) {
            QVariantMap hookMap(hookVar.toMap());
            if (hookMap.keys().contains("desktop")) {
                return it.key();
            }
        }
    }

    return QString();
}

void ClickApplicationsModel::populateFromClickDatabase()
{
    ClickDB *clickdb = click_db_new();

    GError *err = nullptr;
    click_db_read(clickdb, nullptr, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to read click packages db:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    gchar *clickmanifest = click_db_get_manifests_as_string(clickdb, FALSE, &err);
    if (err != nullptr) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to get click packages manifest:" << err->message;
        g_error_free(err);
        g_object_unref(clickdb);
        return;
    }

    g_object_unref(clickdb);

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(clickmanifest, &jsonError);

    g_free(clickmanifest);

    if (jsonError.error != QJsonParseError::NoError) {
        qWarning() << Q_FUNC_INFO << "[ERROR] Unable to parser data from click packages manifest:" << jsonError.errorString();
        return;
    }

    QJsonArray data(jsonDoc.array());
    QJsonArray::ConstIterator it;
    for (it = data.constBegin(); it != data.constEnd(); ++it) {
        QVariantMap manifest = (*it).toObject().toVariantMap();

        if (!clickManifestHasPushHelperHook(manifest)) {
            continue;
        }

        ClickApplicationEntry entry;

        entry.pkgName = manifest.value("name").toString();
        entry.version = manifest.value("version").toString();
        entry.appName = getApplicationNameFromDesktopHook(manifest);

        entry.displayName = manifest.value("title").toString();
        entry.icon = manifest.value("icon").toString();

        getApplicationDataFromDesktopFile(entry);
        getNotificationsSettings(entry);
        addClickApplicationEntry(entry);
    }
}
