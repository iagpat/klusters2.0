# Install script for directory: /home/drew/Desktop/klusters_mod/klusters-2.0.0/doc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/klusters/HTML/en" TYPE FILE FILES
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c1059.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x782.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1498.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x808.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x730.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1725.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x368.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x638.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1739.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1204.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x972.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x108.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c916.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x178.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x539.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1280.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1572.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/index.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1154.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1342.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c835.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c41.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1459.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/a1717.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1391.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x482.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c1697.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x769.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/c50.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x1654.html"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/html/x705.html"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/klusters/HTML/en/Images" TYPE FILE FILES
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/update.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/new_cluster.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/MultipleProjections.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/delete_noise_tool.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/new_clusters.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/XCorrelation-Raw.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Waveform-Scale.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Overview.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/clusterview.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/delete_artefact.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/GroupingAssistant.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/time_tool.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Waveform-Overlay.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Tools.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/forwardCluster.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/delete_noise.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/clusterInformationWindow.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/zoom_tool.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/waveformview.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/group.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/TraceView.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Waveform-Mean.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/XCorrelation-Asymptote.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Parameters-Default.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/GroupingAssistant-Change.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/backCluster.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/delete_artefact_tool.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Klusters.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/grouping_assistant_update.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Parameters-TimeFrame.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/Parameters.png"
    "/home/drew/Desktop/klusters_mod/klusters-2.0.0/doc/en/Images/printer.png"
    )
endif()

