#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct File {
    char* text;
    uint64_t size;
} File;

typedef struct Alphabet {
    char* letters;
    uint16_t* values;
    uint16_t sum;
    uint64_t size;
} Alphabet;

typedef struct Enc {
    uint64_t state;
    char* bitStream;
    uint64_t bitStream_pos;
    uint64_t file_pos;
} Enc;

File* file;
Alphabet* alphabet;
Enc* enc;

void fileReader(char* path)
{
    char* file_data;
    uint64_t file_size;
    FILE* fp;

    if((fp = fopen(path, "r")) == NULL)
    {
        return;
    }

    file = (File*) malloc(sizeof(File));

    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    rewind(fp);
    file->text = (char*) malloc((sizeof(char)) * file->size);
    fread(file->text, sizeof(char), file->size, fp);
    fclose(fp);
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
    printf("%i", alphabet->sum);
    printf("\n========== input =============\n");
    for(size_t i = 0; i < file->size; i++)
    {
        printf("%c", file->text[i]);
    }
    printf("\n%li\n", file->size);
    
}

void fileWriter(char* path)
{
    FILE *fp;
    char buffer[33];

    if((fp = fopen(path, "w")) == NULL)
    {
        return;
    }

    fprintf(fp, "%li, %li", enc->state, enc->file_pos);

    //fputs(enc->bitStream, fp);

    fclose(fp);
}

// =========================== ENCODER =====================================

void generateBistream()
{

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

size_t getSymbolPos(char c)
{
    size_t pos = 0;
    for(pos; pos < alphabet->size; pos++)
    {
        if(alphabet->letters[pos] == c)
        {
            return pos;
        }
    }

    return -1;
}


void encode()
{
    enc = (Enc*) malloc(sizeof(Enc));
    enc->bitStream = (char*) malloc(sizeof(char));
    enc->state = 0;
    enc->file_pos = 0;
    enc->bitStream_pos = 0;
    size_t symbolPos = 0;
    size_t cumulativeProbability = 0;

    //printf("\n%lu\n", enc->intValue);
   
    while(enc->file_pos < file->size)
    {
        symbolPos = getSymbolPos(file->text[enc->file_pos]);
        printf("%li", symbolPos);
        cumulativeProbability = getCumulativeProbability(symbolPos);
        enc->state = (enc->state/alphabet->values[symbolPos]) * alphabet->sum + cumulativeProbability + (enc->state % alphabet->values[symbolPos]);
        printf("\n%lu\n", enc->state);
        enc->file_pos++;
    }

}
