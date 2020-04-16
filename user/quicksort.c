
#include <inc/lib.h>

uint32 ws_size_first=0;

//Functions Declarations
void Swap(int *Elements, int First, int Second);
void InitializeAscending(int *Elements, int NumOfElements);
void InitializeDescending(int *Elements, int NumOfElements);
void InitializeSemiRandom(int *Elements, int NumOfElements);
void PrintElements(int *Elements, int NumOfElements);

void QuickSort(int *Elements, int NumOfElements);
void QSort(int *Elements,int NumOfElements, int startIndex, int finalIndex);
uint32 CheckSorted(int *Elements, int NumOfElements);

void _main(void)
{	
	char Chose ;
	char Line[255] ;
	int Iteration = 0 ;

	do
	{
		int InitFreeFrames = sys_calculate_free_frames() + sys_calculate_modified_frames();

		Iteration++ ;
		//		cprintf("Free Frames Before Allocation = %d\n", sys_calculate_free_frames()) ;

//	sys_disable_interrupt();

			readline("Enter the number of elements: ", Line);
			int NumOfElements = strtol(Line, NULL, 10) ;
			int *Elements = malloc(sizeof(int) * NumOfElements) ;
			cprintf("Choose the initialization method:\n") ;
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

		//sys_enable_interrupt();
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

		QuickSort(Elements, NumOfElements);

		//		PrintElements(Elements, NumOfElements);

		uint32 Sorted = CheckSorted(Elements, NumOfElements);

		if(Sorted == 0) panic("The array is NOT sorted correctly") ;
		else
		{ 
				cprintf("\n===============================================\n") ;
				cprintf("Congratulations!! The array is sorted correctly\n") ;
				cprintf("===============================================\n\n") ;
		}

		//		cprintf("Free Frames After Calculation = %d\n", sys_calculate_free_frames()) ;

			cprintf("Freeing the Heap...\n\n") ;

			free(Elements) ;


		///========================================================================
	//sys_disable_interrupt();
			cprintf("Do you want to repeat (y/n): ") ;
			Chose = getchar() ;
			cputchar(Chose);
			cputchar('\n');
			cputchar('\n');
	//sys_enable_interrupt();

	} while (Chose == 'y');

}

///Quick sort 
void QuickSort(int *Elements, int NumOfElements)
{
	QSort(Elements, NumOfElements, 0, NumOfElements-1) ;
}


void QSort(int *Elements,int NumOfElements, int startIndex, int finalIndex)
{
	if (startIndex >= finalIndex) return;

	int i = startIndex+1, j = finalIndex;

	while (i <= j)
	{
		while (i <= finalIndex && Elements[startIndex] >= Elements[i]) i++;
		while (j > startIndex && Elements[startIndex] <= Elements[j]) j--;

		if (i <= j)
		{
			Swap(Elements, i, j);
		}
	}

	Swap( Elements, startIndex, j);

	QSort(Elements, NumOfElements, startIndex, j - 1);
	QSort(Elements, NumOfElements, i, finalIndex);
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
