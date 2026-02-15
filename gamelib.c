#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "gamelib.h"
#include <string.h>

// Dichiarazioni delle funzioni
void ins_stanza();
void canc_stanza();
void stampa_stanze();
void genera_random();
void chiudi_mappa();
void cancella_tutte_le_stanze();
typedef enum { SCHELETRO, GUARDIA, JAFFAR } TipoNemico;

typedef struct {
    TipoNemico tipo;
    char nome[20];
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
} Nemico;
static void combatti(struct Giocatore* giocatore, Nemico *nemico, const char *tipo_nemico);
static void avanza(struct Giocatore* giocatore);
static void passa(int *indice_turno, int n_giocatori);
static void mescola_turni(int *ordine, int n_giocatori);
static void scappa(struct Giocatore* giocatore);
static void stampa_giocatore(struct Giocatore* giocatore);
static void stampa_zona(struct Stanza* stanza);
static void cerca_porta_segreta(struct Giocatore* giocatore, int tentativo);
static void esegui_attacco(struct Giocatore* Giocatore, Nemico *nemico);
// Definizioni delle variabili globali
struct Stanza* pFirst = NULL;
struct Stanza* pUltima = NULL;
int mappa_terminata = 0;
bool gioco_impostato = false;
bool mappa_creata = false;
bool turno_attivo = false;
int numero_stanze = 0;
int n_giocatori = 0;
bool G_attacante; // G si intende il giocatore
struct Giocatore* giocatori[3] = { NULL, NULL, NULL };
struct Jaffar jaffar = {3, 3, 3, 2};
int indice_turno = 0; // Indice del turno corrente

// Funzione per impostare il gioco
void imposta_gioco() {
    if (gioco_impostato) {
        printf("⚠️ Il gioco è già stato impostato.\n");
        return;
    }
    int principe_presente = 0;

    printf("👥 Inserisci il numero di giocatori: ");
    scanf("%d", &n_giocatori);

    if (n_giocatori < 1 || n_giocatori > 3) {
        printf("⚠️ Numero di giocatori non valido.\n");
        return;
    }

    for (int i = 0; i < n_giocatori; i++) {
        
        giocatori[i] = (struct Giocatore*)malloc(sizeof(struct Giocatore));
        giocatori[i]->posizione = pFirst;
        if (giocatori[i] == NULL) {
            printf("❌ Errore di allocazione della memoria.\n");
            return;
        }

        printf("👤 Inserisci il nome del giocatore %d: ", i + 1);
        scanf("%s", giocatori[i]->nome_giocatore);

        if (i == 0) {
            printf("⚔️ Scegli la classe del giocatore %d (0 per principe, 1 per doppelganger, Ricorda che il primo giocatore deve essere un principe): ", i + 1);
            scanf("%d", (int*)&giocatori[i]->classe);
            if (giocatori[i]->classe == principe) {
                principe_presente = 1;
                giocatori[i]->p_vita_max = 4;
                giocatori[i]->p_vita = 4;
                giocatori[i]->dadi_attacco = 2;
                giocatori[i]->dadi_difesa = 2;
                giocatori[i]->posizione = NULL;
            }
        } else {
            giocatori[i]->classe = doppelganger;
            giocatori[i]->p_vita_max = 3;
            giocatori[i]->p_vita = 3;
            giocatori[i]->dadi_attacco = 2;
            giocatori[i]->dadi_difesa = 2;
            giocatori[i]->posizione = NULL;
        }
    }

    if (!principe_presente) {
        printf("⚠️ Deve esserci almeno un principe per poter giocare.\n");
        for (int i = 0; i < n_giocatori; i++) {
            free(giocatori[i]);
        }
        return;
    }
    printf("\n👥 Giocatori impostati:\n");
    for (int i = 0; i < n_giocatori; i++) {
        if (giocatori[i] != NULL) {
            printf("👤 Giocatore %d:\n", i + 1);
            printf("Nome: %s\n", giocatori[i]->nome_giocatore);
            printf("Classe: %s\n", giocatori[i]->classe == principe ? "Principe" : "Doppelgänger");
            printf("Punti vita: %d/%d\n", giocatori[i]->p_vita, giocatori[i]->p_vita_max);
        } else {
            printf("❌ Errore: Il giocatore %d non è stato inizializzato correttamente.\n", i + 1);
        }
    }

    int scelta;
    do {
        printf("\n📜 Menu:\n");
        printf("1. Inserisci stanza\n");
        printf("2. Cancella stanza\n");
        printf("3. Stampa stanze\n");
        printf("4. Genera mappa casuale\n");
        printf("5. Chiudi mappa\n");
        printf("6. Esci\n");
        printf("Scegli un'opzione: ");
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                ins_stanza();
                break;
            case 2:
                canc_stanza();
                break;
            case 3:
                stampa_stanze();
                break;
            case 4:
                genera_random();
                break;
            case 5:
                chiudi_mappa();
                break;
            case 6:
                printf("🚪 Uscita dal gioco.\n");
                break;
            default:
                printf("⚠️ Opzione non valida.\n");
                break;
        }
    } while (scelta != 6 && !mappa_terminata);

    gioco_impostato = true;
}

// Funzione per inserire una nuova stanza
void ins_stanza() {
    if (numero_stanze == 15) {
        printf("⚠️ Hai raggiunto il numero massimo di stanze.\n");
        mappa_creata = true;
        return;
    }
    struct Stanza* nuova_stanza = (struct Stanza*)malloc(sizeof(struct Stanza));
    if (nuova_stanza == NULL) {
        printf("❌ Errore di allocazione della memoria.\n");
        return;
    }
    printf("🏠 Inserisci il tipo di stanza (0 per corridoio, 1 per scala, 2 per sala banchetto, 3 per magazzino, 4 per posto guardia, 5 per prigione, 6 per armeria, 7 per moschea, 8 per torre, 9 per bagni): ");
    scanf("%d", (int*)&nuova_stanza->tipo_stanza);
    printf("⚠️ Inserisci il tipo di trabocchetto (0 per nessun trabocchetto, 1 per tegola, 2 per lame, 3 per caduta, 4 per burrone): ");
    scanf("%d", (int*)&nuova_stanza->tipo_trabocchetto);
    printf("💎 Inserisci il tipo di tesoro (0 per nessun tesoro, 1 per verde veleno, 2 per blu guarigione, 3 per rosso aumenta vita, 4 per spada tagliente, 5 per scudo): ");
    scanf("%d", (int*)&nuova_stanza->tipo_tesoro);

    nuova_stanza->stanza_precedente = pUltima;
    nuova_stanza->stanza_successiva = NULL;

    if (pFirst == NULL) {
        pFirst = nuova_stanza;
    } else {
        pUltima->stanza_successiva = nuova_stanza;
    }
    pUltima = nuova_stanza;

    printf("✅ Stanza inserita correttamente: Tipo stanza %d, Tipo trabocchetto %d, Tipo tesoro %d.\n", nuova_stanza->tipo_stanza, nuova_stanza->tipo_trabocchetto, nuova_stanza->tipo_tesoro);
}

// Funzione per cancellare l'ultima stanza
void canc_stanza() {
    if (pUltima == NULL) {
        printf("⚠️ Non ci sono stanze da cancellare.\n");
        return;
    }
    int scelta;
    printf("❓ Sicuro di voler cancellare l'ultima stanza? (0 per no, 1 per si): ");
    scanf("%d", &scelta);
    if (scelta == 1) {
        struct Stanza* da_cancellare = pUltima;
        pUltima = pUltima->stanza_precedente;
        if (pUltima != NULL) {
            pUltima->stanza_successiva = NULL;
        } else {
            pFirst = NULL;
        }
        free(da_cancellare);
        printf("✅ Stanza cancellata correttamente.\n");
    } else {
        printf("⚠️ Operazione annullata.\n");
    }
}

// Funzione per stampare tutte le stanze
void stampa_stanze() {
    struct Stanza* corrente = pFirst;
    int contatore = 1;

    if (pFirst == NULL) {
        printf("⚠️ Non ci sono stanze da stampare.\n");
        return;
    }

    printf("📜 Stampa delle stanze:\n");

    while (corrente != NULL) {
        printf("\n🏠 Stanza %d:\n", contatore);
        contatore++;
        printf("Tipo stanza: %d\n", corrente->tipo_stanza);
        printf("Tipo trabocchetto: %d\n", corrente->tipo_trabocchetto);
        printf("Tipo tesoro: %d\n", corrente->tipo_tesoro);
        printf("Puntatore stanza precedente: %p\n", (void*)corrente->stanza_precedente);
        printf("Puntatore stanza successiva: %p\n", (void*)corrente->stanza_successiva);

        corrente = corrente->stanza_successiva;
    }
}

// Funzione per cancellare tutte le stanze
void cancella_tutte_le_stanze() {
    struct Stanza* corrente = pFirst;
    struct Stanza* da_cancellare;

    while (corrente != NULL) {
        da_cancellare = corrente;
        corrente = corrente->stanza_successiva;
        printf("🗑️ Liberando la stanza: %p\n", (void*)da_cancellare);
        free(da_cancellare);
    }

    pFirst = NULL;
    pUltima = NULL;

    printf("✅ Tutte le stanze sono state cancellate correttamente.\n");
    mappa_creata = false;
}

// Funzione per generare una mappa casuale
void genera_random() {
    if (pFirst != NULL) {
        printf("⚠️ Esistono già stanze allocate. Le cancello prima di generare nuove stanze.\n");
        cancella_tutte_le_stanze();
    }

    srand(time(NULL));

    for (int i = 0; i < 15; i++) {
        struct Stanza* nuova_stanza = (struct Stanza*)malloc(sizeof(struct Stanza));
        if (nuova_stanza == NULL) {
            printf("❌ Errore di allocazione della memoria.\n");
            cancella_tutte_le_stanze();
            return;
        }

        nuova_stanza->tipo_stanza = rand() % 10;
        nuova_stanza->tipo_trabocchetto = (rand() % 100) < 65 ? nessuno : rand() % 4 + 1;
        nuova_stanza->tipo_tesoro = rand() % 6;
        nuova_stanza->stanza_precedente = pUltima;
        nuova_stanza->stanza_successiva = NULL;

        if (pFirst == NULL) {
            pFirst = nuova_stanza;
        } else {
            pUltima->stanza_successiva = nuova_stanza;
        }
        pUltima = nuova_stanza;

        printf("🏠 Stanza %d creata all'indirizzo: %p\n", i + 1, (void*)nuova_stanza);
    }

    printf("✅ Mappa generata casualmente con 15 stanze.\n");
    mappa_creata = true;
    printf("DEBUG: mappa_creata impostata a %d\n", mappa_creata);
}

// Funzione per chiudere la mappa
void chiudi_mappa() {
    int count = 0;
    struct Stanza* corrente = pFirst;

    while (corrente != NULL) {
        count++;
        corrente = corrente->stanza_successiva;
    }

    if (count < 15) {
        printf("⚠️ Mappa non chiusa devono esserci almeno 15 stanze.\n");
        return;
    }

    mappa_terminata = 1;
    printf("✅ Mappa chiusa correttamente.\n");
}

// Funzione per mescolare l'ordine dei turni
static void mescola_turni(int* ordine, int n_giocatori) {
    for (int i = n_giocatori - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = ordine[i];
        ordine[i] = ordine[j];
        ordine[j] = temp;
    }
}
typedef struct {
    Nemico base;
} Scheletro;

typedef struct {
    Nemico base;
} Guardia;

typedef struct {
    Nemico base;
} Jaffar;

// Funzione per avanzare nella mappa
Scheletro scheletro = {{SCHELETRO, "Scheletro", 3, 3, 1, 1}};
Guardia guardia = {{GUARDIA, "Guardia", 4, 4, 2, 2}};

static void avanza(struct Giocatore* giocatore) {
    // Se il giocatore non ha una posizione, inizia dalla prima stanza
    if (giocatore->posizione == NULL) {
        giocatore->posizione = pFirst;
        printf("🎮 %s inizia dal principio della mappa.\n", giocatore->nome_giocatore);
        return;
    }

    // Controlla se il giocatore è nell'ultima stanza
    if (giocatore->posizione->stanza_successiva == NULL) {
        printf("⚔️ Sei nell'ultima stanza! Jaffar appare!\n");
        printf("Scegli un'azione: 1) Combatti, 2) Fuggi\n");
        int scelta;
        scanf("%d", &scelta);
        if (scelta == 1) {
            combatti(giocatore, (Nemico*)&jaffar, "Jaffar");
        } else if (scelta == 2) {
            scappa(giocatore);
        } else {
            printf("⚠️ Scelta non valida.\n");
        }
        return;
    }

    // Avanza alla stanza successiva
    giocatore->posizione = giocatore->posizione->stanza_successiva;
    printf("➡️ %s è avanzato alla stanza successiva.\n", giocatore->nome_giocatore);

    // Controlla se il giocatore è caduto in un trabocchetto
    if (giocatore->posizione->tipo_trabocchetto != 0) {
        printf("⚠️ Il giocatore è caduto in un trabocchetto\n");

        if (giocatore->classe == principe) {
            printf("🛡️ Il principe ha evitato il trabocchetto\n");
        } else {
            switch (giocatore->posizione->tipo_trabocchetto) {
                case 1:
                    giocatore->p_vita--;
                    printf("❤️ Il giocatore ha perso 1 punto vita.\n");
                    break;
                case 2:
                    giocatore->p_vita -= 2;
                    printf("❤️ Il giocatore ha perso 2 punti vita.\n");
                    break;
                case 3:
                    giocatore->p_vita -= (rand() % 2) + 1;
                    printf("❤️ Il giocatore ha perso punti vita a causa di una caduta.\n");
                    break;
                case 4:
                    giocatore->p_vita = 0;
                    turno_attivo = false;
                    printf("💀 Il giocatore è morto a causa di un burrone.\n");
                    return;
            }
        }
    }

    // Genera nemici casuali
    int probabilita = rand() % 100;
    if (probabilita < 25) { // 25% di probabilità di incontrare un nemico
        int tipo_nemico = rand() % 100;
        Nemico* nemico = tipo_nemico < 60 ? &scheletro.base : &guardia.base;
        const char* nome = tipo_nemico < 60 ? "Scheletro" : "Guardia";

        printf("⚔️ Un nemico %s è apparso\n", nome);
        printf("Scegli un'azione: 1) Combatti, 2) Fuggi\n");
        int scelta;
        scanf("%d", &scelta);
        if (scelta == 1) {
            combatti(giocatore, nemico, nome);
        } else if (scelta == 2) {
            scappa(giocatore);
        } else {
            printf("⚠️ Scelta non valida.\n");
        }
    }

    // Controlla se il giocatore ha trovato un tesoro
    if (giocatore->posizione->tipo_tesoro != 0) {
        printf("💎 Il giocatore ha trovato un tesoro\n");
        printf("Scegli un'azione: 1) Raccogli, 2) Ignora\n");
        int scelta;
        scanf("%d", &scelta);
        if (scelta == 1) {
            switch (giocatore->posizione->tipo_tesoro) {
                case 1:
                    giocatore->p_vita--;
                    printf("💔 Il tesoro era velenoso! Il giocatore ha perso 1 punto vita.\n");
                    break;
                case 2:
                    giocatore->p_vita++;
                    printf("💖 Il tesoro ha guarito il giocatore! Ha guadagnato 1 punto vita.\n");
                    break;
                case 3:
                    giocatore->p_vita += 2;
                    printf("💖 Il tesoro ha aumentato la vita del giocatore! Ha guadagnato 2 punti vita.\n");
                    break;
                case 4:
                    giocatore->dadi_attacco++;
                    printf("⚔️ Il tesoro ha migliorato l'attacco del giocatore! Ha guadagnato 1 dado d'attacco.\n");
                    break;
                case 5:
                    giocatore->dadi_difesa++;
                    printf("🛡️ Il tesoro ha migliorato la difesa del giocatore! Ha guadagnato 1 dado di difesa.\n");
                    break;
            }
        } else {
            printf("❌ Il giocatore ha ignorato il tesoro.\n");
        }
    }

    printf("%s non può avanzare, è alla prossima stanza.\n", giocatore->nome_giocatore);
}

static void stampa_giocatore(struct Giocatore* giocatore) {
    printf("Nome: %s\n", giocatore->nome_giocatore);
    printf("Punti Vita: %d\n", giocatore->p_vita);
    printf("Punti Vita Massimi: %d\n", giocatore->p_vita_max);
    printf("Dadi di Attacco: %d\n", giocatore->dadi_attacco);
    printf("Dadi di Difesa: %d\n", giocatore->dadi_difesa);
    printf("Posizione: %p\n", (void*)giocatore->posizione);
}
static void stampa_zona(struct Stanza* stanza) {
    printf("Stanza: %p\n", (void*)stanza);
    printf("Stanza Successiva: %p\n", (void*)stanza->stanza_successiva);
    printf("Tipo Trabocchetto: %d\n", stanza->tipo_trabocchetto);
    printf("Tesoro Presente: %s\n", stanza->tipo_tesoro!=0? "Sì" : "No");
}
static void cerca_porta_segreta(struct Giocatore* giocatore, int tentativo) {
    int probabilita;
    if (tentativo == 1) {
        probabilita = 33;
    } else if (tentativo == 2) {
        probabilita = 20;
    } else if (tentativo == 3) {
        probabilita = 15;
    } else {
        printf("⚠️ Tentativo non valido.\n");
        return;
    }

    int tiro = rand() % 100;
    if (tiro < probabilita) {
        printf("🔍 Hai trovato una porta segreta!\n");
        struct Stanza* nuova_stanza = (struct Stanza*)malloc(sizeof(struct Stanza));
        if (nuova_stanza == NULL) {
            printf("❌ Errore di allocazione della memoria.\n");
            return;
        }
        // Genera i valori della nuova stanza casualmente
        nuova_stanza->stanza_successiva = NULL;
        nuova_stanza->tipo_trabocchetto = rand() % 4;
        nuova_stanza->tipo_tesoro = rand() % 5;

        // Salva la stanza corrente
        struct Stanza* stanza_corrente = giocatore->posizione;

        // Sposta il giocatore nella nuova stanza
        giocatore->posizione = nuova_stanza;
        printf("🚪 Il giocatore si è spostato nella nuova stanza segreta.\n");

        // Esegui le azioni nella stanza segreta (aggiungi qui il codice per le azioni nella stanza segreta)

        // Ritorna alla stanza precedente
        giocatore->posizione = stanza_corrente;
        printf("↩️ Il giocatore è tornato alla stanza precedente.\n");

        // Libera la memoria della stanza segreta
        free(nuova_stanza);
        printf("🗑️ La stanza segreta è stata liberata dalla memoria.\n");
    } else {
        printf("❌ Non hai trovato nessuna porta segreta.\n");
    }
}
//Definzione della Funzione combatti
static void combatti(struct Giocatore* giocatore, Nemico *nemico,const char *tipo_nemico){
    printf("Il combattimento è iniziato\n");
    printf("Il nemico è un %s\n",tipo_nemico);
    while(giocatore->p_vita > 0 && nemico->p_vita > 0){
        printf("\nLancio per decidere chi attacca per primo...\n");
        int tiro_giocatore=rand()%6+1;
        int tiro_nemico=rand()%6+1;
        printf("%s tira: %d\n", giocatore->nome_giocatore, tiro_giocatore);
        printf("%s tira: %d\n", nemico->nome, tiro_nemico);

        if(tiro_giocatore>tiro_nemico){
            printf("%s il giocatore attacca per primo", giocatore->nome_giocatore);
            esegui_attacco(giocatore, nemico);
            G_attacante=true;
            if (nemico->p_vita > 0) esegui_attacco(giocatore, nemico);
        }else {
            printf("%s il nemico attacca per primo", nemico->nome);
            esegui_attacco(giocatore, nemico);
            G_attacante=false;
            if (giocatore->p_vita > 0) esegui_attacco(giocatore, nemico);
        }

    }
    if(giocatore->p_vita<=0){
        printf("%s è morto\n", giocatore->nome_giocatore);
        turno_attivo=false;
        return;
    }else if(nemico->p_vita<=0){ 
        printf("%s ha sconfitto il nemico\n", giocatore->nome_giocatore);
        giocatore->p_vita++; // Bonus: guadagna 1 punto vita
        if (strcmp(tipo_nemico, "Jaffar") == 0) {
            printf("Il giocatore ha sconfitto Jaffar! Il gioco è terminato.\n");
            termina_gioco();
            return;
        }
        return;
    }
}
static void esegui_attacco(struct Giocatore*Giocatore, Nemico *nemico){
    if(G_attacante){
    printf("%s attacca\n", Giocatore->nome_giocatore);
    int attacchi_riusciti = 0;
    int difese_riuscite = 0;

    // Lancia i dadi per l'attaccante
    for (int i = 0; i < Giocatore->dadi_attacco; i++) {
        int tiro = rand() % 6 + 1;
        if (tiro >= 4) {
            if (tiro == 6) {
                attacchi_riusciti += 2; // Critico d'attacco
            } else {
                attacchi_riusciti++;
            }
        }
    }
    printf("%s ha ottenuto %d colpi riusciti\n", Giocatore->nome_giocatore, attacchi_riusciti);

    // Lancia i dadi per il difensore
    for (int i = 0; i < nemico->dadi_difesa; i++) {
        int tiro = rand() % 6 + 1;
        if (tiro >= 4) {
            if (tiro == 6) {
                difese_riuscite += 2; // Critico di difesa
            } else {
                difese_riuscite++;
            }
        }
    }
    printf("%s ha ottenuto %d difese riuscite\n", nemico->nome, difese_riuscite);

    // Calcola i danni inflitti
    int danni = attacchi_riusciti - difese_riuscite;
    if (danni > 0) {
        nemico->p_vita -= danni;
        printf("%s ha perso %d punti vita\n", nemico->nome, danni);
    } else {
        printf("Il nemico non ha subito danni\n");
    }
    }else{
        printf("%s attacca\n", nemico->nome);
        int attacchi_riusciti = 0;
        int difese_riuscite = 0;

        // Lancia i dadi per l'attaccante
        for (int i = 0; i < nemico->dadi_attacco; i++) {
            int tiro = rand() % 6 + 1;
            if (tiro >= 4) {
                if (tiro == 6) {
                    attacchi_riusciti += 2; // Critico d'attacco
                } else {
                    attacchi_riusciti++;
                }
            }
        }
        printf("%s ha ottenuto %d colpi riusciti\n", nemico->nome, attacchi_riusciti);

        // Lancia i dadi per il difensore
        for (int i = 0; i < Giocatore->dadi_difesa; i++) {
            int tiro = rand() % 6 + 1;
            if (tiro >= 4) {
                if (tiro == 6) {
                    difese_riuscite += 2; // Critico di difesa
                } else {
                    difese_riuscite++;
                }
            }
        }
        printf("%s ha ottenuto %d difese riuscite\n", Giocatore->nome_giocatore, difese_riuscite);

        // Calcola i danni inflitti
        int danni = attacchi_riusciti - difese_riuscite;
        if (danni > 0) {
            Giocatore->p_vita -= danni;
            printf("%s ha perso %d punti vita\n", Giocatore->nome_giocatore, danni);
        } else {
            printf("Il giocatore non ha subito danni\n");
        }
    }
}
static void scappa(struct Giocatore* giocatore) {
    printf("%s è fuggito dalla stanza\n", giocatore->nome_giocatore);
    giocatore->posizione = giocatore->posizione->stanza_precedente;
    turno_attivo = false;
}
// Funzione per passare il turno
static void passa(int *indice_turno, int n_giocatori) {
    printf("Turno passato da %s.\n", giocatori[indice_turno[0]]->nome_giocatore);

    // Passa al prossimo giocatore
    indice_turno[0] = (indice_turno[0] + 1) % n_giocatori;

    printf("Ora è il turno di %s.\n", giocatori[indice_turno[0]]->nome_giocatore);
}
void gioca() {
    if (!gioco_impostato) {
        printf("⚠️ Il gioco non è stato impostato. Usa imposta_gioco() prima di iniziare.\n");
        return;
    }
    
    if (!mappa_creata) {
        printf("⚠️ La mappa non è stata creata. Crea almeno una stanza per iniziare.\n");
        return;
    }

    printf("\n🎮 Inizio del gioco!\n");

    // Array per memorizzare l'ordine casuale dei giocatori
    int ordine[n_giocatori];
    for (int i = 0; i < n_giocatori; i++) {
        ordine[i] = i;
    }

    // Mischiamo l'ordine dei turni
    mescola_turni(ordine, n_giocatori);

    turno_attivo = true;
    int turno_corrente = 0; // Indice del giocatore attuale nel turno mescolato

    while (turno_attivo) {
        int indice_giocatore = ordine[turno_corrente];
        struct Giocatore* giocatore_corrente = giocatori[indice_giocatore];

        if (giocatore_corrente->p_vita <= 0) {
            printf("💀 %s è stato sconfitto e salta il turno.\n", giocatore_corrente->nome_giocatore);
            turno_corrente = (turno_corrente + 1) % n_giocatori;
            continue;
        }

        printf("\n🎲 Turno di %s\n", giocatore_corrente->nome_giocatore);
        printf("Scegli un'azione:\n");
        printf("1. Avanza\n");
        printf("2. Stampa informazioni giocatore\n");
        printf("3. Stampa informazioni zona\n");
        printf("4. Cerca una porta segreta\n");
        printf("5. Passa il turno\n");
        printf("6. Esci dal gioco\n");

        int scelta;
        scanf("%d", &scelta);

        switch (scelta) {
            case 1:
                avanza(giocatore_corrente);
                break;
            case 2:
                stampa_giocatore(giocatore_corrente);
                break;
            case 3:
                stampa_zona(giocatore_corrente->posizione);
                break;
            case 4:
                cerca_porta_segreta(giocatore_corrente, 1);
                break;
            case 5:
                printf("🔄 %s ha passato il turno.\n", giocatore_corrente->nome_giocatore);
                break;
            case 6:
                printf("🚪 Uscita dal gioco.\n");
                turno_attivo = false;
                break;
            default:
                printf("⚠️ Scelta non valida.\n");
                break;
        }

        // Passa al prossimo turno
        if (turno_attivo) {
            turno_corrente = (turno_corrente + 1) % n_giocatori;
        }
    }

    termina_gioco();
}


void termina_gioco() {
    printf("\n🎭 PRINCE OF INERTIA - FINE DEL GIOCO 🎭\n");
    printf("============================================\n");

    // Stampa i giocatori sopravvissuti
    printf("\n🏆 Eroi sopravvissuti:\n");
    bool qualcuno_vivo = false;
    for (int i = 0; i < n_giocatori; i++) {
        if (giocatori[i] != NULL && giocatori[i]->p_vita > 0) {
            printf("🎖️ %s → ❤️ %d punti vita\n", giocatori[i]->nome_giocatore, giocatori[i]->p_vita);
            qualcuno_vivo = true;
        }
    }

    if (!qualcuno_vivo) {
        printf("💀 Tutti i giocatori sono caduti! Jaffar regna supremo... 💀\n");
    } else {
        printf("🎉 Il male è stato sconfitto! Congratulazioni!\n");
    }

    // Libera la memoria e resetta il gioco
    for (int i = 0; i < n_giocatori; i++) {
        if (giocatori[i] != NULL) {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
    cancella_tutte_le_stanze();

    // Reset variabili globali
    pFirst = NULL;
    pUltima = NULL;
    gioco_impostato = false;
    mappa_creata = false;
    turno_attivo = false;
    numero_stanze = 0;
    n_giocatori = 0;

    printf("\n✨ Grazie per aver giocato a Prince of Inertia! ✨\n");
}



void crediti() {
    printf("Crediti: \n");
    printf("Sviluppato da: Yasser Boussalam\n");
}
