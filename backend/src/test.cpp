#include <armadillo>
using namespace arma;
int main(){
	mat b;
	b << 1 << 1 << endr << 2 << 2;
	mat a; a << 2 << 2 << endr << 3 <<43;
	a = a * b;
	a.print("result:");
	return 0;
}
