#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

uint8_t buffer[512];

uint32_t count_sector;
uint32_t count_start_of_image;
uint32_t count_app0;
uint32_t count_exif;
uint32_t count_ffed;
uint32_t count_ffd8;
uint32_t count_ffdb;
uint32_t image_no;

void report(void) {
   printf("\n");
   printf("%i GB inspected\n",         count_sector/2000000);
   printf("%i sectors inspected\n",    count_sector);
   printf("%i start_of_image count\n", count_start_of_image);
   printf("%i APP0 count\n",           count_app0);
   printf("%i EXIF count\n",           count_exif);
   printf("%i FFED count\n",           count_ffed);
   printf("%i FFD8 count\n",           count_ffd8);
   printf("%i FFDB count\n",           count_ffdb);
}
static void extract_image(FILE *f, off_t pos, off_t size) {
   FILE *new;
   char fname[40];
   char *buffer = malloc(size);

   if(buffer == NULL) {
      fprintf(stderr,"Out of mem\n");
      return;
   } 
   fseek(f, pos,SEEK_SET);
   if(fread(buffer,size,1,f) != 1) {
      fprintf(stderr,"Unable to read data\n");
      free(buffer);
      return;
   }


   sprintf(fname,"out/%010d.jpg",image_no+1);
   new = fopen(fname, "wb");
   if(new != NULL) {
      fwrite(buffer,size,1,new);
      fclose(new);
      image_no++;
      printf("Extracted '%s'\n",fname);
   } else {
      fprintf(stderr,"Unable to open '%s'\n",fname);
   }
   free(buffer);
}

void read_blocks(FILE *f, off_t pos) {
   int type, sizeh, sizel, size;
   int c, cl;
   int keep = 0;

   fseek(f, pos+2,SEEK_SET);
   while(1) {
      c = getc(f);
      if(c != 0xFF) {
         break;
      }

      type = getc(f);
      if(type == EOF) 
         return;
      sizeh = getc(f);
      if(sizeh == EOF) 
         return;
      sizel = getc(f);
      if(sizel == EOF) 
         return;
      size = sizeh*256+sizel;
   
      switch(type) {
         case 0xE0:
            keep = 1;
            count_app0++;
            break;
   
         case 0xE1:
            keep = 1;
            count_exif++;
            break;
   
         case 0xED:
            count_ffed++;
            break;

         case 0xD8:
            count_ffd8++;
            break;
   
         case 0xDB:
            count_ffd8++;
            break;

         default:
            break;
      }
      // Skip over data
      fseek(f,size-2,SEEK_CUR);
   } 

   if(keep) { 
      do {
        cl = c; 
        c = getc(f); 
      } while(c != EOF && !(cl == 0xFF && c == 0xD9));
      if(c != EOF) {
        size = ftell(f)-pos;
        if(size < 15*1024*1024) {
           extract_image(f,pos,size);
        }
      } 
   }
}

int main(int argc, char *argv[]) {
   FILE *f;
   if(argc != 2) {
      fprintf(stderr,"Must supply file name\n");
      return 0;
   }

   f = fopen(argv[1],"rb");
   if(f == NULL) {
      fprintf(stderr,"Unable to open '%s'\n",argv[1]);
      return 0;
   }

   while(1) { 
      off_t pos;
      if(fread(buffer, 512, 1, f) != 1) {
         break;
      } 
      count_sector++;
      if(count_sector % 2000000 == 0) {
         report();
      }
      if(buffer[0] != 0xFF || buffer[1] != 0xD8) {
         continue;
      }
      count_start_of_image++;

      if(buffer[2] != 0xFF) {
         continue;
      }
      // Remember where we are 
      pos = ftell(f);

      // Extract the data
      read_blocks(f,pos-512);

      // Return to where we were
      fseek(f,pos,SEEK_SET);
   }

   fclose(f);
   report();
   return 0;
}
