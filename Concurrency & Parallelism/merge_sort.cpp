#include <iostream>
#include <vector>
#include <thread>

using namespace std;

const int THRESHOLD = 1000; // switch to sequential for small sizes

// Merge function
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);

    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int i = left, k = 0; i <= right; i++, k++)
        arr[i] = temp[k];
}

// Sequential merge sort
void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Parallel merge sort
void parallelMergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    if (right - left < THRESHOLD) {
        mergeSort(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    thread t1(parallelMergeSort, ref(arr), left, mid);
    thread t2(parallelMergeSort, ref(arr), mid + 1, right);

    t1.join();
    t2.join();

    merge(arr, left, mid, right);
}

int main() {
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10};

    parallelMergeSort(arr, 0, arr.size() - 1);

    cout << "Sorted array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    return 0;
}