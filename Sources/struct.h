typedef struct packet{
	int owner;
	int nb_CRAN;
} Packet;

typedef struct queue{
	int size;
	int* queue;
	int* kind;
	int min_id;
	int max_id;
}Queue ;
//Different kinds of messages sent by the servers
typedef enum policy{
    NO_MANAGMENT, CRAN_FIRST
} Policy;

#define DEBUG 0