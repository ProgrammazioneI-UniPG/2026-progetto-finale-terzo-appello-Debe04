#include <stdio.h>
#include "gamelib.h"  // Inclusione dell'header per le dichiarazioni delle funzioni

void stampa_linea() {
    printf("========================================\n");
}

void stampa_menu() {
    printf("                🎭 Prince of Inertia 🎭\n");
    stampa_linea();
    printf("📜 Menu principale 📜\n");
    stampa_linea();
    printf("1️⃣ Imposta gioco\n");
    printf("2️⃣ Gioca\n");
    printf("3️⃣ Termina gioco\n");
    printf("4️⃣ Crediti\n");
    stampa_linea();
    printf("🔹 Inserisci la tua scelta: ");
}

int main() {
    int scelta;
    int gioco_impostato = 0;
    do {
        stampa_menu();
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                imposta_gioco();
                gioco_impostato = 1;
                break;
            case 2:
                if (gioco_impostato) {
                    gioca();
                } else {
                    printf("Devi impostare il gioco prima di giocare!\n");
                }
                break;
            case 3:
                termina_gioco();
                break;
            case 4:
                crediti();
                break;
            default:
                printf("Comando sbagliato, riprova.\n");
                break;
        }
    } while (scelta != 3);

    return 0;
}
