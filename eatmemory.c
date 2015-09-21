/*
 * File:   eatmemory.c
 * Author: Julio Viera <julio.viera@gmail.com>
 *
 * Created on August 27, 2012, 2:23 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

int swappable = 0;

#ifdef _SC_PHYS_PAGES
size_t getTotalSystemMemory(){
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

size_t getFreeSystemMemory(){
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#endif

bool eat(long total,int chunk){
	long i;
	for(i=0;i<total;i+=chunk){
		short *buffer=malloc(sizeof(char)*chunk);
        if(buffer==NULL){
            return false;
        }
		memset(buffer,0,chunk);

        if (!swappable){
            printf("Lock the requested memory address\n");
            // Lock the requested memory address
            int ret = mlock(buffer, chunk);
            if (ret<0) {
                fprintf(stderr, "mlock() failure: error(%s)\n", strerror(errno));
            }
        }
	}
    return true;
}

int main(int argc, char *argv[]){

#ifdef _SC_PHYS_PAGES
    printf("Currently total memory: %zd\n",getTotalSystemMemory());
    printf("Currently avail memory: %zd\n",getFreeSystemMemory());
#endif

    int i;
    for(i=0;i<argc;i++){
        char *arg=argv[i];
        if(strcmp(arg, "-h")==0 || strcmp(arg,"-?")==0  || argc==1){
            printf("Usage: eatmemory [-s] <size>\n");
            printf("-s: swappable\n");
            printf("Size can be specified in megabytes or gigabytes in the following way:\n");
            printf("#          # Bytes      example: 1024\n");
            printf("#M         # Megabytes  example: 15M\n");
            printf("#G         # Gigabytes  example: 2G\n");
#ifdef _SC_PHYS_PAGES            
            printf("#%%         # Percent    example: 50%%\n");
#endif            
            printf("\n");
            break;
        }else if(!strcmp(arg, "-s")){
            swappable = 1;
        }else if(i>0){
            int len=strlen(arg);
            char unit=arg[len - 1];
            long size=-1;
            int chunk=1024;
            if(!isdigit(unit) ){
                if(unit=='M' || unit=='G'){
                    arg[len-1]=0;
                    size=atol(arg) * (unit=='M'?1024*1024:1024*1024*1024);
                }
#ifdef _SC_PHYS_PAGES                
                else if (unit=='%') {
                    size = (atol(arg) * (long)getFreeSystemMemory())/100;
                }
#endif                
                else{
                    printf("Invalid size format\n");
                    exit(0);
                }
            }else{
                size=atoi(arg);
            }
            printf("Eating %ld bytes in chunks of %d...\n",size,chunk);
            if(eat(size,chunk)){
                printf("Done, press any key to free the memory\n");
                getchar();
            }else{
                printf("ERROR: Could not allocate the memory");
            }
        }
    }
    return 0;
}

