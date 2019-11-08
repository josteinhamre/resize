// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

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
    int multi = strtol(argv[1], NULL, 10);

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
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER and create to versions, the original and the new.
    BITMAPINFOHEADER bi;
    BITMAPINFOHEADER biNew;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    fseek(inptr, sizeof(BITMAPINFOHEADER) * -1, SEEK_CUR);
    fread(&biNew, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    // Assign multiplicated data to the new BITMAPINFOHEADER
    biNew.biWidth *= multi;
    biNew.biHeight *= multi;
    biNew.biSizeImage = sizeof(RGBTRIPLE) * biNew.biWidth * abs(biNew.biHeight);
    bf.bfSize = biNew.biSizeImage + bf.bfOffBits;
    // Calculate the line width, to be used by fseek
    int negLineWidth = (bi.biWidth * sizeof(RGBTRIPLE) * -1);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&biNew, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // repeat the iteration over current scanline x amount of times
        for (int m = 0; m < multi; ++m)
        {
            // iterate over pixels in scanlines
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile, repeat based on x
                for (int l = 0; l < multi; ++l)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }
            // move read position back to the begining of the line, if line is to be repeated.
            if (m < (multi - 1))
            {
                fseek(inptr, negLineWidth, SEEK_CUR);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

