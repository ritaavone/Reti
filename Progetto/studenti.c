//Studenti

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "wrapper.h"

// Definizione delle costanti
#define IP_SERVER "127.0.0.1"
#define PORTA_SERVER 8888
#define MAX_DATE_LENGTH 20
#define MAX_ESAME_LENGTH 100

int main() {
    int socket_client = socket(AF_INET, SOCK_STREAM, 0); // Creazione del socket del client
    if (socket_client == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    struct sockaddr_in indirizzo_server;
    indirizzo_server.sin_family = AF_INET;
    indirizzo_server.sin_port = htons(PORTA_SERVER);
    inet_aton(IP_SERVER, &indirizzo_server.sin_addr);

    // Connessione al server
    if (connect(socket_client, (struct sockaddr*)&indirizzo_server, sizeof(indirizzo_server)) == -1) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    // Specifica del tipo di client (2 indica uno studente)
    int tipo_client = 2; // Studente
    send(socket_client, &tipo_client, sizeof(tipo_client), 0);

    // Specifica del tipo di richiesta (2 indica richiesta di esame disponibile)
    int tipo_richiesta = 2; // Richiesta di esame disponibile
    send(socket_client, &tipo_richiesta, sizeof(tipo_richiesta), 0);

    // Inserimento del nome dell'esame da parte dello studente
    char nome_esame[MAX_ESAME_LENGTH];
    printf("Inserisci il nome dell'esame per verificare le date disponibili: ");
    fgets(nome_esame, sizeof(nome_esame), stdin);
    nome_esame[strcspn(nome_esame, "\n")] = '\0'; // Rimuovi il carattere newline
    send(socket_client, nome_esame, sizeof(nome_esame), 0);

    // Ricezione del numero di date disponibili per l'esame
    int conteggio_date;
    recv(socket_client, &conteggio_date, sizeof(conteggio_date), 0);

   // Se ci sono date disponibili, mostra le opzioni all'utente   
   if (conteggio_date > 0) {
      printf("Date disponibili per l'esame '%s':\n", nome_esame);
      
      // Ricezione e stampa delle date disponibili
      for (int i = 0; i < conteggio_date; i++) {
        int lunghezza_stringa;
        recv(socket_client, &lunghezza_stringa, sizeof(lunghezza_stringa), 0);

        char data[MAX_DATE_LENGTH];
        recv(socket_client, data, lunghezza_stringa, 0);
        data[lunghezza_stringa] = '\0'; // Assicura che la stringa ricevuta sia terminata correttamente
        printf("%d. %s\n", i + 1, data);
    }

    // Scelta della data da parte dello studente
    int scelta;
    printf("Inserisci il numero corrispondente alla data scelta: ");
    scanf("%d", &scelta);
    getchar(); // Consuma il carattere newline residuo nel buffer stdin

    send(socket_client, &scelta, sizeof(scelta), 0);
    int numero_prenotazione;
    recv(socket_client, &numero_prenotazione, sizeof(numero_prenotazione), 0);
    
    // Se il numero di prenotazione è positivo, la prenotazione è avvenuta con successo
        if (numero_prenotazione > 0) {
            printf("Prenotazione effettuata con successo. Numero di prenotazione: %d\n", numero_prenotazione);
                // Invio del numero di prenotazione al server
                if (send(socket_client, &numero_prenotazione, sizeof(numero_prenotazione), 0) == -1) {
                  perror("Errore nell'invio del numero di prenotazione al server");
                }
        } else {
            printf("Errore durante la prenotazione.\n");
        }
    } else {
        printf("Non ci sono date disponibili per l'esame '%s'.\n", nome_esame);
    }
    close(socket_client); // Chiusura del socket del client

    return 0;
}
