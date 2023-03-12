#include<iostream>
using namespace std;
int a[8192];

int main() {
	for (int i = 0; i < 8192; i++) {
		a[i] = 1;
	}
	int sum = 0;
	for (int z = 0; z < 100; z++) {
		for (int i = 0; i < 8192; i ++) {
			sum += a[i];
		}
	}
	return 0;
}