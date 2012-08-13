## Makefile.am for klusters

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src/cursors -I$(top_srcdir)/src/icons \
	$(LIBXML_CFLAGS) $(all_includes)

# the library search path.

# the libraries to link against.

# which sources should be compiled for klusters

# these are the headers for your project

# client stuff

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/klusters.pot

KDE_ICON = AUTO

# this is where the kdelnk file will go
# kdelnkdir = $(kde_appsdir)/Science
# kdelnk_DATA = klusters.desktop
xdg_apps_DATA = klusters.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/klusters
noinst_HEADERS = array.h autosavethread.h baseframe.h channelcolors.h \
	channellist.h clusterPalette.h clusterinformationdialog.h \
	clusterpaletteiconviewitem.h clusterpalettetooltip.h clustersprovider.h clusteruserinformation.h \
	clusterview.h configuration.h correlationthread.h correlationview.h data.h \
	dataprovider.h errormatrixthread.h errormatrixview.h eventsprovider.h \
	groupingassistant.h itemcolors.h klusters.h klustersdoc.h klustersview.h \
	klustersxmlreader.h minmaxthread.h pair.h parameterxmlmodifier.h prefclusterview.h \
	prefclusterviewlayout.h prefdialog.h prefgeneral.h prefgenerallayout.h prefwaveformview.h \
	prefwaveformviewlayout.h processlinemaker.h processwidget.h savethread.h sortabletable.h tags.h \
	timer.h tracesprovider.h traceview.h tracewidget.h types.h utilities.h \
	viewwidget.h waveformthread.h waveformview.h zoomwindow.h
rc_DATA = klustersui.rc
noinst_DATA = parameter.xsd
bin_PROGRAMS = klusters
SUBDIRS = cursors icons
klusters_SOURCES = array.cpp autosavethread.cpp baseframe.cpp channelcolors.cpp \
	channellist.cpp clusterPalette.cpp clusterinformationdialog.cpp \
	clusterpaletteiconviewitem.cpp clusterpalettetooltip.cpp clustersprovider.cpp clusterview.cpp \
	configuration.cpp correlationthread.cpp correlationview.cpp data.cpp dataprovider.cpp \
	errormatrixthread.cpp errormatrixview.cpp eventsprovider.cpp groupingassistant.cpp \
	itemcolors.cpp klusters.cpp klustersdoc.cpp klustersview.cpp klustersxmlreader.cpp \
	main.cpp minmaxthread.cpp pair.cpp parameterxmlmodifier.cpp prefclusterview.cpp \
	prefclusterviewlayout.ui prefdialog.cpp prefgeneral.cpp prefgenerallayout.ui \
	prefwaveformview.cpp prefwaveformviewlayout.ui processlinemaker.cpp processwidget.cpp \
	savethread.cpp sortabletable.cpp tags.cpp tracesprovider.cpp traceview.cpp \
	tracewidget.cpp utilities.cpp viewwidget.cpp waveformthread.cpp waveformview.cpp \
	zoomwindow.cpp
klusters_LDFLAGS = $(KDE_RPATH) $(LIBXML_LIBS) $(all_libraries)
klusters_LDADD = $(LIB_KDEPRINT) $(LIB_KFILE)
mimeTypesdir = $(kde_mimedir)/application
