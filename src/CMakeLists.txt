project(klusters-src)

set(klusters_SRCS array.cpp 
	autosavethread.cpp 
	baseframe.cpp 
	channelcolors.cpp
        channellist.cpp 
	clusterPalette.cpp 
	clusterinformationdialog.cpp
	clusterpaletteiconviewitem.cpp 
	#clusterpalettetooltip.cpp 
	clustersprovider.cpp 
	clusterview.cpp
        configuration.cpp 
	correlationthread.cpp 
	correlationview.cpp 
	data.cpp 
	dataprovider.cpp
	errormatrixthread.cpp 
	errormatrixview.cpp 
	eventsprovider.cpp 
	groupingassistant.cpp
        itemcolors.cpp 
	klusters.cpp 
	klustersdoc.cpp 
	klustersview.cpp 
	klustersxmlreader.cpp
	main.cpp 
	minmaxthread.cpp 
	pair.cpp 
	parameterxmlmodifier.cpp 
	prefclusterview.cpp
	#prefdialog.cpp 
	prefgeneral.cpp
	prefwaveformview.cpp 
	processlinemaker.cpp 
	processwidget.cpp
	savethread.cpp 
	sortabletable.cpp 
	tags.cpp 
	tracesprovider.cpp 
	traceview.cpp
        tracewidget.cpp 
	utilities.cpp 
	viewwidget.cpp 
	waveformthread.cpp 
	waveformview.cpp
        zoomwindow.cpp 
	prefgenerallayout.cpp 
	prefwaveformviewlayout.cpp 
	prefclusterviewlayout.cpp)

qt4_add_resources(klusters_SRCS klusters-cursors.qrc klusters-icons.qrc) 

set(klusters_UI prefwaveformviewlayout.ui prefgenerallayout.ui prefclusterviewlayout.ui ) 
qt4_wrap_ui(klusters_SRCS ${klusters_UI})

add_executable(klusters ${klusters_SRCS} )

target_link_libraries(klusters ${QT_LIBRARIES} ${LIBXML2_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTXML_LIBRARY} ${LIBKLUSTERSSHARED_LIBRARY})

install(PROGRAMS klusters DESTINATION bin)
