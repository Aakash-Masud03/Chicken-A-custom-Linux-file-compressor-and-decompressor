////////////////////////////////////////////////////////////////////////
// COMP1521 21T3 --- Assignment 2: `chicken', a simple file archiver
// <https://www.cse.unsw.edu.au/~cs1521/21T3/assignments/ass2/index.html>
//
// Written by Mubtasim Masud (z5362052) on STARTED 14/11/2021
//
// 2021-11-08   v1.1    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "chicken.h"


// ADD ANY extra #defines HERE
#define MAX_NAME_SIZE 100000
#define FILE_SIZE_LENGTH  6
#define NOT_LAST_FILE 0
#define PATHNAME_OFFSET 14

// ADD YOUR FUNCTION PROTOTYPES HERE
uint64_t get_file_size (FILE *f);
uint16_t get_pathname_size (FILE *f);
int number_of_files(char *egg_pathname);
int print_filename (char *egg_pathname, int file_number);
int get_egglet_length (char *egg_pathname, int file_number);
int permissions_to_int(char permissions[10]);
int return_hash (char *pathname, int file_number);

// print the files & directories stored in egg_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)

void list_egg(char *egg_pathname, int long_listing) {


    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }

    if (long_listing) {
        int c = fgetc(f);
        while ((c = fgetc(f)) != EOF) {
            

            int format_number = c; // saveing format number to print for later
          
            //getting and then printing the permissions for the egglet
            char *permissions = malloc(EGG_LENGTH_MODE + 1);
            fread(permissions, 1, EGG_LENGTH_MODE, f);
            permissions[EGG_LENGTH_MODE] = '\0';

            printf("%s", permissions);

            free(permissions);

            printf("  %c",format_number);
            
            
            uint16_t pathname_length = get_pathname_size(f);

            //using pathname length to retrieve the pathnname
            char name[pathname_length + 1];
            for (int i = 0; i < pathname_length; i++) {
                name[i] = fgetc(f);
            }
            name[pathname_length] = '\0'; // adding a NULL terminator because name will be used in a printf later


           
            uint64_t file_size = get_file_size(f);
            
            printf("  %5lu  %s\n", file_size, name); 

            // skips to the starting magic number of the next file
            for (int i = 0; i < (file_size + 2); i++) {
                c = fgetc(f);
                if (c == EOF) {
                    break;
                }
            }
            
            
        }

        
    } else {
        int c = fgetc(f);
        while ((c = fgetc(f)) != EOF) {
        
            for (int i = 0; i < EGG_LENGTH_MODE; i++) {
                fgetc(f);
                
            }         
            uint16_t pathname_length = get_pathname_size(f);

            //using pathname length to retrieve the pathnname
            char name[pathname_length + 1];
            for (int i = 0; i < pathname_length; i++) {
                name[i] = fgetc(f);
            }
            name[pathname_length] = '\0'; // adding a NULL terminator because name will be used in a printf later


            uint64_t file_size = get_file_size(f);


            printf("%s\n", name); 
            // skips to the starting magic number of the next file
            for (uint64_t i = 0; i < (file_size + 2); i++) {
                c = fgetc(f);
                if (c == EOF) {
                    break;
                }
            }
        }
            
        
        
    }
    
    fclose(f);
}


// check the files & directories stored in egg_pathname (subset 1)
//
// prints the files & directories stored in egg_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be

void check_egg(char *egg_pathname) {
    

    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }
    int file_count = number_of_files(egg_pathname);
    
    for (int i = 0; i < file_count; i++) {
        
        int egglet_length = get_egglet_length(egg_pathname, i);
        
        uint calculated_hash = 0;
        uint8_t previous = 0;
        for (int j = 0; j < (egglet_length - 1); j++) {
            int current = fgetc(f);
            if (j == 0) {
                if (current == 'c') {
                     
                } else {
                    fprintf(stderr,"error: incorrect first egglet byte: 0x%x should be 0x63\n", current);
                    exit(1);
                }
            }
            uint8_t current_hash = egglet_hash(previous, current);

            previous = current_hash;
            calculated_hash = current_hash;

            
        }
        uint8_t file_hash = fgetc(f);
        if (calculated_hash == file_hash) {
            print_filename(egg_pathname, i);
            printf(" - correct hash\n");
        } else {
            print_filename(egg_pathname, i);
            printf(" - incorrect hash 0x%x should be 0x%x\n", calculated_hash, file_hash);
        }
    }
    

    fclose(f);
}


// extract the files/directories stored in egg_pathname (subset 2 & 3)

void extract_egg(char *egg_pathname) {
    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }
    
    int file_count = number_of_files(egg_pathname);

    for (int i = 0; i < file_count; i++) {
        char magic_number = fgetc(f);
        if (magic_number != 'c') {
            exit(1);
        }
        char format_number = fgetc(f);
        if (format_number == '8') {
            //silencing dcc unused variable warning becasue it is very annoying
        }

        char permissions[10] = {0};
        for (int j = 0; j < 10; j++) {
            permissions[j] = fgetc(f);
        }
        mode_t permissions_oct = permissions_to_int(permissions);

        int filename_size = get_pathname_size(f);

        char *filename = malloc(filename_size + 1);
        filename[filename_size] = 0; // adding a NULL terminator
        
        
        for (int j = 0; j < filename_size; j++) {
            filename[j] = fgetc(f);
        }

        FILE *newfile = fopen(filename, "w");
        if (chmod(filename, permissions_oct) != 0) {
            perror(filename); // prints why the chmod failed
            exit(1);
        }
        int file_size = get_file_size(f);
        for (int j = 0; j < file_size; j++) {
            int file_content_byte = fgetc(f);
            fputc(file_content_byte, newfile);
        }

        printf("Extracting: ");
        print_filename(egg_pathname, i);
        printf("\n");

        free(filename);

        fclose(newfile);
        fgetc(f); // skipping over hash value

    }

    fclose(f);
}


// create egg_pathname containing the files or directories specified in pathnames (subset 3)
//
// if append is zero egg_pathname should be over-written if it exists
// if append is non-zero egglets should be instead appended to egg_pathname if it exists
//
// format specifies the egglet format to use, it must be one EGGLET_FMT_6,EGGLET_FMT_7 or EGGLET_FMT_8

void create_egg(char *egg_pathname, int append, int format,
                int n_pathnames, char *pathnames[n_pathnames]) {

   if (append) {
        FILE *egglet = fopen(egg_pathname, "ab");
        if (egglet == NULL) {
            perror(egg_pathname);
        }
        
        for (int i = 0; i < n_pathnames; i++) {
            FILE *current_file = fopen(pathnames[i], "r");
            if (current_file == NULL) {
                perror(pathnames[i]);
                exit(1);
            }
            fwrite("c", 1, 1, egglet);
            char hash = egglet_hash(0, 'c');
            hash = egglet_hash(hash, format);
            char n_format[1] = {format};
            fwrite(n_format, 1, 1, egglet);

            struct stat s;
            if (stat(pathnames[i], &s) != 0) {
                perror(pathnames[i]);
                exit(1);
            }
            int permissions = s.st_mode;
            fwrite("-", 1, 1, egglet);
            hash = egglet_hash(hash, '-');
            //owner permissions
            if (permissions & 0400) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0200) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0100) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            // group permissions
            if (permissions & 0040) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0020) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0010) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            //other permissions
            if (permissions & 0004) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0002) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0001) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }

            


            uint16_t pathname_size = strlen(pathnames[i]);
            char pathname_size_byte1[1] = {pathname_size}; 
            fwrite(pathname_size_byte1, 1, 1, egglet);
            hash = egglet_hash(hash, pathname_size_byte1[0]);
            char pathname_size_byte2[1] = {(pathname_size >> 8)}; 
            fwrite(pathname_size_byte2, 1, 1, egglet);
            hash = egglet_hash(hash, pathname_size_byte2[0]);
            
            fwrite(pathnames[i], 1, pathname_size, egglet);
            for (int j = 0; j < pathname_size; j++) {
                hash = egglet_hash(hash, pathnames[i][j]);
            }
            

            uint64_t file_size = s.st_size;
            int shifter = 0;
            for (int j = 0; j < 6; j++) {
                char current_byte[1] = {(file_size >> shifter)};
                fwrite(current_byte, 1, 1, egglet);
                shifter += 8;
                hash = egglet_hash(hash, current_byte[0]);
            }


            for (int j = 0; j < file_size; j++) {
                char file_byte[1] = {fgetc(current_file)};
                fwrite(file_byte, 1, 1, egglet);
                hash = egglet_hash(hash, file_byte[0]);
            }
            char egglet_hash[1] = {hash};
            fwrite(egglet_hash, 1, 1, egglet);


            printf("Adding: %s\n", pathnames[i]);
            fclose(current_file);
            
        }

        fclose(egglet);
    
    } else {
        FILE *egglet = fopen(egg_pathname, "wb");
        if (egglet == NULL) {
            perror(egg_pathname);
        }
        
        for (int i = 0; i < n_pathnames; i++) {
            FILE *current_file = fopen(pathnames[i], "r");
            if (current_file == NULL) {
                perror(pathnames[i]);
                exit(1);
            }
            fwrite("c", 1, 1, egglet);
            char hash = egglet_hash(0, 'c');
            hash = egglet_hash(hash, format);
            char n_format[1] = {format};
            fwrite(n_format, 1, 1, egglet);

            struct stat s;
            if (stat(pathnames[i], &s) != 0) {
                perror(pathnames[i]);
                exit(1);
            }
            int permissions = s.st_mode;
            fwrite("-", 1, 1, egglet);
            hash = egglet_hash(hash, '-');
            //owner permissions
            if (permissions & 0400) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0200) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0100) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            // group permissions
            if (permissions & 0040) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0020) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');
            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0010) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            //other permissions
            if (permissions & 0004) {
                fwrite("r", 1, 1, egglet);
                hash = egglet_hash(hash, 'r');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0002) {
                fwrite("w", 1, 1, egglet);
                hash = egglet_hash(hash, 'w');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }
            if (permissions & 0001) {
                fwrite("x", 1, 1, egglet);
                hash = egglet_hash(hash, 'x');

            } else {
                fwrite("-", 1, 1, egglet);
                hash = egglet_hash(hash, '-');
            }

            


            uint16_t pathname_size = strlen(pathnames[i]);
            char pathname_size_byte1[1] = {pathname_size}; 
            fwrite(pathname_size_byte1, 1, 1, egglet);
            hash = egglet_hash(hash, pathname_size_byte1[0]);
            char pathname_size_byte2[1] = {(pathname_size >> 8)}; 
            fwrite(pathname_size_byte2, 1, 1, egglet);
            hash = egglet_hash(hash, pathname_size_byte2[0]);
            
            fwrite(pathnames[i], 1, pathname_size, egglet);
            for (int j = 0; j < pathname_size; j++) {
                hash = egglet_hash(hash, pathnames[i][j]);
            }
            

            uint64_t file_size = s.st_size;
            int shifter = 0;
            for (int j = 0; j < 6; j++) {
                char current_byte[1] = {(file_size >> shifter)};
                fwrite(current_byte, 1, 1, egglet);
                shifter += 8;
                hash = egglet_hash(hash, current_byte[0]);
            }


            for (int j = 0; j < file_size; j++) {
                char file_byte[1] = {fgetc(current_file)};
                fwrite(file_byte, 1, 1, egglet);
                hash = egglet_hash(hash, file_byte[0]);
            }
            char egglet_hash[1] = {hash};
            fwrite(egglet_hash, 1, 1, egglet);


            printf("Adding: %s\n", pathnames[i]);
            fclose(current_file);
            
        }

        fclose(egglet);
    }
    
    
    
}


// ADD YOUR EXTRA FUNCTIONS HERE


//returns file size of CURRENTLY OPENED file
uint64_t get_file_size (FILE *f) {

    int shifter = 0; // shifts incoming byte the appropriate amount of bits
    uint64_t file_size = 0;
    for (int i = 0; i < FILE_SIZE_LENGTH; i++) {
        uint64_t current_byte = fgetc(f); 
        current_byte <<= shifter;
        shifter += 8;
        file_size |= current_byte;
    }

    return file_size;
        
}
// returns pathname of CURRENTLY OPENED file
uint16_t get_pathname_size (FILE *f) {

    int pathname_length_byte1 = fgetc(f);
    int pathname_length_byte2 = fgetc(f);
    
    pathname_length_byte2 <<= 8;
    uint16_t pathname_length = pathname_length_byte1 | pathname_length_byte2;

    return pathname_length;

}
// returns number of files in an egglet note: file doesnt have to be open
int number_of_files(char *egg_pathname) {
    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }
    int file_count = 0;
    int c = fgetc(f);
        while ((c = fgetc(f)) != EOF) {
            file_count++;
            for (int i = 0; i < EGG_LENGTH_MODE; i++) {
                fgetc(f);
                
            }         
            int pathname_length = get_pathname_size(f);
            if (pathname_length == -1) {
                file_count--;
                break;
            }

            //using pathname length to retrieve the pathnname
            
            for (int i = 0; i < pathname_length; i++) {
                fgetc(f);
            }
            
            uint64_t file_size = get_file_size(f);
 
            // skips to the starting magic number of the next file
            for (uint64_t i = 0; i < (file_size + 2); i++) {
                c = fgetc(f);
                if (c == EOF) {
                    break;
                }
            }


    
        }
    fclose(f);
    return file_count; 
}
// prints the next file in the stream and returns length of egglet, NOTE: FUNCTION DOESN'T PRINT NEWLINE
//file doesnt have to be opened
int print_filename (char *egg_pathname, int file_number) {
    int egglet_length = 14;

    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }

    for (int i = 0; i < (file_number); i++) {
        for (int j = 0; j < 12; j++) {
            fgetc(f); 
        }
        int pathname_size = get_pathname_size(f);
        for (int j = 0; j < pathname_size; j++) {
            fgetc(f);
        }
        int content_length = get_file_size(f);
        for (int j = 0; j < (content_length + 1); j++) {
            fgetc(f);
        }

    }

    for (int i = 0; i < 12; i++) {
        fgetc(f);
        
    }         
    uint16_t pathname_length = get_pathname_size(f);

    //using pathname length to retrieve the pathnname
    char name[pathname_length + 1];
    for (int i = 0; i < pathname_length; i++) {
        name[i] = fgetc(f);
        egglet_length++;
    }
    name[pathname_length] = '\0'; // adding a NULL terminator because name will be used in a printf later


    uint64_t file_size = get_file_size(f);
    egglet_length += 6;


    printf("%s", name); 
    int c;
    for (uint64_t i = 0; i < (file_size + 2); i++) {
        c = fgetc(f);
        egglet_length++;
        if (c == EOF) {
            break;
        }
    }

    egglet_length--;



    fclose(f);
    
    
    return egglet_length;

}
//returns the amount of bytes in an egglet
int get_egglet_length (char *egg_pathname, int file_number) {
    int egglet_length = 14;

    FILE *f = fopen(egg_pathname, "rb");
    if (f == NULL) {
        perror(egg_pathname);
    }

    for (int i = 0; i < (file_number); i++) {
        for (int j = 0; j < 12; j++) {
            fgetc(f); 
        }
        int pathname_size = get_pathname_size(f);
        for (int j = 0; j < pathname_size; j++) {
            fgetc(f);
        }
        int content_length = get_file_size(f);
        for (int j = 0; j < (content_length + 1); j++) {
            fgetc(f);
        }

    }

    for (int i = 0; i < 12; i++) {
        fgetc(f);
        
    }         
    uint16_t pathname_length = get_pathname_size(f);

    
    for (int i = 0; i < pathname_length; i++) {
       fgetc(f);
        egglet_length++;
    }
   


    uint64_t file_size = get_file_size(f);
    egglet_length += 6;


    
    int c;
    for (uint64_t i = 0; i < (file_size + 2); i++) {
        c = fgetc(f);
        egglet_length++;
        if (c == EOF) {
            break;
        }
    }

    egglet_length--;



    fclose(f);
    
    
    return egglet_length;

}
// turns the permission string into octal permissions
int permissions_to_int(char permissions[10]) {
    
    int octal_permissions = 0;

    if (permissions[1] != '-') {
        octal_permissions += 0400;
    }
    if (permissions[2] != '-') {
        octal_permissions += 0200;
    }
    if (permissions[3] != '-') {
        octal_permissions += 0100;
    }
    if (permissions[4] != '-') {
        octal_permissions += 0040;
    }
    if (permissions[5] != '-') {
        octal_permissions += 0020;
    }
    if (permissions[6] != '-') {
        octal_permissions += 0010;
    }
    if (permissions[7] != '-') {
        octal_permissions += 004;
    }
    if (permissions[8] != '-') {
        octal_permissions += 0002;
    }
    if (permissions[9] != '-') {
        octal_permissions += 0001;
    }

    return octal_permissions;
}

int return_hash (char *pathname, int file_number) {


    FILE *f = fopen(pathname, "rb");
    if (f == NULL) {
        perror(pathname);
    }
    
    int calculated_hash = 0;
    for (int i = 0; i < file_number; i++) {
        
        int egglet_length = get_egglet_length(pathname, i);
        
        
        uint8_t previous = 0;
        for (int j = 0; j < (egglet_length - 1); j++) {
            int current = fgetc(f);
            
            uint8_t current_hash = egglet_hash(previous, current);

            previous = current_hash;
            calculated_hash = current_hash;

            
        }
        fgetc(f);
        
    }
    

    fclose(f);
    return calculated_hash;
    }