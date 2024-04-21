// sha1.cpp : cpp source for the hash function sha-1 implemntation
// the implementation is based on the specification doc fips 180-1

#include "sha1.h"
#include <string.h>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void initHs() {
    // load the initial values for H's
    H[0] = 0x67452301;
    H[1] = 0xefcdab89;
    H[2] = 0x98badcfe;
    H[3] = 0x10325476;
    H[4] = 0xc3d2e1f0;
}

void initKs() {
    // load the initial values for K's
    for (int i = 0; i < 20; i++)   K[i] = 0x5a827999;
    for (int i = 20; i < 40; i++)   K[i] = 0x6ed9eba1;
    for (int i = 40; i < 60; i++)   K[i] = 0x8f1bbcdc;
    for (int i = 60; i < 80; i++)   K[i] = 0xca62c1d6;
}

int getMessage(int k) {
    // copy message k from msg[k] to binmsg
    int msgLen = strlen(msg[k]);
    memcpy(binmsg, msg[k], msgLen);
    return msgLen;
}

int paddMessage(int messLen) {
    // pad the binmsg array according to the specification
	// return the number of blocks of the padded message
    binmsg[messLen] = 0x80;
    
    int paddingLength = 56 - (messLen % 64);
    if (paddingLength < 0)
        paddingLength += 64;
    memset(&binmsg[messLen + 1], 0, paddingLength);
    
    uint64_t originalLengthBits = (uint64_t)messLen * 8;
    PUT_UINT32(originalLengthBits >> 32, binmsg, messLen + paddingLength);
    PUT_UINT32(originalLengthBits & 0xFFFFFFFF, binmsg, messLen + paddingLength + 4);
    
    return (messLen + paddingLength + 8) / 64; // nr of blocks
}

void getWsfromM(int currentBlock) {
    for (int i = 0; i < 16; ++i) {
        GET_UINT32(W[i], binmsg, currentBlock * 64 + i * 4);
    }
    for (int i = 16; i < 80; ++i) {
        W[i] = S(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
    }
}

void getAsfromHs() {
    A = H[0];
    B = H[1];
    C = H[2];
    D = H[3];
    E = H[4];
}

void displayDigest(uint32 H[]) {
    printf(" digest value - %8x %8x %8x %8x %8x\n",
           H[0], H[1], H[2], H[3], H[4]);
}

int initial() {
    int i, j, k;
    int messLen;
    int numBlocks;

    // parse all 3 test messages
    for (k = 0; k < 3; k++) {
        initKs();
        initHs();

        if (k != 2) {
            messLen = getMessage(k);
        } else {
            // third message is much bigger
            memset(binmsg, 'a', 1000000);
            messLen = 1000000;
        }
        numBlocks = paddMessage(messLen);

        for (i = 0; i < numBlocks; i++) {
            getWsfromM(i);

            for (j = 0; j < 80; j++) {
                TEMP = S(A, 5);
                int cit = j / 20;

                switch (cit) {
                    case 0:
                        TEMP += F0(B, C, D);
                        break;
                    case 1:
                        TEMP += F1(B, C, D);
                        break;
                    case 2:
                        TEMP += F2(B, C, D);
                        break;
                    case 3:
                        TEMP += F3(B, C, D);
                        break;
                }

                TEMP = TEMP + E + W[j] + K[j];

                E = D;
                D = C;
                C = S(B, 30);
                B = A;
                A = TEMP;
            }

            H[0] += A;
            H[1] += B;
            H[2] += C;
            H[3] += D;
            H[4] += E;
        }
        displayDigest(H);
    }
    return 0;
}

int fromFile(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input_file> [-o output_file]" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = "output.txt"; // default name for file

    // parse args
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
            i++; // skip the next argument which is the output file
        }
    }

    // get input from file
    ifstream ifs(inputFile, ios::binary);
    if (!ifs) {
        cerr << "Error: Unable to open input file." << endl;
        return 1;
    }

    ifs.seekg(0, ios::end);
    size_t fileSize = ifs.tellg(); // get file size
    ifs.seekg(0, ios::beg);

    vector<char> fileContents(fileSize);

    ifs.read(fileContents.data(), fileSize);
    ifs.close();

    initKs();
    initHs();

    // make the whole file a single thing
    memcpy(binmsg, fileContents.data(), fileSize);
    int numBlocks = paddMessage(fileSize / 8); // fileSize is in bytes, but we need bits

    for (int i = 0; i < numBlocks; i++) {
        getWsfromM(i);

        for (int j = 0; j < 80; j++) {
            TEMP = S(A, 5);
            int cit = j / 20;

            switch (cit) {
                case 0:
                    TEMP += F0(B, C, D);
                    break;
                case 1:
                    TEMP += F1(B, C, D);
                    break;
                case 2:
                    TEMP += F2(B, C, D);
                    break;
                case 3:
                    TEMP += F3(B, C, D);
                    break;
            }

            TEMP = TEMP + E + W[j] + K[j];

            E = D;
            D = C;
            C = S(B, 30);
            B = A;
            A = TEMP;
        }

        H[0] += A;
        H[1] += B;
        H[2] += C;
        H[3] += D;
        H[4] += E;
    }
    displayDigest(H);
    ofstream ofs(outputFile);
    if (!ofs) {
        cerr << "Error: Unable to open output file." << endl;
        return 1;
    }
    ofs << hex;
    ofs << H[0] << " " << H[1] << " " << H[2] << " " << H[3] << " " << H[4] << endl;
    ofs.close();

    cout << "Hashing completed successfully. Result written to " << outputFile << endl;
    
    return 0;
}

int main(int argc, char* argv[]) {
    // NOTE: the homework did not specify anything about the arguments
    return fromFile(argc, argv);
}

// g++ -o sha1 updatedsha1.cpp -std=c++11
// otherwise the linker doesn't find some things from std, like vector

