//init fuctions
float ** init_chain(int nb_states);
float *** init_vectors(int nb_states);
void read_SBBP_file(float ** chain, float *** vectors, int nb_states, int *nb_elems);

//free functions
void free_chain(float ** chain,int nb_states);
void free_vectors(float *** vectors, int nb_states,int * nb_elems);

//make a drawing depending of the state of the chain
int sbbp_generation(float *** vectors, int state);
//change the state of the chain
int change_state(float ** chain, int actual_state);


//Trial functions
void print_matrix(float ** chain, int nb_states);
void print_vectors(float *** vectors, int nb_states);
void stationary_distribution(float ** chain, int nb_states);

