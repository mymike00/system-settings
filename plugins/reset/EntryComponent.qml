/*
 * This file is part of system-settings
 *
 * Copyright (C) 2018 The UBports project
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

 import QtQuick 2.4
 import SystemSettings.ListItems 1.0 as SettingsListItems

 SettingsListItems.IconProgression {
     text: i18n.tr(model.displayName)
     iconSource: model.icon
 }
