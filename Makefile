all:
	sh make.sh

strip:
	strip build/bin/er

clean:
	rm -rf build tags
.PHONY:clean strip
