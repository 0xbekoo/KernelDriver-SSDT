/*

@author: 0xbekoo
@Project: Kernel Driver - SSDT
@Last-Update: 2024-12-25

@Warning: Remember that this project is very simple and is only for educational purposes.

*/
#pragma warning(disable: 4083 4005)
#include "main.h"

/*
	SSDT Address needs to be entered manually. For this you need to get the address from windbg:

	kd > dps nt!keservicedescriptortable L1
	 > fffff806`402018c0  fffff802`2a0c7cb0 nt!KiServiceTable

	And then enter the address here like this:
*/
#define KiServiceTableAddress 0xfffff8022a0c7cb0

/*
	SSN (Service System Number) is the index of the service in the SSDT. You can find the SSN by windbg:

	kd> u ntdll!NtCreateFile L2
	 > ntdll!NtCreateFile:
	 > 00007fff`148cdf80 4c8bd1     mov     r10,rcx
	 > 00007fff`148cdf83 b855000000 mov     eax,55h
*/
#define SSN_NtCreateFile 0x55
#define SSN_NtWriteFile 0x8

uint32_t ReadMemory(uint64_t Address) {
	return *(volatile uint32_t*)Address;
}

uint64_t GetAbsoluteAddress(int SSN, UNICODE_STRING APIName) {
	if (0 == SSN) {
		DbgPrintEx(0, 0, "SSN is 0\n");
		return 0;
	}
	DbgPrintEx(0, 0, "Target API Name: %wZ\n", APIName);

	uint64_t RoutineAbsoluteAddress = 0;
	uint64_t OffsetAddress;
	uint32_t Offset = 0;

	// Before we can access the absolute address, we need to get the offset address.
	OffsetAddress = KiServiceTableAddress + 4 * SSN;
	DbgPrintEx(0, 0, "Offset Address: 0x%llx\n", OffsetAddress);

	// Now we can read the offset from the SSDT.
	Offset = ReadMemory(OffsetAddress);
	DbgPrintEx(0, 0, "Offset: 0x%08x\n", Offset);

	// This formula is used to calculate the absolute address of the routine in the SSDT for x64 systems.
	RoutineAbsoluteAddress = KiServiceTableAddress + (Offset >> 4);
	DbgPrintEx(0, 0, "The absolute address of %wZ is: 0x%llx\n", APIName, RoutineAbsoluteAddress);

	return RoutineAbsoluteAddress;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = UnloadDriver;

	HANDLE HandleFile = NULL;
	OBJECT_ATTRIBUTES ObjAttr;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS Status = STATUS_SUCCESS;
	uint64_t Address = 0;
	UNICODE_STRING FileName;
	UNICODE_STRING NtCreateFileName;
	UNICODE_STRING NtWriteFileName;
	UNICODE_STRING Data;

	// Initialize Strings 
	RtlInitUnicodeString(&NtCreateFileName, L"NtCreateFile");
	RtlInitUnicodeString(&NtWriteFileName, L"NtWriteFile");
	RtlInitUnicodeString(&FileName, L"\\??\\C:\\ssdt.txt");
	RtlInitUnicodeString(&Data, L"Hello SSDT!");

	// Initialize ObjectAttributes 
	InitializeObjectAttributes(&ObjAttr, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);	

	// Get Address of NtCreateFile 
	Address = GetAbsoluteAddress(SSN_NtCreateFile, NtCreateFileName);
	if (0 == Address) {
		return STATUS_NOT_FOUND;
	}
	// Check the README to understand why I have given the address of NtCreateFile to the IopCreateFile structure.
	My_IopCreateFile MyIopCreateFile = (My_IopCreateFile)Address;

	// Call IopCreateFile
	Status = MyIopCreateFile(&HandleFile, GENERIC_WRITE, &ObjAttr, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, \
								0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0, 0, NULL, 0, 0, NULL);
	if (!NT_SUCCESS(Status)) {
		DbgPrintEx(0, 0, "Failed to Create File! Error: 0x%08x\n", Status);
		return Status;
	}
	DbgPrintEx(0, 0, "Created File!\n\n");

	// Get Absolute Address of NtWriteFile 
	Address = GetAbsoluteAddress(SSN_NtWriteFile, NtWriteFileName);
	if (0 == Address) {
		return STATUS_NOT_FOUND;
	}
	My_NtWriteFile MyNtWriteFile = (My_NtWriteFile)Address;
	
	// Call NtWriteFile
	Status = MyNtWriteFile(HandleFile, NULL, NULL, NULL, &IoStatusBlock, Data.Buffer, Data.Length, NULL, NULL);
	if (!NT_SUCCESS(Status)) {
		DbgPrintEx(0, 0, "Failed to ZwWriteFile! Error: 0x%08x\n", Status);

		ZwClose(HandleFile);
		return Status;
	}
	DbgPrintEx(0, 0, "Wrote to File\n\n");

	ZwClose(HandleFile);
	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	DbgPrintEx(0, 0, "Unloading the Driver...\n");

	return STATUS_SUCCESS;
}