

CIBLE = testdesasm			#nom du programme généré
DEPENDANCE = desassembleur.o memory.o registre.o 


$(CIBLE): $(DEPENDANCE)
	gcc -o $(CIBLE) -g $(DEPENDANCE)
	
clean:
	rm  *.o *.~ *.orig testdesasm

