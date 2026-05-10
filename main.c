#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    typedef enum effect {
        INVERT, GREYSCALE, BW, PIXELATE, NONE
    } effect;

    effect fx = NONE;
    char *outfilename;

    // process args

    if (argc > 3) {
        printf("Greater than 2 args passed. Exiting...\n");
        printf("Usage: cat {input} | ./main {effect} {output}\n");
        return 1;
    }
    else if (argv[1] && argv[2]) {
        if (strcmp(argv[1], "invert") == 0) { fx = INVERT; }
        else if (strcmp(argv[1], "greyscale") == 0) { fx = GREYSCALE; }
        else if (strcmp(argv[1], "blackandwhite") == 0) { fx = BW; }
        else if (strcmp(argv[1], "pixelate") == 0) { fx = PIXELATE; }
        else if (strcmp(argv[1], "none") == 0) { fx = NONE; }
        else if (strcmp(argv[1], "help") == 0) {
            printf("Usage: cat {input} | ./main {effect} {output}\n");
            return 1;
        }
        else {
            printf("Unknown fx. Exiting...\n");
            return 1;
        }

        outfilename = (strcat(argv[2], ".ppm"));
    }
    else {
        printf("Too few args passed. Exiting...\n");
        printf("Usage: cat {input} | ./main {effect} {output}\n");
        return 1;
    }

    // read header

    char magic_number[3];
    int width, height, maxval;

    fscanf(stdin, "%s", magic_number);
    fscanf(stdin, "%d %d %d", &width, &height, &maxval);

    // check header

    if (magic_number[0] != 'P' || (magic_number[1] != '3' && magic_number[1] != '6')) {
        printf("Invalid file type. Exiting...\n");
        return 1;
    }

    if (magic_number[1] == '3') { // TODO: process P3 at some point
        printf("Image must be P6. Exiting...\n");
        return 1;
    }

    int num_pixels = width * height;

    unsigned char *pixels;

    pixels = malloc(num_pixels * 3);
    if (!pixels) { perror("malloc"); return 1; }

    fread(pixels, 3, num_pixels, stdin); // read pixels

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
        case NONE:
            break;
        default:
            break;
    }

    // writing to file

    // char *outfilename = "output.ppm";

    FILE *outptr = fopen(outfilename, "w");

    int export = 6; // export as P3 or P6?

    if (export == 3) { //export as P3 (plain text)
        fprintf(outptr, "%s\n", "P3");
        fprintf(outptr, "%d %d\n%d\n", width, height, maxval);

        for (int i = 0; i < num_pixels * 3; i += 3) {
            fprintf(outptr, "%d %d %d\n", pixels[i], pixels[i+1], pixels[i+2]);
        }
    }
    else if (export == 6) { //export as P6 (binary)
        fprintf(outptr, "%s\n", magic_number);
        fprintf(outptr, "%d %d\n%d\n", width, height, maxval);
        fwrite(pixels, 3, num_pixels, outptr);
    }

    free(pixels);
    fclose(outptr);

    printf("Image saved at `./%s`\n", outfilename);

    return 0;
}

