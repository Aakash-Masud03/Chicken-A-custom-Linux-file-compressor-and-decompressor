

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int main (int argc, char *argv[]) {

    if (argc != 2) return 1;

    
    FILE *output_stream = fopen(argv[1], "r");
    int c;
    int i = 0;
    while ((c = fgetc(output_stream)) != EOF ) {
        if (isprint(c) != 0 ) {
        printf("byte %4d: %3d 0x%02x \'%c", i, c, c, c);
      
        printf("\'\n");
        } else {
            printf("byte %4d: %3d 0x%02x \n", i, c, c);
        }

        
        i++;
    }


    return 0;
}