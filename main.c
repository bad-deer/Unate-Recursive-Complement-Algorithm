#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


struct cube
{
	char *val;
	struct cube *next;
};

struct split
{
	int index;
	int tr;
	int fs;
	struct split *next;
};

int read_in(struct cube **m, char *filename);
struct cube *complement(struct cube *m, int n); //n is the number of variables
int simpleCube(struct cube *m, int n); //n is the number of variables
struct cube *getComplement(struct cube *m, int mode, int n);
int split_find(struct cube *m, int n);   //return the coordinate of split variable x
struct cube *getPos(struct cube *m, int i, int n); //i is the coordinate of x
struct cube *getNeg(struct cube *m, int i, int n); //i is the coordinate of x
void andPos(struct cube *m, int i);
void andNeg(struct cube *m, int i);
struct cube *orCube(struct cube *m, struct cube *n, int i); //m is the pos n is the neg
void printCube(struct cube *m);
void freeCube(struct cube *m);

int main(int argc, char *args[])
{
	if (argc < 2) {
		printf("input method wrong\n");
	}
	struct cube *m = NULL;
	int var_num = 0;
	var_num = read_in(&m, args[1]);
	struct cube *n = NULL;
	n = complement(m, var_num);
	printf("%d\n", var_num);
	printCube(n);
	freeCube(n);
	return 0;
}

int read_in(struct cube **m, char *filename)
{
	FILE *ifile = fopen(filename, "r");

	*m = (struct cube *)malloc(sizeof(struct cube));
	memset(*m, 0, sizeof(struct cube));
	int var_num = 0;
	char num[10] = { 0 };
	fgets(num, sizeof(num), ifile);
	var_num = atoi(num);           //the number of input variables
	struct cube *mm = *m;
	int val_size = 0;
	val_size = sizeof(char) * var_num + 1;
	mm->val = (char *)malloc(val_size + 1);
    memset(mm->val, 0, val_size + 1);
	while (!feof(ifile)) {
		fgets(mm->val, val_size, ifile);
		if (strlen(mm->val) == val_size - 1) {
			mm->next = (struct cube *)malloc(sizeof(struct cube));
			memset(mm->next, 0, sizeof(struct cube));
			mm = mm->next;
			mm->val = (char *)malloc(val_size + 1);
		    memset(mm->val, 0, val_size + 1);
		}
	}
	return var_num;
	fclose(ifile);
}

struct cube *complement(struct cube *m, int n)
{
	struct cube *mm = m;
	int nn = n;
	int mode = 0;
	mode = simpleCube(mm, nn);
	mm = m;
	if (mode != 0) {
        struct cube *dirCmp = getComplement(mm, mode, nn);
        freeCube(m);
		return dirCmp;
	}
	else if (mode == 0) {
		int x = split_find(mm, nn);
		struct cube *posCofactor = NULL;
		struct cube *negCofactor = NULL;
		posCofactor = getPos(mm, x, nn);
		negCofactor = getNeg(mm, x, nn);
		freeCube(m);
		struct cube *pos = NULL;
		struct cube *neg = NULL;
		pos = complement(posCofactor, nn);
		neg = complement(negCofactor, nn);
        andPos(pos, x);
        andNeg(neg, x);
		struct cube *finalCmp = orCube(pos, neg, nn);    //orcube must free malloc
		freeCube(pos);
		freeCube(neg);
		return finalCmp;
	}
    return 0;
}

int simpleCube(struct cube *m, int n) // return 0: is not simple; return 1: empty; return 2: all-dont-care; return 3: just one cube;
{
	struct cube *mm = m;
	if (strlen(mm->val) < n) {
		return 1;
	}
	char *dontCare;
	dontCare = (char *)malloc(sizeof(char) * n);
	memset(dontCare, '-', sizeof(char) * n);
	while (mm->next) {
		if (strncasecmp(mm->val, dontCare, n) == 0) {
			return 2;
		}
		mm = mm->next;
	}
	free(dontCare);
	int i = 0;
	mm = m;
	while (mm->next) {
		i++;
		mm = mm->next;
	}
	if (i == 1) {
		return 3;
	}
	return 0;

}
struct cube *getComplement(struct cube *m, int mode, int n) //get complement directly for simple cube;
{
	struct cube *directCube = NULL;
	directCube = (struct cube *)malloc(sizeof(struct cube));
	memset(directCube, 0, sizeof(struct cube));

	struct cube *getCube = directCube;
	int i;
	switch (mode)
	{
		case 1:
			directCube->val = (char *)malloc(sizeof(char) * (n + 1));
            memset(directCube->val, 0, sizeof(char) * (n + 1));
			memset(directCube->val, '-', sizeof(char) * n);
			directCube->next = (struct cube *)malloc(sizeof(struct cube));
			memset(directCube->next, 0, sizeof(struct cube));
			return getCube;
		case 2:
			return getCube;
		case 3:
			for (i = 0; i < n; i++) {
				if (m->val[i] == '1') {
					directCube->val = (char *)malloc(sizeof(char) * (n + 1));
                    memset(directCube->val, 0, sizeof(char) * (n + 1));
					memset(directCube->val, '-', sizeof(char) * n);
					directCube->val[i] = '0';
					directCube->next = (struct cube *)malloc(sizeof(struct cube));
					memset(directCube->next, 0, sizeof(struct cube));
					directCube = directCube->next;
				}else if(m->val[i] == '0'){
					directCube->val = (char *)malloc(sizeof(char) * (n + 1));
                    memset(directCube->val, 0, sizeof(char) * (n + 1));
					memset(directCube->val, '-', sizeof(char) * n);
					directCube->val[i] = '1';
					directCube->next = (struct cube *)malloc(sizeof(struct cube));
					memset(directCube->next, 0, sizeof(struct cube));
					directCube = directCube->next;
				}
			}
			return getCube;
	}
	return getCube;
}
int split_find(struct cube *m, int n)
{
	int coor = 0;
	int i = 0;
	struct cube *mm = m;
	struct split *x = NULL;
	x = (struct split *)malloc(sizeof(struct split));
	memset(x, 0, sizeof(struct split));
	struct split *x1 = x;
	for (i = 0; i < n; i++) {
		x1->index = i;
		mm = m;
		while (mm->next) {
			if (mm->val[i] == '1') {
				x1->tr += 1;
			}
			else if (mm->val[i] == '0') {
				x1->fs += 1;
			}
			mm = mm->next;
		}
		x1->next = (struct split *)malloc(sizeof(struct split));
		memset(x1->next, 0, sizeof(struct split));
		x1 = x1->next;
	}
	x1 = x;
	struct split *xx = x;
    struct split *x2 = x->next;
	while (x2->index) {
	if ((x1->tr == 0) || (x1->fs == 0)) {
		if ((x2->tr == 0) || (x2->fs == 0)) {
			if ((x1->fs + x1->tr) < (x2->fs + x2->tr)) {
				coor = x2->index;
				x1 = x2;
			}
		}
		else {
			coor = x2->index;
			x1 = x2;
		}
	}
	else {
		if ((x2->tr != 0) && (x2->fs != 0)) {
			if ((x1->fs + x1->tr) < (x2->fs + x2->tr)) {
				coor = x2->index;
				x1 = x2;
			}
			else if (((x1->fs + x1->tr) == (x2->fs + x2->tr)) && (abs(x1->fs - x1->tr) > abs(x2->fs - x2->tr))) {
				coor = x2->index;
				x1 = x2;
			}
		}
	}
	x2 = x2->next;
	}
	//free split
	while (xx) {
		struct split *tmp;
		tmp = xx->next;
		free(xx);
		xx = tmp;
	}
	return coor;
}
struct cube *getPos(struct cube *m, int i, int n) //with the operation of AND
{
	struct cube *mm = m;
	struct cube *poscof = NULL;
	poscof = (struct cube *)malloc(sizeof(struct cube));
	memset(poscof, 0, sizeof(struct cube));
	poscof->val = (char *)malloc(sizeof(char) * (n + 1));
    memset(poscof->val, 0, sizeof(char) * (n + 1));
	struct cube *returnPos = poscof;
	while (mm->next) {
		if (mm->val[i] == '1') {
			strcpy(poscof->val, mm->val);
			poscof->val[i] = '-';
			poscof->next = (struct cube *)malloc(sizeof(struct cube));
			memset(poscof->next, 0, sizeof(struct cube));
			poscof = poscof->next;
			poscof->val = (char *)malloc(sizeof(char) * (n + 1));
			memset(poscof->val, 0, sizeof(char) * (n + 1));
		}else if(mm->val[i] == '-'){

			strcpy(poscof->val, mm->val);
			poscof->next = (struct cube *)malloc(sizeof(struct cube));
			memset(poscof->next, 0, sizeof(struct cube));
			poscof = poscof->next;
			poscof->val = (char *)malloc(sizeof(char) * (n + 1));
			memset(poscof->val, 0, sizeof(char) * (n + 1));
		}
		mm = mm->next;
	}
	return returnPos;
}
struct cube *getNeg(struct cube *m, int i, int n) //with the operation of AND
{
	struct cube *mm = m;
	struct cube *negcof = NULL;
	negcof = (struct cube *)malloc(sizeof(struct cube));
	memset(negcof, 0, sizeof(struct cube));
	negcof->val = (char *)malloc(sizeof(char) * (n + 1));
	memset(negcof->val, 0, sizeof(char) * (n + 1));
	struct cube *returnNeg = negcof;
	while (mm->next) {
		if (mm->val[i] == '0') {
			strcpy(negcof->val, mm->val);
			negcof->val[i] = '-';
			negcof->next = (struct cube *)malloc(sizeof(struct cube));
			memset(negcof->next, 0, sizeof(struct cube));
			negcof = negcof->next;
			negcof->val = (char *)malloc(sizeof(char) * (n + 1));
			memset(negcof->val, 0, sizeof(char) * (n + 1));
		}else if (mm->val[i] == '-') {

			strcpy(negcof->val, mm->val);
			negcof->next = (struct cube *)malloc(sizeof(struct cube));
			memset(negcof->next, 0, sizeof(struct cube));
			negcof = negcof->next;
			negcof->val = (char *)malloc(sizeof(char) * (n + 1));
			memset(negcof->val, 0, sizeof(char) * (n + 1));
		}
		mm = mm->next;
	}
	return returnNeg;
}
struct cube *orCube(struct cube *m, struct cube *n, int i)
{
	struct cube *orCube = NULL;
	orCube = (struct cube *)malloc(sizeof(struct cube));
	memset(orCube, 0, sizeof(struct cube));
	struct cube *returnOr = orCube;
	struct cube *mm = m;
	struct cube *nn = n;
	while (mm->next) {
		orCube->val = (char *)malloc(sizeof(char) * (i + 1));
		memset(orCube->val, 0, sizeof(char) * (i + 1));
		strcpy(orCube->val, mm->val);
		mm = mm->next;
		orCube->next = (struct cube *)malloc(sizeof(struct cube));
		memset(orCube->next, 0, sizeof(struct cube));
		orCube = orCube->next;
	}
	while (nn->next) {
		orCube->val = (char *)malloc(sizeof(char) * (i + 1));
		memset(orCube->val, 0, sizeof(char) * (i + 1));
		strcpy(orCube->val, nn->val);
		nn = nn->next;
		orCube->next = (struct cube *)malloc(sizeof(struct cube));
		memset(orCube->next, 0, sizeof(struct cube));
		orCube = orCube->next;
	}
	return returnOr;
}

void andPos(struct cube *m, int n)
{
	struct cube *mm = m;
	while (mm->next) {
		mm->val[n] = '1';
		mm = mm->next;
	}
}

void andNeg(struct cube *m, int n)
{
	struct cube *mm = m;
	while (mm->next) {
		mm->val[n] = '0';
		mm = mm->next;
	}
}

void printCube(struct cube *m)
{
	struct cube *mm = m;
	while (mm) {
        if(mm->val)
		    printf("%s\n", mm->val);
		mm = mm->next;
	}
}

void freeCube(struct cube *m)
{
	struct cube *mm = m;
	while (mm) {
		if (mm->val)
			free(mm->val);
		struct cube *tmp;
		tmp = mm->next;
		free(mm);
		mm = tmp;
	}
}

