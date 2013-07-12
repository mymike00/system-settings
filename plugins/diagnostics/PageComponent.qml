/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>,
 *          Evan Dandrea <evan.dandrea@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Diagnostics 1.0

ItemPage {
    id: root

    title: i18n.tr("Diagnostics")
    flickable: scrollWidget

    UbuntuDiagnostics {
        id: diagnosticsWidget
        function maybeUpdate() {
            reportCrashesCheck.checked = diagnosticsWidget.canReportCrashes
        }
        onReportCrashesChanged: maybeUpdate()
    }
    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Header {
                text: i18n.tr("Report to Canonical:")
            }

            DiagnosticsCheckEntry {
                id: reportCrashesCheck
                checked: diagnosticsWidget.canReportCrashes
                onCheckedChanged: {
                    diagnosticsWidget.setReportCrashes(checked);
                    /* Confirm the setting stuck and reflect it in the UI. */
                    if (checked != diagnosticsWidget.canReportCrashes) {
                        checked = !checked;
                    }

                }
                textEntry: i18n.tr("App crashes and errors")
            }

            ListItem.SingleValue {
                text: i18n.tr("Previous error reports")
                progression: true
            }

            ListItem.Caption {
                text: i18n.tr("Includes info about what an app was doing when it failed.")
            }
        }
    }
}
