#include <iostream>
#include <vector>
#include <cmath>
#include "structures.h"

using namespace std;

string citireCaractereFisierCompresat(FILE *file, int lungime)
{
    string codare;
    if(file != NULL) {
        for(int i = 0 ; i < lungime; i++)
        {
            // Citim caracter cu caracter din fisier
            unsigned char c = fgetc(file);
            unsigned int asciiCode = (unsigned int)c;

            for(int i = 0; i < 8; i++)
            {
                if(asciiCode) {
                    if(asciiCode % 2 == 0) {
                        codare.push_back('0');
                    }
                    else {
                        codare.push_back('1');
                    }
                }
                else {
                    codare.push_back('0');
                }
                asciiCode /= 2;
            }

        }
    }

    return codare;
}

void serializeTree(FILE *file, nod* root)
{
    if(root == NULL)
    {
        fwrite("\0",sizeof(char),1,file);
        return;
    }
    else
    {
        fwrite(&root->caracter,sizeof(char),1,file);
        serializeTree(file,root->stg);
        serializeTree(file,root->drt);
    }
}

void deserializeTree(FILE *file, nod*& root)
{
    char character;
    fread(&character,sizeof(char),1,file);
    if(character == '\0') {
        root = NULL;
        return;
    }
    root = new nod;
    root ->caracter = character;
    deserializeTree(file,root->stg);
    deserializeTree(file,root->drt);
}

void numCaracterFisier(string cod, unsigned int &numC)
{
    for(unsigned int i = 0 ; i < cod.size(); i++) {
        ++bytCurent;
        if(bytCurent == 8)
        {
            numC++;
            bytCurent = 0;
        }
    }
}

void scrieByte(FILE *file, string cod)
{
    for(unsigned int i = 0 ; i < cod.size(); i++) {
        unsigned char bit = cod[i];
        if(bit != '0')
            byteToWrite |= (1 << bitCurent);
        ++bitCurent;
        if(bitCurent == 8)
        {
            fwrite(&byteToWrite,sizeof(unsigned char),1,file);
            bitCurent = 0;
            byteToWrite = 0;
        }
    }
}

void determinareFrecventa(FILE *fisier)
{
    if(fisier != NULL) {
        while(true)
        {
            // Citim caracter cu caracter din fisier
            char c = fgetc(fisier);

            //retinem textul initial
            textInitial.push_back(c);

            // Verificicam daca am ajuns la finalul fisierului
            if (feof(fisier))
                break ;

            frecventaCaractere[c]++;
        }
    }
}

void creareListaDeCaractere(nod *&elemente, vector<pair<char,int>> caractere)
{
    for(unsigned int i = 0; i < caractere.size(); ++i)
    {
        nod *nouNod = new nod;
        nouNod -> caracter = caractere[i].first; ///atribuim caracterul
        nouNod -> frec = caractere[i].second; ///atribuim frecventa
        nouNod -> stg = nouNod -> drt = nouNod -> urm = NULL;


        if(elemente == NULL)
        {
            elemente = nouNod;
        }
        else
        {
            nod *p = elemente;
            while(p -> urm != NULL)
            {
                p = p->urm;
            }

            p -> urm = nouNod;
        }
    }
}

bool cmp(pair<char, int> &a, pair<char, int> &b)
{
    return a.second<b.second;
}

vector<pair<char,int>> sortareLista(map <char,int> &caractere)
{
    vector<pair<char,int>> elementeSortate;
    for (auto& i : caractere)
        elementeSortate.push_back(i);
    sort(elementeSortate.begin(), elementeSortate.end(), cmp);

    return elementeSortate;
}

nod* creareNod(nod *elemente)
{
    nod *nouNod = new nod;
    nouNod->frec = elemente->frec + elemente->urm->frec;
    nouNod->urm = NULL;
    nouNod->stg = elemente;
    nouNod->drt = elemente->urm;
    nouNod->caracter = '.';
    return nouNod;
}

void inserareNoduri(nod *&elemente, nod* nodNou)
{
    nod *p = elemente;
    if(elemente == NULL)
    {
        elemente = nodNou;
    }
    else if(nodNou -> frec < p -> frec)
    {
        nodNou->urm = elemente;
        elemente = nodNou;
    }
    else
    {
        while(p -> urm != NULL && nodNou -> frec >= p -> urm -> frec)
        {
            p = p-> urm;
        }

        if(p -> urm == NULL)
        {
            p -> urm = nodNou;
        }
        else
        {
            nodNou -> urm = p -> urm;
            p -> urm = nodNou;
        }
    }
}

void codareCaractere(nod *rad, string cod)
{
    if(rad -> stg == NULL && rad -> drt == NULL)
    {
        ///retinem codurile intr-un hashmap
        coduri[rad->caracter] = cod;
        return;
    }
    else
    {
        codareCaractere(rad->stg, cod + "0");
        codareCaractere(rad->drt, cod + "1");
    }
}

void creareArbore(nod *&elemente)
{
    while(elemente -> urm != NULL) {
        nod *nodNou = creareNod(elemente);
        elemente = elemente -> urm -> urm;
        inserareNoduri(elemente,nodNou);
    }
}


void parcurgereArbore(nod *rad, unsigned int &index, string cod, FILE *fisierDecompresat)
{
    if(rad -> stg == NULL && rad -> drt == NULL)
    {
        fwrite(&rad -> caracter, sizeof(char),1,fisierDecompresat);
        return;
    }
    else
    {
       if(cod[index] == '0')
       {
           index++;
           parcurgereArbore(rad->stg,index,cod,fisierDecompresat);
       }
       else
       {
           index++;
           parcurgereArbore(rad->drt,index,cod,fisierDecompresat);
       }
    }
}

bool singurCaracter(map <char,int> coduri)
{
    int k = 0;
    for(auto i = coduri.begin(); i != coduri.end(); i++)
    {
        k++;
        if(k > 1)
        {
            return false;
        }
    }
    return true;
}
