/*
Approach: 
1) Accept all expressions as string. 
2) Resolve all the dependencies in the expression.
3) Convert these expressions from infix to postfix.
4) Evaluate the postfix expression considering that the number may be multi-digit.
5) Return the answer as string and store it in respective cell.
6) Else report error. 

Note:
Solve() should return an integer in form of int and not string
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 10000      //maximum size of an expression

/*defining all structs and enum to be used in program*/
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

//Stack represents stack data structure
typedef struct{
    int top;
    int arr[MAX_SIZE];
}Stack ;

/*function declarations*/
void parse(char* buffer, int m, Table *t);
void analyse(Table *t);
void solveWrapper(Table *t);
char* solve(Table *t, int i, int j, int visited[t->rows][t->cols]);
char* simplify(char* s, Table* t, int visited[t->rows][t->cols]);
char* infixToPostfix(char *s);
int precedence(char c);

/*driver function*/
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

    // //solve expression cells
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
}

/*function definitions*/
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

                // int x = solve(t, i, j, visited); 
                // sprintf(t->table[i][j].val, "%d", x);
                t->table[i][j].val = solve(t, i, j, visited);
                t->table[i][j].type = NUMBER;
            }
        }
    }   
}

char* solve(Table *t, int i, int j, int visited[t->rows][t->cols])
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

    char* infix = simplify(temp, t, visited);
    char* postfix = infixToPostfix(infix);

    return postfix;
}

char* simplify(char* s, Table* t, int visited[t->rows][t->cols])
{
    char* new = "";
    
    //we start from i = 1 to ignore = symbol
    for (int i = 1 ; i < strlen(s); i++)
    {
        if(s[i] >= 'A' && s[i] <= 'Z')
        {
            int Op, Op_i, Op_j, result ;
            if (i+1 >= strlen(s))
            {
                fprintf(stderr,"ERROR: Cannot resolve expressions in CSV file");
                exit(1);   
            }
            Op_i = s[i+1] - '0';
            Op_j = s[i] - 'A';

            char* temp = (char*)malloc(sizeof(char)*10);
            if (t->table[Op_i][Op_j].type == NUMBER)
            {
                Op = atoi(t->table[Op_i][Op_j].val);
                sprintf(temp, "%d", Op);
            }
            else 
            {
                temp = solve(t, Op_i, Op_j, visited);
            }

            i++;
            char* new_temp = malloc(strlen(new) + strlen(temp) + 1);
            strcpy(new_temp, new);
            strcat(new_temp, temp);
            new = new_temp;
            free(temp);
        }
        else
        {
            char c = s[i];
            char* new_temp = malloc(strlen(new) + 1 + 1);
            strcpy(new_temp, new);
            new_temp[strlen(new)] = c;
            new_temp[strlen(new) + 1] = '\0';
            new = new_temp;
        }
    }
    return new;
}

int precedence(char c)
{
    if (c == '+' || c == '-')
    {
        return 1;
    }
    else if (c == '*' || c == '/')
    {
        return 2;
    }
    else
    {
        return -1;
    }
}

char* infixToPostfix(char *s)
{
    //using | as a delimeter for seperating multi-digit numbers
    int n = strlen(s);
    char* new = (char *)malloc(sizeof(char)*((2*n)+1));
    
    int j = 0;

    Stack st;
    st.top = -1;
    int flag = 0;
    for (int i = 0 ; i < n; i++)
    {
        if (s[i]>='0' && s[i] <= '9')
        {
            if (j > 0 && new[j-1]>='0' && new[j-1] <= '9' && flag == 0)
            {
                new[j] = '|';
                j++;
            }
            new[j] = s[i];
            j++;
            flag = 1;
        }
        else if (st.top == -1)
        {
            st.top++; 
            st.arr[st.top] = s[i];
            flag = 0;
        }
        else if (s[i] == '(')
        {
            st.top++; 
            st.arr[st.top] = s[i]; 
            flag = 0;          
        }
        else if (s[i] == ')')
        {
            while(st.top > -1 && st.arr[st.top] != '(')
            {
                new[j] = st.arr[st.top];
                st.top--;
                j++;
            }
            st.top--;
            flag = 0;
        }
        else
        {
            while (st.top > -1 && precedence(st.arr[st.top]) >= precedence(s[i]))
            {
                new[j] = st.arr[st.top];
                st.top--;
                j++;
            }
            st.top++;
            st.arr[st.top]=s[i];
            flag = 0;
        }
    }

    while (st.top > -1)
    {
        new[j] = st.arr[st.top];
        st.top--;
        j++;        
    }
    new[j] = '\0';

    return new;
}