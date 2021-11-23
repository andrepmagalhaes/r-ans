#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct File {
    uint64_t state;
    uint64_t encodedSymbols;
} File;

typedef struct Alphabet {
    char* letters;
    uint16_t* values;
    uint16_t sum;
    uint64_t size;
} Alphabet;

typedef struct Dec {
    char* bitStream;
    uint64_t bitStream_pos;
    uint64_t file_pos;
} Dec;

File* file;
Alphabet* alphabet;
Dec* dec;

void fileReader(char* path)
{
    char* file_data = (char*) malloc(sizeof(char));
    uint64_t data_counter = 0;
    uint64_t file_size;
    uint64_t char_buffer;
    FILE* fp;

    if((fp = fopen(path, "r")) == NULL)
    {
        return;
    }

    file = (File*) malloc(sizeof(File));
    file->state = 0;
    file->encodedSymbols = 0;

    while ((char_buffer = fgetc(fp)) != EOF)
    {

        file_data = realloc(file_data, sizeof(char) * (data_counter + 1));
        file_data[data_counter] = char_buffer;
        data_counter++;

        if(char_buffer == ',')
        {
            file->state = strtoul(file_data, NULL, 0);
            free(file_data);
            data_counter = 0;
            fgetc(fp);
        }
    }

    file->encodedSymbols = strtoul(file_data, NULL, 0);

    
    //free(file_data);
    fclose(fp);

    printf("\nfile->state: %li | file->symbols: %li\n", file->state, file->encodedSymbols);
}

void alphabetReader(char *path)
{
    FILE* fp;

    if((fp = fopen(path, "r")) == NULL)
    {
        return;
    }

    char* line_buffer = (char*) malloc(sizeof(char));
    uint64_t char_buffer;
    uint64_t counter = 0;
    uint64_t position = 0;

    alphabet = (Alphabet*) malloc(sizeof(Alphabet));
    alphabet->letters = (char*) malloc((sizeof(char)));
    alphabet->values = (uint16_t*) malloc((sizeof(uint16_t)));
    alphabet->sum = 0;
    
    while ((char_buffer = fgetc(fp)) != EOF)
    {
        if(char_buffer == '\n')
        {
            alphabet->letters = realloc(alphabet->letters, (sizeof(char) * (position + 1)));
            alphabet->values = realloc(alphabet->values, (sizeof(uint16_t) * (position + 1)));
            alphabet->size = position + 1;
            
            char* line_buffer_aux = (char*) malloc(sizeof(char));
            char* ptr_aux;
            uint64_t i = 0;
            uint64_t j = 0;

            for (i = 0; i < counter; i++)
            {
                if(((line_buffer[i] == ',') && (line_buffer[i+1] == ',')))
                {
                    i++;
                    break;
                }
                if((line_buffer[i] == ','))
                {
                    break;
                }
            }

            alphabet->letters = realloc(alphabet->letters, sizeof(char) * (position + 1));
            alphabet->letters[position] = line_buffer[j];

            j = 0;

            for (i++ ; i < counter ; i++) 
            {
                line_buffer_aux = realloc(line_buffer_aux, sizeof(char) * (j + 1));
                line_buffer_aux[j] = line_buffer[i];
                j++;
            }
            alphabet->values[position] = strtoul(line_buffer_aux, NULL, 0);
            alphabet->sum += alphabet->values[position];

            counter = 0;
            position++;
            free(line_buffer_aux);
            free(line_buffer);
            line_buffer = malloc(sizeof(char));

        }
        else
        {

            line_buffer = realloc(line_buffer, sizeof(char) * (counter + 1));
            line_buffer[counter] = char_buffer;
            counter++;

        }
    }
    free(line_buffer);
    fclose(fp);


    printf("\n========== ALPHABET =============\n");
    for(size_t i = 0; i < alphabet->size; i++)
    {
        printf("%c|%i\n", alphabet->letters[i], alphabet->values[i]);
    }
    printf("%i\n", alphabet->sum);
    printf("%li\n", alphabet->size);
    // printf("\n========== input =============\n");
    // for(size_t i = 0; i < file->size; i++)
    // {
    //     printf("%c", file->text[i]);
    // }
    // printf("\n%i\n", file->size);
    
}

void fileWriter(char* path)
{
    FILE *fp;

    if((fp = fopen(path, "w")) == NULL)
    {
        return;
    }

    for(int i = 0 ; i < file->encodedSymbols ; i++)
    {
        fputc(dec->bitStream[i], fp);
    }

    //fputs(enc->bitStream, fp);

    fclose(fp);
}

// void fileWriter(char* path)
// {
//     FILE *fp;
//     char buffer[33];

//     if((fp = fopen(path, "w")) == NULL)
//     {
//         return;
//     }

//     fprintf(fp, "%li", dec->);

//     //fputs(enc->bitStream, fp);

//     fclose(fp);
// }

// =========================== ENCODER =====================================

size_t getInverseCumulativeProbability(size_t cumulativeProbability)
{
    size_t auxCumulativeProbability = 0;
    size_t pos = 0;
    printf("\n#inverseCumulative - cumulativeProbability: %li\n", cumulativeProbability);

    for(pos; pos < alphabet->size; pos++)
    {
        printf("\n#inverseCumulative#for - auxCumulativeProb: %li | cumulativeProb: %li\n",auxCumulativeProbability, cumulativeProbability);
        if(cumulativeProbability >= auxCumulativeProbability && cumulativeProbability < (auxCumulativeProbability + alphabet->values[pos + 1]) || cumulativeProbability == auxCumulativeProbability)
        {
            break;
        }
        auxCumulativeProbability += alphabet->values[pos];
    }
    printf("\n#inverseCumulative - pos: %li\n", pos);

    return pos;
}

size_t getCumulativeProbability(size_t pos)
{

    size_t cumulativeProbability = 0;

    for(size_t i = 0; i < pos; i++)
    {
        cumulativeProbability += alphabet->values[i];
    }

    if(pos == 0)
    {
        return 0;
    }

    return cumulativeProbability;
}


void decode()
{

    dec = (Dec*) malloc(sizeof(Dec));
    dec->bitStream = (char*) malloc(sizeof(char) * file->encodedSymbols);
    dec->bitStream_pos = file->encodedSymbols;
    dec->file_pos = 0;
    size_t symbolPos = 0;
    size_t slot = 0;

    while(dec->bitStream_pos > 0)
    {
        slot = (size_t) (file->state % alphabet->sum);
        symbolPos = getInverseCumulativeProbability(slot);
        dec->bitStream[dec->bitStream_pos - 1] = alphabet->letters[symbolPos];
        file->state = (file->state/alphabet->sum) * alphabet->values[symbolPos] + slot - getCumulativeProbability(symbolPos);
        dec->bitStream_pos--;
    }



    // printf("\n");
    // for(size_t i = 0; i < file->encodedSymbols; i++)
    // {
    //     printf("%c", dec->bitStream[i]);
    // }
    // printf("\n");

}
