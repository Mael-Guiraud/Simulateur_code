void rotate_ring(Packet * ring, int size);
int* init_nodes_positions(int nb_nodes,int size,int * antenas_distrib);
Packet* init_ring(int size);
int * init_nodes_queues(int nb_nodes);
float simulate(int ring_size, int nb_nodes,int nb_antenas, int period,int minimal_buffer_size,int deadline,int nb_BBU,int size_CRAN,int size_BE,int packet_size, int emission_time, int emission_gap,Policy mode, int simulation_lenght,int time_before_measure, int max_size,float * tab_BE,float* tab_CRAN,float* tab_ANSWERS,float * tab_BE_BBU, int tab_size ,float * distrib_be_generation, int * be_offset_generation,int res_kind);