
int ** init_nodes_antenas(int nb_nodes, int nb_antenas, int period,int nb_BBU);
void free_nodes_antenas(int ** nodes, int nb_nodes, int nb_BBU);
void generation_BE(Queue * BE_Q, int nb_nodes,float burst_proba, float lambda_burst, float lambda_regular,int size_BE, int current_slot,int max_size);
void generation_CRAN(Queue* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int nb_BBU, int period, int max_size);
void generation_answers(Packet* ring, int* nodes_positions, Queue* CRAN_Q, int nb_BBU, int ring_size, int current_slot, int size_CRAN, int max_size);
int insert_packets(Queue* BE_Q, Queue * CRAN_Q, Packet* ring, int* nodes_positions,int packet_size, int minimal_buffer_size, Policy mode, int nb_nodes, int size_CRAN, int size_BE, int max_size,int current_slot,int nb_BBU, FILE* f_CRAN,FILE * f_BE, FILE* f_ANSWERS,int time_before_measure);
void remove_packets(int* nodes_positions, Packet* ring, int nb_nodes,int ring_size);