SUBDIRS = src map graphics sound

man_MANS = freedroid.6

EXTRA_DIST = $(man_MANS) mac-osx

dist-hook:
	rm -rf `find $(distdir) -name CVS`

## Win32 specific stuff follows here

win_txts = COPYING AUTHORS README Releasetext freedroid-man
win_dir = freedroid-$(VERSION)-win32
dist-win32: src/freedroid.exe
	mkdir -p $(win_dir)
	cp -r graphics map sound $(win_dir)
	cp src/freedroid.exe $(win_dir)
	nroff -man freedroid.6 > freedroid-man
	for i in $(win_txts); do \
	   cp $$i $(win_dir)/$$i.txt; \
	   emacs -batch -q -l unix2dos.el --visit=$(win_dir)/$$i.txt -f unix2dos;\
	done
	zip -r $(win_dir).zip $(win_dir)


## MacOS X stuff follows here (experimental and unfinished)

bundle_contents = FreeDroid.app/Contents
FreeDroid_bundle: src/freedroid
	mkdir -p $(bundle_contents)/MacOS
	mkdir -p $(bundle_contents)/Resources
	echo "FreeDroid Classic " > $(bundle_contents)/PkgInfo
	$(INSTALL_PROGRAM) $< $(bundle_contents)/MacOS/


## PS3 specific
pkg:	freedroid.pkg

PSL1GHT_BIN	= $(PSL1GHT)/host/bin

PKG_TITLE	= Freedroid Classic
PKG_APPID	= FDROID001
PKG_CONTENTID	= UP0001-$(PKG_APPID)_00-0000000000000000

PKG_ICON0	= $(srcdir)/ICON0.PNG
PKG_SFOXML	= $(PSL1GHT_BIN)/sfo.xml

CLEANFILES = freedroid.pkg freedroid.self

pkg_dir = ps3-pkg

freedroid.pkg: src/freedroid $(addprefix $(srcdir)/,$(EXTRA_DIST))
	mkdir -p $(pkg_dir)
	mkdir -p $(pkg_dir)/USRDIR
	cp $(PKG_ICON0) $(pkg_dir)/
	$(PSL1GHT_BIN)/make_self_npdrm $< $(pkg_dir)/USRDIR/EBOOT.BIN $(PKG_CONTENTID) >> /dev/null
	$(PSL1GHT_BIN)/sfo.py --title "$(PKG_TITLE)" --appid "$(PKG_APPID)" -f $(PKG_SFOXML) $(pkg_dir)/PARAM.SFO
	$(MAKE) datarootdir=$(CURDIR)/$(pkg_dir)/USRDIR install-data
	$(PSL1GHT_BIN)/pkg.py --contentid $(PKG_CONTENTID) $(pkg_dir)/ $@ >> /dev/null
	-rm -rf $(pkg_dir)

run: freedroid.self
	$(PSL1GHT_BIN)/ps3load $<

freedroid.self: src/freedroid
	ppu-strip $< -o freedroid.tmp
	$(PSL1GHT_BIN)/sprxlinker freedroid.tmp
	$(PSL1GHT_BIN)/fself.py freedroid.tmp $@
	-rm freedroid.tmp

