LOCAL := $(shell pwd)/backend/tools

all:
	@echo "valid targets:"
	@echo "- install"
	@echo "- uninstall"
	@echo "- start"
	@echo "- stop"

start:
	@cd backend ; node_modules/forever/bin/forever start live.js

stop:
	@cd backend ; node_modules/forever/bin/forever stop live.js

install: nodejs tools

uninstall:
	rm -fr backend/node_modules
	rm -fr $(LOCAL)

nodejs:
	cd backend ; npm install express forever dateformat mkdirp
	mkdir -p backend/workdir

tools: tool-lola tool-pnapi tool-wendy tool-mia tool-sara

tool-wendy: tool-pnapi tool-lola
	wget http://download.gna.org/service-tech/wendy/wendy.tar.gz
	tar xfz wendy.tar.gz
	rm -fr wendy.tar.gz
	cd wendy-* ; PATH=${LOCAL}/bin:$$PATH ./configure --prefix=${LOCAL}
	cd wendy-* ; make all install
	rm -fr wendy-*

tool-mia: tool-wendy
	wget http://download.gna.org/service-tech/mia/mia.tar.gz
	tar xfz mia.tar.gz
	rm -fr mia.tar.gz
	cd mia-* ; PATH=${LOCAL}/bin:$$PATH ./configure --prefix=${LOCAL}
	cd mia-* ; make all install
	rm -fr mia-*

tool-pnapi:
	wget http://download.gna.org/service-tech/pnapi/pnapi.tar.gz
	tar xfz pnapi.tar.gz
	rm -fr pnapi.tar.gz
	cd pnapi-* ; PATH=${LOCAL}/bin:$$PATH ./configure --prefix=${LOCAL}
	cd pnapi-* ; make all install
	rm -fr pnapi-*

tool-lola:
	wget http://download.gna.org/service-tech/lola/lola.tar.gz
	tar xfz lola.tar.gz
	rm -fr lola.tar.gz
	cd lola-* ; PATH=${LOCAL}/bin:$$PATH ./configure --prefix=${LOCAL}
	cd lola-* ; make lola-statespace lola-statespace1 install
	rm -fr lola-*

tool-sara: tool-pnapi
	wget http://download.gna.org/service-tech/sara/sara.tar.gz
	tar xfz sara.tar.gz
	rm -fr sara.tar.gz
	cd sara-* ; PATH=${LOCAL}/bin:$$PATH ./configure --prefix=${LOCAL}
	cd sara-* ; make all install
	rm -fr sara-*
