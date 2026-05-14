#pragma once

template <typename T>
void fill(T *arr, T val, int start, int end)
{
	for (int i = start; i < end; i++)
		arr[i] = val;
}