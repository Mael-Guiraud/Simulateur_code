void rotate_ring(Packet * ring, int size);
int* init_nodes_positions(int nb_nodes,int size);
Packet* init_ring(int size);
int * init_nodes_queues(int nb_nodes);
void simulate(int ring_size, int nb_nodes,int nb_antenas, int period, float burst_proba,float lambda_burst,float lambda_regular,int minimal_buffer_size,int nb_BBU,int size_CRAN,int size_BE,int packet_size, Policy mode, int simulation_lenght);