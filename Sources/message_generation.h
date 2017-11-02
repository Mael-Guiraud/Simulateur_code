
int ** init_nodes_antenas(int nb_nodes, int nb_antenas, int period,int nb_BBU, Policy mode,int ** nodes_positions,int * antenas_distrib, int res_kind, int emission_gap, int emission_time,int ring_size,int ** shift);
void free_nodes_antenas(int ** nodes, int nb_nodes, int nb_BBU);
void generation_BE(Queue * BE_Q, int nb_nodes,int size_BE, int current_slot, int max_size, float *** vectors,float ** chain, int* state);
void generation_CRAN(Queue* CRAN_Q,int** nodes_antenas, int nb_nodes, int nb_antenas, int current_slot, int size_CRAN,int nb_BBU, int period, int max_size,int emission_time, int emission_gap,int* antenas_distrib,Policy mode,int ** nodes_positions,int ring_size,int ** shift);
void generation_answers(Packet* ring, int** nodes_positions, Queue* CRAN_Q, int nb_BBU, int ring_size, int current_slot, int size_CRAN, int max_size);
int insert_packets(Queue* BE_Q, Queue * CRAN_Q, Packet* ring, int** nodes_positions,int packet_size, int minimal_buffer_size, Policy mode, int nb_nodes, int size_CRAN, int size_BE, int max_size,int current_slot,int nb_BBU, float* tab_BE,float * tab_CRAN, float* tab_ANSWERS,float* tab_BE_BBU,int time_before_measure,int table_Size);
void remove_packets(int** nodes_positions, Packet* ring, int nb_nodes,int ring_size);
void reservation_management(Packet* ring, int ring_size, int** nodes_antenas, int** nodes_positions, int nb_nodes, int current_slot,int nb_BBU, int period, int emission_time, int emission_gap,int * antenas_distrib,Policy mode,int ** shift);