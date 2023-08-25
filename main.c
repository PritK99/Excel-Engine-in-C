#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Table represents the structure of csv file
typedef struct {
    int rows;
    int cols;
    int** table;
}Table;

//Cell represents indivisual cell in csv file
typedef struct {
    char* val;
    int type;
}Cell ;

void parse(char* buffer, int m, Table t)
{
    int row = 0;
    int col = 0;

    //detecting number of rows and columns in csv file
    for (int i = 0; i < m ; i++)
    {
        if (buffer[i] == '\n')
        {
            row++;
        }
        if (row == 0 && buffer[i] == '|')
        {
            col++;
        }
    }

    //incrementing since a single '|' will represent two columns
    t.rows = row++;
    t.cols = col++;
    printf("The dimension of csv file is %dx%d", row, col);

    int *p = NULL;
    t.table = malloc(sizeof(p)*t.rows);

    for (int i = 0 ; i < t.rows; i++)
    {
        *t.table[i] = malloc(sizeof(Cell)*t.cols);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: input file is not provided\n");
        exit(1);
    }

    char* file_path = argv[1];

    FILE *f = fopen(file_path, "rb");

    if (f == NULL)
    {
        fprintf(stderr, "ERROR: file not found\n");
        exit(1);
        fclose(f);
    }

    //creating a temporary pointer to obtain size of file
    FILE* temp = f;
    fseek(temp, 0, SEEK_END);
    long m = ftell(temp);
    fclose(temp);

    //storing contents of file in buffer
    char* buffer = malloc(sizeof(char)*m);
    if (buffer == NULL)
    {
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        exit(1);
        fclose(f);
    }

    //printing contents of buffer
    printf("Your input CSV file:\n");
    fwrite(buffer, sizeof(char), m, stdout);
    printf("\n");

    //parse the csv file and store it in Table t
    Table t;
    parse(buffer, m, t);

    return 0;
}