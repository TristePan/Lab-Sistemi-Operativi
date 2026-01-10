void bonus_mezzelfo(Personaggio *p) {
    char *scelta = scelta = (char *)calloc(1, sizeof(char) * 10);
    char gia_scelta[] = "carisma";
    if(scelta == NULL) {
        printf("Errore: memoria esaurita...");
        return;
    }

    int statistiche_scelte = 0;

    printf("Il bonus +2 a Carisma è stato inserito automaticamente.\n");
    printf("Inserisci le due caratteristiche diverse da Carisma a cui dare il +1: ");

    while(statistiche_scelte < 2) {
        printf("Seleziona le caratteristiche: ");
        fgets(scelta, 15, stdin);
        scelta[strcspn(scelta, "\n")] = 0; // Rimuove l'invio

        if(strcasecmp(scelta, gia_scelta) == 0) {
            printf("Scelta non valida... Il carisma non è una scelta valida");
            continue;
        }
        if(strcasecmp(scelta, "Forza") == 0) {
            p -> stats.forza += 1;
        } else if(strcasecmp(scelta, "Destrezza") == 0) {
            p -> stats.destrezza += 1;
        } else if(strcasecmp(scelta, "Costituzione") == 0) {
            p -> stats.costituzione += 1;
        } else if(strcasecmp(scelta, "Intelligenza") == 0) {
            p -> stats.intelligenza += 1;
        } else if(strcasecmp(scelta, "Saggezza") == 0) {
            p -> stats.saggezza += 1;
        }

        *gia_scelta = scelta;
        statistiche_scelte++;
        printf("Punto assegnato!\n");
    }
    while(getchar() != '\n');
}