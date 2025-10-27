#include <stdio.h>
#include <string.h>

int p10[] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
int p8[]  = {6, 3, 7, 4, 8, 5, 10, 9};
int p4[]  = {2, 4, 3, 1};
int ip[]  = {2, 6, 3, 1, 4, 8, 5, 7};
int ipinv[]= {4, 1, 3, 5, 7, 2, 8, 6};
int ep[]  = {4, 1, 2, 3, 2, 3, 4, 1};

int s0[][4] = {
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {0, 2, 1, 3},
    {3, 1, 3, 2}
};

int s1[][4] = {
    {0, 1, 2, 3},
    {2, 0, 1, 3},
    {3, 0, 1, 0},
    {2, 1, 0, 3}
};

// Permutation function
void permute(char op[], char ip[], int p[], int n) {
    for (int i = 0; i < n; i++) {
        op[i] = ip[p[i] - 1];
    }
    op[n] = '\0';
}

// Circular left shift of n bits on a string
void circularls(char pr[], int n) {
    char ch = pr[0];
    for (int i = 0; i < n - 1; i++) {
        pr[i] = pr[i + 1];
    }
    pr[n - 1] = ch;
}

// Key generation: generates two 8-bit subkeys k1 and k2 from 10-bit key
void keygen(char k1[], char k2[], char key[]) {
    char keytemp[11];
    permute(keytemp, key, p10, 10);

    circularls(keytemp, 5);
    circularls(keytemp + 5, 5);

    permute(k1, keytemp, p8, 8);

    circularls(keytemp, 5);
    circularls(keytemp, 5);
    circularls(keytemp + 5, 5);
    circularls(keytemp + 5, 5);

    permute(k2, keytemp, p8, 8);
}

// XOR of two binary strings (char arrays of '0' and '1')
void Xor(char op[], char ip[]) {
    int len = strlen(op);
    for (int i = 0; i < len; i++) {
        op[i] = ((op[i] - '0') ^ (ip[i] - '0')) + '0';
    }
}

// S-box substitution
void sbox(char op[], char ip[], int s[][4]) {
    int row = (ip[0] - '0') * 2 + (ip[3] - '0');
    int col = (ip[1] - '0') * 2 + (ip[2] - '0');
    int value = s[row][col];

    op[0] = (value / 2) + '0';
    op[1] = (value % 2) + '0';
    op[2] = '\0';
}

// The fk function performs one round of the Feistel structure
void fk(char op[], char ip[], char k[]) {
    char l[5], r[5], tmp[9], tmp1[3], tmp2[3];
    strncpy(l, ip, 4);
    l[4] = '\0';
    strncpy(r, ip + 4, 4);
    r[4] = '\0';

    permute(tmp, r, ep, 8);
    Xor(tmp, k);

    sbox(tmp1, tmp, s0);
    sbox(tmp2, tmp + 4, s1);

    strcat(tmp1, tmp2);
    permute(tmp, tmp1, p4, 4);

    Xor(tmp, l);

    strcpy(op, tmp);
    strcat(op, r);
}

// Swap the left 4 bits and right 4 bits
void sw(char pr[]) {
    char tmp[9];
    strncpy(tmp, pr + 4, 4);
    strncpy(tmp + 4, pr, 4);
    tmp[8] = '\0';
    strcpy(pr, tmp);
}

int main() {
    char key[11], k1[9], k2[9], plain[9], cipher[9], tmp[9];

    printf("Enter 10 bit key: ");
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';  // Remove newline

    if (strlen(key) != 10) {
        printf("Invalid key length !!\n");
        return 1;
    }

    keygen(k1, k2, key);
    printf("Subkey k1: %s\n", k1);
    printf("Subkey k2: %s\n", k2);

    printf("Enter 8 bit plain text: ");
    fgets(plain, sizeof(plain), stdin);
    plain[strcspn(plain, "\n")] = '\0';  // Remove newline

    if (strlen(plain) != 8) {
        printf("Invalid plain text length !!\n");
        return 1;
    }

    permute(tmp, plain, ip, 8);
    fk(cipher, tmp, k1);
    sw(cipher);
    fk(tmp, cipher, k2);
    permute(cipher, tmp, ipinv, 8);

    printf("Cipher text is: %s\n", cipher);

    // Decryption process
    permute(tmp, cipher, ip, 8);
    fk(plain, tmp, k2);
    sw(plain);
    fk(tmp, plain, k1);
    permute(plain, tmp, ipinv, 8);

    printf("Decrypted text is: %s\n", plain);

    return 0;
}
