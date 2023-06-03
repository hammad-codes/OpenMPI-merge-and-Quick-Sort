#include <stdio.h>
#include <stdlib.h>
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
// ----------------------------
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
// ----------------------------

void performMergeSort(int arraySize, int array[])
{
    int operation_count = 1000000000; // number of op
    struct timeval start, end;

    gettimeofday(&start, NULL);

    // Calling the Function
    merge_sort(array, arraySize);

    gettimeofday(&end, NULL);

    /* convert time to Gflop/s */
    double seconds = (end.tv_sec - start.tv_sec) +
                     1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9 * operation_count / seconds;

    FILE *fptr;
    fptr = fopen("Q1_Performances", "a");
    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }
    fprintf(fptr, "Given an ArraySize of %d Merge Sort Performance in Gflops is %.3f Gflop/s\n", arraySize, Gflops);
    fclose(fptr);
}
void performQuickSort(int arraySize, int array[])
{
    int operation_count = 1000000000; // number of op
    struct timeval start, end;

    gettimeofday(&start, NULL);

    // Calling the Function
    quicksort(array, 0, arraySize - 1);

    gettimeofday(&end, NULL);

    /* convert time to Gflop/s */
    double seconds = (end.tv_sec - start.tv_sec) +
                     1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9 * operation_count / seconds;

    // write to file a single line and append to the file
    FILE *fptr;
    fptr = fopen("Q1_Performances", "a");
    if (fptr == NULL)
    {
        printf("Error!");
        exit(1);
    }
    fprintf(fptr, "Given an ArraySize of %d Quick Sort Performance in Gflops is %.3f Gflop/s\n\n", arraySize, Gflops);
    fclose(fptr);
}
int main(int argc, char *argv[])
{
    int arraySize = atoi(argv[1]);
    arraySize = pow(2, arraySize);
    printf("Hello World %d\n", arraySize);

    int *array = malloc(arraySize * sizeof(int));

    randomize(arraySize, array);

    writeToFile(arraySize, array, "unsorted.txt");

    performMergeSort(arraySize, array);

    performQuickSort(arraySize, array);

    writeToFile(arraySize, array, "sorted.txt");

    free(array);
}