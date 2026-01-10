#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

typedef enum {
    Dragonide,
    Elfo,
    Forgiato,
    Gnomo,
    Halfling,
    Mezzelfo,
    Mezzorco,
    Nano,
    Umano,
    Tiefling
}Razze;

typedef enum {
    Barbaro,
    Bardo,
    Chierico,
    Druido,
    Guerriero,
    Ladro,
    Mago,
    Monaco,
    Paladino,
    Ranger,
    Stregone,
    Warlock
}Classi;

typedef struct {
    int forza;
    int destrezza;
    int costituzione;
    int intelligenza;
    int saggezza;
    int carisma;
}Stats;

typedef struct {
    char nome[50];
    int livello;
    Razze razza;
    Classi classe;
    Stats stats;
    int hp;
    float velocità;
}Personaggio;

const char *traduttore_razze(Razze r) {
    switch(r) {
        case Dragonide: 
            return "Dragonide";
        case Elfo:
            return "Elfo";
        case Forgiato:
            return "Forgiato";
        case Gnomo:
            return "Gnomo";
        case Halfling:
            return "Halfling";
        case Mezzelfo:
            return "Mezzelfo";
        case Mezzorco:
            return "Mezzorco";
        case Nano:
            return "Nano";
        case Umano:
            return "Umano";
        case Tiefling:
            return "Tiefling";
        default:
            return "Sconosciuto";
    }
}

const char *traduttore_classe(Classi c) {
    switch(c) {
        case Barbaro: 
            return "Barbaro";
        case Bardo:
            return "Bardo";
        case Chierico:
            return "Chierico";
        case Druido:
            return "Druido";
        case Guerriero:
            return "Guerriero";
        case Ladro:
            return "Ladro";
        case Mago:
            return "Mago";
        case Monaco:
            return "Monaco";
        case Paladino:
            return "Paladino";
        case Ranger:
            return "Ranger";
        case Stregone:
            return "Stregone";
        case Warlock:
            return "Warlock";
        default:
            return "Sconosciuto";
    }
}

void inserisci_nome(Personaggio *p) {
    printf("Inserisci il nome dell'eroe: ");
    fgets(p -> nome, sizeof(p -> nome), stdin); // fgets per nome con spazi
    p -> nome[strcspn(p -> nome, "\n")] = 0;

    if(p -> nome[0] != '\0') {
        p -> nome[0] = toupper(p -> nome[0]);
        
        for(int i = 1; p -> nome[i] != '\0'; i++) {
            p -> nome[i] = tolower(p -> nome[i]);
        }
    }
}

void inserisci_livello(Personaggio *p) {
    int liv_tmp;
    do {
        printf("Inserisci il livello del personaggio (1-20): ");
        
        if(scanf("%d", &liv_tmp) != 1) {
            printf("Errore: devi inserire un numero intero...\n");
            while(getchar() != '\n');
            liv_tmp = 0; // Serve per forzare il fallimento del ciclo e riprovare
            continue;
        }

        if(liv_tmp < 1 || liv_tmp > 20) {
            printf("%d\n", liv_tmp);
            printf("Non puoi essere livello 0 o superiore al 20, dai si sa...\n");
        }
    }while (liv_tmp < 1 || liv_tmp > 20);

    p -> livello = liv_tmp;

    while(getchar() != '\n');

}

void inserisci_razza(Personaggio *p) {
    char *input_razza;
    input_razza = (char *)calloc(1, sizeof(char) * 30);
    
    int valida = 0;

    if(input_razza == NULL) {
        printf("Errore: memoria esaurita...");
        return;
    }

    do {
        printf("\n--- SELEZIONE LA RAZZA ---\n");
        printf("- Dragonide\n");
        printf("- Elfo\n");
        printf("- Forgiato\n");
        printf("- Gnomo\n");
        printf("- Halfling\n");
        printf("- Mezzelfo\n");
        printf("- Mezzorco\n");
        printf("- Nano\n");
        printf("- Umano\n");
        printf("- Tiefling\n");
        printf("Scegli la tua razza: ");
        fgets(input_razza, 15, stdin);
        input_razza[strcspn(input_razza, "\n")] = 0; // Rimuove l'invio

        // Da qui il confronto delle stringhe
        if(strcasecmp(input_razza, "Dragonide") == 0) {
            p -> razza = Dragonide;
            valida = 1;
        } else if(strcasecmp(input_razza, "Elfo") == 0) {
            p -> razza = Elfo;  
            valida = 1;
        } else if(strcasecmp(input_razza, "Forgiato") == 0) { 
            p -> razza = Forgiato;
            valida = 1;
        } else if(strcasecmp(input_razza, "Gnomo") == 0) {
            p -> razza = Gnomo;
            valida = 1;
        } else if(strcasecmp(input_razza, "Halfling") == 0) {
            p -> razza = Halfling;
            valida = 1;
        } else if(strcasecmp(input_razza, "Mezzelfo") == 0) {
            p -> razza = Mezzelfo;
            valida = 1;
        }  else if(strcasecmp(input_razza, "Mezzorco") == 0) {
            p -> razza = Mezzorco;
            valida = 1;
        }  else if(strcasecmp(input_razza, "Nano") == 0) {
            p -> razza = Nano;
            valida = 1;
        }  else if(strcasecmp(input_razza, "Umano") == 0) {
            p -> razza = Umano;
            valida = 1;
        }  else if(strcasecmp(input_razza, "Tiefling") == 0) {
            p -> razza = Tiefling;
            valida = 1;
        } else {
            printf("\nRazza non valida...\n");
        }
    } while(!valida);

    free(input_razza);
}

void inserisci_classe(Personaggio *p) {
    char *input_classe;
    input_classe = (char *)calloc(1, sizeof(char));
    int valida = 0;

    if(input_classe == NULL) {
        printf("Errore... Memoria insufficiente");
        return;
    }

    do {
        printf("\n--- SELEZIONA LA CLASSE ---\n");
        printf("- Barbaro   | Bardo\n");
        printf("- Chierico  | Druido\n");
        printf("- Guerriero | Ladro\n");
        printf("- Ladro     | Mago\n");
        printf("- Monaco    | Paladino\n");
        printf("- Ranger    | Stregone\n");
        printf("- Warlock\n");
        printf("Scegli la tua classe: ");
        fgets(input_classe, 10, stdin);
        input_classe[strcspn(input_classe, "\n")] = 0; // Rimuove l'invio

        // Da qui il confronto delle stringhe
        if(strcasecmp(input_classe, "Barbaro") == 0) {
            p -> classe = Barbaro;
            valida = 1;
        } else if(strcasecmp(input_classe, "Bardo") == 0) {
            p -> classe = Bardo;
            valida = 1;
        } else if(strcasecmp(input_classe, "Chierico") == 0) {
            p -> classe = Chierico;
            valida = 1;
        } else if(strcasecmp(input_classe, "Druido") == 0) {
            p -> classe = Druido;
            valida = 1;
        } else if(strcasecmp(input_classe, "Guerriero") == 0) {
            p -> classe = Guerriero;
            valida = 1;
        } else if(strcasecmp(input_classe, "Ladro") == 0) {
            p -> classe = Ladro;
            valida = 1;
        } else if(strcasecmp(input_classe, "Mago") == 0) {
            p -> classe = Mago;
            valida = 1;
        } else if(strcasecmp(input_classe, "Monaco") == 0) {
            p -> classe = Monaco;
            valida = 1;
        } else if(strcasecmp(input_classe, "Paladino") == 0) {
            p -> classe = Paladino;
            valida = 1;
        } else if(strcasecmp(input_classe, "Ranger") == 0) {
            p -> classe = Ranger;
            valida = 1;
        } else if(strcasecmp(input_classe, "Stregone") == 0) {
            p -> classe = Stregone;
            valida = 1;
        } else if(strcasecmp(input_classe, "Warlock") == 0) {
            p -> classe = Warlock;
            valida = 1;
        } else {
            printf("\nClasse non valida...\n");
        }
    } while(!valida);

    free(input_classe);
}

int modificatore_caratteristica(int punteggio) {
    return (punteggio - 10) / 2;
}

void bonus_mezzelfo(Personaggio *p) {
    char *scelta = (char *)calloc(1, sizeof(char) * 10);;
    char prima_scelta[15] = "";
    if(scelta == NULL) {
        printf("Errore: memoria esaurita...");
        return;
    }

    int statistiche_scelte = 0;

    printf("\n--- BONUS MEZZELFO ---\n");
    printf("Il bonus +2 a Carisma e' stato inserito automaticamente.\n");
    printf("Inserisci le due caratteristiche diverse da Carisma a cui dare il +1: ");

    while(statistiche_scelte < 2) {
        printf("Seleziona le caratteristiche [%d/2]: ", statistiche_scelte + 1);
        fgets(scelta, 15, stdin);
        scelta[strcspn(scelta, "\n")] = 0; // Rimuove l'invio
        int valida = 0;

        if(strcasecmp(scelta, "Carisma") == 0) {
            printf("Scelta non valida... Il carisma non e' una scelta valida...\n");
            continue;
        }
        if(strcasecmp(scelta, prima_scelta) == 0) {
            printf("Hai gia' scelto questa statistica. Scegline un'altra!\n");
            continue;
        }
        if(strcasecmp(scelta, "Forza") == 0) {
            p -> stats.forza += 1;
            valida = 1;
        } else if(strcasecmp(scelta, "Destrezza") == 0) {
            p -> stats.destrezza += 1;
            valida = 1;
        } else if(strcasecmp(scelta, "Costituzione") == 0) {
            p -> stats.costituzione += 1;
            valida = 1;
        } else if(strcasecmp(scelta, "Intelligenza") == 0) {
            p -> stats.intelligenza += 1;
            valida = 1;
        } else if(strcasecmp(scelta, "Saggezza") == 0) {
            p -> stats.saggezza += 1;
            valida = 1;
        }

        if(valida) {
            // *prima_scelta = scelta;
            strncpy(prima_scelta, scelta, sizeof(prima_scelta));
            // strcpy(prima_scelta, scelta); // memorizza la scelta
            statistiche_scelte++;
            printf("Punto dato a %s!\n", scelta);
        } else {
            printf("Statistica non valida...Riprova\n");
        }
        
    }
    // while(getchar() != '\n');
    free(scelta);
}
void bonus_umano(Personaggio *p) {
    char *scelta = (char *)calloc(1, sizeof(char) * 5);
    char *scelta_umano = (char *)calloc(1, sizeof(char) * 5);
    char prima_scelta[15] = "";
    int statistiche_scelte = 0;
    
    if(scelta_umano == NULL) {
        printf("Errore: memoria non allocata...\n");
        return;
    }    
    if(scelta == NULL) {
        printf("Errore: memoria non allocata...\n");
        return;
    }

    printf("\n--- BONUS UMANO ---\n");
    printf("Vuoi fare l'umano normale o l'umano variante: ");
    fgets(scelta_umano, 15, stdin);
    scelta_umano[strcspn(scelta, "\n")] = 0; // Rimuove l'invio
    
    if(strcasecmp(scelta_umano, "Variante")){
        while(statistiche_scelte < 2) {
            printf("Seleziona le caratteristiche [%d/2]: ", statistiche_scelte + 1);
            fgets(scelta, 15, stdin);
            scelta[strcspn(scelta, "\n")] = 0; // Rimuove l'invio
            int valida = 0;
            
            if(strcasecmp(scelta, prima_scelta) == 0) {
                printf("Hai gia' scelto questa statistica. Scegline un'altra!\n");
                continue;
            }
            if(strcasecmp(scelta, "Forza") == 0) {
                p -> stats.forza += 1;
                valida = 1;
            }else if(strcasecmp(scelta, "Destrezza") == 0) {
                p -> stats.destrezza += 1;
                valida = 1;
            }else if(strcasecmp(scelta, "Costituzione") == 0) {
                p -> stats.costituzione += 1;
                valida = 1;
            }else if(strcasecmp(scelta, "Intelligenza") == 0) {
                p -> stats.intelligenza += 1;
                valida = 1;
            }else if(strcasecmp(scelta, "Saggezza") == 0) {
                p -> stats.saggezza += 1;
                valida = 1;
            }else if(strcasecmp(scelta, "Carisma") == 0) {
                p -> stats.carisma += 1;
                valida = 1;
            }

            if(valida) {
                strncpy(prima_scelta, scelta, sizeof(prima_scelta));
                // strcpy(prima_scelta, scelta); // memorizza la scelta
                statistiche_scelte++;
                printf("--- Punto dato a %s!\n", scelta);
            } else {
                printf("Statistica non valida...Riprova\n");
            }

        }

        free(scelta_umano);
        free(scelta);
    } else if(strcasecmp(scelta_umano, "Normale")) {
        p -> stats.carisma += 1;
        p -> stats.costituzione += 1;
        p -> stats.destrezza += 1;
        p -> stats.forza += 1;
        p -> stats.intelligenza += 1;
        p -> stats.saggezza += 1;

        free(scelta_umano);
        free(scelta);
    }
}
int bonus_razziali(Personaggio *p) {
    if(p -> razza == Dragonide) {
        p -> velocità = 9.0;
        p -> stats.forza += 2;
        p -> stats.carisma += 1;
    }
    if(p -> razza == Elfo) {
        p -> velocità = 9.0;
        p -> stats.destrezza += 2;
    }
    if(p -> razza == Forgiato) {
        p -> velocità = 9.0;
        p -> stats.destrezza += 2;
    }
    if(p -> razza == Gnomo) {
        p -> velocità = 7.5;
        p -> stats.intelligenza += 2;
    }
    if(p -> razza == Halfling) {
        p -> velocità = 7.5;
        p -> stats.destrezza += 2;
    }
    if(p -> razza == Mezzelfo) {
        p -> velocità = 9.0;
        p -> stats.carisma += 2;
        bonus_mezzelfo(p);
    }
    if(p -> razza == Mezzorco) {
        p -> velocità = 9.0;
        p -> stats.forza += 2;
        p -> stats.costituzione += 1;
    }
    if(p -> razza == Nano) {
        p -> velocità = 7.5;
        p -> stats.costituzione += 2;
    }
    if(p -> razza == Umano) {
        p -> velocità = 9.0;
        bonus_umano(p);
        // p -> stats.carisma += 1;
        // p -> stats.costituzione += 1;
        // p -> stats.destrezza += 1;
        // p -> stats.forza += 1;
        // p -> stats.intelligenza += 1;
        // p -> stats.saggezza += 1;
    }
    if(p -> razza == Tiefling) {
        p -> velocità = 9.0;
        p ->stats.carisma += 2;
        p ->stats.intelligenza += 1;
    }
}

void inserisci_stats(Personaggio *p) {
    printf("--- ASSEGNA LE STATS ---");

    printf("\nInserisci la Forza: ");
    scanf("%d", &p -> stats.forza);

    printf("\nInserisci la Destrezza: ");
    scanf("%d", &p -> stats.destrezza);

    printf("\nInserisci la Costituzione: ");
    scanf("%d", &p -> stats.costituzione);

    printf("\nInserisci la Intelligenza: ");
    scanf("%d", &p -> stats.intelligenza);

    printf("\nInserisci la Saggezza: ");
    scanf("%d", &p -> stats.saggezza);

    printf("\nInserisci la Carisma: ");
    scanf("%d", &p -> stats.carisma);

    while(getchar() != '\n');   
}

void imposta_hp(Personaggio *p) {
    int dado_vita;

    // Determiniamo il dado vita in base alla classe
    switch(p -> classe) {
        case Barbaro: 
            dado_vita = 12;
            break;
        case Guerriero: case Paladino: case Ranger:
            dado_vita = 10;
            break;
        case Bardo: case Chierico: case Druido: case Ladro: case Monaco: case Warlock:
            dado_vita = 8;
            break;
        case Mago: case Stregone:
            dado_vita = 6;
            break;
        default:
            break;
    }

    int mod_cost = modificatore_caratteristica(p -> stats.costituzione);

    p -> hp = dado_vita + mod_cost;
    if(p -> hp < 1)
        p -> hp = 1;
}

void salva_pg(Personaggio *p) {
    FILE *file = fopen("Scheda_personaggio.txt", "w");

    if (file == NULL) {
        printf("Errore nell'apertura del file");
        return;
    }

    fprintf(file, "--- SCHEDA PERSONAGGIO ---");
    fprintf(file, "\nNOME: %s", p -> nome);
    fprintf(file, "\nLIVELLO: %d", p -> livello);
    fprintf(file, "\nRAZZA: %s", traduttore_razze(p -> razza));
    fprintf(file, "\nCLASSE: %s", traduttore_classe(p -> classe));

    // --- STATISTICHE ---
    fprintf(file, "\n\n--- CARATTERISTICHE ---");
    fprintf(file, "\nForza:         %2d (Mod: %+d)", p -> stats.forza, modificatore_caratteristica(p -> stats.forza));
    fprintf(file, "\nDestrezza:     %2d (Mod: %+d)", p -> stats.destrezza, modificatore_caratteristica(p -> stats.destrezza));
    fprintf(file, "\nCostituzione:  %2d (Mod: %+d)", p -> stats.costituzione, modificatore_caratteristica(p -> stats.costituzione));
    fprintf(file, "\nIntelligenza:  %2d (Mod: %+d)", p -> stats.intelligenza, modificatore_caratteristica(p -> stats.intelligenza));
    fprintf(file, "\nSaggezza:      %2d (Mod: %+d)", p -> stats.saggezza, modificatore_caratteristica(p -> stats.saggezza));
    fprintf(file, "\nCarisma:       %2d (Mod: %+d)\n", p -> stats.carisma, modificatore_caratteristica(p -> stats.carisma));

    // --- HP ---
    fprintf(file, "\n--- I TUOI HP ---\n");
    fprintf(file, "Hp:              %2d", p -> hp);  
    
    // --- Velocità ---
    fprintf(file, "\n--- VELOCITÀ ---\n");
    fprintf(file, "Velocità:        %g", p -> velocità);

    fclose(file);
    printf("\nScheda creata con successo\n");
}

void crea_nuovo_personaggio(Personaggio *p) {
    // p -> stats = (Stats){0};
    
    inserisci_nome(p);
    inserisci_livello(p);
    inserisci_razza(p);
    inserisci_classe(p);
    inserisci_stats(p);
    bonus_razziali(p);
    imposta_hp(p);

    salva_pg(p);
}


int main(int argc, char *argv[]) {
    Personaggio pg = {0};   
    // pg.stats = (Stats){0};
    crea_nuovo_personaggio(&pg);

    return 0;
}