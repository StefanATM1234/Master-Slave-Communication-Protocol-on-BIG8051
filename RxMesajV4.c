#include <c8051F040.h>	// declaratii SFR
#include <uart1.h>
#include <Protocol.h>
#include <UserIO.h>

extern nod retea[];					// reteaua cu 3 noduri

extern unsigned char TIP_NOD;		// tip nod
extern unsigned char ADR_MASTER;	// adresa nodului master

extern unsigned char timeout;		// variabila globala care indica expirare timp de asteptare eveniment
//***********************************************************************************************************
unsigned char RxMesaj(unsigned char i);				// primire mesaj de la nodul i

//***********************************************************************************************************
unsigned char RxMesaj(unsigned char i){					// receptie mesaj															   
	unsigned char j, ch, sc, adresa_hw_src, screc, src, dest, lng, tipmes;
	
																			// dezactivare Tx, validare RX UART1
																			// dezactivare Tx, validare RX RS485
																			// receptie doar octeti de adresa (mod MULTIPROC_ADRESA)

																			// Daca nodul este master sau detine jetonul ...
																				// M: asteapta cu timeout raspunsul de la slave
																				// M: timeout, terminare receptie,  devine master sau regenerare jeton

																				// M: raspunsul de la slave a venit, considera ca mesajul anterior a fost transmis cu succes	
																				// M: adresa HW gresita, terminare receptie
																		
																			// Altfel ...
																			
																					// S: asteapta inceput mesaj nou
																					
																					// S: nu exista activitate in retea
																				
																				// S: iese doar atunci cand mesajul ii este adresat acestui nod
 																			
	
																			// receptie octeti de date (mod MULTIPROC_DATA)

																			// M+S: initializeaza screc cu adresa HW dest (ADR_NOD)
	
																			// M+S: Asteapta cu timeout receptia adresei HW sursa
																				// M+S: mesaj incomplet
	
																			// M+S: ia in calcul in screc adresa hw src

																			// S: actualizeaza adresa master
	
																			// M+S: Asteapta cu timeout receptie tip mesaj
																				// M+S: mesaj incomplet
																			
																				// M+S: tip mesaj eronat, terminare receptie
																			
																			// M+S: ia in calcul in screc tipul mesajului
	
																			// M+S: Daca mesajul este unul de date (USER_MES)
																				// M+S: asteapta cu timeout adresa nodului sursa
																				
																				// M+S: ia in calcul in screc adresa src
		
																				// M+S: asteapta cu timeout adresa nodului destinatie
																				
																				// M+S: ia in calcul in screc adresa dest
		
																				// Daca nodul este master...
																					// M: bufferul destinatie este deja plin, terminare receptie
																				

																				// M+S: asteapta cu timeout receptia lng
																				
																				// M+S: ia in calcul in screc lungimea datelor
		
																				// Daca nodul este master...
																					// M: stocheaza adresa HW sursa	= ADR_NOD
																					// M: stocheaza tipul mesajului	
																					// M: stocheaza la src codul nodului sursa al mesajului	
																					// M: stocheaza la dest codul nodului destinatie a mesajului	
																					// M: stocheaza lng
																					
																						// M: asteapta cu timeout un octet de date
																						
																						// M: ia in calcul in screc datele
																						
																					
																					// M: Asteapta cu timeout receptia sumei de control
																					

																					// Daca sc e corect...
																						// M: mesaj corect, marcare buffer plin
																						
																					// Altfel...
																						// M: eroare SC, terminare receptie
																															
																				// Altfel (nodul este slave)
																					// S: stocheaza la destsrc codul nodului sursa al mesajului	
																					// S: stocheaza lng
																					
																						// S: asteapta cu timeout un octet de date
																						
																						// S: ia in calcul in screc octetul de date
																					
																					
																					// S: Asteapta cu timeout receptia sumei de control
																					

																					// Daca sc e corect...
																						// S: mesaj corect, marcare buffer plin
																						
																					// Altfel...
																						// S: eroare SC, terminare receptie
																		
																			// Altfel (este un mesaj POLL_MES sau JET_MES)
																					// JT: memoreaza de la cine a primit jetonul
																					// M+S: Asteapta cu timeout receptia sumei de control
																					
																					// M+S:Daca sc e corect, returneaza POK
																					// M+S: Altfel, eroare SC, terminare receptie
																					
		return TMO; 		// simuleaza timeout receptie, dupa implementare linia se sterge																
}

