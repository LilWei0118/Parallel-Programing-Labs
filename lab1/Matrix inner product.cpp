#include<iostream>
using namespace std;
int a[1000];
int b[1000][1000];
int sum[1000] = { 0 };
int main() {
	for (int i = 0; i < 1000; i++) {
		a[i] = 1;
	}
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 100; j++) {
			b[i][j] = 1;
		}
	}//Normal
	//for (int z = 0; z < 100; z++) {
	//	for (int i = 0; i < 1000; i++) {
	//		for (int j = 0; j < 1000; j++) {
	//				sum[i] += b[i][j] * a[j];
	//			
	//	}
	//}
	//cache optimize
	//for (int z = 0; z < 100; z++) {
	//	for (int j = 0; j < 1000; j++) {
	//		for (int i = 0; i < 1000; i++) {
	//				sum[i] += b[i][j] * a[j];
	//		}
	//	}
	//}

	return 0;
}
