// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    float multiplication = strtof(argv[1], NULL);
    printf("%.1f\n", multiplication);

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bfOld;
    BITMAPFILEHEADER bfNew;
    fread(&bfOld, sizeof(BITMAPFILEHEADER), 1, inptr);
    fseek(inptr, sizeof(BITMAPFILEHEADER) * -1, SEEK_CUR);
    fread(&bfNew, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER and create to versions, the original and the new.
    BITMAPINFOHEADER biOld;
    BITMAPINFOHEADER biNew;
    fread(&biOld, sizeof(BITMAPINFOHEADER), 1, inptr);
    fseek(inptr, sizeof(BITMAPINFOHEADER) * -1, SEEK_CUR);
    fread(&biNew, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bfOld.bfType != 0x4d42 || bfOld.bfOffBits != 54 || biOld.biSize != 40 ||
        biOld.biBitCount != 24 || biOld.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    // Assign multiplicated data to the new BITMAPINFOHEADER
    biNew.biWidth *= multiplication;
    biNew.biHeight *= multiplication;

    // determine padding for scanlines
    int paddingOld = (4 - (biOld.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int paddingNew = (4 - (biNew.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Continue Assign multiplicated data to the new BITMAPINFOHEADER
    biNew.biSizeImage = sizeof(RGBTRIPLE) * biNew.biWidth * abs(biNew.biHeight) + (biNew.biWidth * paddingNew);
    bfNew.bfSize = biNew.biSizeImage + bfNew.bfOffBits;
    // Calculate the line width, to be used by fseek
    int LineWidthOld = (biOld.biWidth * sizeof(RGBTRIPLE));
    int LineWidthNew = (biNew.biWidth * sizeof(RGBTRIPLE));

    // write outfile's BITMAPINFOHEADER & BITMAPFILEHEADER
    fwrite(&bfNew, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&biNew, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over new files scanlines
    for (int i = 0, biHeight = abs(biNew.biHeight); i < biHeight; i++)
    {
        // Determine wich row to read from and set the position
        int row = i / multiplication;
        int positionY = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (LineWidthOld * row) + (paddingOld * row);
        fseek(inptr, positionY, SEEK_SET);


        // iterate over pixels in scanlines
        for (int j = 0; j < biNew.biWidth; j++)
        {
            // Determine wich column to read from and set the position
            int column = j / multiplication;
            int positionX = positionY + (sizeof(RGBTRIPLE) * column);
            fseek(inptr, positionX, SEEK_SET);

            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding if any
        fseek(inptr, paddingOld, SEEK_CUR);

        // then add it back based on the new dimentions
        for (int k = 0; k < paddingNew; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

