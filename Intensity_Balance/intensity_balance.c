/*
 Intensity balance calculator
 based on the .eps levelscheme format
 *
 * Compile with: gcc -o intbal intensity_balance.c -lm
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <math.h>



int main( int argc, char **argv )
{
  
    
  
  printf("\n\t\t----------------------------------");
  printf("\n\t\t      Intensity balance calculator");
  printf("\n\t\t           v16.08.2015  ");
  printf("\n\t\thttps://github.com/rlica/LevelScheme");
  printf("\n\t\t----------------------------------");
  printf("\n\n");

  if ( argc < 2 || !fopen(argv[1], "r"))  {
    printf(" ERROR: Input file not provided as argument \n");
    printf("Format: 3 Columns ->[Initial Level] [Final Level] [Intensity] [Intensity_err] \n");
    exit(0);
  }

  //Opening input and output files
  FILE *in_file, *out_file;
  char out_name[100], line[1000];
  sprintf(out_name, "result_%s", argv[1]);

  in_file  = fopen(argv[1], "rt");
  out_file = fopen(out_name, "wt");


  //Reading the input
  int nline=0, nlevels=0, status=0;
  double data[1000][4];
  while( fgets(line, 100, in_file) != NULL ) {
    
      // Parse the line
      status = sscanf(line, "%lf%lf%lf%lf\n", &data[nline][0], &data[nline][1], &data[nline][2], &data[nline][3]); //cal[4] is timing delay
      
      if (status <= 0)
        continue;
      else if (status != 4 ) {
        printf("ERROR: Bad input at line %d \n", nline);
        exit(0);
      }
      else nline++;
  }
   
  printf("Read %d lines from %s.\n", nline, argv[1]);

  
  
  //Saving the list of levels


  double level[nline],
        i_up[nline], err_i_up[nline],
        i_down[nline], err_i_down[nline],
        feeding[nline], err_feeding[nline];
  double prev_level=-1.0;
  int i, j;

  for (i=0; i<nline; i++) {
       level[i]=10000.0;
        i_up[i]=0.0;
      i_down[i]=0.0;
     feeding[i]=0.0;
    err_i_up[i]=0.0;
  err_i_down[i]=0.0;
 err_feeding[i]=0.0;
  }

  //Getting an ordered list of levels
  for (i=0; i<nline; i++) {
    //printf("%d      %f %f %f %f \n", i, data[i][0], data[i][1],data[i][2], data[i][3]);
    for (j=0; j<nline; j++) {   
      if (level[i] > data[j][0] && data[j][0] > prev_level ) level[i] = data[j][0];
      if (level[i] > data[j][1] && data[j][1] > prev_level ) level[i] = data[j][1];
    }
        
    if (level[i] == 10000.0) {
      nlevels = i;
      break;
    }
    else prev_level = level[i];

  }
  printf("Identified %d levels \n", nlevels);



  
  //Calculating the up and down intensity on each level
  for (i=0; i<nlevels; i++)
    for (j=0; j<nline; j++) {
      if (level[i] == data[j][0]) {
        i_down[i]     += data[j][2];
        err_i_down[i] += data[j][3]*data[j][3];
      }
      else if (level[i] == data[j][1]) {
            i_up[i] += data[j][2];
        err_i_up[i] += data[j][3]*data[j][3];
      }
    
  }

 
  //Calculating the feeding and writing in the file
  printf("Results printed in: %s \n\n", out_name);
  printf("Energy        Int    Int_err \n");
  for (i=0; i<nlevels; i++) {
    feeding[i] = i_down[i] - i_up[i];
    err_feeding[i] = sqrt(err_i_down[i]+err_i_up[i]);
    fprintf(out_file, "%6.1lf %10.1lf %10.1lf \n", level[i], feeding[i], err_feeding[i]);
     printf(          "%6.1lf %10.1lf %10.1lf \n", level[i], feeding[i], err_feeding[i]);
    
  }

  

  exit(0);
   
}//end of main 

