#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// #include "../snake/snake.h"
#include "neuralNetwork.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                              Other                                /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Fonction d'activation
*/
double sigmoid(double x){
    return 1 / (1 + exp(-(x)) ); //default sigmoid
}

/*
générateur aléatoire entre 0 et 1
*/
double rand_gen(){
    return ( (double)(rand_r(&seed)) + 1. )/( (double)(RAND_MAX) + 1. );
}

/*
générateur aléatoire suivant une loi normale
*/
double normalRandom(){
    double v1=rand_gen();
    double v2=rand_gen();
    return cos(2*3.14*v2)*sqrt(-2.*log(v1));
}

/*
fonction de configuration de la variable globale params
@params size_t taille_population : Taille de la Population
@params size_t generation : nombre de génération

@params size_t nbNeuronsInput : nombre de neurones dans l'input layer
@params size_t nbNeuronsHidden : nombre de neurones dans l'hidden layer
@params size_t nbNeuronsOutput : nombre de neurones dans l'output layer

@params size_t nbHiddenLayer : nombre de hidden layer

@params double mutationRate : probabilité de mutation de chaque poid
@params double sigmaMutation :
@params double crossoverRate : part de l'individu pere qui sera concervé dans l'individu fils

@params size_t nbThread : nombre de thread que le programme va utiliser

*/
Config NewConfig(
    size_t taille_population,
    size_t generation,

    size_t nbNeuronsInput,
    size_t nbNeuronsHidden,
    size_t nbNeuronsOutput,

    size_t nbHiddenLayer,

    double mutationRate,
    double sigmaMutation,
    double crossoverRate,

    size_t nbThread
){

    params.taille_population = taille_population;
    params.generation = generation;

    params.nbNeuronsInput = nbNeuronsInput;
    params.nbNeuronsHidden = nbNeuronsHidden;
    params.nbNeuronsOutput = nbNeuronsOutput;

    params.nbHiddenLayer = nbHiddenLayer;

    params.mutationRate = mutationRate;
    params.sigmaMutation = sigmaMutation;
    params.crossoverRate = crossoverRate;

    if( params.nbHiddenLayer == 0 ){
        params.totalWeight = ( params.nbNeuronsInput /*+ 1*/ ) * params.nbNeuronsOutput;
    }
    else{
        params.totalWeight = ( params.nbNeuronsInput /*+ 1*/ ) * params.nbNeuronsHidden;
        for ( size_t i = 1; i < params.nbHiddenLayer; i ++){
            params.totalWeight +=  ( params.nbNeuronsHidden /*+ 1*/ ) * params.nbNeuronsHidden;
        }
        params.totalWeight += ( params.nbNeuronsHidden /*+ 1*/ ) * params.nbNeuronsOutput;
    }

    params.tailleCrossoverMax  = params.totalWeight * params.crossoverRate ;

    params.nbThread = nbThread;
    params.tailleThread = params.taille_population / nbThread;
    return params;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                              Layer                                /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Initiliase un layer
@param size_t size : taille du layer à creer
@param Layer * previousLayer : pointeur sur le layer à créer

@return le nouveau layer

*/
Layer * newLayer(size_t size, Layer * previousLayer){
    Layer * new = malloc(sizeof(Layer));

    new->size = size;

    // init neurons
    new->neurons = malloc(new->size * sizeof(double));
    memset(new->neurons, 0, size);

    new->previousLayer = NULL;
    new->nextLayer = NULL;

    if(previousLayer != NULL){

        // init weights
        new->weights = malloc(size * previousLayer->size * sizeof(double));
        printf("size : %ld, prev size : %ld, \n", size, previousLayer->size);
        memset(new->weights, 0, size * previousLayer->size);

        // init weights
        new->bias = malloc(size * sizeof(double));
        memset(new->bias, 0, size );

        //
        initWeigth(new,  previousLayer->size );

        //
        new->previousLayer = previousLayer;

    }

    return new;
}

/*
Initiliase tout les poids d'un layer aléatoirement entre -1 et 1
@param Layer * layer: le layer à initialiser
@param size_t previousSize; la taille du précédent layer
*/
void initWeigth(Layer * layer, size_t previousSize){
    for(size_t i = 0; i < layer->size; i++){
        layer->bias[i] = 1 - ((double)rand_r(&seed) / (double)RAND_MAX)*2 ;
    }

    for(size_t i = 0; i < layer->size * previousSize; i++){
        layer->weights[i] = 1 - ((double)rand_r(&seed) / (double)RAND_MAX)*2 ;
    }

}

/*
Initiliase la valeur des neurones d'un layer avec une liste d'input
@param Layer * layer
@param double * inputList : La liste d'input
*/
void setInput(Layer * layer, double * inputList){
    for(size_t i = 0; i < params.nbNeuronsInput; i++){
        // layer->neurons[i] = sigmoid(inputList[i]);
        layer->neurons[i] = inputList[i];
    }
    free(inputList);
}


/*
Lance le calcul du réseau de neurone sur un layer
*/
void computeLayer(Layer * layer){
    double s = 0;

    Layer * previousLayer = layer->previousLayer;
    size_t previousSize = previousLayer->size;
    size_t size = layer->size;

    for (size_t i = 0; i < size ; i++){
        s = layer->bias[i];

        for(size_t j = 0; j < previousSize; j++){
            s += layer->weights[i * previousSize + j] * previousLayer->neurons[j]; // stocker ou appeller à chaque fois ?
        }

        layer->neurons[i] = sigmoid(s);
        // layer->neurons[i] = s;
    }
}

/*
Mute tout les poids et les biais dun layer
*/
void mutateLayer(Layer * layer){

    size_t previousSize = layer->previousLayer->size;
    size_t size = layer->size;

    for (size_t i = 0; i < size ; i++) {

        if( params.mutationRate > (double) rand_r(&seed) / RAND_MAX ){ (layer->bias)[i] += normalRandom() * params.sigmaMutation; }

        for (size_t j = 0; j < previousSize; j++ ){
            if( params.mutationRate > (double) rand_r(&seed) / RAND_MAX ){ (layer->weights)[i * previousSize + j] += normalRandom() * params.sigmaMutation; }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                          NeuralNetwork                            /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Créé un neural network
@param size_t numNN : id du neural network
*/
NeuralNetwork * newNeuralNetwork( size_t numNN ) {

    //
    NeuralNetwork * nn;
    nn = malloc( sizeof(NeuralNetwork) );

    //
    //m_size = 2 + params.nhiddenlayer;
    nn->size = 2 + params.nbHiddenLayer ;
    nn->numNN = numNN;
    //
    Layer * currentLayer = newLayer( params.nbNeuronsInput, NULL);
    Layer * previousLayer;
    nn->firstLayer = currentLayer;


    for(size_t i = 0; i < params.nbHiddenLayer; i++){
            previousLayer = currentLayer;
            currentLayer = newLayer(params.nbNeuronsHidden, previousLayer);
            previousLayer->nextLayer = currentLayer;
            // lastLayer->nextLayer = currentLayer;
            // lastLayer = currentLayer;
    }

    previousLayer = currentLayer;
    currentLayer = newLayer(params.nbNeuronsOutput, previousLayer);
    previousLayer->nextLayer = currentLayer;
    //lastLayer->nextLayer = currentLayer;

    //
    nn->lastLayer = currentLayer;

    nn->score = -1;
    nn->fitness = -1;

    return nn;

}

/*
Lance le calcule sur tout le neural Network
@param double * inputList : Liste d'input

@return l'indice du neurone ouput le plus grand
*/
size_t computeNN(NeuralNetwork * nn, double * inputList ){

    //
    Layer * layer = nn->firstLayer;
    setInput(layer, inputList);

    layer = layer->nextLayer;

    //
    while(layer){
        computeLayer(layer);
        layer = layer->nextLayer;
    }
    return output( nn );
}

/*
@return l'indice du neurone ouput le plus grand
*/
size_t output(NeuralNetwork * nn ){
    Layer * layer = nn->lastLayer;

    double max = layer->neurons[0];
    size_t index = 0;

    for (size_t i = 1; i < layer->size; i++) {

        if (layer->neurons[i] > max)
        {
            max = layer->neurons[i];
            index = i;
        }
    }
    return index;

}

/*
Effectue un croisement entre un neural network pere et neural network mere
@param NeuralNetwork * nn : nn fils
@param NeuralNetwork * father : nn pere
@param NeuralNetwork * mother : nn mere
*/
void crossover(NeuralNetwork * nn, NeuralNetwork * father, NeuralNetwork * mother ){
    int remainingToLocation = ((double) rand_r(&seed)/ (double) RAND_MAX )*(params.totalWeight - params.tailleCrossoverMax);
    int crossoverRemaining = params.tailleCrossoverMax;
    size_t size;
    double layerSize;
    size_t i;
    // printf("totalWeight%ld\n", params.tgmg);

    Layer * layerA = father->firstLayer->nextLayer;
    Layer * layerB = mother->firstLayer->nextLayer;
    Layer * layer = nn->firstLayer->nextLayer;
    printf("NEW ----- \n" );
    // printf("total weights %ld\n", params.totalWeight);
    while(layer){
        // printf("remainingToLocation : %d\n", remainingToLocation);
        // printf("crossoverRemaining : %d\n", crossoverRemaining);
        layerSize = layer->size * layer->previousLayer->size;
        memcpy(layer->weights, layerA->weights, layerSize * sizeof(double));
        memcpy(layer->bias, layerA->bias, layer->size * sizeof(double));
        remainingToLocation -= layerSize;
        if( remainingToLocation < 0 && crossoverRemaining > 0){
            i = layerSize + remainingToLocation;
            if(crossoverRemaining > (layerSize - i))
                size = layerSize - i;
            else
                size = crossoverRemaining;
            crossoverRemaining -= size;
            remainingToLocation = 0;
            printf("totalWeight %ld \n", params.totalWeight);
            printf("layer size %f \n", layerSize);
            printf("cr : %d\n",  crossoverRemaining);
            printf("rtl : %d\n",  remainingToLocation);
            printf("i %ld \n", i);
            printf("size %ld \n", size);
            printf("-----------------\n");

            memcpy(&(layer->weights[i]), &(layerB->weights[i]), (size-1) * sizeof(double));
        }

        layer = layer->nextLayer;
        layerA = layerA->nextLayer;
        layerB = layerB->nextLayer;

    }

}

// void crossover(NeuralNetwork * nn, NeuralNetwork * father, NeuralNetwork * mother ){
//     int remainingToLocation = ((double) rand_r(&seed)/ (double) RAND_MAX )*(params.totalWeight - params.tailleCrossoverMax);
//     int crossoverRemaining = params.tailleCrossoverMax;
//
//     printf("totalWeight%ld\n", params.totalWeight);
//     printf("tailleCrossoverMax%ld\n", params.tailleCrossoverMax);
//     printf("remainingToLocation%d\n", remainingToLocation);
//     printf("crossoverRemaining%d\n", crossoverRemaining);
//
//     Layer * layerA = father->firstLayer->nextLayer;
//     Layer * layerB = mother->firstLayer->nextLayer;
//     Layer * layer = nn->firstLayer->nextLayer;
//
//     while(layer){
//         for (unsigned long long i = 0; i < layer->size ; i++) {
//             if( crossoverRemaining == 0 || remainingToLocation != 0){
//                 layer->bias[i] = layerA->bias[i];
//                 remainingToLocation -= 1;
//             }
//             else {
//                 layer->bias[i] = layerB->bias[i];
//                 crossoverRemaining -= 1;
//             }
//
//             for (unsigned long long j = 0; j < layer->previousLayer->size; j++ ){
//
//                 if( crossoverRemaining == 0 || remainingToLocation != 0){
//                     layer->weights[i * layer->size + j] = (layerA->weights)[i * layer->size + j];
//                     remainingToLocation -= 1;
//                 }
//                 else {
//                     printf("rl : %d\n", remainingToLocation );
//                     printf("cr : %d\n", crossoverRemaining );
//                     printf("i : %lld\n", i );
//                     printf("j : %lld\n", j );
//                     printf("layer->size : %ld\n", layer->size );
//                     printf("layer->previousLayer->size : %ld\n", layer->previousLayer->size );
//                     layer->weights[i * layer->size + j] = (layerB->weights)[i * layer->size + j];
//                     crossoverRemaining -= 1;
//                 }
//             }
//         }
//         layer = layer->nextLayer;
//         layerA = layerA->nextLayer;
//         layerB = layerB->nextLayer;
//
//     }
//
// }

/*
Lance la mutation sur un neural network
*/
void mutate(NeuralNetwork * nn ){
    Layer * layer = nn->firstLayer->nextLayer;

    //
    while(layer){
        mutateLayer(layer );
        layer = layer->nextLayer;
    }
}

/*
Donne un score au neural network en fonction de sa performance sur le jeu
@param double score : score du jeu controlé par le neural network
@param double fruit :
*/
void setScore(NeuralNetwork * nn, double score,  double fruit){
    nn->score = score*score;
    // nn->score = exp(score);
    nn->bruteScore = score;
    nn->nbFruit = fruit;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                           population                              /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Créé une nouvelle population
@return la nouvelle population
*/
Population * newPopulation( ){
    Population * population;
    population = malloc( sizeof(Population) );

    population->size = params.taille_population;

    population->firstPopulation = malloc(population->size * sizeof(NeuralNetwork)) ;
    population->secondPopulation = malloc(population->size * sizeof(NeuralNetwork)) ;

    for(size_t i = 0; i < population->size; i++){
        population->firstPopulation[i] = newNeuralNetwork( i );
        population->secondPopulation[i] = newNeuralNetwork( i );
    }

    return population;
}



/*
Echange les pointeurs de la liste courante et précédente de neural network
*/
void swap(Population * population){
        NeuralNetwork ** swap = population->firstPopulation;
        population->firstPopulation = population->secondPopulation;
        population->secondPopulation = swap;
}

/*
Sélectionne aléatoirement un individu de la population en fonction de sa fitness
méthode à optimiser ASAP

@return un individu de la population
*/
NeuralNetwork * pickOne(Population *population ){
    size_t index = 0;

    double r = (double) rand_r(&seed) / (double) RAND_MAX;
    double rr = r;

    NeuralNetwork ** ListNeuralNetwork = population->firstPopulation;

    while ( r > 0) {
        if(ListNeuralNetwork[index]->fitness <= 0)
            printf("r : %lf rr: %lf index:%ld, fitt : %lf \n",r,rr,index, ListNeuralNetwork[index]->fitness );
        r -= ListNeuralNetwork[index]->fitness;
        index += 1;
    }

    if (index == 0)
        index = 1;

    return ListNeuralNetwork[index - 1];
}

/*
sélectionne l'individu avec le meilleur score
@return le meilleur individu de la population
*/
NeuralNetwork * bestElement(Population *population ){
    double BestScore = 0;
    size_t  index = 0;
    for( size_t i = 0; i < population->size; i++){
        if(population->firstPopulation[i]->bruteScore > BestScore){
            index = i;
            BestScore = population->firstPopulation[i]->bruteScore;
        }

    }

    return population->firstPopulation[index];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                             Thread                                /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Créé une structure Thread
@param Population *population : la population
@param size_t numThread : numéro du thread
@param pthread_t * id : id pthread
*/
Thread * NewThread(Population *population, size_t numThread, pthread_t * id){

    Thread * thread = malloc(sizeof(Thread));

    thread->count = 0;

    thread->id = id;
    thread->numThread = numThread;
    thread->population = population;

    size_t tailleThread = params.tailleThread;

    if(numThread == params.nbThread - 1 ){
        tailleThread += params.taille_population % params.nbThread;
    }

    size_t debut = numThread * params.tailleThread;

    thread->size = tailleThread;
    thread->debut = numThread * params.tailleThread;
    thread->fin = thread->debut + thread->size - 1;

    thread->ListNeuralNetwork_A = malloc( thread->size * sizeof(Thread) );
    thread->ListNeuralNetwork_B = malloc( thread->size * sizeof(Thread) );

    for( size_t i = 0; i < thread->size; i++ ){
        thread->ListNeuralNetwork_A[i] = population->firstPopulation[ debut + i ];
        thread->ListNeuralNetwork_B[i] = population->secondPopulation[ debut + i ];
        // printf("%ld \n", debut + i);
    }




    return thread;
}


/*
Fonction que chaque thread fils va executer
*/
void * runFils(void *voidArgs){//void *voidThread, void (*gameFunc) (NeuralNetwork * nn) ){

    thread_args * args = (thread_args *)voidArgs;
    // Thread * thread = (Thread *)voidThread;
    Thread * thread = args->thread;
    printf("%ld\n", thread->size );

    for(size_t gen = 0; gen < params.generation; gen++){

        //thread synchronisation pour la game
        pthread_barrier_wait(&barrier1);
        printf("play \n" );
        for (size_t i = 0; i < thread->size; i++) {
            args->func(thread->ListNeuralNetwork_A[i], args->gameArgs);
        }

        //thread synchronisation pour le calcule de fitness
        pthread_barrier_wait(&barrier2);
        printf("calcul fitness\n" );
        calculateFitness(thread);

        //thread synchronisation pour evolution
        pthread_barrier_wait(&barrier3);
        printf("evolve\n" );
        evolve(thread);

        printf("end thread\n" );

    }

    return 0;
}







/*
Fonction executée par le thread père
elle créer n threads fils puis les coordonne un certain nombre de génération
*/
void runPere( char *fileName, void (*gameFunc) (NeuralNetwork * nn, void * gameArgs) , void (*playBestFunc)(NeuralNetwork * nn, void * gameArgs), void * gameArgs ){

    //ouverture fichier de log
    fileScore = openLog( fileName );

    //création de la population
    Population * population = newPopulation(  );

    //initialisation des barrières
    //la barriere bloque les threads jusqu'à ce que 'params.nbThread + 1' soient bloqués
    pthread_barrier_init(&barrier1, NULL, params.nbThread + 1);
    pthread_barrier_init(&barrier2, NULL, params.nbThread + 1);
    pthread_barrier_init(&barrier3, NULL, params.nbThread + 1);


    // boucle de création et lancement des threads

    Thread ** threadList = malloc(params.nbThread * sizeof(Thread));
    pthread_t * idList = malloc(params.nbThread * sizeof(pthread_t));
    thread_args * args = malloc( params.nbThread * sizeof(thread_args) );

    for( size_t i = 0; i < params.nbThread; i++){
        args[i].func = gameFunc;
        args[i].gameArgs = gameArgs;
        threadList[i] = NewThread(population, i, &idList[i] );
        args[i].thread = threadList[i];
        pthread_create(&idList[i], NULL, runFils, &args[i] );
        // getchar()
    }

    //
    for(size_t g = 0; g < params.generation; g++){
        // printf("GEN : %ld\n", g );

        // GAME - resynchronise les threads
        pthread_barrier_wait(&barrier1);


        // FITNESS - resynchronise les threads
        pthread_barrier_wait(&barrier2);

        // EVOLUTION - resynchronise les threads
        pthread_barrier_wait(&barrier3);

        // printf("FIN GEN \n" );


        printf("gen :  %ld\n", g);

        //écrit les scores dans les logs
        writeLogScore(fileScore, population);
        if( g%10 == 0){
            playBestFunc(bestElement(population), gameArgs );
        }
    }

    //termine les threads
    for( size_t i = 0; i < params.nbThread; i++){
        pthread_join(idList[i], NULL );
    }

    // libération de la mémoire
    for( size_t i = 0; i < params.nbThread; i++){
        free(threadList[i]->ListNeuralNetwork_A);
        free(threadList[i]->ListNeuralNetwork_B);
        free(threadList[i]);
    }

    free(threadList);
    free(idList);
    free(args);

    freePopulation( population );
    closeLog(fileScore);

    return;
}

/*
Calcule la fitness de chaque element du thread
Calcul sum à optimiser  !
*/
void calculateFitness(Thread *thread ){
    double sum = 0;

    NeuralNetwork ** ListNeuralNetwork = thread->population->firstPopulation;

    for( size_t i = 0; i < thread->population->size; i++){
        sum += ListNeuralNetwork[i]->score;
        // printf("%p :: score : %f\n", thread, ListNeuralNetwork[i]->score );
        }
    for( size_t i = 0; i < thread->size; i++){
        thread->ListNeuralNetwork_A[i]->fitness = thread->ListNeuralNetwork_A[i]->score  / sum;
        printf("%p :: score : %f, sum : %f, fitness : %f\n", thread, thread->ListNeuralNetwork_A[i]->score, sum, thread->ListNeuralNetwork_A[i]->score  / sum);
    }

}

/*
Lance l'évolution pour tout les éléments du thread
*/
void evolve(Thread *thread ){

    NeuralNetwork * tmp;
    for( size_t i = 0; i < thread->size; i++){
        // tmp = population->secondPopulation[i];
        tmp = thread->ListNeuralNetwork_B[i];

        crossover(tmp, pickOne(thread->population), pickOne(thread->population));
        mutate(tmp);
    }

    NeuralNetwork ** swap = thread->ListNeuralNetwork_A;
    thread->ListNeuralNetwork_A = thread->ListNeuralNetwork_B;
    thread->ListNeuralNetwork_B = swap;

    return;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                              Game                                 /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Fonction jeu qui sera controllé par un nn
@param NeuralNetwork * nn : nn controllant le jeu
*/
void game(NeuralNetwork * nn){
    /*

    Snake * snake ;
    snake = malloc(sizeof(Snake));
    initSnake(snake);

    int resultat = 4;
    int end = 0;

    //
    while (end == 0 && snake->health != 0) {
        resultat = computeNN( nn, getInput(snake, params.nbNeuronsInput) );

        //                      Affichage
        // jump(10);
        // printNetwork(population[i]);
        // afficherData(population[i]);
        // afficherJeu(resultat);
        // printf(">\n");
        // getchar();

        switch (resultat) {
            case 0:
                end = move(snake, -1, 0);
                break;
            case 1:
                end = move(snake, 1, 0);
                break;
            case 2:
                end = move(snake, 0, -1);
                break;
            case 3:
                end = move(snake, 0, 1);
                break;
            default:
                //break;
                //printf("fin du jeu\n" );
                printf("%d\n", resultat );
                exit(0);
                end = 1;
                break;
        }

    }

    setScore(nn, getScore(snake), getFruit(snake));
    destroySnake(snake);

    */
    return;
}
/*
Joue le meileur élément de la population
*/

void playBest( NeuralNetwork * nn){/*
    // printf("aaaaaa\n" );
    Snake * snake ;

    int resultat = 4;
    int end = 0;


    snake = malloc(sizeof(Snake));
    initSnake(snake);

    //
    while (end == 0 && snake->health != 0) {
        resultat = computeNN( nn, getInput(snake, params.nbNeuronsInput) );

        //                      Affichage
        jump(10);
        // printNetwork(nn);
        // afficherData(nn);
        printNetwork(nn);
        afficherJeu(resultat, snake);
        printf(">\n");
        getchar();

        switch (resultat) {
            case 0:
                end = move(snake, -1, 0);
                break;
            case 1:
                end = move(snake, 1, 0);
                break;
            case 2:
                end = move(snake, 0, -1);
                break;
            case 3:
                end = move(snake, 0, 1);
                break;
            default:
                //break;
                //printf("fin du jeu\n" );
                printf("%d\n", resultat );
                exit(0);
                end = 1;
                break;
        }

    }

    destroySnake(snake);

    return;
    */
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                            Affichage                              /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Affiche le NN
@param NeuralNetwork *nn : nn à afficher
*/
void printNetwork(NeuralNetwork *nn ){


    Layer * current = nn->firstLayer;
    size_t i = 1;
    while (current->nextLayer) {
        printf("Layer %ld\n",i );
        for(size_t t = 0; t < current->size; t++){
            printf("%ld - [%lf] : ", t+1, current->neurons[t]);
            for(size_t n = 0; n < current->nextLayer->size; n++){
                printf(" {[%lf] -> %ld} ",  current->nextLayer->weights[ n * current->size + t], n+1);
            }
            printf("\n" );
        }
        printf("\n\n\n" );


        current = current->nextLayer;
        i++;
    }
    printf("Layer %ld Ouput\n",i );
    for(size_t t = 0; t < current->size; t++){
        printf("%ld - [%lf] ", t+1, current->neurons[t]);
        printf("\n" );
    }
    printf("\n\n\n\n\n\n" );

    //
    // size_t sizePrev;
    // size_t sizeCurrent;
    // size_t sizeNext;
    //
    // while( current){
    //     if(current->previousLayer)
    //         sizePrev = current->previousLayer->size;
    //     else
    //         sizePrev = 0;
    //
    //
    //     sizeCurrent = current->size;
    //
    //     if(current->nextLayer)
    //         sizeNext = current->nextLayer->size;
    //     else
    //         sizeNext = 0;
    //
    //
    //     printf("%ld > %ld > %ld\n", sizePrev, sizeCurrent, sizeNext );
    //
    //     current = current->nextLayer;
    // }

}

/*
Affiche la population
@param Population *population : population à afficher
*/
void printPopulaton(Population *population ){
    printf("//////////////////////////////////////////////\n" );
    printf("///////             SECOND            ////////\n" );
    printf("//////////////////////////////////////////////\n" );
    for( size_t i = 0; i < population->size; i++){
        printNetwork( population->secondPopulation[i]);
    }


    printf("//////////////////////////////////////////////\n" );
    printf("///////              FIRST            ////////\n" );
    printf("//////////////////////////////////////////////\n" );
    for( size_t i = 0; i < population->size; i++){
        printNetwork( population->firstPopulation[i]);
    }

}

void jump(int a){
    for( int i = 0; i < a; i++){
        printf("\n");
    }
}

/*
Affiche le jeu
@param int resultat :
@param Snake * snake :
*/
/*
void afficherJeu(int resultat, Snake * snake ){
    afficheGrille( snake );
    switch (resultat) {
        case 0:
            printf(">haut\n" );
            break;
        case 1:
            printf(">Bas\n" );
            break;
        case 2:
            printf(">gauche\n" );
            break;
        case 3:
            printf(">droite\n" );
            break;
        default:
            break;
        }
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                       garbage collector                           /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void freePopulation( Population * population ){
    for( size_t i = 0; i < population->size; i++){
        freeNeuralNetwork( population->firstPopulation[i]);
        freeNeuralNetwork( population->secondPopulation[i]);
    }

    free(population->firstPopulation);
    free(population->secondPopulation);

    free(population);
    return;
}

void freeNeuralNetwork(NeuralNetwork * nn){


    Layer * layer = NULL;
    layer = nn->lastLayer;

    Layer * previousLayer = NULL;


    while(layer){
        previousLayer = layer->previousLayer;
        freeLayer(layer);
        layer = previousLayer;
    }
    free(nn);

    return;
}

void freeLayer(Layer * layer){

    free(layer->neurons);
    if(layer->previousLayer){

        free(layer->bias);
        free(layer->weights);
    }


    free(layer);
    return ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////                               Log                                 /////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
FILE* openLog( char *fileName ){
    FILE* file = NULL ;
    file = fopen(fileName, "r");

    if( file != NULL){
        fseek(file, 0, SEEK_END);
        //printf("%ld\n", ftell(file) );

        if( ftell(file) != 0 ){
            // printf("le fichier n'est pas vide !\n" );
            //fclose( file );
            //exit(1);
        }
        fclose( file );
    }
    file = fopen( fileName, "w");
    return file;
}

// void writeLogScore ( FILE* fichier,  NeuralNetwork ** population ){}
// void writeLogId ( FILE* fichier,  NeuralNetwork ** population ){}

void writeLogScore ( FILE* file,  Population * population ){

    for( size_t i = 0; i < params.taille_population; i++){

        fprintf( file, "%lf,",population->firstPopulation[i]->nbFruit);
        //fprintf( file, "%lf,",population[i]->score);
        //fprintf( file, "%lf,",population[i]->fitness);
    }
    fputc('\n', file);
}

void closeLog( FILE* file){
    fclose( file );
}
