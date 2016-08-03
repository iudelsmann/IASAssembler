all:
	gcc main.c listas.c auxiliares.c computar.c -o assembler

clean:
	rm -rf assembler
