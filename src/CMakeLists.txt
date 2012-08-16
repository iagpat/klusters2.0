project(klusters-src)

add_subdirectory(cursors)
add_subdirectory(icons)

set(klusters_SRCS array.cpp autosavethread.cpp baseframe.cpp channelcolors.cpp
        channellist.cpp clusterPalette.cpp clusterinformationdialog.cpp
        clusterpaletteiconviewitem.cpp clusterpalettetooltip.cpp clustersprovider.cpp clusterview.cpp
        configuration.cpp correlationthread.cpp correlationview.cpp data.cpp dataprovider.cpp
        errormatrixthread.cpp errormatrixview.cpp eventsprovider.cpp groupingassistant.cpp
        itemcolors.cpp klusters.cpp klustersdoc.cpp klustersview.cpp klustersxmlreader.cpp
        main.cpp minmaxthread.cpp pair.cpp parameterxmlmodifier.cpp prefclusterview.cpp
        prefdialog.cpp prefgeneral.cpp
        prefwaveformview.cpp processlinemaker.cpp processwidget.cpp
        savethread.cpp sortabletable.cpp tags.cpp tracesprovider.cpp traceview.cpp
        tracewidget.cpp utilities.cpp viewwidget.cpp waveformthread.cpp waveformview.cpp
        zoomwindow.cpp )

kde3_automoc(${klusters_SRCS})

set(klusters_UI prefwaveformviewlayout.ui prefgenerallayout.ui prefclusterviewlayout.ui ) 
qt4_wrap_ui(klusters_SRCS ${klusters_UI})

add_executable(klusters ${klusters_SRCS} )

target_link_libraries(klusters ${QT_LIBRARIES} ${LIBXML2_LIBRARIES})

install(PROGRAMS klusters DESTINATION bin)
