set(QML_SOURCES EntryComponent.qml
    EraseEverything.qml
    PageComponent.qml
    ResetAllSettings.qml
    ResetLauncherHome.qml
)

add_library(UbuntuResetPanel MODULE
  plugin.cpp
  reset.cpp
  plugin.h
  reset.h
  ${QML_SOURCES}
)

target_link_libraries(UbuntuResetPanel Qt5::Qml Qt5::Quick Qt5::DBus uss-accountsservice)

set(PLUG_DIR ${PLUGIN_PRIVATE_MODULE_DIR}/Ubuntu/SystemSettings/Reset)
install(TARGETS UbuntuResetPanel DESTINATION ${PLUG_DIR})
install(FILES qmldir DESTINATION ${PLUG_DIR})
install(FILES reset.settings DESTINATION ${PLUGIN_MANIFEST_DIR})
install(FILES settings-reset.svg DESTINATION ${PLUGIN_MANIFEST_DIR}/icons)
install(FILES ${QML_SOURCES} DESTINATION ${PLUGIN_QML_DIR}/reset)
