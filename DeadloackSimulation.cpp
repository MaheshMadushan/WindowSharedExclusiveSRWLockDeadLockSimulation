#include <windows.h>
#include <tchar.h>
#include <iostream>

#define MAX_THREADS 2

DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void recursiveAcquire(SRWLOCK& lock_, int depth);
void exclusiveAcquire(SRWLOCK& lock_);

typedef struct MyData {
    int val1;
    SRWLOCK SRWLOCK;
} MYDATA, *PMYDATA;

SRWLOCK srwLock_;
int _tmain()
{
    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    InitializeSRWLock(&srwLock_);

    for( int i=0; i<MAX_THREADS; i++ )
    {
        pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(MYDATA));

        if( pDataArray[i] == NULL )
        {
            ExitProcess(2);
        }

        pDataArray[i]->val1 = i;
        pDataArray[i]->SRWLOCK = srwLock_;

        hThreadArray[i] = CreateThread(
            NULL,
            0,
            MyThreadFunction,
            pDataArray[i],
            0,
            &dwThreadIdArray[i]);


        if (hThreadArray[i] == NULL)
        {
           ExitProcess(3);
        }
    } // End of main thread creation loop.

    // Wait until all threads have terminated.

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);


    for(int i=0; i<MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if(pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }

    return 0;
}


DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{
    PMYDATA pDataArray;
    pDataArray = (PMYDATA)lpParam;

    if (pDataArray->val1 / 1 == 0)
    {
        // acquire shared lock recursively (r/w lock)
        recursiveAcquire(pDataArray->SRWLOCK,0);
        return 0;
    }
    // else acquire exclusive lock (r/w lock)
    exclusiveAcquire(pDataArray->SRWLOCK);
    return 0; 
}

void recursiveAcquire(SRWLOCK& lock_, int depth)
{
    AcquireSRWLockShared(&srwLock_);
    std::cout << "critical section recursiveAcquire(SRWLOCK& lock_)" << std::endl;
    if (depth <= 1)
    {
        recursiveAcquire(lock_, depth+1);
    }
}

void exclusiveAcquire(SRWLOCK& lock_)
{
    AcquireSRWLockExclusive(&srwLock_);
    std::cout << "critical section exclusiveAcquire(SRWLOCK& lock_)" << std::endl;
}
