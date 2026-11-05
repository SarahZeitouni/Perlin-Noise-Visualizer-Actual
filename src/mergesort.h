#ifndef MERGESORT_H
#define MERGESORT_H

#define mergesort(arr, begin, end) 								\
_Generic((void(*)(typeof(arr), typeof(begin), typeof(end)))0,	\
	void(*)(int*, int, int): 	mergesort_int,					\
	void(*)(float*, int, int):	mergesort_float					\
)(arr, begin, end)

void mergesort_int(int* arr, int begin, int end) {
	if (begin + 1 >= end)
		return;

	int mid = (begin+end)/2;
	mergesort_int(arr, begin, mid);
	mergesort_int(arr, mid, end);

	int lower[mid - begin];
	int upper[end - mid];
	for (int i = 0; i < mid - begin; i ++)
		lower[i] = arr[begin + i];
	for (int i = 0; i < end - mid; i ++)
		upper[i] = arr[mid + i];

	int l = 0, u = 0;
	while (l < mid - begin && u < end - mid) {
		if (lower[l] < upper[u]) {
			arr[begin + l + u] = lower[l];
			l ++;
		}
		else {
			arr[begin + l + u] = upper[u];
			u ++;
		}
	}
	while (l < mid - begin) {
		arr[begin + l + u] = lower[l];
		l ++;
	}
	while (u < end - mid) {
		arr[begin + l + u] = upper[u];
		u ++;
	}
}

void mergesort_float(float* arr, int begin, int end) {
	if (begin + 1 >= end)
		return;

	int mid = (begin+end)/2;
	mergesort_float(arr, begin, mid);
	mergesort_float(arr, mid, end);

	float lower[mid - begin];
	float upper[end - mid];
	for (int i = 0; i < mid - begin; i ++)
		lower[i] = arr[begin + i];
	for (int i = 0; i < end - mid; i ++)
		upper[i] = arr[mid + i];

	int l = 0, u = 0;
	while (l < mid - begin && u < end - mid) {
		if (lower[l] < upper[u]) {
			arr[begin + l + u] = lower[l];
			l ++;
		}
		else {
			arr[begin + l + u] = upper[u];
			u ++;
		}
	}
	while (l < mid - begin) {
		arr[begin + l + u] = lower[l];
		l ++;
	}
	while (u < end - mid) {
		arr[begin + l + u] = upper[u];
		u ++;
	}
}

#endif
