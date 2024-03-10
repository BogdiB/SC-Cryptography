// https://www.ibm.com/docs/en/i/7.3?topic=extensions-standard-c-library-functions-table-by-name
#include <stdbool.h>
#include <string.h>
#include <ctype.h> // for isalpha
#include <stdio.h>

// Hardcoded shift for Caesar cipher.
short key = 5;
bool flagUsed = false;
// Index. +1 will be the fileName.
int flagAt = 0;
bool outputNameGiven = false;
// Points to the argv index where the output name is located.
char *outputName = NULL;

void invalid()
{
    printf("Invalid arguments. For argument reference run the program with the \"help\" argument.");
}

bool checkArgumentValidity(int argc, char **argv)
{
    if (argv[1][0] != '-' || isalpha(argv[2][0]) == 0)
    {
        invalid();
        return false;
    }
    if (argv[1][1] == 'e' || argv[1][1] == 'd')
    {
        flagUsed = !flagUsed;
        flagAt = 1;
    }
    else if (argv[1][1] == 'o')
    {
        outputNameGiven = !outputNameGiven;
        outputName = argv[2];
    }
    else
    {
        invalid();
        return false;
    }
    if (argc == 5)
    {
        if (argv[3][0] != '-' || isalpha(argv[4][0]) == 0)
        {
            invalid();
            return false;
        }
        if (argv[3][1] == 'e' || argv[3][1] == 'd')
        {
            if (flagUsed)
            {
                invalid();
                return false;
            }
            flagUsed = !flagUsed;
            flagAt = 3;
        }
        else if (argv[3][1] == 'o')
        {
            if (outputNameGiven)
            {
                invalid();
                return false;
            }
            outputNameGiven = !outputNameGiven;
            outputName = argv[4];
        }
        else
        {
            invalid();
            return false;
        }
    }
    return true;
}

void help()
{
    printf("Arguments can be:\n");
    printf("\t\"help\"\n");
    printf("\t\"-e <fileName>\" = encrypt the file with the given fileName\n");
    printf("\t\"-d <fileName>\" = decrypt the file with the given fileName\n");
    printf("\t\"-o <fileName>\" = name the output file with the given fileName (optional)\n");
    printf("\"fileName\" must start with a letter of the alphabet (capitalization is irrelevant).\n");
    printf("You cannot use the \"e\" and \"d\" flags in the same instance of the program.");
}

void encrypt(int argc, char **argv)
{
    FILE* rFile = fopen(argv[flagAt+1], "rb");
    FILE* wFile = fopen(outputName, "wb");
    if (!rFile)
    {
        printf("Cannot open the file provided for the encryption. Please try again.");
        return ;
    }
    if (!wFile)
    {
        printf("Cannot create/open the output file provided for the encryption. Please try again.");
        return ;
    }
    char buffer[1] = "\t";
    while (fread(buffer, sizeof(buffer[0]), 1, rFile) != 0)
    {
        if (isalnum(buffer[0]))
        {
            if (islower(buffer[0]))
                buffer[0] = (char) ((buffer[0] - 'a' + key) % 26 + 'a');
            else if (isupper(buffer[0]))
                buffer[0] = (char) ((buffer[0] - 'A' + key) % 26 + 'A');
            else if (isdigit(buffer[0]))
                buffer[0] = (char) ((buffer[0] - '0' + key) % 10 + '0');
        }
        fwrite(buffer, sizeof(buffer[0]), 1, wFile);
    }
    fclose(rFile);
    fclose(wFile);
}

void decrypt(int argc, char **argv)
{
    FILE* rFile = fopen(argv[flagAt+1], "rb");
    FILE* wFile = fopen(outputName, "wb");
    if (!rFile)
    {
        printf("Cannot open the file provided for the decryption. Please try again.");
        return ;
    }
    if (!wFile)
    {
        printf("Cannot create/open the output file provided for the decryption. Please try again.");
        return ;
    }
    char buffer[1] = "\t";
    while (fread(buffer, sizeof(buffer[0]), 1, rFile) != 0)
    {
        if (isalnum(buffer[0]))
        {
            if (islower(buffer[0]))
                buffer[0] = (char) ((buffer[0] - 'a' - key + 26) % 26 + 'a');
            else if (isupper(buffer[0]))
                buffer[0] = (char) ((buffer[0] - 'A' - key + 26) % 26 + 'A');
            else if (isdigit(buffer[0]))
                buffer[0] = (char) ((buffer[0] - '0' - key + 10) % 10 + '0');
        }
        fwrite(buffer, sizeof(buffer[0]), 1, wFile);
    }
    fclose(rFile);
    fclose(wFile);
}

void process(int argc, char **argv)
{
    if (argv[flagAt][1] == 'e')
        encrypt(argc, argv);
    else if (argv[flagAt][1] == 'd')
        decrypt(argc, argv);
    else
        invalid();
}

int main(int argc, char **argv)
{
    if (argc == 1 || argc == 4 || argc > 5)
        printf("Improper number of arguments. Run the program with the \"help\" argument for help.");
    else if (argc == 2 && strcmp(argv[1], "help") == 0)
        help();
    else if ((argc == 3 || argc == 5) && checkArgumentValidity(argc, argv))
        process(argc, argv);
    else
        invalid();
    
    printf("\nDone.\n");

    return 0;
}