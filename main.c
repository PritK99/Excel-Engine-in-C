#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Type represents different types of cell
typedef enum {
    NUMBER, POSITION, EXPRESSION
}Type;

//Type represents different types of cell
typedef enum {
    ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION
}Operation;


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

int solve(Table *t, int i, int j, int visited[t->rows][t->cols])
{
    char* temp = t->table[i][j].val;
    if (visited[i][j] == 1)
    {
        fprintf(stderr,"ERROR: Circular dependecy at cell (%d, %d)\n", i, j);
        exit(1);  
    }
    else
    {
        visited[i][j] = 1;
    }
    //keeping record of visited Cells to detect circular dependencies
    

    if (strlen(temp) == 6)
    {
        int Op1, Op2, Op1_i, Op2_i, Op1_j, Op2_j, result ;
        Op1_i = temp[2] - '0';
        Op2_i = temp[5] - '0';
        Op1_j = temp[1] - 'A';
        Op2_j = temp[4] - 'A';

        if (t->table[Op1_i][Op1_j].type == NUMBER)
        {
            Op1 = atoi(t->table[Op1_i][Op1_j].val);
        }
        else 
        {
            visited[Op1_i][Op1_j] = 1;
            Op1 = solve(t, Op1_i, Op1_j, visited);
        }

        if (t->table[Op2_i][Op2_j].type == NUMBER)
        {
            Op2 = atoi(t->table[Op2_i][Op2_j].val);
        }
        else 
        {
            Op2 = solve(t, Op2_i, Op2_j, visited);
        }
        

        if (temp[3] == '+')
        {
            result = Op1 + Op2 ;
        }
        else if (temp[3] == '*')
        {
            result = Op1 * Op2 ;
        }
        else if (temp[3] == '-')
        {
            result = Op1 - Op2 ;
        }
        else if (temp[3] == '/')
        {
            if (Op2 != 0)
            {
                result = Op1 + Op2 ;
            }
            else
            {
                fprintf(stderr, "ERROR: Cannot divide by Zero\n");
                exit(1);                 
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Unknown operator detected\n");
            exit(1);  
        }

        return result;
    }
    else
    {
        fprintf(stderr, "ERROR: expression can not be evaluated\n");
        exit(1);    
    }
}

void solveWrapper(Table *t)
{
    for (int i = 0 ; i < t->rows; i++)
    {
        for (int j = 0 ; j < t->cols; j++)
        {
            if(t->table[i][j].type == EXPRESSION)
            {
                int visited[t->rows][t->cols];
                for (int x = 0; x < t->rows; x++)
                {
                    for (int y = 0 ; y < t->cols; y++)
                    {
                        visited[x][y] = 0;
                    }
                }

                int x = solve(t, i, j, visited); 
                sprintf(t->table[i][j].val, "%d", x);
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
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        fclose(f);
        exit(1);
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
        fclose(f);
        exit(1);
    }

    //printing contents of buffer
    printf("Your input CSV file:\n");
    fwrite(buffer, sizeof(char), m, stdout);
    printf("\n");

    //parse the csv file and store it in Table t
    Table t;
    parse(buffer, m, &t);

    //assign each cell a type
    analyse(&t);

    //solve expression cells
    solveWrapper(&t);

    //print output csv file
    printf("The output file is: \n");
    for (int i = 0 ; i < t.rows; i++)
    {
        for (int j = 0 ; j < t.cols; j++)
        {
            printf("%s ", t.table[i][j].val);
        }
        printf("\n");
    }  

    return 0;
}