#include "../snake/snake.h"

#ifndef NN_H
    #define NN_H

    char *inputChar[20];// = {"haut","bas","gauche","droite"};
    char *outputChar[4];

    FILE* fileScore;
    FILE* fileId;
    FILE* fileFruit;



    typedef struct Config{
        size_t taille_population;
        size_t generation;

        size_t nbNeuronsInput;
        size_t nbNeuronsHidden;
        size_t nbNeuronsOutput;

        size_t nbHiddenLayer;

        double mutationRate;
        double sigmaMutation;
        double crossoverRate;

        size_t tailleCrossoverMax ;
        size_t totalWeight ;

        size_t nbThread;
        size_t tailleThread;

    }Config;

    typedef struct Layer
    {
        double * neurons;
        double * weights;
        double * bias;

        size_t size;

        struct Layer * nextLayer;
        struct Layer * previousLayer;
    }Layer;

    //
    typedef struct NeuralNetwork
    {
        double score;
        double bruteScore;
        double nbFruit;
        double fitness;

        size_t size;
        size_t numNN;
        struct Layer * firstLayer;
        struct Layer * lastLayer;

    }NeuralNetwork;



    //
    typedef struct Population
    {

        size_t size;

        /*
        struct NeuralNetwork ***current;
        struct NeuralNetwork ***old;
        */
        struct NeuralNetwork ** firstPopulation;
        struct NeuralNetwork ** secondPopulation;



    }Population;


    //
    typedef struct Thread
    {
        Population * population;

        pthread_t * id;
        // short lock;
        // pthread_mutex_t mutex;

        size_t numThread;
        size_t size;
        size_t debut;
        size_t fin;
        int count;

        NeuralNetwork ** ListNeuralNetwork_A;
        NeuralNetwork ** ListNeuralNetwork_B;



    }Thread;


    Config params;
    unsigned int  seed;


    pthread_barrier_t barrier1;
    pthread_barrier_t barrier2;
    pthread_barrier_t barrier3;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                              Other                                /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    double sigmoid(double x);
    double rand_gen();
    double normalRandom();

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
    );

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                              Layer                                /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    Layer * newLayer();

    void initWeigth(Layer * layer, size_t previousSize);

    void setInput(Layer * layer, double * inputList);

    void computeLayer(Layer * layer);

    void mutateLayer(Layer * layer);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                          NeuralNetwork                            /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    NeuralNetwork * newNeuralNetwork( size_t numNN );

    size_t computeNN(NeuralNetwork * nn, double * inputList );

    size_t output(NeuralNetwork * nn );

    void crossover(NeuralNetwork * nn, NeuralNetwork * father, NeuralNetwork * mother );

    void mutate(NeuralNetwork * nn );

    void setScore(NeuralNetwork * nn, double score,  double fruit);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                           population                              /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    Population * newPopulation();

    void swap(Population *population );

    NeuralNetwork * pickOne(Population *population );

    NeuralNetwork * bestElement(Population *population );

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                             Thread                                /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void boucle( int a );

    Thread * NewThread(Population *population, size_t numThread, pthread_t * id);

    void *runFils(void *voidThread );

    void runPere( );

    void calculateFitness(Thread *thread );

    void evolve(Thread *thread );

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                              Game                                 /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void game(NeuralNetwork * nn);

    void playBest( NeuralNetwork * nn);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                            Affichage                              /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void printNetwork(NeuralNetwork *nn );

    void printPopulaton(Population *population );

    void afficherJeu(int resultat, Snake * snake );

    void jump(int a);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                       garbage collector                           /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void freePopulation( Population * population);

    void freeNeuralNetwork( NeuralNetwork * nn);

    void freeLayer( Layer * layer);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////                               Log                                 /////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    FILE* openLog( char *fileName );

    void writeLogScore ( FILE* fichier,  Population * population );

    void writeLogId ( FILE* fichier,  Population * population );

    void writeLogFruit( FILE* fichier,  Population * population );

    void closeLog( FILE* fichier);



#endif
