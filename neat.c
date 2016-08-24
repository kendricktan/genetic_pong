// Inputs distance between ball and paddle
// Outputs up, or down
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Probability stuff
#define INPUTS 1
#define OUTPUTS 1

#define POOLSIZE 10
#define MAXGENES 12
#define GENERATIONS 100

#define MUTATIONRATE 0.25
#define ELIMINATIONRATE 0.1

#define TIMEOUT 0.2

// Which genome are we currently on?
int current_genome = 0;

// Genetics!
struct Gene{    
    double weight; // Whats the weighting on this particular gene?
    double result; // Whats the result? (this->result = readFrom->result*this->weight)

    struct Gene *readFrom; // Which gene do we read from?    
};

struct Genome{
    struct Gene *genes;
    int fitness;    
};

struct Genome *genomes; // Our pool of genomes

// Random numbers
double randBetween(double M, double N){        
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;  
}

// Generate a gene with a random weight
struct Gene gen_gene(void){
    struct Gene gene;
    gene.weight = randBetween(-1, 1);    
    gene.readFrom = malloc(sizeof(struct Gene*));
    return gene;
}

// Generates a genome
struct Genome gen_genome(void){
    struct Genome genome;
    genome.genes = malloc(MAXGENES*sizeof(struct Gene));
    genome.fitness = 0;

    for (int i = 0; i < MAXGENES; i++){
        genome.genes[i] = gen_gene();        
    }

    return genome;
}

// Randomly point genes within a genome towards each other
void randomize_genes(struct Genome *genome){
    for (int i = 0; i < MAXGENES; i++){
        int n = randBetween(0, MAXGENES-1);
        genome->genes[i].readFrom = &genome->genes[n];
    }
}

// Generate a pool of genomes
// using the global variable
void gen_genome_pool(void){
    // Memory allocation bitches
    genomes = malloc(POOLSIZE*sizeof(*genomes));

    // Generate our pool and randomize
    // their connections
    for (int j = 0; j < POOLSIZE; j++){
        genomes[j] = gen_genome();
        randomize_genes(&genomes[j]);   
    }
}

// Sort pool by fitness function

// Debug
void print_genome(){
    for (int i = 0; i < POOLSIZE; i++){
        for(int j = 0; j < MAXGENES; j++){            
            double result = genomes[i].genes[j].weight * genomes[i].genes[j].readFrom->weight;

            printf("%.2f\n",result);
        }
    }
}

double sigmoid(double x){
    return 1/(1+exp(x));
}

int main(){
    // More random numbers!
    srand(time(NULL));

    gen_genome_pool();

    print_genome();
    return 0;
}