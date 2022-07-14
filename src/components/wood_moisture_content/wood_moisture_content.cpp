#include "wood_moisture_content.h"
#include <Arduino.h>

float getClosest(float val1, float val2, float target) {
  if (target - val1 >= val2 - target)
    return val2;
  else
    return val1;
}

int search(float A[], int nLevels, float value) {
  Serial.print("Arr[0]: ");
  Serial.println(A[0]);
  for(int i = 0; i < nLevels; i++){
    if(A[i] == value){
      return i;
    }
  }
  return 100;
}

float getMoisturePct(float arr[], int n, float target) {
  if (target <= arr[0])
    return arr[0];
  if (target >= arr[n - 1])
    return arr[n - 1];

  int i = 0, j = n, mid = 0;
  while (i < j) {
    mid = (i + j) / 2;

    if (arr[mid] == target)
      return arr[mid];

    if (target < arr[mid]) {
      if (mid > 0 && target > arr[mid - 1])
        return getClosest(arr[mid - 1],
                          arr[mid], target);
      j = mid;
    }
    else {
      if (mid < n - 1 && target < arr[mid + 1]) {
        return getClosest(arr[mid],
                          arr[mid + 1], target);
      }
        i = mid + 1; 
    }
  }
  return arr[mid];
}
