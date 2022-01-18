#include <stdio.h>
#include <windows.h>

// https://ntopcode.wordpress.com/2018/02/26/anatomy-of-the-process-environment-block-peb-windows-internals/
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING, PUNICODE_STRING;
typedef const UNICODE_STRING* PCUNICODE_STRING;
typedef struct _CLIENT_ID {
    PVOID UniqueProcess;
    PVOID UniqueThread;
} CLIENT_ID, * PCLIENT_ID;
typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;
typedef struct _PEB_LDR_DATA {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;
typedef struct _LDR_DATA_TABLE_ENTRY {
    PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    PVOID Reserved2[2];
    PVOID BaseAddress;
    PVOID Reserved3[2];
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    BYTE Reserved4[8];
    PVOID Reserved5[3];
#pragma warning(push)
#pragma warning(disable: 4201) // we'll always use the Microsoft compiler
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    } DUMMYUNIONNAME;
#pragma warning(pop)
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _API_SET_NAMESPACE {
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    ULONG EntryOffset;
    ULONG HashOffset;
    ULONG HashFactor;
} API_SET_NAMESPACE, * PAPI_SET_NAMESPACE;
typedef struct _API_SET_HASH_ENTRY {
    ULONG Hash;
    ULONG Index;
} API_SET_HASH_ENTRY, * PAPI_SET_HASH_ENTRY;
typedef struct _API_SET_NAMESPACE_ENTRY {
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG HashedLength;
    ULONG ValueOffset;
    ULONG ValueCount;
} API_SET_NAMESPACE_ENTRY, * PAPI_SET_NAMESPACE_ENTRY;
typedef struct _API_SET_VALUE_ENTRY {
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY, * PAPI_SET_VALUE_ENTRY;

typedef enum _PROCESSINFOCLASS
{
	ProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
} PROCESSINFOCLASS;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationProcess(
	_In_ HANDLE ProcessHandle,
	_In_ PROCESSINFOCLASS ProcessInformationClass,
	_Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
	_In_ ULONG ProcessInformationLength,
	_Out_opt_ PULONG ReturnLength
);

const NTSTATUS STATUS_SUCCESS = 0;

typedef struct _PEB
{
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    union
    {
        BOOLEAN BitField;
        struct
        {
            BOOLEAN ImageUsesLargePages : 1;
            BOOLEAN IsProtectedProcess : 1;
            BOOLEAN IsImageDynamicallyRelocated : 1;
            BOOLEAN SkipPatchingUser32Forwarders : 1;
            BOOLEAN IsPackagedProcess : 1;
            BOOLEAN IsAppContainer : 1;
            BOOLEAN IsProtectedProcessLight : 1;
            BOOLEAN IsLongPathAwareProcess : 1;
        };
    };

    HANDLE Mutant;

    PVOID ImageBaseAddress;
    /*PPEB_LDR_DATA*/ void* Ldr;
    /*PRTL_USER_PROCESS_PARAMETERS*/ void* ProcessParameters;
    PVOID SubSystemData;
    PVOID ProcessHeap;
    PRTL_CRITICAL_SECTION FastPebLock;
    PVOID AtlThunkSListPtr;
    PVOID IFEOKey;
    union
    {
        ULONG CrossProcessFlags;
        struct
        {
            ULONG ProcessInJob : 1;
            ULONG ProcessInitializing : 1;
            ULONG ProcessUsingVEH : 1;
            ULONG ProcessUsingVCH : 1;
            ULONG ProcessUsingFTH : 1;
            ULONG ProcessPreviouslyThrottled : 1;
            ULONG ProcessCurrentlyThrottled : 1;
            ULONG ReservedBits0 : 25;
        };
    };
    union
    {
        PVOID KernelCallbackTable;
        PVOID UserSharedInfoPtr;
    };
    ULONG SystemReserved[1];
    ULONG AtlThunkSListPtr32;
    PVOID ApiSetMap;
} PEB, * PPEB;

void main()
{
    /* https://gist.github.com/lucasg/9aa464b95b4b7344cb0cddbdb4214b25 */

    PAPI_SET_NAMESPACE apiSetMap = NULL;

	// NtCurrentPeb replacement
#ifdef _M_X64
	PPEB pPEB = (PPEB)__readgsqword(0x60);
#elif _M_IX86
#error "PPEB Architecture Unsupported"
#else
#error "PPEB Architecture Unsupported"
#endif

    /* have namespace now */
    apiSetMap = (PAPI_SET_NAMESPACE)pPEB->ApiSetMap;

    UINT_PTR namespace_addr = (UINT_PTR)apiSetMap;
    PAPI_SET_NAMESPACE_ENTRY pnamespace_entry = (PAPI_SET_NAMESPACE_ENTRY)(namespace_addr + apiSetMap->EntryOffset);

    int i = 0, j = 0;
    UNICODE_STRING origin_name, forward_name;
    for (i = 0; i < apiSetMap->Count; i++)
    {
        origin_name.Buffer = (wchar_t*)(namespace_addr + pnamespace_entry->NameOffset);
        origin_name.Length = pnamespace_entry->NameLength;
        origin_name.MaximumLength = pnamespace_entry->NameLength;
        printf("%wZ.dll,", &origin_name);

        PAPI_SET_VALUE_ENTRY pvalue_entry = (PAPI_SET_VALUE_ENTRY)(namespace_addr + pnamespace_entry->ValueOffset);

        for (j = 0; j < pnamespace_entry->ValueCount; j++)
        {
            forward_name.Buffer = (wchar_t*)(namespace_addr + pvalue_entry->ValueOffset);
            forward_name.Length = pvalue_entry->ValueLength;
            forward_name.MaximumLength = pvalue_entry->ValueLength;
            printf("%wZ", &forward_name);

            if ((j + 1) != pnamespace_entry->ValueCount)
            {
                printf(",");
            }

            if (pvalue_entry->NameLength != 0)
            {
                origin_name.Buffer = (wchar_t*)(namespace_addr + pvalue_entry->NameOffset);
                origin_name.Length = pvalue_entry->NameLength;
                origin_name.MaximumLength = pvalue_entry->NameLength;
                printf("[%wZ]", &origin_name);
            }
            pvalue_entry++;
        }
        printf("\n");
        pnamespace_entry++;
    }
}
