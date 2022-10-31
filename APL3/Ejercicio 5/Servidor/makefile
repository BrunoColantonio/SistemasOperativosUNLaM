.PHONY: clean

servidorComp: servidor.o
	g++ -o servidorComp servidor.o 
	
servidor.o: servidor.c
	g++ -c servidor.c

clean: 
	rm *.o 
