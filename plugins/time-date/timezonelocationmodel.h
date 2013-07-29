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
 * Authors:
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#ifndef TIMEZONELOCATIONMODEL_H
#define TIMEZONELOCATIONMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QThread>

class TimeZoneLocationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TimeZoneLocationModel(QObject *parent = 0);
    ~TimeZoneLocationModel();

    enum Roles {
        TimeZoneRole = Qt::UserRole + 1,
        CityRole,
        CountryRole
    };

    struct TzLocation {
        bool operator<(const TzLocation &other) const
        {
            QString pattern("%1, %2");

            return pattern.arg(city).arg(country) <
                    pattern.arg(other.city).arg(other.country);
        }

        QString city;
        QString country;
        QString timezone;
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

public Q_SLOTS:
    void processModelResult(TzLocation);

private:
    QList<TzLocation> m_locations;
};

Q_DECLARE_METATYPE (TimeZoneLocationModel::TzLocation)

class TimeZoneFilterProxy: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TimeZoneFilterProxy(TimeZoneLocationModel *parent = 0);
};


class TimeZonePopulateWorker : public QThread
{
    Q_OBJECT

public:
    void run();

Q_SIGNALS:
    void resultReady(TimeZoneLocationModel::TzLocation);

private:
    void buildCityMap();

};

#endif // TIMEZONELOCATIONMODEL_H