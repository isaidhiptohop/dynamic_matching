all:
	cd lib && make 
	cd app && make 
	cd gpa/lib && make 
	cd gpa/app && make 
	cp app/compare/compare deploy/
	cp app/executer/executer deploy/
	cp app/sequencer/sequencer deploy/
	cp gpa/app/gpa_matching deploy/
