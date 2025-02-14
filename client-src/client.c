#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>        
#include <sys/ioctl.h>        
#include <bits/ioctls.h>      
#include <net/if.h>           
#include <linux/if_ether.h>   
#include <linux/if_packet.h>  
#include <net/ethernet.h>

#include "ini_parser.h"
#include "udp_client.h"
#include "tcp_client.h"
#include "logger.h"

#define INI_NAME "config.ini"

/**
 * @brief Performs configuration for the client
 * 
 * @return int failure or success
 */
int main(int argc, char* argv[]) {
    LOGP("Client Started :)\n");

    char *filename = calloc(strlen(INI_NAME), sizeof(char)); // defaulting a config file
    strcpy(filename, INI_NAME);
    if(argc == 2) {
        printf("Using the configuration file from command line\n");
        memset(filename, 0, sizeof(INI_NAME));
        filename = calloc(strlen(argv[1]), sizeof(char));
        strcpy(filename, argv[1]); // name for the config file is provided
    } else {
        printf("Defaulting to 'config.ini'\n");
    }
    LOG("Size of ini_info struct: %ld\n", sizeof(struct ini_info));
    struct ini_info *info = calloc(1, sizeof(struct ini_info));
    LOGP("Set struct ini_info and filename\n");
    strncpy(info->file_name, filename, 1023);
    LOGP("Copied file name into struct\n");

    if(parse_ini(info) == -1) {
        fprintf(stderr, "Failed to parse the INI file!\n");
        return EXIT_FAILURE;
    }
    LOGP("Successfully parsed INI file\n");

    send_config(info);

    int i = udp_train(info);
    if(i == 0) {
        LOGP("UDP Client was successful!\n");
    }
    
    double low_arrival, high_arrival;
    i = receive_results(info->server_port, info, &low_arrival, &high_arrival);
    if(i == 0) {
        LOGP("Results were received successfuly!\n");
    }

    /* Filling our data with info from retrieved ini struct */
    /* Information needed for a client: all of the fields */

    // FOR THE FOLLOWING WE WILL USE THE SELECTION INTEGER TO DECIDE WHICH SERVICE TO START
    // establish UDP communication between a udp client and a server
    // establish TCP communication between a server and tcp client

    free(filename);
    free(info);
    return EXIT_SUCCESS;
}
