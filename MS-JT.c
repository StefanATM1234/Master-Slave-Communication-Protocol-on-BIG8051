#include <c8051F040.h>	// declaratii SFR
#include <wdt.h>
#include <osc.h>
#include <port.h>
#include <uart0.h>
#include <uart1.h>
#include <lcd.h>
#include <keyb.h>
#include <Protocol.h>
#include <UserIO.h>

nod retea[NR_NODURI];				// retea cu 3 noduri

unsigned char STARE_COM = 0;		// starea initiala a FSA COM
unsigned char STARE_IO 	= 0;		// stare initiala FSA interfata IO - asteptare comenzi
unsigned char TIP_NOD	= 0;		// tip nod initial: Slave sau No JET
unsigned char ADR_MASTER;		// adresa nod master - numai MS

extern unsigned char AFISARE;		// flag permitere afisare

//***********************************************************************************************************
void TxMesaj(unsigned char i);				// transmisie mesaj destinat nodului i
unsigned char RxMesaj(unsigned char i);		// primire mesaj de la nodul i

//***********************************************************************************************************
void main (void) {
	unsigned char i, found;						// variabile locale
	
	WDT_Disable();							// dezactiveaza WDT
	SYSCLK_Init();							// initializeaza si selecteaza oscilatorul ales in osc.h
	UART1_Init(NINE_BIT, BAUDRATE_COM);		                // initilizare UART1 - port comunicatie (TxD la P1.0 si RxD la P1.1)
	UART1_TxRxEN (1,1);						// validare Tx si Rx UART1
	
 	PORT_Init ();							// conecteaza perifericele la pini (UART0, UART1) si stabileste tipul pinilor

	LCD_Init();    							// 2 linii, display ON, cursor OFF, pozitia initiala (0,0)
	KEYB_Init();							// initializare driver tastatura matriciala locala
	UART0_Init(EIGHT_BIT, BAUDRATE_IO);				// initializare UART0  - conectata la USB-UART (P0.0 si P0.1)

	Timer0_Init();  						// initializare Timer 0

 	EA = 1;                         		// valideaza intreruperile
 	SFRPAGE = LEGACY_PAGE;          		// selecteaza pagina 0 SFR
	
	for(i = 0; i < NR_NODURI; i++){					 // initializare structuri de date
		retea[i].full = 0;					// initializeaza buffer gol pentru toate nodurile
		retea[i].bufasc[0] = ':';			       // pune primul caracter in buffer-ele ASCII ':'
	}

	Afisare_meniu();			   			// Afiseaza meniul de comenzi
	
 	while(1){								// bucla infinita
																
		switch(STARE_COM){
			case 0:

#if(PROTOCOL == MS)							// nodul este slave, asteapta mesaj complet si corect de la master	

				switch(RxMesaj(ADR_NOD)){			// asteapta un mesaj de la master
					case TMO:	
						// anunta ca nodul curent devine master
						Error("\n\rNod SLAVE -> MASTER!");  
						// nodul curent devine master
						TIP_NOD = MASTER;				
						// Afiseaza meniul de comenzi						
						Afisare_meniu(); 
						// trece in starea 2										
						STARE_COM = 2;	
						// primul slave va fi cel care urmeaza dupa noul master										
						i = ADR_NOD;												
						break;

					case ROK:		// a primit un mesaj de la master, il afiseaza si trebuie sa raspunda
						STARE_COM = 1; 
						Afisare_mesaj(); 
						break;	
					
					case POK:	
						STARE_COM = 1; 	
						break;		
					
					case CAN:	
						Error("\n\rMesaj incomplet!"); // afiseaza eroare Mesaj incomplet
						break;
					
					case TIP:	
						Error("\n\rMesaj necunoscut!"); // afiseaza eroare Tip mesaj necunoscut
						break;	
					
					case ESC:									
						Error("\n\rEroare SC!"); // afiseaza Eroare SC
						break;	
					
					default:	
						Error("\n\rErr necunoscuta!"); // afiseaza cod eroare necunoscut, asteapta 1 sec
						break;	
				}
				break;


#endif
					case 1:			

#if(PROTOCOL == JT)									// nodul nu detine jetonul, asteapta un mesaj util sau jetonul
				
				switch(RxMesaj(ADR_NOD)){				// asteapta jetonul de la master
					
					case TMO:								// a primit un mesaj USER_MES
																// anunta ca nodul a regenerat jetonul 
																// nodul curent detine jetonul
																// Daca e permisa afisarea, afiseaza meniul de comenzi
																// trece in starea 1
						break;

					case ROK:								// a primit un mesaj USER_MES
																// il afiseaza			
						break;
					
					case JOK:								// a primit jetonul
						
						Delay(WAIT/2);						// asteapta WAIT/2 ms
					
																						// adresa HW dest este adr_hw_src
																						// adresa HW src este ADR_NOD
																						// tip mesaj = JET_MES
																						// transmite mesajul JET_MES din bufferul ADR_NOD
																						// nodul curent detine jetonul
						if(AFISARE) 										// Daca e permisa afisarea
																				// afiseaza meniul de comenzi		
																			// trece in starea 1
						break; 								// acum nodul detine jetonul, poate trece sa transmita un mesaj de date

					case CAN:												break;		// afiseaza eroare Mesaj incomplet
					case CAN_adresa_hw_src:	 								break;		// afiseaza eroare Mesaj incomplet (adresa_hw_src)
					case CAN_tipmes:										break;		// afiseaza eroare Mesaj incomplet (tip mesaj)
					case CAN_sc:											break;		// afiseaza eroare Mesaj incomplet (sc)
					case TIP:												break;		// afiseaza eroare Tip mesaj necunoscut
					case ESC:	 											break;		// afiseaza Eroare SC
					default:												break;		// afiseaza cod eroare necunoscut, apoi asteapta 1 sec
				}
#endif								
				

												

#if(PROTOCOL == MS)										// nodul este slave, transmite mesaj catre master			
				// cauta sa gaseasca un mesaj util de transmis
				found = 0;  
				for(i = 0; i < NR_NODURI; i++){
					if(retea[i].full == 1){
							found = 1;
							break;
					}
				}																
				
				if(found) // daca gaseste un nod i
				{  
					// pune adresa HW dest este ADR_MASTER
					retea[i].bufbin.adresa_hw_dest = ADR_MASTER; 
					// transmite mesajul catre nodul i
					TxMesaj(i); 
				}
				else // daca nu gaseste, construieste un mesaj in bufferul ADR_MASTER
				{  
					// adresa HW dest este ADR_MASTER
					retea[ADR_MASTER].bufbin.adresa_hw_dest = ADR_MASTER;
					// adresa HW src este ADR_NOD 
					retea[ADR_MASTER].bufbin.adresa_hw_src = ADR_NOD;	
					// tip mesaj = POLL_MES
					retea[ADR_MASTER].bufbin.tipmes = POLL_MES;		
					// transmite mesajul din bufferul ADR_MASTER
					TxMesaj(ADR_MASTER);  
				}

				// trece in starea 0, sa astepte un nou mesaj de la master
				STARE_COM = 0;				
				break;
#endif						
			break;	
				
			case 2:											// tratare stare 2 si comutare stare


#if(PROTOCOL == JT)							// nodul detine jetonul, poate transmite un mesaj USER_MES				
				
				
																			// cauta sa gaseasca un mesaj util de transmis
	




																					// daca gaseste un mesaj de transmis catre nodul i
																						// adresa HW dest este dest
																						// transmite mesajul catre nodul i
																						// asteapta procesarea mesajului la destinatie (WAIT/2 msec)

																		// va incerca sa transmita jetonul nodului urmator 
																		// trece in starea 2, sa transmita jetonul urmatorului nod
#endif	
				
#if(PROTOCOL == MS)											// nodul este master, tratare stare 2 si comutare stare
		// nodul este master, tratare stare 2 si comutare stare
				do {
					// selecteaza urmatorul slave (incrementeaza i)
					i = (++i) % NR_NODURI;	
				} while(i == ADR_NOD);

				// adresa HW dest este i	
				retea[i].bufbin.adresa_hw_dest = i;	

				// daca in bufferul i se afla un mesaj util, il transmite										
				if(retea[i].full == 1)  
				{
					TxMesaj(i);									
				}
				else  // altfel, construieste un mesaj de interogare in bufferul i
				{	
					// adresa HW src este ADR_NOD
					retea[i].bufbin.adresa_hw_src = ADR_NOD; 
					// tip mesaj = POLL_MES
					retea[i].bufbin.tipmes = POLL_MES;      
					// transmite mesajul din bufferul i
					TxMesaj(i);
				}				

				// trece in starea 3, sa astepte raspunsul de la slave-ul i						
				STARE_COM = 3; 
				break;

#endif

#if(PROTOCOL == JT)											// nodul transmite jetonul urmatorului nod 		
				
																				// selecteaza urmatorul slave (incrementeaza i si sare peste adresa proprie)
					
	
	
	
																				// asteapta WAIT/2 sec
				
																				// adresa HW dest este i
																				// adresa HW src este ADR_NOD
																				// tip mesaj = JET_MES
																				// transmite mesajul din bufferul i
																				// trece in starea 3, sa astepte confirmarea de la nodul i ca jetonul a fost primit
#endif

			break;

			case 3:

#if(PROTOCOL == MS)										// nodul este master, asteapta mesaj de la slave	
																
				switch(RxMesaj(i)) // asteapta un raspuns de la slave i
				{						
					case TMO:	
						// afiseaza eroare Timeout nod i
						Error("\n\rTimeout nod ");
						if (AFISARE)
						{
							UART0_Putch(i+'0');
							LCD_Putch(i+'0');
						}
						break;

					case ROK:	Afisare_mesaj(); break;	// a primit un mesaj, il afiseaza
					case POK:	break;							

					case ERI:	
						// afiseaza Eroare incadrare
						Error("\n\rEroare incadrare!");
						break;	

					case ERA: 
						// afiseaza Eroare adresa
						Error("\n\rEroare adresa!");
						break;	

					case TIP:
						// afiseaza Tip mesaj necunoscut
						Error("\n\Mesaj necunoscut!");
						break;	

					case OVR:	
						// afiseaza Eroare suprapunere
						Error("\n\rEroare suprapunere!");
						break;	

					case ESC:
						// afiseaza Eroare SC
						Error("\n\rEroare SC!");
						break;	

					case CAN:
						// afiseaza mesaj incomplet
						Error("\n\rMesaj incomplet!");
						break;	

					case TEST:
						// afiseaza Eroare TEST
						Error("\n\rMesaj incomplet!");
						break;				

					default:
						// afiseaza Eroare necunoscuta, apoi asteapta 1000ms
						Error("\n\rErr necunoscuta!");
						break;	
				}
				// revine in starea 2 (a primit sau nu un raspuns de la slave, trece la urmatorul slave)
				STARE_COM = 2;
				break;	
#endif
			
			break;			
		}
		
		UserIO();							// apel functie interfata
	}
}



			
#if(PROTOCOL == JT)												// asteapta confirmarea primirii jetonului de care nodul i		
				
#endif

