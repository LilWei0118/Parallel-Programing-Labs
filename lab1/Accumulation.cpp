#include<iostream>
using namespace std;
int a[8192];


int main() {
	for (int i = 0; i < 8192; i++) {
		a[i] = 1;
	}//Normal
	/*int sum = 0;
	for (int z = 0; z < 100; z++) {
		for (int i = 0; i < 8192; i ++) {
			sum += a[i];
		}
	}*/


	//Multilink
	/*int sum1 = 0, sum2 = 0, sum = 0;
	for (int z = 0; z < 100; z++) {

		for (int i = 0; i < 8192; i += 2) {
			sum1 += a[i];
			sum2 += a[i + 1];
		}
		sum = sum1 + sum2;
	}*/

	//recursion
	//for (int z = 0; z < 100; z++) {

	//	for (int m = 8192; m > 1; m /= 2) {
	//		for (int i = 0; i < m / 2; i++) {
	//			a[i] = a[i * 2] + a[i * 2 + 1];
	//		}
	//	}

	//}
	return 0;
}
