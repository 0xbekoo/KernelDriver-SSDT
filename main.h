#include <ntddk.h>
#include <stdint.h>

uint32_t ReadMemory(
	uint64_t Address
);

uint64_t GetAbsoluteAddress(
	int SSN,
	UNICODE_STRING APIName
);

NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject, 
	PUNICODE_STRING RegistryPath
);

NTSTATUS UnloadDriver(
	PDRIVER_OBJECT DriverObject
);


typedef (NTAPI* My_NtCreateFile)(
	_Out_ PHANDLE FileHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_opt_ PLARGE_INTEGER AllocationSize,
	_In_ ULONG FileAttributes,
	_In_ ULONG ShareAccess,
	_In_ ULONG CreateDisposition,
	_In_ ULONG CreateOptions,
	_In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
	_In_ ULONG EaLength
);

typedef (NTAPI* My_NtWriteFile)(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	_In_reads_bytes_(Length) PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
);