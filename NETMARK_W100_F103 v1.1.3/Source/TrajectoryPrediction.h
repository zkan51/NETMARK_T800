#ifndef __TrajectoryPrediction_H_
#define __TrajectoryPrediction_H_

#include "gps.h"
#include "stdio.h"
#include "usart1.h"
#include "time.h"
#define max_buf_gps_data_len 600

double calSphereDist(double lat_1,double lon_1, double lat_2, double lon_2);
int sign(double x);
void bubbleSort(double* a, int n);
double gpsDataMedfilt(double* x, int N);
extern int TrajectoryPrediction(char * gpsMsg, GPS_INFO *GPS);

#endif/*__TrajectoryPrediction_H_*/
