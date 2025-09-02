.PHONY: gen_debug_build gen_release_build compile_build build clean

gen_debug_build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON

gen_release_build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

compile_build:
	cmake --build build

coverage:
#only run this once test bin has executed and prof exists (for now)
	llvm-profdata merge -sparse ./bin/default.profraw -o ./bin/default.profdata
	llvm-cov show ./bin/scpsolver_test -instr-profile=./bin/default.profdata -format=html -output-dir=coverage_report src/

clean:
	rm -rf build bin coverage_report