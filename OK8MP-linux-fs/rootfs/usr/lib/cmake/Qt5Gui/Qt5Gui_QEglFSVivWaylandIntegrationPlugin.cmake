
add_library(Qt5::QEglFSVivWaylandIntegrationPlugin MODULE IMPORTED)


_populate_Gui_plugin_properties(QEglFSVivWaylandIntegrationPlugin RELEASE "egldeviceintegrations/libqeglfs-viv-wl-integration.so" FALSE)

list(APPEND Qt5Gui_PLUGINS Qt5::QEglFSVivWaylandIntegrationPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_egldeviceintegrations Qt5::QEglFSVivWaylandIntegrationPlugin)
set_property(TARGET Qt5::QEglFSVivWaylandIntegrationPlugin PROPERTY QT_PLUGIN_TYPE "egldeviceintegrations")
set_property(TARGET Qt5::QEglFSVivWaylandIntegrationPlugin PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QEglFSVivWaylandIntegrationPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QEglFSVivWaylandIntegrationPlugin")
