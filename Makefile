.PHONY: all clean install

PREFIX=/srv/environment

BIN_DIR=$(PREFIX)/bin
HTDOCS_DIR=$(PREFIX)/htdocs
CGIBIN_DIR=$(PREFIX)/cgi-bin
LOG_DIR=$(HTDOCS_DIR)/log

all:
	make -C cgi-bin/
	make -C driver/
	make -C environment_daemon/

clean:
	make clean -C cgi-bin/
	make clean -C driver/
	make clean -C environment_daemon/

install:
	mkdir -p $(PREFIX) $(BIN_DIR) $(HTDOCS_DIR) $(CGIBIN_DIR) $(LOG_DIR)
	chmod 1777 $(LOG_DIR)
	cp cgi-bin/get_sensor $(CGIBIN_DIR)
	cp environment_daemon/environment_daemon $(BIN_DIR)
	cp -a htdocs/* $(HTDOCS_DIR)

