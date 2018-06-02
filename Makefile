.PHONY: all clean install

PREFIX=/srv/environment

all:
	make -C driver/
	make -C get_sensor/
	make -C environment_daemon/

clean:
	make clean -C driver/
	make clean -C get_sensor/
	make clean -C environment_daemon/

install:
	cp get_sensor/get_sensor ${PREFIX}/cgi-bin
	cp environment_daemon/environment_daemon ${PREFIX}/bin
	cp -a htdocs/* ${PREFIX}/htdocs

