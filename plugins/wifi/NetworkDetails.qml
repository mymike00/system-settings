/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Phone 1.0
import Ubuntu.SystemSettings.Wifi 1.0

ItemPage {

    id: networkDetails
    property string networkName
    property string password
    property string lastUsed
    property string dbusPath

    title: i18n.tr("Network") + " '" + networkName  + "'"

    Column {

        anchors.fill: parent
        
        ListItem.Standard {
            id: lastLabel
            text: i18n.tr("Last connected")
            control: Label {
                id: lastField

                text: networkDetails.lastUsed
            }
        }

        ListItem.Standard {
            id: passwordLabel
            text: i18n.tr("Password")
            control: TextInput {
                id: passwordField
                readOnly: true
                text: networkDetails.password
                echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
            }
        }

        ListItem.Standard {
            text: i18n.tr("Show password")
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        Button {
            text : i18n.tr("Forget network")
            anchors.left: parent.left
            anchors.right: parent.right
            onClicked : {
                DbusHelper.forgetConnection(dbusPath)
                pageStack.pop()
                // Go back two steps so we don't have to update the model.
                // If the user goes back to network list, the model is rebuilt.
                pageStack.pop()
            }
        }
    }
}
