
#include <math.h>
#include <string.h>

#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"
#include "defs.h"       // header file that contains the necessary function prototypes

void error(char *name);

int main (int argc, char **argv) 
{
  FILE *fp;
  struct TIFF_img input_img, output_img;
  unsigned int** filtered;
  int32_t i,j;

  if ( argc != 2 ) error( argv[0] );

  /* open image file */
  if ( ( fp = fopen ( argv[1], "rb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file %s\n", argv[1] );
    exit ( 1 );
  }

  /* read image */
  if ( read_TIFF ( fp, &input_img ) ) {
    fprintf ( stderr, "error reading file %s\n", argv[1] );
    exit ( 1 );
  }

  /* close image file */
  fclose ( fp );

  /* check the type of image data */
  if ( input_img.TIFF_type != 'g' ) {
    fprintf ( stderr, "error:  image must be grayscale\n" );
    exit ( 1 );
  }

  /* set up structure for output achromatic image */
  /* to allocate a full color image use type 'c' */
  get_TIFF ( &output_img, input_img.height, input_img.width, 'g' );
    
  /* call filterImage function which loops through each */
  /* pixel and applies weighted median filtering */
  filtered = filterImage(input_img);

  /* assign each element of filtered array to the */
  /* monochrome data of the output image TIFF object */
  for (i = 0; i < input_img.height; i++) {
      for (j = 0; j < input_img.width; j++) {
          output_img.mono[i][j] = filtered[i][j];
      }
  }

  /* free data from filtered variable */
  free_img((void*)filtered);

  /* open green image file */
  if ( ( fp = fopen ( "filtered_output.tif", "wb" ) ) == NULL ) {
    fprintf ( stderr, "cannot open file green.tif\n");
    exit ( 1 );
  }

  /* write green image */
  if ( write_TIFF ( fp, &output_img ) ) {
    fprintf ( stderr, "error writing TIFF file %s\n", argv[2] );
    exit ( 1 );
  }

  /* close green image file */
  fclose ( fp );

  /* de-allocate space which was used for the images */
  free_TIFF ( &(input_img) );
  free_TIFF ( &(output_img) );

  return(0);
}

void error(char *name)
{
    printf("usage:  %s  image.tiff \n\n",name);
    printf("this program reads in a 24-bit color TIFF image.\n");
    printf("It then horizontally filters the green component, adds noise,\n");
    printf("and writes out the result as an 8-bit image\n");
    printf("with the name 'green.tiff'.\n");
    printf("It also generates an 8-bit color image,\n");
    printf("that swaps red and green components from the input image");
    exit(1);
}

unsigned int calculateWMAtLocation(struct TIFF_img input, int xpos, int ypos) {
    /* initialize necessary variables & arrays */
    int temp, i, j, k, istar, weightedMedian, sum1, sum2;
    unsigned int input_pixels[25];
    unsigned int weightingFactors[25] = { 1, 1, 1, 1, 1,
    1, 2, 2, 2, 1,
    1, 2, 2, 2, 1,
    1, 2, 2, 2, 1,
    1, 1, 1, 1, 1 };

    /* populating input_pixels array from input TIFF object */
    k = 0;
    for (i = xpos - 2; i < xpos + 3; i++) {
        for (j = ypos - 2; j < ypos + 3; j++) {
            input_pixels[k] = input.mono[i][j];
            k++;
        }
    }

    /* apply brief sorting algorithm for both input pixels */
    /* and corresponding weighting factors */
    for (i = 0; i < 25; i++) {
        for (j = i + 1; j < 25; j++) {
            if (input_pixels[i] < input_pixels[j]) {
                temp = input_pixels[i];
                input_pixels[i] = input_pixels[j];
                input_pixels[j] = temp;

                temp = weightingFactors[i];
                weightingFactors[i] = weightingFactors[j];
                weightingFactors[j] = temp;
            }
        }
    }

    /* initialize summing variables */
    sum1 = 0;
    sum2 = 0;
    for (i = 0; i < 25; i++) {
        sum2 += sum2 + weightingFactors[i];
    }

    /* determining value for weighted median by incrementing */
    /* i* and using Equation 7 from the assignment handout */
    istar = 0;
    while (sum1 < sum2) {
        sum1 = 0;
        sum2 = 0;
        for (i = 0; i <= istar; i++) {
            sum1 += sum1 + weightingFactors[i];
        }
        for (i = istar + 1; i < 25; i++) {
            sum2 += sum2 + weightingFactors[i];
        }

        if (sum1 >= sum2) {
            weightedMedian = input_pixels[istar];
        }
        else {
            istar++;
        }
    }
    return weightedMedian;
}

unsigned int** filterImage(struct TIFF_img input) {
    /* initializing looping variables and 2D storage array */
    int i, j;
    unsigned int** filtered = (unsigned int**)get_img(input.width, input.height, sizeof(unsigned int));

    /* loop through each pixel of input image and apply weighted */
    /* median filtering if proper window can be constructed */
    for (i = 0; i < input.height; i++) {
        for (j = 0; j < input.width; j++) {
            if (i >= 2 && i < input.height - 2 && j >= 2 && j < input.width - 2) {
                filtered[i][j] = calculateWMAtLocation(input, i, j);
            }
            else {
                filtered[i][j] = input.mono[i][j];
            }
        }
    }
    return filtered;
}
