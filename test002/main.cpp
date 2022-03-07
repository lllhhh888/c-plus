#include <iostream>
#include <stdlib.h>
using namespace std;

int main() {
#ifdef _DEBUG
	cout << "bebug" << endl;
#endif // DEBUG

	int a = 1;
	int num[5] = { 1,5,7,2,2 };

	int* numPar = num;
	
	for (int i = 0; i <= 5;i++) {
		cout << i << endl;
	}

	std::cout << "test002" << std::endl;
	cout << a << endl;
	system("pause");
	return 0;
}