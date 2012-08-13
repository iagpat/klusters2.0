####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = delete_artefact_cursor.png delete_noise_cursor.png new_cluster_cursor.png new_clusters_cursor.png zoom_cursor.png 

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/klusters/pics/
	$(INSTALL_DATA) $(srcdir)/delete_artefact_cursor.png $(kde_datadir)/klusters/pics/delete_artefact_cursor.png
	$(mkinstalldirs) $(kde_datadir)/klusters/pics/
	$(INSTALL_DATA) $(srcdir)/delete_noise_cursor.png $(kde_datadir)/klusters/pics/delete_noise_cursor.png
	$(mkinstalldirs) $(kde_datadir)/klusters/pics/
	$(INSTALL_DATA) $(srcdir)/new_cluster_cursor.png $(kde_datadir)/klusters/pics/new_cluster_cursor.png
	$(mkinstalldirs) $(kde_datadir)/klusters/pics/
	$(INSTALL_DATA) $(srcdir)/new_clusters_cursor.png $(kde_datadir)/klusters/pics/new_clusters_cursor.png
	$(mkinstalldirs) $(kde_datadir)/klusters/pics/
	$(INSTALL_DATA) $(srcdir)/zoom_cursor.png $(kde_datadir)/klusters/pics/zoom_cursor.png

uninstall-local:
	-rm -f $(kde_datadir)/klusters/pics/delete_artefact_cursor.png
	-rm -f $(kde_datadir)/klusters/pics/delete_noise_cursor.png
	-rm -f $(kde_datadir)/klusters/pics/new_cluster_cursor.png
	-rm -f $(kde_datadir)/klusters/pics/new_clusters_cursor.png
	-rm -f $(kde_datadir)/klusters/pics/zoom_cursor.png

####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes)
picsdir = $(kde_datadir)/klusters/pics/
pics_DATA = delete_artefact_cursor.png delete_noise_cursor.png new_cluster_cursor.png new_clusters_cursor.png zoom_cursor.png select_time_cursor.png
