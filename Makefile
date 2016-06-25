all:
	sh make.sh

strip:
	strip build/bin/er

clean:
	rm -rf build tags
	rm -rf *.out
.PHONY:clean strip
