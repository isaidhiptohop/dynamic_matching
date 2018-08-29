.PHONY: all app lib gpa

all:
	make lib
	make app
	make gpa
	make copy-executables

debug:
	make lib-debug
	make app
	make gpa-debug
	make copy-executables

debug-w-counters:
	make lib-debug-w-counters
	make app
	make gpa-debug
	make copy-executables

w-counters:
	make lib-w-counters
	make app
	make gpa
	make copy-executables

lib:
	cd lib && make 

lib-debug:
	cd lib && make library_debug

lib-debug-w-counters:
	cd lib && make library_counters_debug

lib-w-counters:
	cd lib && make library_counters

app:
	cd app && make 
	
gpa:
	cd gpa/lib && make 
	cd gpa/app && make 

gpa-debug:
	cd gpa/lib && make
	cd gpa/app && make debug

copy-executables:
	cp app/compare/compare deploy/
	cp app/executer/executer deploy/
	cp app/sequencer/sequencer deploy/
	cp gpa/app/gpa_matching deploy/
	mkdir -p deploy/output

