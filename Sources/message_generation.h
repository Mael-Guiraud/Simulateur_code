
int ** init_nodes_antenas(int nb_nodes, int nb_antenas, int period,int nb_BBU);
void free_nodes_antenas(int ** nodes, int nb_nodes, int nb_BBU);
void generation_BE(int * BE_Q, int nb_nodes,float burst_proba, float lambda_burst, float lambda_regular,int size_BE);
void generation_CRAN(int* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int nb_BBU, int period);
void generation_answers(Packet* ring, int* nodes_positions, int* CRAN_Q, int nb_BBU, int ring_size);
void insert_packets(int* BE_Q, int * CRAN_Q, Packet* ring, int* nodes_positions,int packet_size, int minimal_buffer_size, Policy mode, int nb_nodes);
void remove_packets(int* nodes_positions, Packet* ring, int nb_nodes,int ring_size);