.PHONY: gen_build compile_build build clean

gen_build:
	cmake -S . -B build

compile_build:
	cmake --build build

clean:
	rm -rf build bin