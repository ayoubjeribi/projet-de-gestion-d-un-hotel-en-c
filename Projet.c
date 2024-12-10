#include <stdio.h>
#include <stdlib.h> 
#include <string.h>



#define Max 10

// creer structure chambre
typedef struct {
    int numch;
    char type[100];
    float tarif;   // prix de chambre par nuit
    char statut[100];
    }chambre;


chambre Chambre[Max]={
    {1,"single",50.00,"disponible"},
    {2,"double",80.00,"disponible"},
    {3,"double",150.00,"disponible"},
    {4,"single",30.00,"disponible"},
    {5,"double",100.00,"disponible"},
    {6,"single",70.00,"disponible"},
    {7,"double",120.00,"disponible"},
    {8,"single",60.00,"disponible"},
    {9,"double",90.00,"disponible"},
    {10,"single",40.00,"disponible"}
};




typedef struct listeclient{
    int idClient;
    struct listeclient* suivant;
}listeclient;

listeclient *tete = NULL ;


#define MAX_QUEUE_SIZE 1000


typedef struct {
    void *contents[MAX_QUEUE_SIZE];  // Tableau contenant les pointeurs vers les éléments de la file (maintenaces)
    unsigned front, rear;            // Indices pour le début et la fin de la file
}queue;
// on a une files qui contient des maintenaces;



typedef struct maintenance {
    int idClient;              // id du client ayant fait la demande
    int numChambre;            // Numéro de la chambre concernée
    char description[200];     // Description de la maintenance
} maintenance;



void lirereservation() {
    FILE *f;
    int first,second;
    float third;
    f=fopen("reservations.txt","r");
    char str[100];
    while (fgets(str,100,f)!=NULL) {
        sscanf(str,"%d %d %f",&first,&second,&third);
        strcpy(Chambre[second-1].statut,"non disponible");
    }
    fclose(f);
}



void affichage() {
    printf("---------------------------------------------\n");
    printf("           Welcome to Mouradi Hotel         \n");
    printf("---------------------------------------------\n");
    printf("1. Afficher les chambres disponibles\n");
    printf("2. Reserver une chambre\n");
    printf("3. Annuler reservation\n");
    printf("4. Afficher clients\n");
    printf("5. ajouter maintenance\n");
    printf("6. afficher maintenances\n");
    printf("7. Quitter\n");

}



void afficherchambredisponibles(){
    lirereservation();
    printf("chambre disponibles:\n");
    for (int i = 0; i < Max; ++i) {
        if (strcmp(Chambre[i].statut,"disponible")==0){
            printf("Num Chambre : %d\n", Chambre[i].numch);
            printf("Type : %s\n", Chambre[i].type);
            printf("Tarif par nuit : %.2f\n", Chambre[i].tarif);
            printf("\n");
        }
    }
}



listeclient* ajouterclient(listeclient* tete,int id) {
    // on ajoute un nouveau client referé par son id en tete de la liste chainée
    listeclient* nv=malloc(sizeof(listeclient));
    nv->idClient=id;
    nv->suivant=tete;
    return nv;
}



void afficherclient(listeclient* tete) {
    listeclient* temp=tete;
    while (temp!=NULL) {
        printf("Id client : %d\n", temp->idClient);
        temp=temp->suivant;
    }
}



FILE *res;

void ajouterreservation() {
    int idc,numch,nmbnuits;
    printf("Inserer votre Id de client :\n");
    scanf("%d",&idc);
    printf("Inserer le nombre de chambre a reserver:\n");
    scanf("%d",&numch);
    printf("Inserer le nombre de nuits:\n");
    scanf("%d",&nmbnuits);
    for (int i = 0; i < Max; ++i) {
        if (numch == Chambre[i].numch && strcmp(Chambre[i].statut,"disponible")==0) {
            strcpy(Chambre[i].statut, "non disponible");
            float prix = Chambre[i].tarif*nmbnuits;
            printf("Reservation reussie:\n");
            printf("le prix total est : %.2f\n",prix);
            res = fopen("reservations.txt","a");
            fprintf(res,"%d %d %.2f \n",idc,numch,prix);
            fclose(res);
            // ajouter le client dans la liste chainée
            tete=ajouterclient(tete,idc);
            return;
        }
    }
    printf("cette chambre n'est pas disponible ou n'existe pas\n");


}


void annulerreservation() {
    int numchamb;
    printf("Inserer le numero de chambre :\n");
    scanf("%d",&numchamb);

    int test=0;
    for (int i = 0; i < Max; ++i) {
        if ((Chambre[i].numch == numchamb)&&(strcmp(Chambre[i].statut,"non disponible")==0)) {
            strcpy(Chambre[i].statut, "disponible");
            test=1;
        }
    }

    if (test==0) {
        printf("Cette chambre n'est pas reservee\n");
    } else {
        printf("la reservation de la chambre est annulee avec succes\n");
    }
    res = fopen("reservations.txt","r");
    char str[100];
    FILE *temp;  // fichier temporaire
    temp = fopen("temporaire.txt","w");
    while (fgets(str,100,res)) {
        int first,second,third;
        sscanf(str,"%d %d %.2f",&first,&second,&third);
        if (second != numchamb) {
            fprintf(temp,"%s",str);
        }
    }
    fclose(temp);
    fclose(res);
    remove("reservations.txt");
    rename("temporaire.txt","reservations.txt");

}



void queue_new(queue* q) {
    q->rear=q->front=0;
}
//on a une file de maintenaces



void enqueue(queue* q, int idclient, int numChambre, const char* description) {
    maintenance* nouvelledemande = malloc(sizeof(maintenance));
    nouvelledemande->idClient=idclient;
    nouvelledemande->numChambre=numChambre;
    strcpy(nouvelledemande->description, description);
    q->contents[q->rear]=nouvelledemande;
    q->rear=(q->rear+1)%MAX_QUEUE_SIZE;
    printf("Demande de maintenance ajoutee avec succes!\n");

}



void ajoutermaintenance(queue* q) {
    int idClient, numChambre;
    char description[200];
    printf("entrer id du client :\n");
    scanf("%d", &idClient);

    printf("entrer numero de chambre a maintenir :\n");
    scanf("%d", &numChambre);

    printf("entrez la description de la maintenance :\n");
    getchar();  // Pour consommer le caractère de nouvelle ligne restant après scanf
    fgets(description, sizeof(description), stdin);
    enqueue(q, idClient, numChambre, description);
}



void affichermanitenances(queue* q) {
    if (q->rear==q->front) {
        printf("Aucune demande de maintenance dans la file.\n");
        return;
    }
    else {
        int x=q->front;
        while (x != q->rear) {
            maintenance* M = q->contents[x];  // recuperer pointeur sur une demande de maintenance
            int idclient = M->idClient;
            int numChambre = M->numChambre;
            char description[200];
            strcpy(description, M->description);

            // Affiche les détails de la demande de maintenance
            printf("id de client: %d\n",idclient);
            printf("num chambre: %d\n",numChambre);
            printf("description: %s\n",description);
            x++;
        }

    }


}



int main(){
    queue q;
    queue_new(&q);
    int  n;
    do {
        affichage();
        scanf("%d",&n);
        switch (n) {
            case 1:
                afficherchambredisponibles();
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n");
                getchar();
                getchar();
                break;

            case 2:
                ajouterreservation();
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n ");
                getchar();
                getchar();
                break;

            case 3:
                annulerreservation();
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n ");
                getchar();
                getchar();
                break;

            case 5:
                ajoutermaintenance(&q);
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n");
                getchar();
                getchar();
                break;
            case 4:
                afficherclient(tete);
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n");
                getchar();
                getchar();
                break;
            case 6:
                affichermanitenances(&q);
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n");
                getchar();
                getchar();
                break;
            case 7:
                printf("Merci d'avoir visiter notre Hotel\n");
                break;
            default:
                printf("Option Invalide\n");
                printf("appuyer sur n'importe quelle touche pour revenir au menu ....\n");
                getchar();
                getchar();
                break;
        }

    } while (n!=7);



    return 0;

}
