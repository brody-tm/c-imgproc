#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// imgpc [input filename] [effect] [output filename] [output encoding]

int main(int argc, char *argv[]) {
    typedef enum effect {
        INVERT, GREYSCALE, BW, PIXELATE, BLUEISH, NONE
    } effect;

    effect fx = NONE;
    char *infilename;
    char *outfilename;

    // process args

    if (argc > 5) {
        printf("Too many arguments. Exiting...\n");
        printf("Usage: ./main [input filename] [effect] [output filename] [output encoding]\n");
        return 1;
    }
    else if (argv[1] && argv[2] && argv[3] && argv[4]) {
        infilename = (argv[1]); // should check if this is valid

        if (strcmp(argv[2], "invert") == 0) { fx = INVERT; }
        else if (strcmp(argv[2], "greyscale") == 0) { fx = GREYSCALE; }
        else if (strcmp(argv[2], "blackandwhite") == 0) { fx = BW; }
        else if (strcmp(argv[2], "pixelate") == 0) { fx = PIXELATE; }
        else if (strcmp(argv[2], "blueish") == 0) { fx = BLUEISH; }
        else if (strcmp(argv[2], "none") == 0) { fx = NONE; }
        else if (strcmp(argv[2], "help") == 0) {
            return 1;
        }
        else {
            printf("Unknown fx '%s'. Exiting...\n", argv[2]);
            printf("Usage: ./main [input filename] [effect] [output filename] [output encoding]\n");
            return 1;
        }

        outfilename = (argv[3]); // should check if this is valid

        char *output_enc = argv[4];

        if (strcmp(output_enc, "p3") != 0 && strcmp(output_enc, "p6") != 0) {
            printf("Output encoding much be 'p3' or 'p6'. Is: %s. Exiting...\n", argv[4]);
            printf("Usage: ./main [input filename] [effect] [output filename] [output encoding]\n");
            return 1;
        }

    }
    else {
        printf("Too few args passed. Exiting...\n");
        printf("Usage: ./main [input filename] [effect] [output filename] [output encoding]\n");
        return 1;
    }

    infilename = strcat(infilename, ".ppm");
    outfilename = strcat(outfilename, ".ppm");

    FILE *fptr = fopen(infilename, "rb");

    if (fptr == NULL) {
        printf("File %s does not exist. Exiting...\n", infilename);
        return 1;
    }

    // read header

    char magic_number[3];
    int width, height, maxval;

    fscanf(fptr, "%s", magic_number);
    fscanf(fptr, "%d %d %d", &width, &height, &maxval);

    // check header

    if (magic_number[0] != 'P' || (magic_number[1] != '3' && magic_number[1] != '6')) {
        printf("Invalid file type. Exiting...\n");
        return 1;
    }

    int num_pixels = width * height;
    unsigned char *pixels;

    pixels = malloc(num_pixels * 3);
    if (!pixels) { perror("malloc"); return 1; }

    if (magic_number[1] == '3') {
        for (int i = 0; i < num_pixels * 3; i++) {
            int val;
            fscanf(fptr, "%d", &val);
            pixels[i] = (unsigned char)val;
        }
    }
    else if (magic_number[1] == '6') {
        fread(pixels, 3, num_pixels, fptr); // read pixels
    }

    // EFFECTS

    unsigned char avg;

    switch (fx) {
        case INVERT:
            for (int i = 0; i < num_pixels * 3; i += 3) {
                unsigned char r = pixels[i];
                unsigned char g = pixels[i+1];
                unsigned char b = pixels[i+2];

                pixels[i] = maxval - r;
                pixels[i+1] = maxval - g;
                pixels[i+2] = maxval - b;
            }
            break;
        case GREYSCALE:
            for (int i = 0; i < num_pixels * 3; i += 3) {
                unsigned char r = pixels[i];
                unsigned char g = pixels[i+1];
                unsigned char b = pixels[i+2];

                avg = (r+g+b)/3;

                pixels[i] = avg;
                pixels[i+1] = avg;
                pixels[i+2] = avg;
            }

            break;
        case BW:
            for (int i = 0; i < num_pixels * 3; i += 3) {
                unsigned char r = pixels[i];
                unsigned char g = pixels[i+1];
                unsigned char b = pixels[i+2];

                avg = (r+g+b)/3;

                if (avg > 130) {avg = 255;} else {avg = 0;}

                pixels[i] = avg;
                pixels[i+1] = avg;
                pixels[i+2] = avg;
            }

            break;
        case PIXELATE:
            break;
        case BLUEISH:
            for (int i = 0; i < num_pixels * 3; i += 3) {
                pixels[i] = 0;
                pixels[i+1] = 160;
                // pixels[i+2] = 0;
            }
            break;
        case NONE:
            break;
        default:
            break;
    }

    // writing to file

    // char *outfilename = "output.ppm";

    FILE *outptr = fopen(outfilename, "w");

    int export = 3; // export as P3 or P6?

    if (export == 3) { //export as P3 (plain text)
        fprintf(outptr, "%s\n", "P3");
        fprintf(outptr, "%d %d\n%d\n", width, height, maxval);

        for (int i = 0; i < num_pixels * 3; i += 3) {
            fprintf(outptr, "%d %d %d\n", pixels[i], pixels[i+1], pixels[i+2]);
        }
    }
    else if (export == 6) { //export as P6 (binary)
        fprintf(outptr, "%s\n", "P6");
        fprintf(outptr, "%d %d\n%d\n", width, height, maxval);
        fwrite(pixels, 3, num_pixels, outptr);
    }

    free(pixels);
    fclose(outptr);

    printf("Image saved at `./%s`\n", outfilename);

    return 0;
}

