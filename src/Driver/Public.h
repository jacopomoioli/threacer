/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_Driver,
    0x29d7cfe5,0xaff8,0x443c,0xbe,0x28,0x51,0x5e,0x87,0x40,0x86,0x52);
// {29d7cfe5-aff8-443c-be28-515e87408652}
