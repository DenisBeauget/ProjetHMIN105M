
/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lSharedSize(struct LShared * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].name)==0) && (l[i].quantity==0)) //vérifie si le nom et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

/*RENVOIE LA QUANTITY MAX DE LA LISTE PARTAGER*/
int maxLShared(struct LShared * l){
    int max = 0;
    int size = lSharedSize(l);

    if (size==0)
        return max;

    for (int i = 0; i < size; ++i)
    {
        if (max<l[i].quantity)       
            max = l[i].quantity;
    }
    return max;
}

int maxLSharedType(struct dataStruct * data,int position,char * type){
    int max = -1;

    if (strcmp(type,"GO")==0)
    {
        max = maxLShared(data[position].LSGo);
    }

    if (strcmp(type,"CPU")==0)
    {
        max = maxLShared(data[position].LSCpu);
    }
    return max;
}

/*PREMIERE APPARITION DU CLIENT DANS LA LISTE                                               Inutiliser*/
int lSharedPosition(struct LShared * l,char * name){
    int size =  lSharedSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].name,name)==0 && strlen(name)!=0)
            return i;
    }
    return -1;
}


/*SUPPRESSION SUIVANT UNE POSITION */
void suppressionShared(struct LShared * l, int position, int size){
    for (int i = position; i < size; ++i)
    {
        l[i]=l[i+1];
    }
}
/*SUPPRIME UNE VALEUR SELON SA POSITION                                                                                   INUTILISER
void suppressionSharedType(struct dataStruct* data,char * site,char * type,int pos,int size){
    int position = positionSite(data,site);  
    if (strcmp(type,"GO")==0)
    {   
        suppressionShared(data[position].LSGo, pos, size);
    }

    if (strcmp(type,"CPU")==0)
    {
        suppressionShared(data[position].LSCpu, pos, size);
    }
}*/
/*SUPPRESSION DE TOUTE LES APPARITIONS DU CLIENT */
void suppressionSharedClient(struct LShared * l, char* name, int size){
    for (int i = 0; i < size; ++i)
    {
        if(strcmp(name,l[i].name)==0 && strlen(name)!=0){
            suppressionShared(l,i, size);
            i = i -1;
            size = size-1;
        }
    }
}


/*ADD/MISE A JOUR DANS LA LISTE DES RESERVATION PARTAGER DANS LA STRUCTURE PARTAGER*/
void actionShared(struct dataStruct* data,int position,int pos, char * name, char * type, int value){  
    struct LShared l;
    strcpy(l.name , name);
    l.quantity = value;


    if (strcmp(type,"GO")==0)
    {   
        //int size = lSharedSize(data[position].LSGo);
        data[position].LSGo[pos]= l;
    }

    if (strcmp(type,"CPU")==0)
    {
        //int size = lSharedSize(data[position].LSCpu);
        data[position].LSCpu[pos] = l;
    }

}


void printShared(struct LShared * l, int size){
    for (int i = 0; i < size; ++i)
    {
        printf("position %i : %s : %i \n",i, l[i].name, l[i].quantity );
    }
}


int inShared(struct LShared * l, char *name, int size){

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].name,name)==0&&strlen(name)!=0)
        {
            return i;
        }
    }
    return size;
}

int isInSharedType(struct dataStruct * data,int position, struct clientStruct * client ,char * type){

    if (strcmp(type,"GO")==0)
    {   int size = lSharedSize(data[position].LSGo);
        return(inShared(data[position].LSGo, client->name,size));
        printShared(data[position].LSGo,size);
        printf("size = %i\n",size );
    }

    if (strcmp(type,"CPU")==0)
    {
        int size = lSharedSize(data[position].LSCpu);
        return(inShared(data[position].LSCpu, client->name,size));
        printShared(data[position].LSCpu,size);
        printf("size = %i\n",size );
    }
}










/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lExcluSize(struct LExclu * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].site)==0)&& (l[i].quantity==0)) //vérifie si le site et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

/*RENVOIE LA POSITION DU SITE SI IL EST DANS LA LISTE EXCLUSIF -1 SINON                                                              inutilisé*/    
int lExcluPosition(struct LExclu * l,char * type, char * site){
    int size = lExcluSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].type,type)==0 && strcmp(l[i].site,site)==0)
            return i;
    }
    return -1;
}

/*SUPRESSION D'UN  SITE EN PARTICULIER                                                                 INUTILISÉ*/
void suppressionExcluClient(struct clientStruct * client,int position, int size){
    for (int i = position; i < size; ++i)
    {
        client->exclu[i] = client->exclu[i+1] ;
    }
    //la magie est que les tableau sont de taille 101 donc forcement il y aura un site ="" et quantity = 0
}


/*MET A JOUR LA STRUCTURE CLIENT OU BIEN AJOUTE À LA FIN DE LA LISTE*/
void actionExcluClient(struct clientStruct * client,int size, char * type, char * site, int value){
    strcpy(client->exclu[size].type,type);
    strcpy(client->exclu[size].site,site);
    client->exclu[size].quantity= value;    
}