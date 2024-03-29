#include<efi.h>
#include<efilib.h>



EFI_STATUS
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_DEVICE_PATH *Path;
    EFI_LOADED_IMAGE *LoadedImageParent;
    EFI_LOADED_IMAGE *LoadedImage;
    EFI_HANDLE Image;
    EFI_STATUS Status=EFI_SUCCESS;

    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, EFI!\n");

    Status = uefi_call_wrapper ( RT->SetVariable,
                    L"SecureBoot",               // VariableName
                    &gEfiGlobalVariableGuid, // VendorGuid
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                                                           // Attributes
                    0,        // DataSize
                    NULL            // Data
                    );

    if (EFI_ERROR(Status)) {
                  Print(L"Could not set SecureBoot Variable %r\n", Status);
                  return Status;
                        }

    Status = uefi_call_wrapper ( RT->SetVariable,
                    L"SetupMode",               // VariableName
                    &gEfiGlobalVariableGuid, // VendorGuid
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS,
                                                           // Attributes
                    0,        // DataSize
                    NULL            // Data
                    );

    if (EFI_ERROR(Status)) {
                  Print(L"Could not set SetupMode Variable %r\n", Status);
                  return Status;
                        }

    Status = uefi_call_wrapper ( RT->SetVariable,
                    L"PK",               // VariableName
                    &gEfiGlobalVariableGuid, // VendorGuid
                    EFI_VARIABLE_NON_VOLATILE |
                    EFI_VARIABLE_BOOTSERVICE_ACCESS |
                    EFI_VARIABLE_RUNTIME_ACCESS |
                    EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS,
                                                           // Attributes
                    0,        // DataSize
                    NULL            // Data
                    );

    if (EFI_ERROR(Status)) {
                  Print(L"Could not set Public Platform Key %r\n", Status);
                  return Status;
                        }


    Status = uefi_call_wrapper(BS->OpenProtocol, 6, ImageHandle, &LoadedImageProtocol, &LoadedImageParent, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(Status)) {
        Print(L"Could not get LoadedImageProtocol handler %r\n", Status);
        return Status;
    }

    Path = FileDevicePath(LoadedImageParent->DeviceHandle, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi");
    if (Path == NULL) {
        Print(L"Could not get device path.");
        return EFI_INVALID_PARAMETER;
    }

    Status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, ImageHandle, Path, NULL, 0, &Image);
    if (EFI_ERROR(Status)) {
        Print(L"Could not load %r", Status);
        FreePool(Path);
        return Status;
    }

    Status = uefi_call_wrapper(BS->StartImage, 3, Image, NULL, NULL);
    uefi_call_wrapper(BS->UnloadImage, 1, Image);
    FreePool(Path);

    return EFI_SUCCESS;
}
