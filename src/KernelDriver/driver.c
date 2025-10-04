#include <ntddk.h>

// called when unloading the driver
VOID DriverUnload(
    PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    
    DbgPrint("[Threacer] Driver unloading...\n");
    DbgPrint("[Threacer] Goodbye from kernel mode!\n");
}

// called when driver is loaded
NTSTATUS DriverEntry(   
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    
    DbgPrint("[Threacer] Hello from kernel mode!\n");
    DbgPrint("[Threacer] Driver loaded successfully\n");
    
    // Set unload routine (per poter scaricare il driver)
    DriverObject->DriverUnload = DriverUnload;
    
    return STATUS_SUCCESS;
}

