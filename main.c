#include <stdio.h>

extern void* PoolAlloc(int);
extern void PoolFree();
extern void PoolInit();
extern void PoolRelease();
int main() {
	int i;
	void *a,*b,*c,*d,*e,*f,*g,*h;

	PoolInit();

	a = PoolAlloc(1);
	b = PoolAlloc(5);
	c = PoolAlloc(26);
	d = PoolAlloc(345);
	e = PoolAlloc(553);
	f = PoolAlloc(1025);
	for(i=0; i<30; i++) {
		g = PoolAlloc(561);
		PoolFree(g);
	}
	PoolRelease();
	return 0;
}
