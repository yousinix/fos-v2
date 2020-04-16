#include <inc/lib.h>

uint32 ws_size_first=0;

//Functions Declarations
void Swap(int *Elements, int First, int Second);
void InitializeAscending(int *Elements, int NumOfElements);
void InitializeDescending(int *Elements, int NumOfElements);
void InitializeSemiRandom(int *Elements, int NumOfElements);
void PrintElements(int *Elements, int NumOfElements);

void MSort(int* A, int p, int r);
void Merge(int* A, int p, int q, int r);

uint32 CheckSorted(int *Elements, int NumOfElements);

void _main(void)
{
	char Line[255] ;
	char Chose ;
	do
	{
		//2012: lock the interrupt
		sys_disable_interrupt();

		cprintf("\n");
		cprintf("!!!!!!!!!!!!!!!!!!!!\n");
		cprintf("!!!! MERGE SORT !!!!\n");
		cprintf("!!!!!!!!!!!!!!!!!!!!\n");
		cprintf("\n");
		readline("Enter the number of elements: ", Line);
		int NumOfElements = strtol(Line, NULL, 10) ;
		int *Elements = malloc(sizeof(int) * NumOfElements) ;
		cprintf("Chose the initialization method:\n") ;
		cprintf("a) Ascending\n") ;
		cprintf("b) Descending\n") ;
		cprintf("c) Semi random\n");
		do
		{
			cprintf("Select: ") ;
			Chose = getchar() ;
			cputchar(Chose);
			cputchar('\n');
		} while (Chose != 'a' && Chose != 'b' && Chose != 'c');

		//2012: lock the interrupt
		sys_enable_interrupt();

		int  i ;
		switch (Chose)
		{
		case 'a':
			InitializeAscending(Elements, NumOfElements);
			break ;
		case 'b':
			InitializeDescending(Elements, NumOfElements);
			break ;
		case 'c':
			InitializeSemiRandom(Elements, NumOfElements);
			break ;
		default:
			InitializeSemiRandom(Elements, NumOfElements);
		}

		MSort(Elements, 1, NumOfElements);

		sys_disable_interrupt();
		cprintf("Sorting is Finished!!!!it'll be checked now....\n") ;
		//PrintElements(Elements, NumOfElements);
		sys_enable_interrupt();

		uint32 Sorted = CheckSorted(Elements, NumOfElements);

		if(Sorted == 0) panic("The array is NOT sorted correctly") ;
		else
		{
			sys_disable_interrupt();
			cprintf("===============================================\n") ;
			cprintf("Congratulations!! The array is sorted correctly\n") ;
			cprintf("===============================================\n\n") ;
			sys_enable_interrupt();
		}

		free(Elements) ;

		sys_disable_interrupt();
			Chose = 0 ;
			while (Chose != 'y' && Chose != 'n')
			{
				cprintf("Do you want to repeat (y/n): ") ;
				Chose = getchar() ;
				cputchar(Chose);
				cputchar('\n');
				cputchar('\n');
			}
		sys_enable_interrupt();

	} while (Chose == 'y');

}


uint32 CheckSorted(int *Elements, int NumOfElements)
{
	uint32 Sorted = 1 ;
	int i ;
	for (i = 0 ; i < NumOfElements - 1; i++)
	{
		if (Elements[i] > Elements[i+1])
		{
			Sorted = 0 ;
			break;
		}
	}
	return Sorted ;
}

///Private Functions


void Swap(int *Elements, int First, int Second)
{
	int Tmp = Elements[First] ;
	Elements[First] = Elements[Second] ;
	Elements[Second] = Tmp ;
}

void InitializeAscending(int *Elements, int NumOfElements)
{
	int i ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		(Elements)[i] = i ;
	}

}

void InitializeDescending(int *Elements, int NumOfElements)
{
	int i ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		Elements[i] = NumOfElements - i - 1 ;
	}

}

void InitializeSemiRandom(int *Elements, int NumOfElements)
{
	int i ;
	int Repetition = NumOfElements / 3 ;
	for (i = 0 ; i < NumOfElements ; i++)
	{
		Elements[i] = i % Repetition ;
		//	cprintf("i=%d\n",i);
	}

}

void PrintElements(int *Elements, int NumOfElements)
{
	int i ;
	int NumsPerLine = 20 ;
	for (i = 0 ; i < NumOfElements-1 ; i++)
	{
		if (i%NumsPerLine == 0)
			cprintf("\n");
		cprintf("%d, ",Elements[i]);
	}
	cprintf("%d\n",Elements[i]);

}


void MSort(int* A, int p, int r)
{
	if (p >= r)
	{
		return;
	}

	int q = (p + r) / 2;

	MSort(A, p, q);

	MSort(A, q + 1, r);

	Merge(A, p, q, r);

}

void Merge(int* A, int p, int q, int r)
{
	int leftCapacity = q - p + 1;

	int rightCapacity = r - q;

	int leftIndex = 0;

	int rightIndex = 0;

	int* Left = malloc(sizeof(int) * leftCapacity);

	int* Right = malloc(sizeof(int) * rightCapacity);

	//	int Left[5000] ;
	//	int Right[5000] ;

	int i, j, k;
	for (i = 0; i < leftCapacity; i++)
	{
		Left[i] = A[p + i - 1];
	}
	for (j = 0; j < rightCapacity; j++)
	{
		Right[j] = A[q + j];
	}

	for ( k = p; k <= r; k++)
	{
		if (leftIndex < leftCapacity && rightIndex < rightCapacity)
		{
			if (Left[leftIndex] < Right[rightIndex] )
			{
				A[k - 1] = Left[leftIndex++];
			}
			else
			{
				A[k - 1] = Right[rightIndex++];
			}
		}
		else if (leftIndex < leftCapacity)
		{
			A[k - 1] = Left[leftIndex++];
		}
		else
		{
			A[k - 1] = Right[rightIndex++];
		}
	}

}

