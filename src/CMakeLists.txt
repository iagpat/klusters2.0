project(klusters-src)

set(klusters_SRCS array.cpp autosavethread.cpp baseframe.cpp channelcolors.cpp
        channellist.cpp clusterPalette.cpp clusterinformationdialog.cpp
        clusterpaletteiconviewitem.cpp clusterpalettetooltip.cpp clustersprovider.cpp clusterview.cpp
        configuration.cpp correlationthread.cpp correlationview.cpp data.cpp dataprovider.cpp
        errormatrixthread.cpp errormatrixview.cpp eventsprovider.cpp groupingassistant.cpp
        itemcolors.cpp klusters.cpp klustersdoc.cpp klustersview.cpp klustersxmlreader.cpp
        main.cpp minmaxthread.cpp pair.cpp parameterxmlmodifier.cpp prefclusterview.cpp
        prefclusterviewlayout.ui prefdialog.cpp prefgeneral.cpp prefgenerallayout.ui
        prefwaveformview.cpp prefwaveformviewlayout.ui processlinemaker.cpp processwidget.cpp
        savethread.cpp sortabletable.cpp tags.cpp tracesprovider.cpp traceview.cpp
        tracewidget.cpp utilities.cpp viewwidget.cpp waveformthread.cpp waveformview.cpp
        zoomwindow.cpp )

add_executable(klusters ${klusters_SRCS} )


