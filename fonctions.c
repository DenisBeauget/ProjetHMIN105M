#include "fonctions.h"
#include "fonctionPart.c"


int nbLigne(FILE *file) {
    int nbLignes = 0;
    int cpt;
    while((cpt = fgetc(file)) != EOF) if(cpt == '\n') nbLignes++;
        rewind(file);
        return nbLignes;
    }

int taille = 0; //Pour plus de modularité plus tard

int initTaille(){
    FILE* file = NULL;
    file = fopen("data.txt","r");
    taille = nbLigne(file);
}

void InitDataFromFile(struct dataStruct *data) {
    FILE* file = NULL;
    file = fopen("data.txt","r");
    taille = nbLigne(file);

    if(file != NULL) {
        for(int i = 0; i < taille; i++) {
           if(fscanf(file,"%s %i %i", data[i].site,&data[i].go,&data[i].cpu) == 0) {
               printf("Erreur fscanf !");
               exit(0);
           }
           data[i].id = i;
           data[i].maxGo = data[i].go;
           data[i].maxCpu = data[i].cpu;

           for (int j = 0; j < 101; ++j)
           {
               strcpy(data[i].LSGo[j].name,"");
               data[i].LSGo[j].quantity = 0;

               strcpy(data[i].LSCpu[j].name,"");
               data[i].LSCpu[j].quantity = 0;
           }
        }
    }
    else {
        printf("erreur file");
    }
}

void initClient(struct clientStruct * client,char * name,int socket, int socketServer, char * ip ,char * port,struct dataStruct * data){
    strcpy(client->name,name);
    client->socket = socket;
    client->socketServer = socketServer;
    strcpy(client->ip,ip);
    strcpy(client->port,port);
    client->data = data;

    for (int i = 0; i < 101; ++i)
    {
        strcpy(client->exclu[i].site,"");
        strcpy(client->exclu[i].type,"");
        client->exclu[i].quantity = 0;
    }
}

void affichageClient(struct clientStruct client){
    printf(GRN"%s : ",client.name);
    printf("socketSend: %i ",client.socket );
    printf("socketRecv %i ",client.socketServer );
    printf("ip: %s ", client.ip );
    printf("port: %s\n"RESET, client.port );
}

void affichageEtat(struct dataStruct* data) {
    printf(YEL"                 --- AFFICHAGE ETAT SYSTEME --- \n \n");
for(int i = 0; i < taille;i++) {
    printf("Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",data[i].site,data[i].go,data[i].maxGo,data[i].cpu,data[i].maxCpu);
}
printf(RED"|||| POUR TOUTE DEMANDE VEUILLEZ LIRE NOTRE DOCUMENTATION |||| \n" RESET);

}

void affichageEtatMaj(struct dataStruct* data) {
        printf(YEL"                 --- MISE A JOURS SYSTEME --- \n \n");
        printf(RED" Une réservation a eu lieu, l'état du système a changer \n \n");
    for(int i = 0; i < taille;i++) {
        printf(YEL"Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",data[i].site,data[i].go,data[i].maxGo,data[i].cpu,data[i].maxCpu);
    }
}


int isInt(char * msg){
    int size = strlen(msg);
    int mult = 1;
    int number = 0;
    for (int i = size-1; i > -1; --i)//je vais de la droite vers la gauche
    {
        if(isdigit(msg[i])){
            number = number + ((msg[i] - 48) * mult); //ex : number = number (char '5' - 48) * 1 sachant que int('5') = 5+48
            mult = mult * 10; //je change de de mutiple  15  = 5*1 + 1*10
       }
       else
        return -1;
    }
    return number;
}
 

int sendFunction(int dsClient, char *message, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = send(dsClient,message + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur sendFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur sendFunction()");
            return -1;
        }
        nbOctet += value;
    }
    return nbOctet;
}

int RecvFunction(int dsClient, char *reponse, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = recv(dsClient,reponse + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur RecvFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur RecvFunction()");
            return -1;
        }
        nbOctet += value;
    }
    return 1;
}

int sendAll(int socket,char * m){
    int size = strlen(m);
    int returnSend; 

    if(sendFunction(socket, (char*)&size, sizeof(size))<1){
        printf("sendSize \n");
        return -1;}

    if(sendFunction(socket, m, size)<1){
        printf("sendMsg\n");
        return -1;}
    return 1;

}

int recvAll(int socket,char * msg){
    int size =0;
    int returnSend; 

    if(RecvFunction(socket, (char*)&size, sizeof(size))<1){
        printf("recvSize \n");
        return -1;}

    char * buffer = malloc(sizeof(char) * (size + 1));

    if(RecvFunction(socket, buffer, size)<1){
        printf("recvMsg \n");
        return -1;}

   // printf("Serveur : %s a envoyé : <%s> \n", client.name, buffer);

    strcpy(msg, buffer);//enregiste le message dans la variable msg
    return 1;

}
int positionSite(struct dataStruct * data, char * site){
    for (int i = 0; i < taille; ++i)
    {
        if( strcmp(data[i].site,site)==0){
            return i;
        }
    }
    return -1;
}

/*MET A JOUR LA STRUCTURE EXLUSIF EN DIMINUANT LE NOMBRE DE DE GO/CPU */
void actionExclu(struct dataStruct* data, char * site, char * type, int value){
    int position = positionSite(data,site);

    if (strcmp(type,"GO")==0)
    {
        data[position].go = data[position].go - value;
    }

    if (strcmp(type,"CPU")==0)
    {
        data[position].cpu = data[position].cpu - value;
    }
}

/*MET A JOUR LA STRUCTURE EXCLUSIF EN AJOUTANT LE NOMBRE DE DE GO/CPU */
void suppressionExclu(struct dataStruct* data, int position, char * type, int value){
    //int position = positionSite(data,site);

    if (strcmp(type,"GO")==0)
    {   
        data[position].go = data[position].go + value;
    }

    if (strcmp(type,"CPU")==0)
    {
        data[position].cpu = data[position].cpu + value;
    }
}




void suppressionSharedClientAll(struct dataStruct* data,char * name){
    for (int i = 0; i < taille; ++i)
    {   int maxGO = maxLSharedType(data , i ,"GO");
        int maxCpu = maxLSharedType(data , i ,"CPU");
        int sizeGo = lSharedSize(data[i].LSGo);
        int sizeCpu = lSharedSize(data[i].LSCpu);
        suppressionSharedClient(data[i].LSGo,name,sizeGo);
        suppressionSharedClient(data[i].LSCpu,name,sizeCpu);
        int nMaxGO = maxLSharedType(data , i ,"GO");
        int nMaxCpu = maxLSharedType(data , i ,"CPU");

        if (maxGO>nMaxGO)
            suppressionExclu(data,i,"GO",maxGO-nMaxGO); //réaloue les go 

        if (maxCpu>nMaxCpu)
            suppressionExclu(data,i,"CPU",maxCpu>nMaxCpu);
    }
}

void suppressionExcluClientAll(struct dataStruct* data,struct clientStruct * client){
    //int position = positionSite(data,client->exclu->site);
    int size = lExcluSize(client->exclu);

    for (int i = 0; i < size; ++i)
    {   int position = positionSite(data,client->exclu[i].site);
        suppressionExclu(data,position,client->exclu[i].type,client->exclu[i].quantity);
    }
}


/*
* FONCTIONS SEND COTÉ CLIENT
*/


int isExcluSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if ((strcmp(m,"e")==0 ||strcmp(m,"E")==0||strcmp(m,"EXCLUSIF")==0||strcmp(m,"exclusif")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"1");
        return 1;
    }

    if ((strcmp(m,"p")==0 ||strcmp(m,"P")==0||strcmp(m,"PARTAGE")==0||strcmp(m,"partage")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"0");
        return 1;
    }

    return -1;

}

int siteSend(char * msg, struct dataStruct *data){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if(positionSite(data,m)<0){ //la ville n'existe pas
        return -1;
    }

    strcpy(msg,m);
    return 1;
}

int typeSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if ((strcmp(m,"cpu")==0 ||strcmp(m,"CPU")==0||strcmp(m,"c")==0||strcmp(m,"C")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"CPU");
        return 1;
    }

    if ((strcmp(m,"go")==0 ||strcmp(m,"GO")==0||strcmp(m,"g")==0||strcmp(m,"G")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"GO");
        return 1;
    }

    return -1;
}

int valueSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if (isInt(m)<1)
    {
        return -1;
    }

    strcpy(msg,m);
    return 1;
}


int SendClient(struct dataStruct* data, struct clientStruct * client,char * size){
    int nbrRequetes=isInt(size);

    for (int i = 0; i <nbrRequetes; ++i)
    {   printf("Requête : %i\n",i+1 ); 
        //printf("exclu?,Site,type,value\n");
        char * isExclu = malloc (20 * sizeof (char));
        char * site =  malloc (20 * sizeof (char));
        char * type  =  malloc (20 * sizeof (char));
        char * value=  malloc (20 * sizeof (char));


        printf(GRN"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        while(isExcluSend(isExclu)<0){
            printf(YEL"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        }

        printf(GRN"Quel ville ?\n"RESET);
        while(siteSend(site,data)<0){
            printf(YEL"Quel ville ?\n"RESET);
        }

        printf(GRN"Cpu(c/C) ou go (g/G) ?\n"RESET);
        while(typeSend(type)<0){
            printf(YEL"Cpu(c/C) ou go (g/G) ?\n"RESET);
        }

        printf(GRN"Combien ?\n"RESET);
        while(valueSend(value)<0){
            printf(YEL"Combien ?\n"RESET);
        }

        if (sendAll(client->socket, isExclu) < 1)
            return -1;

        if (sendAll(client->socket, site) < 1)
            return -1;

        if (sendAll(client->socket, type) < 1)
            return -1;

        if (sendAll(client->socket, value) < 1)
            return -1;

        free(isExclu);
        free(site);
        free(type); 
        free(value);      
    }
    
    return 1;
}

/*FONCTIONS RECV COTÉ SERVEUR*/
int recvServer(struct clientStruct  client,struct recvStruct * recvS ,int size){
    //strcpy(m,"");
    char recv[4][20];
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {   
            char * m = malloc (20 * sizeof (char));
            if (recvAll(client.socketServer,m)<1)
            {
                printf("Client %s : erreur au recv", client.name);
                return -1;
            }
            strcpy(recv[j],m);

            free(m);            
        }  

        strcpy(recvS[i].name,client.name);
        recvS[i].isExclu=isInt(recv[0]);
        strcpy(recvS[i].site,recv[1]); 
        strcpy(recvS[i].type,recv[2]);
        recvS[i].value= isInt(recv[3]);

        printf("Requête %i de %s : ",i+1,recvS[i].name);
        printf("Exlusif?:%i ",recvS[i].isExclu);
        printf("%s ",recvS[i].site);
        printf("%s ",recvS[i].type);
        printf("%i\n",recvS[i].value); 
    }
    return 1;
}

int restantType(struct dataStruct * data,int position,char * type, int value){
    if (strcmp(type,"GO")==0)
    {   
        return(data[position].go-value);
    }

    if (strcmp(type,"CPU")==0)
    {
        return(data[position].cpu-value);
    }
}


void actionExcluAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS){
    int size = lExcluSize(client->exclu);

    actionExclu(data, recvS.site, recvS.type, recvS.value);
    actionExcluClient(client,size, recvS.type, recvS.site,recvS.value);
}



int getValue(struct dataStruct *data, int position,char * type, int posList){
    if (strcmp(type,"GO")==0)
    {   
        return(data[position].LSGo[posList].quantity);
    }

    if (strcmp(type,"CPU")==0)
    {
        return(data[position].LSCpu[posList].quantity);
    }
}

void actionSharedAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS){
    int position= positionSite(data,recvS.site);

    int oldMax = maxLSharedType(data,position,recvS.type);
    //printf("%i\n", oldMax);

    int positionToAdd= isInSharedType(data,position,client,recvS.type);

    int value = recvS.value + getValue(data, position,recvS.type,positionToAdd);
    actionShared(data,position,positionToAdd,client->name, recvS.type, value);

    printf("%s : %s position %s %i, value %i/%i \n", client->name,data[position].site,recvS.type ,positionToAdd , recvS.value,value);

    int newMax = maxLSharedType(data,position,recvS.type);
    printf("%i\n", newMax);

    if (oldMax<newMax)
    {
        actionExclu(data, recvS.site, recvS.type, (newMax-oldMax));
    }

}




void actionAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct * recvS,int size){
    for (int i = 0; i < size; ++i)
    {
        if (recvS[i].isExclu==1)
        {
            actionExcluAll(data,client,recvS[i]);
        }

        if (recvS[i].isExclu==0){
            actionSharedAll(data,client,recvS[i]);
        }
    }
}






void initMutex(pthread_mutex_t * verrou) {
    pthread_mutex_init(verrou,NULL);
}


int P(int semid, int semnum, int n) {   //-1
    struct sembuf buf = {semnum, -n, 0};
    return semop(semid, &buf, 1);
}


// incrémentation du sémpahore
int V(int semid, int semnum, int n) { //+1
    struct sembuf buf = {semnum, n, 0};
    return semop(semid, &buf, 1);
}


// attente du 0
int Z(int semid, int semnum) { //0
    struct sembuf buf = {semnum, 0, 0};
    return semop(semid, &buf, 1);
}














int recvAll2(int socket, char * buf, int len) { // recvAll function
  int remaining = len;
  key_t keysem = ftok("shmfile", 10);
  int idSem = semget(keysem, 7, IPC_CREAT | 0666);
  while (remaining) {
    int received = recv(socket, buf, remaining, 0);
    if (received <= 0) {
      semctl(idSem, 5, SETVAL, semctl(idSem, 5, GETVAL) - 1);
     // printf( "[Quit] Client %s:%d disconnected !\n", ip, port);
      exit(1);
    }
    buf += received;
    remaining -= received;
  }
  return 0;
}

int recvWithSize2(int sock, char * data) {
  char sizeToRecv[4];
  if (recvAll2(sock, sizeToRecv, sizeof(sizeToRecv)) == 1) {
    return -1;
  };
  if (recvAll2(sock, data, *((int * ) sizeToRecv)) == 1) {
    return -1;
  };
  return *((int * ) sizeToRecv);
}

int sendall2(int sock, const char* data, int data_length){
        int bytessend = 0;
        while ( bytessend < data_length) {
                int result = send(sock, data + bytessend, data_length - bytessend, 0);
                if ( result == -1) { perror("send error"); exit(1);}    // not exit if errno == EAGAIN
                bytessend += result;
        }
        return bytessend;
}


int sendWithSize2(int sock, const char* data, int data_length){
        char sizeToSend[4];
        *((int*) sizeToSend ) = data_length;
        sendall2(sock, sizeToSend,sizeof(sizeToSend) );
        sendall2(sock, data,data_length);
        return 1;
}


void * UpdateClient(void *param) {

    while(1){
        //printf("J'attend un message d'amour while\n");
        struct gestionSendUpdate * p = (struct gestionSendUpdate*) param;
        printf("J'attend un message d'amour\n");
        //recvAll(p->socket,p->msg);
        recvWithSize2(p->socket, p->msg);
        printf("reçus");
        printf("%s\n",p->msg);
        //printf("%s\n", p->msg);

        pthread_cond_broadcast(p->cond);

    }
}

void * UpdateServer(void *param) {
    struct SendUpdate * p = (struct SendUpdate*) param;

    key_t keySem = ftok("sharedSem.txt",10);
    int idSem = semget(keySem,taille,IPC_CREAT | 0666);

    while(1){
        if ( semctl(idSem, 1, GETVAL) > 0 ) {

            printf("Envoie valeur %i \n" ,semctl(idSem, 1, GETVAL) );
            sendWithSize2(p->socket, "PING DE MAJ", sizeof("PING DE MAJ"));
            printf("envoyé\n");
            //sendAll(p->socket,"COUCOU");
            P(idSem,1,1);
            //Z(idSem,2);
        }
        /*sleep(3);
        printf("pouet\n");
        printf("0 : %i \n" ,semctl(idSem, 0, GETVAL) );
        printf("1 : %i \n" ,semctl(idSem, 1, GETVAL) );
        printf("2 : %i \n" ,semctl(idSem, 2, GETVAL) );
        printf("3 : %i \n" ,semctl(idSem, 3, GETVAL) );*/


    }
}





void printSharedData(struct dataStruct * data ){
    for (int i = 0; i < taille; ++i)
    {   printf("%s\n",data[i].site );
        int sizeGo= lSharedSize(data[i].LSGo);
        int sizeCpu= lSharedSize(data[i].LSCpu);
        printShared(data[i].LSGo, sizeGo);
        printShared(data[i].LSCpu,sizeCpu);
    }    
}