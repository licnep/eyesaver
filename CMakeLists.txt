# Project Needs a name ofcourse
project(plasma-eyesaver)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )
 
# We add our source code here
set(tutorial1_SRCS eyesaver.cpp)
 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_eyesaver ${tutorial1_SRCS})
target_link_libraries(plasma_applet_eyesaver 
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${PHONON_LIBS} ${KDE4_KIO_LIBRARY})
 
install(TARGETS plasma_applet_eyesaver
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-eyesaver.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}) 

install(FILES images/occhio2.svg
        DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
	