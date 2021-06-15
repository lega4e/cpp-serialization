cflags  := -std=gnu++17 -c -Wall
ldflags :=
libs    :=





# main
build: main

run: main
	main 


re: clean build

rerun: clean run


source_dirs     := . ./src ./src/funs
header_dirs     := ../ ./src/tmpls
search_wildcard := $(wildcard $(addsuffix /*.cpp,$(source_dirs)))
object_files    := $(patsubst %.cpp,%.o,$(search_wildcard))
object_files    := $(notdir $(object_files))
object_files    := $(addprefix target/,$(object_files))
VPATH           := $(source_dirs)

main: ./target $(object_files)
	g++ $(ldflags) -o $@ $(object_files) $(libs)

./target:
	if ! [ -d ./target ]; then mkdir target; fi

target/%.o: %.cpp
	g++ $(cflags) -o $@ -MD $(addprefix -I,$(header_dirs)) $<

include $(wildcard target/*.d)



# clean
clean:
	-rm target/* main





# end
