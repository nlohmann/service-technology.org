all: sara-1.11-unreleased lola-2.0-unreleased

sara-1.11-unreleased: sara-1.11-unreleased.tar.gz
	./install.sh sara-1.11-unreleased.tar.gz

lola-2.0-unreleased: lola-2.0-unreleased.tar.gz
	./install.sh lola-2.0-unreleased.tar.gz

distclean:
	rm -fr sara-1.11-unreleased lola-2.0-unreleased

EXTRA_DIST = sara-1.11-unreleased.tar.gz lola-2.0-unreleased.tar.gz install.sh

