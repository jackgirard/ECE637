
#include <math.h>
#include "tiff.h"
#include "allocate.h"
#include "randlib.h"
#include "typeutil.h"

void error(char *name);
// initialize limitIntensity function
int limitIntensity(double value);
// initialize applyFilter function
void applyFilter(struct TIFF_img* output_img, struct TIFF_img* input_img, double** usedFilter, int PSF_dim, double lambda);

int main (int argc, char **argv) 
{
    FILE *fp;
    struct TIFF_img input_img, color_img;

    // adjustment: if number of arguments is greater than 3, since third will be lambda value
    if ( argc > 3 ) error( argv[0] );

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
    if ( input_img.TIFF_type != 'c' ) {
    fprintf ( stderr, "error:  image must be 24-bit color\n" );
    exit ( 1 );
    }

    // declare 1D array of double pointers filter
    double* filter[5];
    // declare double to store scaling factor lambda - default value below
    double lambda = 1.0;
    if (argc > 2) {
        lambda = atof(argv[2]);
    }

    // iterate through 1D array filter and allocate enough memory for 9 doubles each
    for (int i = 0; i < 5; i++) {
        filter[i] = malloc(sizeof(double) * 9);
    }
    // populate filter array according to g(m,n)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (i == 2 && j == 2) {
                filter[i][j] = 1 + lambda * (1 - 1.0 / 25.0);
            }
            else {
                filter[i][j] = lambda * (-1.0 / 25.0);
            }
        }
    }
    
    /* set up structure for output color image */
    /* Note that the type is 'c' rather than 'g' */
    get_TIFF ( &color_img, input_img.height, input_img.width, 'c' );

    // declare and initialize integer to store the dimension of the point spread function
    int PSF_dim = 5;

    // apply filter using applyFilter function as defined below main
    applyFilter(&color_img, &input_img, filter, PSF_dim, lambda); 
    
    /* open color image file */
    if ( ( fp = fopen ( "sharpened.tif", "wb" ) ) == NULL ) {
        fprintf ( stderr, "cannot open file color.tif\n");
        exit ( 1 );
    }
    
    /* write color image */
    if ( write_TIFF ( fp, &color_img ) ) {
        fprintf ( stderr, "error writing TIFF file %s\n", argv[2] );
        exit ( 1 );
    }
    
    /* close color image file */
    fclose ( fp );

    /* de-allocate space which was used for the images */
    free_TIFF ( &(input_img) );
    free_TIFF ( &(color_img) );

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

// limitIntensity function definition
int limitIntensity(double inputValue) {
    // declare an integer variable newValue and initialize it to zero
    int newValue = 0;
    // if input value parameter is less than zero, assign new value to 0
    if (inputValue < 0) {
        newValue = 0;
    } 
    // if input value parameter is greater than 255, assign new value to 255
    else if(inputValue > 255) {
        newValue = 255;
    }
    // otherwise, assign new value to the input value parameter re-cast as an integer
    else {
        newValue = (int)inputValue;
    }
    return newValue;
}

// applyFilter function definition
void applyFilter(struct TIFF_img* output_img, struct TIFF_img* input_img, double **usedFilter, int PSF_dim, double lambda) {
    // declare and define N - the dimension of the point spread function (PSF)
    int N = (PSF_dim - 1) / 2;
    // declare and define image height and width based on input image TIFF struct methods
    int img_height = input_img->height;
    int img_width = input_img->width;
    // declare doubles to store red, green, and blue value for each pixel
    double redPlane, greenPlane, bluePlane;
    // declare PSF variables
    int m, n;
    // declare variables to store current location within PSF
    int a, b;
    // for each pixel
    for (int i = 0; i < img_height; i++) {
        for (int j = 0; j < img_width; j++) {
            // initialize RGB values to zero
            redPlane = 0.0;
            greenPlane = 0.0;
            bluePlane = 0.0;
            // for each pixel in the PSF (5*5 in this case)
            for (m = -N; m <= N; m++) {
                for (n = -N; n <= N; n++) {
                    // assign a and b to current PSF matrix location
                    a = i - m;
                    b = j - n;
                    // if a and b are within the image boundaries
                    if (a >= 0 && a < img_height && b >= 0 && b < img_width) {
                        // apply filter by summing across PSF according to difference equation for 2D filters
                        redPlane += usedFilter[m + N][n + N] * input_img->color[0][a][b];
                        greenPlane += usedFilter[m + N][n + N] * input_img->color[1][a][b];
                        bluePlane += usedFilter[m + N][n + N] * input_img->color[2][a][b];
                    }
                }

            }
            // populate output image method for color after calling limitIntensity function to ensure acceptable RGB values
            output_img->color[0][i][j] = limitIntensity(redPlane);
            output_img->color[1][i][j] = limitIntensity(greenPlane);
            output_img->color[2][i][j] = limitIntensity(bluePlane);
        }
    }
}