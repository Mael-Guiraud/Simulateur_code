typedef struct packet{
	int owner;
	int nb_CRAN;
	int reserved_for;
	int pre_reserved;
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
    NO_MANAGMENT, CRAN_FIRST, RESERVATION1
} Policy;

#define DEBUG 0