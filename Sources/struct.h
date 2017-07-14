typedef struct packet{
	int owner;
	int nb_CRAN;
} Packet;

//Different kinds of messages sent by the servers
typedef enum policy{
    NO_MANAGMENT, CRAN_FIRST
} Policy;

#define DEBUG 1