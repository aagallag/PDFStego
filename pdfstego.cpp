/*
 * PDFStego, Copyright (C) 2016  Aaron Gallagher
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contact: Aaron Gallagher <aaron.b.gallagher@gmail.com>
 */

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "stegolib.h"

using namespace std;

char COPYRIGHT[] = "PDFStego, Copyright (C) 2016  Aaron Gallagher\nThis is free software, and you are welcome to redistribute it\n";

char USAGE[] = "Usage:\n\n  ::Embed::\n\t./pdfstego -e -c <coverfile> -m <messagefile> -s <stegofile>\n  ::Extract::\n\t./pdfstego -x -s <stegofile> -m <messagefile>\n";

void printcopyright() {
    printf("%s", COPYRIGHT);
}

void usage() {
    printf("\n%s\n", USAGE);
}

int printerror(const char* errmsg) {
    printf("Error: %s\n", errmsg);
    return -1;
}

int main(int argc, char** argv) {
    int opt;
    char* coverfilename     = NULL;
    char* filenameToHide    = NULL;
    char* stegoFile         = NULL;
    bool isembed = false;
    bool isextract = false;
    char password [128];

    const char* short_opt = "hexc:m:s:";

    printcopyright();

    //Parse the arguments
    while ((opt = getopt(argc, argv, short_opt)) != -1)
    {
        switch (opt)
        {
            case 'e':
                isembed = true;
                break;
            case 'x':
                isextract = true;
                break;
            case 'c':
                coverfilename = optarg;
                break;
            case 'm':
                filenameToHide = optarg;
                break;
            case 's':
                stegoFile = optarg;
                break;
            case 'h':
                usage();
                return 1;
        }
    }

    /* error check the operation */
    if (isembed == true && isextract == true)
    {
        usage();
        return printerror("Can not use both -e(embed) and -x(extract) arguments at the same time\n");
    }


    if (isembed)
    {
        /* error check params for embed operation */
        if (coverfilename == NULL   ||
        filenameToHide == NULL  ||
        stegoFile == NULL)
        {
            usage();
            return printerror("Missing arguments required for the embed operation.\n");
        }

        /* prompt user for password */
        printf("Enter password: ");
        scanf("%127s", password);

        /* perform the operation */
        encemb_pdf(coverfilename, filenameToHide, stegoFile, password);
    }
    else if (isextract) {
        /* error check params for extraction operation */
        if (filenameToHide == NULL  ||
        stegoFile == NULL)
        {
            usage();
            return printerror("Missing arguments required for the extract operation.\n");
        }

        /* prompt user for password */
        printf("Enter password: ");
        scanf("%127s", password);

        /* perform the operation */
        extdec_pdf(stegoFile, filenameToHide, password);
    }
    else
    {
        usage();
        return printerror("Missing either -e(embed) or -x(extract) argument\n");
    }

    return 0;
}
