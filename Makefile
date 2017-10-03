all: master palin palin.out nopalin.out

palin.out:
	touch palin.out

nopalin.out:
	touch nopalin.out

master:
	gcc -o master master.c

palin:
	gcc -o palin palin.c

clean:
	rm master
	rm palin
	rm *.out
