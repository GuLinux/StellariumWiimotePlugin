
INCLUDE_DIRECTORIES(
  ${STELLARIUM_BUILD_DIR}
  ${STELLARIUM_SOURCE_DIR}
  ${STELLARIUM_SOURCE_DIR}/core
  ${STELLARIUM_SOURCE_DIR}/core/external
  ${STELLARIUM_SOURCE_DIR}/core/modules
  ${STELLARIUM_SOURCE_DIR}/gui
  ${STELLARIUM_SOURCE_DIR}/scripting
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/gui
  $ENV{CMAKE_INCLUDE_PATH}
  )

LINK_DIRECTORIES(${STELLARIUM_BUILD_DIR}/src)

SET(WiimotePlugin_SRCS
  WiimotePlugin.hpp
  WiimotePlugin.cpp
  gui/WiimotePluginWindow.hpp
  gui/WiimotePluginWindow.cpp
  gui/StelDialogWiimotePlugin.hpp
  gui/StelDialogWiimotePlugin.cpp
    wiimote/cwiidconnectionworker.cpp
    wiimote/wiimote.cpp
    wiimote/wiimotemanager.cpp
    wiimote/bigdecimal.cpp
    wiimote/wiimote_messages.cpp
    wiimote/timeutils.cpp
    wiimote/angles.cpp
  )

# Add here all the .ui files
SET(WiimotePlugin_UIS
  gui/wiimotePluginDialogWindow.ui
  )
QT4_WRAP_UI(WiimotePlugin_UIS_H ${WiimotePlugin_UIS})

# Compile the resource files
SET(WiimotePlugin_RES
  ${CMAKE_SOURCE_DIR}/wiimotePlugin.qrc)
QT4_ADD_RESOURCES(WiimotePlugin_RES_CXX ${WiimotePlugin_RES})

# Add here all the files which require moc compilation
# that is, all the headers with SIGNAL/SLOTS/PLUGIN_INTERFACE etc..
SET(WiimotePlugin_MOC_HDRS
  gui/StelDialogWiimotePlugin.hpp
  gui/WiimotePluginWindow.hpp
  WiimotePlugin.hpp
  wiimote/wiimotemanager.h
  wiimote/wiimote.h
  wiimote/cwiidconnectionworker.h
  )

# After this call, WiimotePlugin_MOC_SRCS = moc_WiimotePlugin.cxx
QT4_WRAP_CPP(WiimotePlugin_MOC_SRCS ${WiimotePlugin_MOC_HDRS})

SET(extLinkerOption ${QT_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} ${ICONV_LIBRARIES} ${INTL_LIBRARIES}
    -lcln -lcwiid
)

############### For building the dynamic library ######################
IF(BUILD_DYNAMIC_LIB)
 ADD_LIBRARY(WiimotePlugin MODULE ${WiimotePlugin_SRCS} ${WiimotePlugin_MOC_SRCS} ${WiimotePlugin_RES_CXX} ${WiimotePlugin_UIS_H})

 IF(APPLE)
  FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
  MARK_AS_ADVANCED(OPENGL_LIBRARY)
  SET_TARGET_PROPERTIES(WiimotePlugin PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" SUFFIX ".dylib")
 ENDIF(APPLE)

 IF(WIN32)
  SET_TARGET_PROPERTIES(WiimotePlugin PROPERTIES LINK_FLAGS "-enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition" )
  SET(StelMain stelMain)
 ELSE(WIN32)
  SET(StelMain )
 ENDIF(WIN32)

 TARGET_LINK_LIBRARIES(WiimotePlugin ${StelMain} ${extLinkerOption})

 INSTALL(TARGETS WiimotePlugin DESTINATION "modules/${PACKAGE}")
ENDIF(BUILD_DYNAMIC_LIB)

############### For building the static library ######################
IF(BUILD_STATIC_LIB)
 ADD_LIBRARY(WiimotePlugin-static STATIC ${WiimotePlugin_SRCS} ${WiimotePlugin_MOC_SRCS} ${WiimotePlugin_RES_CXX} ${WiimotePlugin_UIS_H})
 SET_TARGET_PROPERTIES(WiimotePlugin-static PROPERTIES OUTPUT_NAME "WiimotePlugin")
 TARGET_LINK_LIBRARIES(WiimotePlugin-static ${StelMain} ${extLinkerOption})
 SET_TARGET_PROPERTIES(WiimotePlugin-static PROPERTIES COMPILE_FLAGS "-fPIC -DQT_STATICPLUGIN")
ENDIF(BUILD_STATIC_LIB)
