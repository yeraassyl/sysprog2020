#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#define TAG "Lab8"
#define MAX_NUM 10

static int arr[MAX_NUM];
int from = 0;
int to = 3;
struct mutex mut;

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    int L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
}


int gen_int(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

void fillArr(void){
    int rnd;
    int k;
    for (k = 0; k < MAX_NUM; ++k){
        rnd = gen_int();
        arr[k] = rnd;
    }
}

int threadSort(void *data){
    mutex_lock(&mut);
    mergeSort(data, from, to);
    from = to + 1;
    to += 3;
    mutex_unlock(&mut);
}

void createThreads(void) {
    int n; 
    n = gen_int();
    if (printk_ratelimit()) {
       printk(KERN_INFO "random number is %d\n", n);
    }

    struct task_struct *thread;
    struct task_struct *thread1;
    struct task_struct *thread2;
    thread = kthread_run(threadSort, NULL, "thread1");
    thread1 = kthread_run(threadSort, NULL, "thread2");
    thread2 = kthread_run(threadSort, NULL, "thread3");
}

int init_module(void)
{
    fillArr();
    mutex_init(&mut);
    printk(KERN_INFO "START %s\n", TAG);
    createThreads();
    int k;
    for (k = 0; k < MAX_NUM; ++k){
        printk(arr[k]);
    }
    return 0;
}

void cleanup_module(void)
{
        printk(KERN_DEBUG "END %s\n", TAG);
}
