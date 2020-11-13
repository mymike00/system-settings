/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authors: Iain Lane <iain.lane@canonical.com>
 *
*/

#ifndef CLICK_H
#define CLICK_H

#include <QAbstractTableModel>
#include <QDir>
#include <QObject>
#include <QSortFilterProxyModel>

class ClickModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ClickModel(QObject *parent = 0);
    ~ClickModel();

    Q_ENUMS(Roles)

    enum Roles {
        DisplayNameRole = Qt::DisplayRole,
        InstalledSizeRole = Qt::UserRole + 1,
        CacheSizeRole = Qt::UserRole + 2,
        ConfigSizeRole = Qt::UserRole + 3,
        DataSizeRole = Qt::UserRole + 4,
        AppTotalSizeRole = Qt::UserRole + 5,
        AppIdRole = Qt::UserRole + 6,
        VersionRole = Qt::UserRole + 7,
        IconRole
    };

    struct Click {
        QString name;
        QString displayName;
        QString icon;
        QString appId;
        QString version;
        uint installSize;
        uint cacheSize;
        uint configSize;
        uint dataSize;
        uint appTotalSize;
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    quint64 getClickSize() const;
    quint64 getBiggestAppTotalSize() const;
    quint64 getBiggestConfigSize() const;
    quint64 getBiggestCacheSize() const;
    quint64 getBiggestDataSize() const;
    quint64 getBiggestInstallSize() const;
    void refresh();

private:
    void populateFromDesktopOrIniFile(Click *newClick,
                                      QVariantMap hooks,
                                      QDir directory,
                                      QString name);
    Click buildClick(QVariantMap manifest);
    QList<Click> buildClickList();

    QList<Click> m_clickPackages;
    int m_totalClickSize;
    uint m_biggestAppTotalSize;
    uint m_biggestConfigSize;
    uint m_biggestCacheSize;
    uint m_biggestDataSize;
    uint m_biggestInstallSize;

};

Q_DECLARE_METATYPE (ClickModel::Roles)

class ClickFilterProxy: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ClickFilterProxy(ClickModel *parent = 0);

};

#endif // CLICK_H
