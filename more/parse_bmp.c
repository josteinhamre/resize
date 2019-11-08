// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
  if (argc != 2)
  {
    printf("Usage: copy infile outfile\n");
    return 1;
  }

    // remember filenames
  char *infile = argv[1];

    // open input file
  FILE *inptr = fopen(infile, "r");
  if (inptr == NULL)
  {
    printf("Could not open %s.\n", infile);
    return 2;
  }

    // read infile's BITMAPFILEHEADER
  BITMAPFILEHEADER bf;
  fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

  printf("bfType: %i\n", bf.bfType);
  printf("bfSize: %i\n", bf.bfSize);
  printf("bfReserved1: %i\n", bf.bfReserved1);
  printf("bfReserved2: %i\n", bf.bfReserved2);
  printf("bfOffBits: %i\n", bf.bfOffBits);

    // read infile's BITMAPINFOHEADER
  BITMAPINFOHEADER bi;
  fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

  printf("biSize: %i\n", bi.biSize);
  printf("biWidth: %i\n", bi.biWidth);
  printf("biHeight: %i\n", bi.biHeight);
  printf("biPlanes: %i\n", bi.biPlanes);
  printf("biBitCount: %i\n", bi.biBitCount);
  printf("biCompression: %i\n", bi.biCompression);
  printf("biSizeImage: %i\n", bi.biSizeImage);
  printf("biXPelsPerMeter: %i\n", bi.biXPelsPerMeter);
  printf("biYPelsPerMeter: %i\n", bi.biYPelsPerMeter);
  printf("biClrUsed: %i\n", bi.biClrUsed);
  printf("biClrImportant: %i\n", bi.biClrImportant);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
  if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
    bi.biBitCount != 24 || bi.biCompression != 0)
  {
    fclose(inptr);
    printf("Unsupported file format.\n");
    return 4;
  }

    // determine padding for scanlines
  int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
  for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
  {
            // iterate over pixels in scanlines
    for (int j = 0; j < bi.biWidth; j++)
    {
                // temporary storage
      RGBTRIPLE triple;

                // read RGB triple from infile
      fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

      (triple.rgbtRed > 10) ? printf("%i ", triple.rgbtRed): printf("%i   ", triple.rgbtRed);;
      (triple.rgbtGreen > 10) ? printf("%i ", triple.rgbtGreen): printf("%i   ", triple.rgbtGreen);;
      (triple.rgbtBlue > 10) ? printf("%i ", triple.rgbtBlue): printf("%i   ", triple.rgbtBlue);;
                // printf("%i ", triple.rgbtRed);
                // printf("%i ", triple.rgbtGreen);
                // printf("%i ", triple.rgbtBlue);
      printf("| ");
    }

            // skip over padding, if any
    fseek(inptr, padding, SEEK_CUR);
    printf("\n");
  }

    // close infile
fclose(inptr);

    // success
return 0;
}

