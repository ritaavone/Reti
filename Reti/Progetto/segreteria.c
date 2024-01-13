//Segreteria 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "wrapper.h"

// Dichiarazione delle costanti
#define IP_SERVER "127.0.0.1"
#define PORTA_SERVER 8888
#define MAX_DATE_LENGTH 20
#define MAX_ESAME_LENGTH 100

int main() {
    int socket_client = socket(AF_INET, SOCK_STREAM, 0); // Creazione del socket
    if (socket_client == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in indirizzo_server; // Struttura per l'indirizzo del server
    indirizzo_server.sin_family = AF_INET; // Tipo di indirizzo
    indirizzo_server.sin_port = htons(PORTA_SERVER); // Porta del server
    inet_aton(IP_SERVER, &indirizzo_server.sin_addr); // Indirizzo IP del server

    if (connect(socket_client, (struct sockaddr*)&indirizzo_server, sizeof(indirizzo_server)) == -1) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    int tipo_client = 1; // Indica che il client è della segreteria
    send(socket_client, &tipo_client, sizeof(tipo_client), 0); // Invia il tipo di client al server

    int tipo_richiesta = 1; // Indica che si vuole aggiungere un esame
    send(socket_client, &tipo_richiesta, sizeof(tipo_richiesta), 0); // Invia il tipo di richiesta al server

    char nome_esame[MAX_ESAME_LENGTH];
    char date_input[100]; // Stringa per l'input delle date
    char date[3][MAX_DATE_LENGTH];

    printf("Inserisci il nome dell'esame: ");
    fgets(nome_esame, sizeof(nome_esame), stdin);
    nome_esame[strcspn(nome_esame, "\n")] = '\0';  // Rimuovi il carattere newline
    send(socket_client, nome_esame, sizeof(nome_esame), 0); // Invia il nome dell'esame al server

    int conteggio_date = 0; 
    printf("Inserisci le date dell'esame una alla volta (max 3, es. 01/01/2024):\n");
    for (int i = 0; i < 3; i++) {
        printf("Data %d: ", i + 1);
        fgets(date[i], sizeof(date[i]), stdin);
        date[i][strcspn(date[i], "\n")] = '\0'; // Rimuovi il carattere newline
        if (strlen(date[i]) > 0) {
            conteggio_date++;
        }
    }

    send(socket_client, &conteggio_date, sizeof(conteggio_date), 0); // Invia il conteggio delle date valide

    // Invia le date una alla volta al server
    for (int i = 0; i < 3; i++) {
        if (strlen(date[i]) > 0) {
            char cleaned_date[MAX_DATE_LENGTH];
            sscanf(date[i], " %19[^\n]", cleaned_date);

            int lunghezza_stringa = strlen(cleaned_date); // Lunghezza della data senza il terminatore null
            send(socket_client, &lunghezza_stringa, sizeof(lunghezza_stringa), 0); // Invia la lunghezza della data
            send(socket_client, cleaned_date, lunghezza_stringa, 0); // Invia la data stessa
        }
    }

    // Ricevi la risposta dal server universitario dopo l'aggiunta dell'esame
    int risposta;
    recv(socket_client, &risposta, sizeof(risposta), 0);

    if (risposta == 1) {
        printf("Esame aggiunto con successo.\n");
    } else {
        printf("Errore durante l'aggiunta dell'esame.\n");
    }

    close(socket_client);

    return 0;
}

