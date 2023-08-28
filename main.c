#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Type represents different types of cell
typedef enum {
    NUMBER, POSITION, EXPRESSION
}Type;

//Cell represents indivisual cell in csv file
typedef struct {
    char* val;
    Type type;
}Cell ;

//Table represents the structure of csv file
typedef struct {
    int rows;
    int cols;
    Cell** table;
}Table;


void parse(char* buffer, int m, Table *t)
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
    t->rows = ++row;
    t->cols = ++col;
    printf("The dimension of csv file is %dx%d\n", t->rows, t->cols);

    Cell *p = NULL;
    t->table = (Cell**)malloc(sizeof(p)*t->rows);

    for (int i = 0 ; i < t->rows; i++)
    {
        t->table[i] = (Cell*)malloc(sizeof(Cell)*t->cols);
    }

    int k = 0;
    for (int i = 0 ; i < t->rows; i++)
    {
        for (int j = 0 ; j < t->cols; j++)
        {
            char *temp = "";
            while(k < m && buffer[k] != '|' && buffer[k] != '\n')
            {
                if(buffer[k] == ' ')
                {
                    k++;
                    continue;
                }
                char c = buffer[k];
                char* new_temp = malloc(strlen(temp) + 1 + 1);
                strcpy(new_temp, temp);
                new_temp[strlen(temp)] = c;
                new_temp[strlen(temp) + 1] = '\0';
                temp = new_temp;
                k++;
            }
            (t->table[i][j]).val = temp;
            k++;
        }
    }
}

void analyse(Table *t)
{
    for (int i = 0 ; i < t->rows; i++)
    {
        for (int j = 0 ; j < t->cols; j++)
        {
            if(t->table[i][j].val[0] == '=')
            {
                t->table[i][j].type = EXPRESSION;
            }
            else if (t->table[i][j].val[0] >= '0' && t->table[i][j].val[0] <= '9')
            {
                t->table[i][j].type = NUMBER;
            }
            else
            {
                t->table[i][j].type = POSITION;
            }
        }
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
    parse(buffer, m, &t);
    printf("Your CSV file after parsing:\n");
    for (int i = 0 ; i < t.rows; i++)
    {
        for (int j = 0 ; j < t.cols; j++)
        {
            printf("%s\n", (t.table[i][j]).val);
        }
    }

    //assign each cell a type
    analyse(&t);
    for (int i = 0 ; i < t.rows; i++)
    {
        for (int j = 0 ; j < t.cols; j++)
        {
            printf("%d ", (t.table[i][j]).type);
        }
        printf("\n");
    }

    return 0;
}