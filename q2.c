#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

void randomize(int arraySize, int array[])
{
    for (int i = 0; i < arraySize; i++)
    {
        array[i] = rand();
    }
}

void printArray(int arraySize, int array[])
{
    for (int i = 0; i < arraySize; i++)
    {
        printf("%d \n", array[i]);
    }
}
//------------------------------------------
// ---- code for mergesort ----
void merge(int arr[], int left[], int left_size, int right[], int right_size)
{
    int i = 0, j = 0, k = 0;
    while (i < left_size && j < right_size)
    {
        if (left[i] <= right[j])
        {
            arr[k++] = left[i++];
        }
        else
        {
            arr[k++] = right[j++];
        }
    }
    while (i < left_size)
    {
        arr[k++] = left[i++];
    }
    while (j < right_size)
    {
        arr[k++] = right[j++];
    }
}

void merge_sort(int arr[], int size)
{
    if (size <= 1)
    {
        return;
    }
    int mid = size / 2;
    int left[mid], right[size - mid];
    for (int i = 0; i < mid; i++)
    {
        left[i] = arr[i];
    }
    for (int i = mid; i < size; i++)
    {
        right[i - mid] = arr[i];
    }
    merge_sort(left, mid);
    merge_sort(right, size - mid);
    merge(arr, left, mid, right, size - mid);
}
//-----------------------------------------

// ---- code for quicksort ----

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivot = partition(arr, low, high);

        quicksort(arr, low, pivot - 1);
        quicksort(arr, pivot + 1, high);
    }
}

void writeToFile(int arraySize, int array[], char *filename)
{
    FILE *fptr;
    fptr = fopen(filename, "w");
    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }
    for (int i = 0; i < arraySize; i++)
    {
        fprintf(fptr, "%d \n", array[i]);
    }
    fclose(fptr);
}

int isSorted(int arraySize, int array[])
{
    for (int i = 0; i < arraySize - 1; i++)
    {
        if (array[i] > array[i + 1])
        {
            return 0;
        }
    }
    return 1;
}

// ----------------------------
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size, root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int *array, *rcvBuf;
    int arraySize = atoi(argv[1]); // Extract the array size from the command line

    if (rank == 0)
    {
        array = (int *)malloc(arraySize * sizeof(int));
        randomize(arraySize, array);

        // printArray(arraySize, array);
    }

    int operation_count = arraySize * log(arraySize) / log(2); // number of op
    struct timeval start, end;

    gettimeofday(&start, NULL);

    int i = 1;
    while (i <= size)
    {
        if (rank % i == 0)
        {
            int num_procs = size / i;
            int segmentSize = arraySize / (size / i);
            rcvBuf = (int *)malloc(segmentSize * sizeof(int));

            MPI_Request req = MPI_REQUEST_NULL;
            MPI_Status status;

            if (rank == 0)
            {
                merge_sort(array, segmentSize);
                // quicksort(array, 0, segmentSize - 1);
                int x = 1;
                for (int j = 1; j < size; j++)
                {
                    if (j % i == 0)
                    {
                        MPI_Isend(array + x * segmentSize, segmentSize, MPI_INT, j, j, MPI_COMM_WORLD, &req);
                        x++;
                    }
                }
                for (int j = 1; j < size; j++)
                {
                    if (j % i == 0)
                    {
                        MPI_Irecv(rcvBuf, segmentSize, MPI_INT, j, j, MPI_COMM_WORLD, &req);
                        MPI_Wait(&req, &status);
                    }
                }
            }
            else
            {
                MPI_Irecv(rcvBuf, segmentSize, MPI_INT, 0, rank, MPI_COMM_WORLD, &req);
                MPI_Wait(&req, &status);

                merge_sort(rcvBuf, segmentSize);
                // quicksort(rcvBuf, 0, segmentSize - 1);

                MPI_Isend(rcvBuf, segmentSize, MPI_INT, 0, rank, MPI_COMM_WORLD, &req);
                MPI_Wait(&req, &status);
            }
            free(rcvBuf);
        }
        i = i * 2;
    }
    printf("Process %d finished \n", rank);
    MPI_Finalize();

    if (rank == 0)
    {
        // merge_sort(array, arraySize);
        // printArray(arraySize, array);
        printf("%d ", isSorted(arraySize, array));
        writeToFile(arraySize, array, "output.txt");
        free(array); // Only the root process needs to free the array

        /* convert time to Gflop/s */
        double seconds = (end.tv_sec - start.tv_sec) +
                         1.0e-6 * (end.tv_usec - start.tv_usec);
        double Gflops = 2e-9 * operation_count / seconds;

        printf("\nGflops =  %.3f Gflop/s\n\n", Gflops);

        // MPI_Abort(MPI_COMM_WORLD, 0);
    }
    // printf("Process %d finished ", rank);
    return 0;
}
