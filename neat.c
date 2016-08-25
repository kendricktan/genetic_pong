// Inputs = distance between ball and paddle
// Outputs up, or down
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Probability stuff
#define INPUTS 1
#define OUTPUTS 1

#define MINFITNESS 5 // minimum must have scored at least 2 points in order to not be removed 

#define POOLSIZE 10
#define MAXGENES 4
#define GENERATIONS 500

#define MUTATIONRATE 0.25
#define ELIMINATIONRATE 0.1
#define MATERATE 0.2

// Which genome are we currently on?
int current_genome = 0;

// Genetics!
struct Gene{    
    double weight; // Whats the weighting on this particular gene?
    double result; // Whats the result?

    struct Gene *in; // Which gene do we go in via?
    struct Gene *out; // Which gene do we go out?    
};

struct Genome{
    struct Gene *genes;
    double fitness;    
};

struct Genome *genomes; // Our pool of genomes

// Random numbers
double randBetween(double M, double N){        
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;  
}

// Generate a gene with a random weight
struct Gene gen_gene(void){
    struct Gene gene;
    gene.result = 0;
    gene.weight = randBetween(-5, 5);    
    gene.in = malloc(sizeof(struct Gene*));
    gene.out = malloc(sizeof(struct Gene*));
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

// Mates between two genome
struct Genome mate(struct Genome genome1, struct Genome genome2){
    struct Genome genome;    
    genome.genes = malloc(MAXGENES*sizeof(struct Gene));
    genome.fitness = 0;
    for (int i = 0; i < MAXGENES; i++){
        // Generate gene and then assign
        genome.genes[i] = gen_gene();        

        // LOL
        if (rand() & 1){            
            genome.genes[i].weight = genome1.genes[i].weight;            
        }

        else{
            genome.genes[i].weight = genome2.genes[i].weight;
        }            
    }        
    return genome;
}

// Mutates a genome
void mutate(struct Genome *genome){
    for (int i = 0; i < MAXGENES; i++){
        int out = randBetween(0, MAXGENES-1);
        int in = randBetween(0, MAXGENES-1);

        // Can't have same in out
        while(in == out){
            in = randBetween(0, MAXGENES-1);
        }


        if (rand() & 1){
            genome->genes[i].in = &genome->genes[in];
        }

        if (rand() & 1){
            genome->genes[i].out = &genome->genes[out];
        }
    }
}

// Randomly point genes within a genome towards each other
void randomize_genes(struct Genome *genome){
    for (int i = 0; i < MAXGENES; i++){
        int out = randBetween(0, MAXGENES);
        int in = randBetween(0, MAXGENES-1);

        // Can't have same in out
        while(in == out){
            in = randBetween(0, MAXGENES-1);
        }

        genome->genes[i].out = &genome->genes[out];
        genome->genes[i].in = &genome->genes[in];
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

// Sort pool by fitness
// fucking bubble sort #yolo
void sort_genome_pool(void){
    for (int i = 0; i < POOLSIZE; i++){
        for (int j = 0; j < POOLSIZE; j++){
            if (genomes[i].fitness > genomes[j].fitness){
                struct Genome temp = genomes[j];
                genomes[j] = genomes[i];
                genomes[i] = temp;
            }
        }
    }
}

// Gets our genome with top fitness 
// and try and mutate between them 
void evolve(void){
    // Sort our genepool first
    sort_genome_pool();

    // Create new good genes
    for (int i = 0; i < POOLSIZE; i++){
        printf("fitness %d: %.2f\n", i, genomes[i].fitness);        
        if(genomes[i].fitness < MINFITNESS){        

            if (randBetween(0.0, 1.0) < MATERATE){
                genomes[i] = mate(genomes[0], genomes[1]);
            }
            else{
                genomes[i] = gen_genome();
            }
        }

        if (randBetween(0, 1) < MUTATIONRATE){
            mutate(&genomes[i]);
        }        
    }
}

// Predict output stuff
double predict(int genome_no, double input){
    // Assume first gene has no in
    genomes[genome_no].genes[0].out->result = genomes[genome_no].genes[0].weight * input;

    for (int i = 1; i < MAXGENES; i++){      
        struct Gene *cur_gene = &genomes[genome_no].genes[i]; 
        
        cur_gene->result = cur_gene->in->weight*cur_gene->in->result;
        cur_gene->out->result = cur_gene->weight*cur_gene->result;
    }

    // Assume last gene is output
    return genomes[genome_no].genes[MAXGENES-1].result;
}

// Print fittest genome 
void print_genome_fittest(void){
    double fittest = 0;
    for (int i = 0; i < POOLSIZE; i++){
        if (genomes[i].fitness > fittest){
            fittest = genomes[i].fitness;
        }
    }
    printf("%.2f\n", fittest);
}

double sigmoid(double x){
    return 1/(1+exp(x));
}

int iimain(){

    // More random numbers!
    srand(time(NULL));

    gen_genome_pool();

    // test, try and get number to as big as possible 
    double i_input = 32767.32;
    for (int j = 0; j < GENERATIONS; j++){
        for(int i = 0; i < POOLSIZE; i ++){
            genomes[i].fitness = predict(i, i_input);
        }
        evolve();
        printf("Evolution %d, fittest: ", j);
        print_genome_fittest();
        sleep(1);
    }

    free(genomes);
    return 0;
}