#pragma once

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace containerium {

template<typename KeyExtractor>
static auto compareAsc(KeyExtractor extractor) {
    return [extractor](const auto& a, const auto& b) {
        return extractor(a) < extractor(b);
    };
}

template<typename KeyExtractor>
static auto compareDesc(KeyExtractor extractor) {
    return [extractor](const auto& a, const auto& b) {
        return extractor(a) > extractor(b);
    };
}

template<typename T, typename Comparator>
void merge(std::vector<T>& vec, int left, int mid, int right, Comparator comp) {
    std::vector<T> temp(vec.begin() + left, vec.begin() + right + 1);
    int firstLen = mid - left + 1;
    int totalLen = right - left + 1;
    int i = 0;
    int j = firstLen;
    int k = left;
    while (i < firstLen && j < totalLen) {
        if (!comp(temp[j], temp[i])) {
            vec[k++] = temp[i++];
        } else {
            vec[k++] = temp[j++];
        }
    }
    while (i < firstLen) vec[k++] = temp[i++];
    while (j < totalLen) vec[k++] = temp[j++];
}

template<typename T, typename Comparator>
int partition(std::vector<T>& vec, int low, int high, Comparator comp) {
    T pivot = vec[high];
    int i = low - 1;
    
    for (int j = low; j < high; ++j) {
        if (!comp(pivot, vec[j])) {
            ++i;
            std::swap(vec[i], vec[j]);
        }
    }
    std::swap(vec[i + 1], vec[high]);
    return i + 1;
}

template<typename T, typename Comparator>
void heapify(std::vector<T>& vec, int n, int i, Comparator comp) {
    int extreme = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < n && comp(vec[left], vec[extreme])) extreme = left;
    if (right < n && comp(vec[right], vec[extreme])) extreme = right;
    if (extreme != i) {
        std::swap(vec[i], vec[extreme]);
        heapify(vec, n, extreme, comp);
    }
}

/*
 冒泡排序 (Bubble Sort)：
     时间复杂度：最坏/平均情况 O(n²) ，最好情况（已排序）O(n) 
     空间复杂度：O(1)
     稳定性：稳定
     原地排序：是
     思想：重复比较相邻元素，将较大元素向后移动
     适用场景：小规模数据，或者基本有序的数据
 */
template<typename T, typename Comparator>
void bubbleSort(std::vector<T>& vec, Comparator comp) {
    int n = vec.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (comp(vec[j + 1], vec[j])) {
                std::swap(vec[j], vec[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

/*
 选择排序 (Selection Sort)：
     时间复杂度：所有情况 O(n²) 
     空间复杂度：O(1)
     稳定性：不稳定
     原地排序：是
     思想：每次选择最小（或最大）元素放到已排序序列末尾
     适用场景：小规模数据，移动成本较高的场景
 */
template<typename T, typename Comparator>
void selectionSort(std::vector<T>& vec, Comparator comp) {
    int n = vec.size();
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (comp(vec[j], vec[minIndex])) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(vec[i], vec[minIndex]);
        }
    }
}

/*
 插入排序 (Insertion Sort)：
     时间复杂度：最坏/平均情况 O(n²) ，最好情况（已排序） O(n) 
     空间复杂度：O(1)
     稳定性：稳定
     原地排序：是
     思想：将未排序元素逐个插入到已排序序列的适当位置
     适用场景：小规模数据，或者基本有序的数据
 */
template<typename T, typename Comparator>
void insertionSort(std::vector<T>& vec, Comparator comp) {
    int n = vec.size();
    for (int i = 1; i < n; i++) {
        T key = vec[i];
        int j = i - 1;
        while (j >= 0 && comp(key, vec[j])) {
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;
    }
}

/*
 希尔排序 (Shell Sort)：
     时间复杂度：O(n log n) ~ O(n²)，取决于增量序列
     空间复杂度：O(1)
     稳定性：不稳定
     原地排序：是
     思想：改进的插入排序，通过增量分组进行预排序
     适用场景：中等规模数据，对性能有一定要求
 */
template<typename T, typename Comparator>
void shellSort(std::vector<T>& vec, Comparator comp) {
    int n = vec.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            T temp = vec[i];
            int j = i;
            while (j >= gap && comp(temp, vec[j - gap])) {
                vec[j] = vec[j - gap];
                j -= gap;
            }
            vec[j] = temp;
        }
    }
}

/*
 归并排序 (Merge Sort)：
     时间复杂度：所有情况 O(n log n) 
     空间复杂度：O(n)
     稳定性：稳定
     原地排序：否（需要额外空间）
     思想：分治法，将数组分成两半分别排序后合并
     适用场景：大规模数据，需要稳定排序的场景
 */
template<typename T, typename Comparator>
void mergeSort(std::vector<T>& vec, Comparator comp) {
    if (vec.empty()) return;
    std::function<void(int, int)> mergeSortHelper = [&](int left, int right) {
            if (left < right) {
                int mid = left + (right - left) / 2;
                mergeSortHelper(left, mid);
                mergeSortHelper(mid + 1, right);
                merge(vec, left, mid, right, comp);
            }
        };
    mergeSortHelper(0, vec.size() - 1);
}

/*
 快速排序 (Quick Sort)：
     时间复杂度：平均情况 O(n log n) ，最坏情况 O(n²) 
     空间复杂度：O(log n) ~ O(n)（递归栈空间）
     稳定性：不稳定
     原地排序：是
     思想：分治法，选择基准值将数组分成两部分递归排序
     适用场景：大规模数据，平均性能优秀
 */
template<typename T, typename Comparator>
void quickSort(std::vector<T>& vec, Comparator comp) {
    if (vec.empty()) return;
    std::function<void(int, int)> quickSortHelper = [&](int low, int high) {
            if (low < high) {
                int pi = partition(vec, low, high, comp);
                quickSortHelper(low, pi - 1);
                quickSortHelper(pi + 1, high);
            }
        };
    quickSortHelper(0, vec.size() - 1);
}

/*
 堆排序 (Heap Sort)：
     时间复杂度：所有情况 O(n log n) 
     空间复杂度：O(1)
     稳定性：不稳定
     原地排序：是
     思想：利用堆数据结构，每次取出最大（或最小）元素
     适用场景：大规模数据，需要保证最坏情况性能
 */
template<typename T, typename Comparator>
void heapSort(std::vector<T>& vec, Comparator comp) {
    int n = vec.size();
    for (int i = n / 2 - 1; i >= 0; --i) heapify(vec, n, i, comp);
    for (int i = n - 1; i > 0; --i) {
        std::swap(vec[0], vec[i]);
        heapify(vec, i, 0, comp);
    }
}

} // namespace containerium
