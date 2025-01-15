#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#define int_INFINITY 2147483647


struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};
typedef struct vector vector;
typedef struct cord cord;


void input_to_Llist(struct vector**,double**,int,int);
void free_Llist(struct vector**);
void cluster_assign(double**, int*, vector*,int, int, int);
int update_centroids(double**, int*, vector*,int,int,int,double);
double euclidean_distance(double*, cord*, int);
double euclidean_distance_new(double*, double*, int);
void handle_memory_error(vector **, double **, int , int*) ;
void input_validation(char*, int f);



double** ofek(int n,int k,int iter,int d,double epsilon,double** centroids,double** points) {
    vector *head = NULL;
    int  flag = 1, t = 0;
    int  *cluster;
    input_to_Llist(&head,points,n,d);
    cluster = calloc(n, sizeof(int));
    if (!cluster) handle_memory_error(&head, centroids, k, NULL);
    while (flag &&(t<iter))
    {
        cluster_assign(centroids, cluster, head, n, d, k);
        flag = update_centroids(centroids, cluster, head, n, d, k,epsilon);
        t++;
    }
    free_Llist(&head);
    free(cluster);
    return centroids;
}

void input_to_Llist(struct vector** head_vec, double** points, int n, int d)
{
    int j = 0, t=0;
    vector *curr_vec;
    cord *head_cord, *curr_cord;

    
    head_cord = malloc(sizeof(struct cord));
    if (!head_cord) handle_memory_error(NULL, NULL, 0, NULL);
    curr_cord = head_cord;
    curr_cord->next = NULL;
    curr_cord->value = 0.0;

    *head_vec = malloc(sizeof(struct vector));
    if (!*head_vec)
    {
        free(head_cord);
        handle_memory_error(NULL, NULL, 0, NULL);
    }
    curr_vec = *head_vec;
    curr_vec->next = NULL;
    curr_vec->cords = NULL;
   for(j=0; j<n; j++)
    {
        for (t=0; t<d; t++)
        {
            if (t == d-1)
            {
                curr_cord->value = points[j][t];
                curr_vec->cords = head_cord;
                curr_vec->next = malloc(sizeof(struct vector));
                if (!curr_vec->next) handle_memory_error(head_vec, NULL, 0, NULL);
                curr_vec = curr_vec->next;
                curr_vec->next = NULL;
                curr_vec->cords = NULL;

                head_cord = malloc(sizeof(struct cord));
                if (!head_cord) handle_memory_error(head_vec, NULL, 0, NULL);

                curr_cord = head_cord;
                curr_cord->next = NULL;
                curr_cord->value = 0.0;
            }
            else
            {
                curr_cord->value = points[j][t];
                curr_cord->next = malloc(sizeof(struct cord));
                if (!curr_cord->next) handle_memory_error(head_vec, NULL, 0, NULL);

                curr_cord = curr_cord->next;
                curr_cord->next = NULL;
                curr_cord->value = 0.0;
            }
        }
    }
    free(curr_cord);
    return;
}


void cluster_assign(double** centroids , int* cluster_assign, vector* head,int n, int d, int k)
{
    int i = 0, j = 0;
    double tmp, min = int_INFINITY;
    int mini = 0;
    for (i =0; i  < n; i++)
    {
        for( j = 0; j <k; j++)
        {
          tmp = euclidean_distance(centroids[j], head->cords, d);
          if (tmp < min)
          {
             min = tmp;
             mini = j;
          }
         
        }
        cluster_assign[i] = mini;
        head = head->next;
        min = int_INFINITY;
    }
}

double euclidean_distance(double* centroid , cord* cords, int d)
{
    int i = 0;
    double sum = 0;
    for (i = 0; i < d; i++)
    {
        sum += pow((cords->value - centroid[i]), 2);
        cords = cords->next;
    }
    sum = sqrt(sum);
    return sum;
}

int update_centroids(double** centroids , int* cluster_assign, vector* head,int n, int d, int k,double epsilon)
{
    int flag = 0;
    int i =0, j = 0;
    double** new_centroids;
    double delta = 0;
    vector *temp_head = NULL;

    new_centroids = calloc(k, sizeof(double*));
    if (!new_centroids) handle_memory_error(&head, centroids, k, cluster_assign);
    for (i = 0; i < k; i++)
    {
        new_centroids[i] = calloc(d+1, sizeof(double));
        if (!new_centroids[i]) handle_memory_error(&head, centroids, k, cluster_assign);
    }

    temp_head = head; 
    for (i = 0; i < n; i++) {
        cord *temp_cord = temp_head->cords;
        for (j = 0; j < d; j++) {
            new_centroids[cluster_assign[i]][j] += temp_cord->value;
            temp_cord = temp_cord->next;
        }
        new_centroids[cluster_assign[i]][d]++;
        temp_head = temp_head->next;
    }
    for( i = 0; i < k; i++)
    {
        for( j = 0; j < d; j++)
        {
            new_centroids[i][j] /= new_centroids[i][d];
        }
        delta = euclidean_distance_new(centroids[i], new_centroids[i], d);
        if (delta >= epsilon)
        {
            flag = 1;
        }
        free(centroids[i]);
        centroids[i] = new_centroids[i];
    }
    free(new_centroids); 
    return flag;
}

double euclidean_distance_new(double* centroid , double* cords, int d)
{
    int i = 0;
    double sum = 0;
    for (i = 0; i < d; i++)
    {
        sum += pow((cords[i] - centroid[i]), 2);
    }
    sum = sqrt(sum);
    return sum;
}


void free_Llist(struct vector** head)
{
    vector *temp_vec = *head;
    vector *next_vec = NULL;
    cord *temp_cord = NULL;
    cord *next_cord = NULL;
    while (temp_vec != NULL)
    {
        next_vec = temp_vec->next;
        temp_cord = temp_vec->cords;
        while (temp_cord != NULL)
        {
            next_cord = temp_cord->next;
            free(temp_cord);
            temp_cord = next_cord;
        }
        free(temp_vec);
        temp_vec = next_vec;
    }
}

void handle_memory_error(vector **head, double **centroids, int k, int *cluster) {
    int i = 0;
    printf("An Error Has Occurred\n");
    if (centroids) {
        for (i = 0; i < k; i++) {
            if (centroids[i]) { 
                free(centroids[i]);
            }
        }
        free(centroids);
    }
    if (cluster) {
        free(cluster);
    }
    if (head && *head) {
        free_Llist(head);  
    }
    exit(1);
}

double** pylist_to_carray(PyObject *pylist, int rows, int cols) {
    double **carray = (double **)malloc(rows * sizeof(double *));
    int j, i;
    PyObject *row, *item;
    if (carray == NULL) {
        printf("An Error Has Occurred\n");
        return NULL;
    }
    for ( i = 0; i < rows; i++) {
        row = PyList_GetItem(pylist, i);
        carray[i] = (double *)malloc(cols * sizeof(double));
        if (carray[i] == NULL) {
            printf("An Error Has Occurred\n");
            for (j = 0; j < i; j++) free(carray[j]);
            free(carray);
            return NULL;
        }
        for (int j = 0; j < cols; j++) {
            item = PyList_GetItem(row, j);
            carray[i][j] = PyFloat_AsDouble(item);
        }
    }
    return carray;
}
PyObject* carray_to_pylist(double** centroids, int k, int d)
{
    PyObject *result = PyList_New(k);
    PyObject *row;
    int i,j;
    for( i=0; i<k; i++)
    {
        row = PyList_New(d);
        for(j=0; j<d; j++)
        {
            PyList_SetItem(row, j, PyFloat_FromDouble(centroids[i][j]));
        }
        PyList_SetItem(result, i, row);
    }
    return result;
}

static PyObject* process_args(PyObject* self, PyObject* args) 
{
    int n,k,iter,d,i;
    double epsilon;
    PyObject *centroids_obj, *points_obj, *resultList;
    double **centroids, **points;
    if (!PyArg_ParseTuple(args, "iiiidOO",&n, &k, &iter, &d, &epsilon, &centroids_obj, &points_obj)) 
    {
        return NULL;
    }
    points = pylist_to_carray(points_obj, n, d);
    centroids = pylist_to_carray(centroids_obj, k, d);
    if (centroids == NULL || points == NULL) {
        return NULL;
    }
    ofek(n,k,iter,d,epsilon,centroids,points);
    resultList = carray_to_pylist(centroids, k, d);
    for ( i = 0; i < k; i++) {
        free(centroids[i]);
    }
    free(centroids);
    for( i = 0; i < n; i++) {
        free(points[i]);
    }
    free (points);
    return resultList;

}

static PyMethodDef Methods[] = {
    {"fit", process_args, METH_VARARGS, PyDoc_STR("expecting 7 arguments: n,k,iter,d,epsilon,centroids,points")},
    {NULL, NULL, 0, NULL}
};
static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", // Module name
    NULL,      // Module documentation
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) 
{
    PyObject *m;
    m = PyModule_Create(&module);
    if (!m)
    {
        return NULL;
    }
    return m;
}