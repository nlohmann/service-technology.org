#############################################################################
# GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models
#
# Copyright (C) 2006, 2007  Niels Lohmann,
#                           Christian Gierds, and
#                           Martin Znamirowski
# Copyright (C) 2005        Niels Lohmann and
#			    Christian Gierds
#
# GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.
#
# GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# GNU BPEL2oWFN; if not, see http://www.gnu.org/licenses or write to the Free
# Software Foundation,Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA.
#############################################################################

EXTRA_DIST = @PACKAGE@.spec

version = @VERSION@
release = 0.`date +%Y%m%d`
cparams = '--enable-all-sets'
mparams = 'CFLAGS="-O2"'

rpm: @PACKAGE@.spec
	@\
	version=$(version); \
	release=$(release); \
	cparams='$(cparams)'; \
	mparams='$(mparams)'; \
	tarball=@PACKAGE@-$$version.tar.gz; \
	tarball_full=`cd $(top_srcdir) && pwd`/$$tarball; \
	if [ ! -f $$tarball_full ]; then echo "No $$tarball"; exit -1; fi; \
	ln -sf $$tarball_full /tmp/$$tarball || exit -1; \
	echo ""; \
	echo "==== Creating rpm from /tmp/$$tarball, release $$release ===="; \
	echo ""; \
	sleep 3; \
	sed < @PACKAGE@.spec > tmp-@PACKAGE@.spec \
		-e "s/%define ver /&$$version  %define old_ver /" \
		-e "s/%define rel /&$$release  %define old_rel /" \
		-e "s/%define c_p /&$$cparams  %define old_c_p /" \
		-e "s/%define m_p /&$$mparams  %define old_m_p /" \
	; \
	rpmbuild -ba tmp-@PACKAGE@.spec --clean --target=noarch-fvwm-linux; \
	rm /tmp/$$tarball; \
	rm tmp-@PACKAGE@.spec

