
#ifndef DWM_ARG_H
#define DWM_ARG_H

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

#endif // !DWM_ARG_H
