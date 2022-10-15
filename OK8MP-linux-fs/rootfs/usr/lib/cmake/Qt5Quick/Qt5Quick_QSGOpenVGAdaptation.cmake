
add_library(Qt5::QSGOpenVGAdaptation MODULE IMPORTED)


_populate_Quick_plugin_properties(QSGOpenVGAdaptation RELEASE "scenegraph/libqsgopenvgbackend.so" FALSE)

list(APPEND Qt5Quick_PLUGINS Qt5::QSGOpenVGAdaptation)
set_property(TARGET Qt5::Quick APPEND PROPERTY QT_ALL_PLUGINS_scenegraph Qt5::QSGOpenVGAdaptation)
set_property(TARGET Qt5::QSGOpenVGAdaptation PROPERTY QT_PLUGIN_TYPE "scenegraph")
set_property(TARGET Qt5::QSGOpenVGAdaptation PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QSGOpenVGAdaptation PROPERTY QT_PLUGIN_CLASS_NAME "QSGOpenVGAdaptation")
