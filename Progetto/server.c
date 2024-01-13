// Server Universitario

// Librerie
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

// Definizione di costanti
#define MAX_ESAMI 100
#define MAX_DATE_LENGTH 20
#define MAX_ESAME_LENGTH 100
#define MAX_LINE_LENGTH 256 // Lunghezza massima di una riga nel file

// Definizione di una struttura Esame
typedef struct {
    char nome_esame[MAX_ESAME_LENGTH];
    char date[3][MAX_DATE_LENGTH];
} Esame;

// Dichiarazione e inizializzazione di varibili globali
Esame esami[MAX_ESAMI];
int conteggio_esami = 0;

int numero_prenotazione=0;


// Funzione utilizzata per salvare gli esami su un file chiamato "esami.txt"
void salva_esami_su_file(Esame *esami, int conteggio_esami) {
    FILE *file_esami = fopen("esami.txt", "w");
    if (file_esami == NULL) {
        perror("Errore nell'apertura del file esami.txt");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < conteggio_esami; i++) {
        fprintf(file_esami,"%s\n", esami[i].nome_esame); // Scrivi il nome dell'esame su una nuova riga nel file
        // Scrivi le date una alla volta su righe separate nel file
        for (int j = 0; j < 3; j++) {
            if (strlen(esami[i].date[j]) != 0) {
                fprintf(file_esami, "%s\n", esami[i].date[j]);
            }
        }
        fprintf(file_esami, "\n"); // Aggiungi una riga vuota tra un esame e l'altro nel file
    }
    fclose(file_esami);
}

// Funzione che carica gli esami dal file "esami.txt"
void carica_esami_da_file(Esame *esami, int *conteggio_esami) {
    FILE *file_esami = fopen("esami.txt", "r");
    if (file_esami == NULL) {
        perror("Errore nell'apertura del file esami.txt");
        exit(EXIT_FAILURE);
    }

// Dichiarazione di variabili per leggere una riga alla volta dal file e tenere traccia dell'indice corrente degli esami e delle date.
    char riga[MAX_LINE_LENGTH];
    int index = -1;
    int index_date = 0;

    while (fgets(riga, MAX_LINE_LENGTH, file_esami) != NULL) {
        riga[strcspn(riga, "\n")] = '\0';

        if (strlen(riga) > 0) {
            if (index == -1) { //Se si è appena iniziato a leggere un nuovo esame, incrementa l'indice, il conteggio degli esami e salva il nome dell'esame.
                index++;
                (*conteggio_esami)++;
                strncpy(esami[index].nome_esame, riga, MAX_ESAME_LENGTH - 1);
                esami[index].nome_esame[MAX_ESAME_LENGTH - 1] = '\0';
                index_date = 0;
            } else {
                if (index_date < 3) { // Se è una data, la salva nell'array delle date corrispondenti all'esame corrente. Se la riga è vuota, resetta l'indice degli esami.
                    strncpy(esami[index].date[index_date], riga, MAX_DATE_LENGTH - 1);
                    esami[index].date[index_date][MAX_DATE_LENGTH - 1] = '\0';
                    index_date++;
                }
            }
        } else{
            index = -1;
          }
    }
    fclose(file_esami);
}

// Funzione che gestisce le richieste provenienti dalla segreteria. 
// Attende il tipo di richiesta, e se è di tipo 1, riceve e aggiunge un nuovo esame.
void gestisci_richiesta_segreteria(int socket_client, Esame *esami, int *conteggio_esami) {
    int tipo_richiesta;
    recv(socket_client, &tipo_richiesta, sizeof(tipo_richiesta), 0);

    if (tipo_richiesta == 1) { // Aggiungi esame
    
    	// Variabili per memorizzare il nome dell'esame e le date
        char nome_esame[MAX_ESAME_LENGTH];
        char date[3][MAX_DATE_LENGTH];

        // Ricevi il nome dell'esame
        recv(socket_client, nome_esame, sizeof(nome_esame), 0);

        int conteggio_date = 0; // Conteggio delle date effettivamente ricevute
        recv(socket_client, &conteggio_date, sizeof(conteggio_date), 0); // // Ricevi il conteggio delle date dalla segreteria

        // Ricevi le date una alla volta
        for (int i = 0; i < 3; i++) {
            int lunghezza_date; // Variabile per memorizzare la lunghezza della data
            recv(socket_client, &lunghezza_date, sizeof(lunghezza_date), 0); // Ricevi la lunghezza della data dalla segreteria

            if (lunghezza_date > 0) {
                char data[MAX_DATE_LENGTH]; // Variabile per memorizzare la data
                recv(socket_client, data, lunghezza_date, 0); // Ricevi la data dalla segreteria
                data[lunghezza_date] = '\0'; // Assicura che la stringa ricevuta sia terminata correttamente

                // Assegna la data ricevuta alla struttura appropriata (esami[*conteggio_esami].date[i])
                strncpy(esami[*conteggio_esami].date[i], data, MAX_DATE_LENGTH - 1);
                esami[*conteggio_esami].date[i][MAX_DATE_LENGTH - 1] = '\0';
            } else {
                esami[*conteggio_esami].date[i][0] = '\0'; // Se la lunghezza della data è 0, imposta la data come vuota
            }
        }

        // Aggiungi l'esame alla struttura dati
        strcpy(esami[*conteggio_esami].nome_esame, nome_esame);
        (*conteggio_esami)++; // Incrementa il conteggio degli esami

        int risposta = 1; // Esame aggiunto con successo
        send(socket_client, &risposta, sizeof(risposta), 0);
    }
}

// Funzione che gestisce le richieste provenienti dagli studenti. 
// Se la richiesta è di tipo 2, invia le date disponibili e riceve la scelta dello studente.
void gestisci_richiesta_studente(int socket_client, Esame *esami, int conteggio_esami) {
    int tipo_richiesta;
    recv(socket_client, &tipo_richiesta, sizeof(tipo_richiesta), 0);

    if (tipo_richiesta == 2) { 
        char nome_esame[MAX_ESAME_LENGTH];
        if (recv(socket_client, nome_esame, sizeof(nome_esame), 0) <= 0) { // Ricevi il nome dell'esame dallo studente
            perror("Errore nella ricezione del nome dell'esame");
            close(socket_client);
            return;
        }

	// Dichiarazione di variabili locali
        int conteggio_date = 0;
        char date_disponibili[3][MAX_DATE_LENGTH];

        // Cerca l'esame corrispondente nella struttara degli esami
        for (int i = 0; i < conteggio_esami; i++) {
            if (strcmp(esami[i].nome_esame, nome_esame) == 0) {
            	// Copia le date disponibili per l'esame selezionato
                for (int j = 0; j < 3; j++) {
                    if (strcmp(esami[i].date[j], "") != 0) {
                        strncpy(date_disponibili[conteggio_date], esami[i].date[j], MAX_DATE_LENGTH - 1);
                        date_disponibili[conteggio_date][MAX_DATE_LENGTH - 1] = '\0';
                        conteggio_date++;
                    }
                }
                break; // Esci dal ciclo se l'esame è stato trovato
            }
        }

	// Invia al client il conteggio delle date disponibili
        if (send(socket_client, &conteggio_date, sizeof(conteggio_date), 0) <= 0) {
            perror("Errore nell'invio del conteggio delle date disponibili");
            close(socket_client);
            return;
        }

	// Invia le date disponibili una alla volta
        for (int i = 0; i < 3; i++) {
            // Calcola la lunghezza della stringa della data
            int lunghezza_stringa = strlen(date_disponibili[i]) + 1;
            // Invia la lunghezza della stringa al client
            if (send(socket_client, &lunghezza_stringa, sizeof(lunghezza_stringa), 0) <= 0) {
                perror("Errore nell'invio della lunghezza della data disponibile");
                close(socket_client);
                return;
            }
            // Invia la data disponibile al client
            if (send(socket_client, date_disponibili[i], lunghezza_stringa, 0) <= 0) {
                perror("Errore nell'invio della data disponibile");
                close(socket_client);
                return;
            }
        }

	 // Ricevi la scelta della data da parte dello studente
        int scelta;
        if (recv(socket_client, &scelta, sizeof(scelta), 0) <= 0) {
            perror("Errore nella ricezione della scelta della data");
            close(socket_client);
            return;
        }

	// Verifica se la scelta è valida
        if (scelta >= 1 && scelta <= conteggio_date) {
            // Incrementa il numero di prenotazione globale
            numero_prenotazione++;

            // Invia il numero di prenotazione globale allo studente
            if (send(socket_client, &numero_prenotazione, sizeof(numero_prenotazione), 0) <= 0) {
                perror("Errore nell'invio del numero di prenotazione");
                close(socket_client);
                return;
            }
        } else {
            // Se la scelta non è valida, invia la conferma di nessuna data disponibile
            int conferma_invio_date = 0;
            if (send(socket_client, &conferma_invio_date, sizeof(conferma_invio_date), 0) <= 0) {
                perror("Errore nell'invio della conferma di nessuna data disponibile");
                close(socket_client);
                return;
            }
        }
           // Ricevi il numero di prenotazione confermato dallo studente
           int numero_prenotazione;
    	   if (recv(socket_client, &numero_prenotazione, sizeof(numero_prenotazione), 0) <= 0) {
       		 printf("Errore: %d\n", errno); // Stampa il valore di errno per ottenere dettagli sull'errore
        	return;
    	   }
    	   // Stampa il numero di prenotazione ricevuto dallo studente
    	   printf("Numero di prenotazione ricevuto dallo studente: %d\n", numero_prenotazione);
           close(socket_client); // Chiudi la connessione con lo studente
    }
}

// Funzione principale del server
int main() {
    // Dichiarazione delle variabili per il socket server e client, gli indirizzi del server e del client
    int socket_server, socket_client;
    struct sockaddr_in indirizzo_server, indirizzo_client;
    socklen_t lunghezza_indirizzo_client = sizeof(indirizzo_client);

    // Creazione del socket del server
    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    indirizzo_server.sin_family = AF_INET;
    indirizzo_server.sin_addr.s_addr = INADDR_ANY;
    indirizzo_server.sin_port = htons(8888);

    // Binding del socket alla configurazione dell'indirizzo del server
    if (bind(socket_server, (struct sockaddr *)&indirizzo_server, sizeof(indirizzo_server)) == -1) {
        perror("Errore nel binding del socket");
        exit(EXIT_FAILURE);
    }

    // Ascolto del socket server con una coda massima di 5 connessioni in attesa
    if (listen(socket_server, 5) == -1) { 
        perror("Errore nell'ascolto del socket");
        exit(EXIT_FAILURE);
    }
    // Stampa a console un messaggio di avviso che il server è in ascolto sulla porta 8888
    printf("Server in ascolto sulla porta 8888...\n");


    // Carica gli esami dal file all'avvio del server, se il file esiste
    carica_esami_da_file(esami, &conteggio_esami);

    while (1) {
    	// Accetta una connessione in entrata, ottenendo il socket del client e l'indirizzo del client
        socket_client = accept(socket_server, (struct sockaddr *)&indirizzo_client, &lunghezza_indirizzo_client);
        if (socket_client == -1) {
            perror("Errore nell'accettare la connessione");
            continue; // Riprende l'iterazione del loop se c'è un errore nell'accettare la connessione
        }

	 // Variabile per contenere il tipo di client (segreteria o studente)
        int tipo_client;
        recv(socket_client, &tipo_client, sizeof(tipo_client), 0);

	// Gestisce le richieste in base al tipo di client
   	if (tipo_client == 1) { // Se il tipo è 1, gestisce la richiesta della segreteria
            gestisci_richiesta_segreteria(socket_client, esami, &conteggio_esami);
            salva_esami_su_file(esami, conteggio_esami); // Salva gli esami su file dopo aver gestito la richiesta
        } else if (tipo_client == 2) { // Se il tipo è 2, gestisce la richiesta dello studente
            gestisci_richiesta_studente(socket_client, esami, conteggio_esami);
        }
        close(socket_client); // Chiude il socket del client dopo aver gestito la richiesta
    }
    close(socket_server); // Chiude il socket del server al termine dell'esecuzione
    return 0;
}
