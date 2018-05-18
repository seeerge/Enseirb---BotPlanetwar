/*java -jar tools/PlayGame.jar maps/map7.txt 1000 1500 log.txt "java -jar example_bots/DualBot.jar" "./JMbot" | java -jar tools/ShowGame.jar */


# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <math.h>

#define TAILLE_MAX 100
#define MAX_PLANET 100
#define NB_ARMIES 250
#define NUM_PLAYER 1


struct planet {
    float x;
    float y;
    int owner;
    int ships;
    int growth;
    int id;

};

struct fleet {
    int owner;
    int ships;
    int source;
    int destination;
    int total_turns;
    int remaining_turns;
    int id;

};


/****      les indices servent a incrementer les tableaux contenant les differents
        types de planetes ou flottes (alliees, ennemies...) et a connaitre leur 
        nombre pendant un tour  
****/ 
struct indice {   
    int indp_ally;
    int indp_ennemy;
    int indp_neutral;
    int indf_ally;
    int indf_ennemy;
};



/***********************recupere les donnees du moteur dans un buffer (sous forme d'une seule string)********************/





void listen(char buffer[]){   /*stocke dans buffer une ligne de stdout*/
    fgets(buffer, TAILLE_MAX, stdin);
}

void go(){ /*envoie go au moteur de jeu*/
    printf("go\n");
}






/*************fcts d'initialisation et de raz : *************/

void init_planets(struct planet new_planets[MAX_PLANET]){
    int i;
    for(i=0;i<=MAX_PLANET;i++){
        new_planets[i].x = 0;
        new_planets[i].y = 0;
        new_planets[i].owner = 0;
        new_planets[i].ships = 0;
        new_planets[i].growth = 0;
        new_planets[i].id = 0;
    }
}



void init_indice(struct indice* indice){
    indice->indp_ally = 0;
    indice->indp_ennemy = 0;
    indice->indp_neutral = 0;
    indice->indf_ally = 0;
    indice->indf_ennemy = 0;
}





            /*******************************************
             * Partie dediee au traitement des donnees *
             *******************************************/







/*****pour traduire le char du buffer en structures de planetes ou de fleet****/




/****      ces 2 programmes modifient les valeurs des composants des structures 
        passées en argument pour traduire la ligne stdin stockée dans le buffer
****/

void traduction_planet(char buffer[], struct planet* planete){
    sscanf(buffer, "P %f %f %d %d %d", &planete->x, &planete->y, &planete->owner, &planete->ships, &planete->growth);
}


void traduction_fleet(char buffer[], struct fleet* current_fleet){
    sscanf(buffer, "F %d %d %d %d %d %d", &current_fleet->owner, &current_fleet->ships, &current_fleet->source, &current_fleet->destination, &current_fleet->total_turns,
    &current_fleet->remaining_turns);
}






/****      copy la struct d'une planete dans une autre structure (permettra la 
        recup de certaines planetes precises)   
****/        
        
        
void copy_planet(struct planet* planet_source, struct planet* planet_copy){
    planet_copy->x = planet_source->x;
    planet_copy->y = planet_source->y;
    planet_copy->owner = planet_source->owner;
    planet_copy->ships = planet_source->ships;
    planet_copy->growth = planet_source->growth;
    planet_copy->id = planet_source->id;
}

void copy_fleet(struct fleet* fleet_source, struct fleet* fleet_copy){
    fleet_copy->owner = fleet_source->owner;
    fleet_copy->ships = fleet_source->ships;
    fleet_copy->source = fleet_source->source;
    fleet_copy->destination = fleet_source->destination;
    fleet_copy->total_turns = fleet_source->total_turns;
    fleet_copy->remaining_turns = fleet_source->remaining_turns;
    fleet_copy->id = fleet_source->id;
}



/****      ce programme enregistre le contenu du buffer dans le tableau 
        correspondant (p alliees, f ennemies etc...), il utilise des pointeurs
        pour modifier durablement les tableaux du main et il incremente les
        compteurs (=les indices) de ceux-ci.
        Au final on obtient des tableaux remplis de structures et le nombre
        total de types de planetes/flottes pour le tour.        
***/


void game_stat(char buffer[], struct planet p_allied[MAX_PLANET], struct planet p_ennemy[MAX_PLANET], struct planet p_neutral[MAX_PLANET], struct fleet f_allied[NB_ARMIES], struct fleet f_ennemy[NB_ARMIES], int* id_planet, int* id_fleet, struct indice* indice){
    struct planet current_planet;
    struct fleet current_fleet;
    if(buffer[0] == 'P'){
        traduction_planet(buffer, &current_planet);
        current_planet.id = *id_planet;
        if(current_planet.owner == 1){
            copy_planet(&current_planet, &(p_allied[(indice->indp_ally)]));
            (indice->indp_ally)++;      /* indice de la planete en tant que 
                                           planete alliee, indique sa position 
                                           dans le tableau p_allied        */
            }
        if(current_planet.owner == 2){
            copy_planet(&current_planet, &(p_ennemy[(indice->indp_ennemy)]));
            (indice->indp_ennemy)++;
            }
        if(current_planet.owner == 0){
            copy_planet(&current_planet, &(p_neutral[(indice->indp_neutral)]));
            (indice->indp_neutral)++;
            }
        (*id_planet)++;       /*id_planet = indice reel de la planete pour le 
                                            moteur de jeu  */
        }
        
    if(buffer[0] == 'F'){
        traduction_fleet(buffer, &current_fleet);
        current_fleet.id = *id_fleet;
        if(current_fleet.owner == 1){
            copy_fleet(&current_fleet, &(f_allied[(indice->indf_ally)]));
            (indice->indf_ally)++;
            }
        if(current_fleet.owner == 2){
            copy_fleet(&current_fleet, &(f_ennemy[(indice->indf_ennemy)]));
            (indice->indf_ennemy)++;
            }
        (*id_fleet)++;      /*id_fleet = indice reel de la flotte pour le 
                                         moteur de jeu  */
        }
}







            /*****************************
             * Partie dediee aux ordres  *
             *****************************/





/**********************************************************************/

/*  ordre de lancement d'une flotte */

void attack(struct planet* planet_source, struct planet* planet_destination, int nb_ships){
    if(planet_source->id != planet_destination->id && nb_ships <= planet_source->ships){
        printf("%d %d %d \n",planet_source->id, planet_destination->id, nb_ships);
        planet_source->ships = planet_source->ships-nb_ships;    /*actualise le nb de vaisseaux restant 
                                                                sur la planete apres l'attaque car le moteur 
                                                                de jeu ne fera de maj qu'apres un "go"*/
    }
}



/* renvoie la distance en tours entre 2 plaetes   */

int distance(struct planet planete1, struct planet planete2){
    return (sqrt( (planete2.x-planete1.x)*(planete2.x-planete1.x) + (planete2.y-planete1.y)*(planete2.y-planete1.y)));
}




/******************    etude des donnees du tour :      **********************/




/*****  prend un tableau de planetes en entree et modifie un 2e tableau 
        pour qu'il contienne les planetes de premier tableau classees de 
        la moins peuplee a la plus peuplee          
*****/


void weak_sorting(struct planet pNotSorted[MAX_PLANET], struct planet pSorted[MAX_PLANET], int id_max){
    int i;
    int j;
    struct planet planet_tmp;
    for( i=0 ; i<id_max ; i++ ){
        copy_planet(&(pNotSorted[i]), &(pSorted[i]));
    }
    for( i=0 ; i<id_max ; i++ ){
        for( j=i; j>=1 ; j--){
            if (pSorted[j].ships < pSorted[j-1].ships){
                copy_planet(&(pSorted[j-1]), &planet_tmp);
                copy_planet(&(pSorted[j]) , &(pSorted[j-1]));
                copy_planet(&(planet_tmp) , &(pSorted[j]));
            }
        }
    }
}
      
   
/*  meme programe mais qui classe les planetes de la plus proche a la plus eloignee
    en fonction d'une planete de reference      */

void close_sorting(struct planet pNotSorted[MAX_PLANET], struct planet pSorted[MAX_PLANET], int id_max, struct planet planet_ref){
    int i;
    int j;
    int d_avant;
    int d_suivant;
    struct planet planet_tmp;
    int x_ref = planet_ref.x;
    int y_ref = planet_ref.y;
    for( i=0 ; i<id_max ; i++ ){
        copy_planet(&(pNotSorted[i]), &(pSorted[i]));
    }
    for( i=0 ; i<id_max ; i++ ){
        for( j=i; j>=1 ; j--){
        
            d_avant = sqrt(  (pSorted[j-1].x-x_ref)*(pSorted[j-1].x-x_ref) + (pSorted[j-1].y-y_ref)*(pSorted[j-1].y-y_ref));
            d_suivant = sqrt(  (pSorted[j].x-x_ref)*(pSorted[j].x-x_ref) + (pSorted[j].y-y_ref)*(pSorted[j].y-y_ref));
            
            if ( d_suivant < d_avant ){
                copy_planet(&(pSorted[j-1]), &planet_tmp);
                copy_planet(&(pSorted[j]) , &(pSorted[j-1]));
                copy_planet(&(planet_tmp) , &(pSorted[j]));
            }
        }
    }
}








/*****      modifie un pointeur de planete pour y stocker les donnees de la planete d'origine d'une
        flotte
*****/

void planet_origin(struct planet* planet_source, struct planet planet[MAX_PLANET], struct fleet fleet, int indice){
    int i;
    for( i=0 ; i<indice ; i++){
        if( fleet.source == planet[i].id ){
            copy_planet(&(planet[i]), planet_source);
            return;
        }
    }
}


/*****      renvoie 1 si une flotte alliee est deja partie vers la position ciblee
*****/   
  
int already_gone(struct fleet fleet_ally[NB_ARMIES], int position, struct indice* indice){
    int i;
    for( i=0 ; i< (indice->indf_ally) ; i++){
        if(fleet_ally[i].destination == position){
            return 1;
        }
    }
    return 0;
}



/*****      renvoie 1 si les troupes deja parties vers la planete sont 
            suffisantes pour en prendre le controle
*****/ 

int already_taken(struct fleet fleet[NB_ARMIES], struct planet planet, struct indice* indice, struct planet planetes[MAX_PLANET]){
    int i;
    int nb_ships_incoming = 0;
    struct planet planete_source;
    for( i=0 ; i< indice->indf_ally ; i++){
        if(fleet[i].destination == planet.id){
            nb_ships_incoming += fleet[i].ships;
            planet_origin( &(planete_source), planetes, fleet[i], indice->indp_ally);
        }
    }
    if ( nb_ships_incoming > planet.ships+(planet.growth*distance(planet, planete_source))){
        return 1;
    }
    return 0;
}


/*****      renvoie le nombre de vaisseaux en cours de voyage vers la planete
*****/


int float_incoming ( struct planet planet_ref , struct fleet f_ennemy[NB_ARMIES] , struct indice* indice ){
    int i;
    int ships_incoming = 0;
    for( i=0 ; i<(indice->indf_ennemy) ; i++){
        if (f_ennemy[i].destination == planet_ref.id){
            ships_incoming += f_ennemy[i].ships;
        }
    }
    return ships_incoming;
}


/****   renvoie 0 si les flottes ennemies convergeant vers la planete sont suffisantes pour 
        en prendre le controle si celle-ci envoie nb_attacking de ses vaisseaux a l'attaque
****/

int planet_safe(struct planet* planet, struct fleet fleet[NB_ARMIES], struct indice* indice , int nb_attacking){
    int i;
    int nb_ships_incoming = 0;
    for( i=0 ; i<(indice->indf_ennemy) ; i++){
        if(fleet[i].destination == planet->id){
            nb_ships_incoming += fleet[i].ships;
            if( (planet->ships)+(planet->growth * fleet[i].remaining_turns)-(nb_attacking) < nb_ships_incoming+2 ){
                return 0;
            }
        }
    }
    return 1;
}



/****   envoie une flotte suffisante pour conquerir une planete ennemies si la flotte
        sur la planete de depart est suffisante (repaire les planetes ennemies faibles
         et proches).
****/

int easy_take(struct planet planet, struct planet p_cibles[MAX_PLANET], struct fleet f_ennemy[NB_ARMIES], struct fleet f_allied[NB_ARMIES], struct indice* indice){
    struct planet ennemy_sorted[MAX_PLANET];
    weak_sorting(p_cibles , ennemy_sorted , indice->indp_ennemy);
    int nb_ships_recquired = (ennemy_sorted[0].ships+distance(planet, ennemy_sorted[0])+1)*ennemy_sorted[0].growth+1;
    if( planet_safe( &(planet), f_ennemy, indice, nb_ships_recquired ) && !already_gone(f_allied, ennemy_sorted[0].id, indice) && nb_ships_recquired < planet.ships){
        attack( &(planet), &(ennemy_sorted[0]), nb_ships_recquired);
        return 1;
    }
    return 0;
}






    
/************************************************************************/



/*      mise en place de la strategie type controle  :     */
        


/****      la planete attaque la planete neutre de la moins peuplee 
        tant qu'elle possede suffisament de vaisseaux pour le faire et 
        qu'elle ne risque pas d'etre conquise par une flotte ennemie 
        deja en chemin.
****/

void colonize_Weak(struct planet p_allied, struct planet p_neutral[MAX_PLANET], struct indice* indice, struct fleet fleet_ally[NB_ARMIES], struct fleet f_ennemy[NB_ARMIES]){
    struct planet neutral_sorted[MAX_PLANET];
    weak_sorting(p_neutral , neutral_sorted , indice->indp_neutral);
    int i;
    int j;
    for( i=0; i<=(indice->indp_ally) ; i++){
        for( j=0 ; j<(indice->indp_neutral) ; j++){
            if( planet_safe(&(p_allied), f_ennemy, indice, neutral_sorted[j].ships+15) && !already_gone(fleet_ally, neutral_sorted[j].id, indice)){
                    attack( &(p_allied) , &(neutral_sorted[j]) , neutral_sorted[j].ships+15); 
            }
        }
    }
}



/****      la planete attaque la planete neutre la plus proche
        tant qu'elle possede suffisament de vaisseaux pour le faire et 
        qu'elle ne risque pas d'etre conquise par une flotte ennemie 
        deja en chemin.
****/


void colonize_Close(struct planet p_allied, struct planet p_neutral[MAX_PLANET], struct indice* indice, struct fleet fleet_ally[NB_ARMIES] , struct fleet f_ennemy[NB_ARMIES]){
        struct planet neutral_sorted[MAX_PLANET];
        close_sorting(p_neutral , neutral_sorted , indice->indp_neutral , p_allied);
        if( ( planet_safe(&(p_allied), f_ennemy, indice , neutral_sorted[0].ships+10)) && !already_gone(fleet_ally, neutral_sorted[0].id, indice) && p_allied.ships -(neutral_sorted[0].ships+10) >= 0){
                    attack( &(p_allied) , &(neutral_sorted[0]) , neutral_sorted[0].ships+10);
        }
}




/*****      toutes les planetes alliees (si elles ne risquent pas de se faire conquerir par l'ennemi),
        envoient la moitié de leur flotte disponible vers les planetes ennemies.
*****/


void attack_all(struct planet p_allied[MAX_PLANET], struct planet p_ennemy[MAX_PLANET],struct fleet f_ally[NB_ARMIES],struct fleet f_ennemy[NB_ARMIES], struct indice* indice){
    int i = 0;
    while(i < (indice->indp_ally)) {
        if( p_allied[i].ships >= 60 && planet_safe( &(p_allied[i]), f_ennemy, indice, (p_allied[i].ships)/2) && 0 < (indice->indp_ennemy)){

                    attack(&(p_allied[i]) , &(p_ennemy[0]) , (p_allied[i].ships)/2);
                
        }
        i++;
    }
}






/*      strategie du bot : colonise des planetes neutres le plus vite possible tant qu'il y a plus de planetes 
        neutres  ou ennemies que de planetes alliees ; lorsque cette condition n'est plus realisee, toutes 
        les planetes envoient des attaques vers les planetes ennemies pour les conquerir.
        */


            
void serge(struct planet p_allied[MAX_PLANET], struct planet p_ennemy[MAX_PLANET], struct planet p_neutral[MAX_PLANET], struct indice* indice, struct fleet f_allied[NB_ARMIES], struct fleet f_ennemy[NB_ARMIES]){
    int i;
    if((indice->indp_neutral > indice->indp_ally ||  indice->indp_ennemy>indice->indp_ally) && indice->indp_neutral > 0){
        
        for(i=0 ; i<indice->indp_ally ; i+=2){
            if (!easy_take( p_allied[i], p_ennemy, f_ennemy, f_allied, indice)){
                colonize_Weak(p_allied[i], p_neutral, indice, f_allied, f_ennemy);
            }
        }
        for(i=1 ; i<indice->indp_ally ; i += 2){
            if (!easy_take( p_allied[i], p_ennemy, f_ennemy, f_allied, indice)){
                colonize_Close(p_allied[i], p_neutral, indice, f_allied, f_ennemy);
            }
        }
    }
    else{
        attack_all(p_allied , p_ennemy, f_allied, f_ennemy, indice);
    }
}


/******************************************************************************/















/*****  main : cree les el vides, boucle infinie qui lit les donnees, 
             les traduit, quand un go est detecte donne les ordres, raz les 
             indices puis print go.
*****/


int main(int argc, char *argv[]){
    setvbuf(stdout, NULL, _IOLBF, 0);
    char buffer[TAILLE_MAX];
    struct planet p_allied[MAX_PLANET];
    struct planet p_ennemy[MAX_PLANET];
    struct planet p_neutral[MAX_PLANET];
    struct fleet f_allied[NB_ARMIES];
    struct fleet f_ennemy[NB_ARMIES];
    struct indice indice;
    int id_planet = 0;
    int id_fleet = 0;
    int nb_turn = 0;
    init_indice(&indice);
    while (!feof(stdin)){
        listen(buffer);
        if (buffer[0]=='g' && buffer[1]=='o'){
            if(nb_turn++){
                serge(p_allied, p_ennemy, p_neutral, &indice, f_allied, f_ennemy);
            }
            init_indice(&indice);
            id_planet = 0;
            id_fleet = 0;
            go();           
        }
        else{
            game_stat(buffer, p_allied, p_ennemy, p_neutral, f_allied, f_ennemy, &id_planet, &id_fleet, &indice);
        }
    }
    return 0;
}










