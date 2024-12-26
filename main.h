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


typedef (NTAPI* My_IopCreateFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG Disposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength,
	IN CREATE_FILE_TYPE CreateFileType,
	IN PVOID ExtraCreateParameters OPTIONAL,
	IN ULONG Options,
	IN ULONG Flags,
	IN PDEVICE_OBJECT DeviceObject OPTIONAL
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