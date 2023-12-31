/** @file

  Copyright (c) 2008-2009, Apple Inc. All rights reserved.
  
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciEmulation.h"
#include <Omap3530/Omap3530.h>

EFI_CPU_ARCH_PROTOCOL      *gCpu;
EMBEDDED_EXTERNAL_DEVICE   *gTPS65950;

#define HOST_CONTROLLER_OPERATION_REG_SIZE  0x44

typedef struct {
  ACPI_HID_DEVICE_PATH      AcpiDevicePath;
  PCI_DEVICE_PATH           PciDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevicePath;
} EFI_PCI_IO_DEVICE_PATH;

typedef struct {
  UINT32                  Signature;
  EFI_PCI_IO_DEVICE_PATH  DevicePath;
  EFI_PCI_IO_PROTOCOL     PciIoProtocol;
  PCI_TYPE00              *ConfigSpace;
  PCI_ROOT_BRIDGE         RootBridge;
  UINTN                   Segment;
} EFI_PCI_IO_PRIVATE_DATA;

#define EFI_PCI_IO_PRIVATE_DATA_SIGNATURE     SIGNATURE_32('p', 'c', 'i', 'o')
#define EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(a)  CR(a, EFI_PCI_IO_PRIVATE_DATA, PciIoProtocol, EFI_PCI_IO_PRIVATE_DATA_SIGNATURE)

EFI_PCI_IO_DEVICE_PATH PciIoDevicePathTemplate = 
{
  {
    { ACPI_DEVICE_PATH, ACPI_DP, sizeof (ACPI_HID_DEVICE_PATH), 0},
    EISA_PNP_ID(0x0A03),  // HID
    0                     // UID
  },
  {
    { HARDWARE_DEVICE_PATH, HW_PCI_DP, sizeof (PCI_DEVICE_PATH), 0},
    0,
    0
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}
};

STATIC
VOID
ConfigureUSBHost (
  VOID
  )
{
  EFI_STATUS Status;
  UINT8      Data = 0;

  // Get the Power IC protocol.
  Status = gBS->LocateProtocol(&gEmbeddedExternalDeviceProtocolGuid, NULL, (VOID **)&gTPS65950);
  ASSERT_EFI_ERROR(Status);

  //Enable power to the USB host.
  Status = gTPS65950->Read(gTPS65950, EXTERNAL_DEVICE_REGISTER(I2C_ADDR_GRP_ID3, LEDEN), 1, &Data);
  ASSERT_EFI_ERROR(Status);

  //LEDAON & LEDAPWM control the power to the USB host so enable those bits.
  Data |= (LEDAON | LEDAPWM);

  Status = gTPS65950->Write(gTPS65950, EXTERNAL_DEVICE_REGISTER(I2C_ADDR_GRP_ID3, LEDEN), 1, &Data);
  ASSERT_EFI_ERROR(Status);

  // USB reset (GPIO 147 - Port 5 pin 19) output low
  MmioAnd32 (GPIO5_BASE + GPIO_OE, ~BIT19);
  MmioWrite32 (GPIO5_BASE + GPIO_CLEARDATAOUT, BIT19);
  
  // Turn on functional & interface clocks to the USBHOST power domain
  MmioOr32 (CM_FCLKEN_USBHOST, CM_FCLKEN_USBHOST_EN_USBHOST2_ENABLE | CM_FCLKEN_USBHOST_EN_USBHOST1_ENABLE);
  MmioOr32 (CM_ICLKEN_USBHOST, CM_ICLKEN_USBHOST_EN_USBHOST_ENABLE);
  // Wait for clock to become active
  while (0 == (MmioRead32 (CM_CLKSTST_USBHOST) & 1));



  // Take USB host out of force-standby mode
  MmioWrite32 (UHH_SYSCONFIG,  UHH_SYSCONFIG_MIDLEMODE_NO_STANDBY
                             | UHH_SYSCONFIG_CLOCKACTIVITY_ON
                             | UHH_SYSCONFIG_SIDLEMODE_NO_STANDBY
                             | UHH_SYSCONFIG_ENAWAKEUP_ENABLE    
                             | UHH_SYSCONFIG_SOFTRESET
                             );
   while ((MmioRead32 (UHH_SYSSTATUS) & UHH_SYSSTATUS_RESETDONE) != UHH_SYSSTATUS_RESETDONE);

  MmioWrite32 (UHH_SYSCONFIG,  UHH_SYSCONFIG_CLOCKACTIVITY_ON
                             | UHH_SYSCONFIG_SIDLEMODE_NO_STANDBY
                             | UHH_SYSCONFIG_ENAWAKEUP_ENABLE    
                             );
                             
                             
  MmioWrite32 (UHH_HOSTCONFIG,  UHH_HOSTCONFIG_ENA_INCR16_ENABLE           
                              | UHH_HOSTCONFIG_ENA_INCR8_ENABLE            
                              | UHH_HOSTCONFIG_ENA_INCR4_ENABLE            
                              );

  // USB reset output high
  MmioWrite32 (GPIO5_BASE + GPIO_SETDATAOUT, BIT19);

}

EFI_STATUS
PciIoPollMem (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoPollIo (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN  UINT8                        BarIndex,
  IN  UINT64                       Offset,
  IN  UINT64                       Mask,
  IN  UINT64                       Value,
  IN  UINT64                       Delay,
  OUT UINT64                       *Result
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoMemRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRead (&Private->RootBridge.Io, 
                                (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                                Private->ConfigSpace->Device.Bar[BarIndex] + Offset,
                                Count,
                                Buffer
                                );
}

EFI_STATUS
PciIoMemWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemWrite (&Private->RootBridge.Io, 
                                 (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width,
                                 Private->ConfigSpace->Device.Bar[BarIndex] + Offset,
                                 Count,
                                 Buffer
                                 );
}

EFI_STATUS
PciIoIoRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoIoWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        BarIndex,
  IN     UINT64                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoPciRead (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)Width, 
                               Count, 
                               TRUE, 
                               (PTR)(UINTN)Buffer, 
                               TRUE, 
                               (PTR)(UINTN)(((UINT8 *)Private->ConfigSpace) + Offset)
                              );
}

EFI_STATUS
PciIoPciWrite (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT32                       Offset,
  IN     UINTN                        Count,
  IN OUT VOID                         *Buffer
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  return PciRootBridgeIoMemRW ((EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH) Width, 
                               Count, 
                               TRUE, 
                               (PTR)(UINTN)(((UINT8 *)Private->ConfigSpace) + Offset), 
                               TRUE, 
                               (PTR)(UINTN)Buffer
                               );
}

EFI_STATUS
PciIoCopyMem (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH    Width,
  IN     UINT8                        DestBarIndex,
  IN     UINT64                       DestOffset,
  IN     UINT8                        SrcBarIndex,
  IN     UINT64                       SrcOffset,
  IN     UINTN                        Count
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoMap (
  IN EFI_PCI_IO_PROTOCOL                *This,
  IN     EFI_PCI_IO_PROTOCOL_OPERATION  Operation,
  IN     VOID                           *HostAddress,
  IN OUT UINTN                          *NumberOfBytes,
  OUT    EFI_PHYSICAL_ADDRESS           *DeviceAddress,
  OUT    VOID                           **Mapping
  )
{
  MAP_INFO_INSTANCE     *Map;
  EFI_STATUS            Status;

  if ( HostAddress == NULL || NumberOfBytes == NULL || 
       DeviceAddress == NULL || Mapping == NULL ) {
    
    return EFI_INVALID_PARAMETER;
  }
  

  if (Operation >= EfiPciOperationMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  *DeviceAddress = ConvertToPhysicalAddress (HostAddress);

  // Data cache flush (HostAddress, NumberOfBytes);

  // Remember range so we can flush on the other side
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (PCI_DMA_MAP), (VOID **) &Map);
  if (EFI_ERROR(Status)) {
    return  EFI_OUT_OF_RESOURCES;
  }
  
  *Mapping = Map;

  Map->HostAddress   = (UINTN)HostAddress;
  Map->DeviceAddress = *DeviceAddress;
  Map->NumberOfBytes = *NumberOfBytes;
  Map->Operation     = Operation;

  // EfiCpuFlushTypeWriteBack, EfiCpuFlushTypeInvalidate
  gCpu->FlushDataCache (gCpu, (EFI_PHYSICAL_ADDRESS)(UINTN)HostAddress, *NumberOfBytes, EfiCpuFlushTypeWriteBackInvalidate);
  
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoUnmap (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  VOID                         *Mapping
  )
{
  PCI_DMA_MAP *Map;
  
  if (Mapping == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  
  Map = (PCI_DMA_MAP *)Mapping;
  if (Map->Operation == EfiPciOperationBusMasterWrite) {
    //
    // Make sure we read buffer from uncached memory and not the cache
    //
    gCpu->FlushDataCache (gCpu, Map->HostAddress, Map->NumberOfBytes, EfiCpuFlushTypeInvalidate);
  } 
  
  FreePool (Map);

  return EFI_SUCCESS;
}

EFI_STATUS
PciIoAllocateBuffer (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  EFI_ALLOCATE_TYPE            Type,
  IN  EFI_MEMORY_TYPE              MemoryType,
  IN  UINTN                        Pages,
  OUT VOID                         **HostAddress,
  IN  UINT64                       Attributes
  )
{
  if (Attributes & EFI_PCI_ATTRIBUTE_INVALID_FOR_ALLOCATE_BUFFER) {
    return EFI_UNSUPPORTED;
  }

  if (HostAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The only valid memory types are EfiBootServicesData and EfiRuntimeServicesData
  //
  // We used uncached memory to keep coherency
  //
  if (MemoryType == EfiBootServicesData) {
    *HostAddress = UncachedAllocatePages (Pages);
  } else if (MemoryType != EfiRuntimeServicesData) {
    *HostAddress = UncachedAllocateRuntimePages (Pages);
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciIoFreeBuffer (
  IN EFI_PCI_IO_PROTOCOL           *This,
  IN  UINTN                        Pages,
  IN  VOID                         *HostAddress
  )
{
  if (HostAddress == NULL) {
     return EFI_INVALID_PARAMETER;
  } 
  
  UncachedFreePages (HostAddress, Pages);
  return EFI_SUCCESS;
}


EFI_STATUS
PciIoFlush (
  IN EFI_PCI_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetLocation (
  IN EFI_PCI_IO_PROTOCOL          *This,
  OUT UINTN                       *SegmentNumber,
  OUT UINTN                       *BusNumber,
  OUT UINTN                       *DeviceNumber,
  OUT UINTN                       *FunctionNumber
  )
{
  EFI_PCI_IO_PRIVATE_DATA *Private = EFI_PCI_IO_PRIVATE_DATA_FROM_THIS(This);

  if (SegmentNumber != NULL) {
    *SegmentNumber = Private->Segment;
  }

  if (BusNumber != NULL) {
    *BusNumber = 0xff;
  }

  if (DeviceNumber != NULL) {
    *DeviceNumber = 0;
  }

  if (FunctionNumber != NULL) {
    *FunctionNumber = 0;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciIoAttributes (
  IN EFI_PCI_IO_PROTOCOL                       *This,
  IN  EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION  Operation,
  IN  UINT64                                   Attributes,
  OUT UINT64                                   *Result OPTIONAL
  )
{
  switch (Operation) {
  case EfiPciIoAttributeOperationGet:
  case EfiPciIoAttributeOperationSupported:
    if (Result == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    // We are not a real PCI device so just say things we kind of do
    *Result = EFI_PCI_IO_ATTRIBUTE_MEMORY | EFI_PCI_IO_ATTRIBUTE_BUS_MASTER | EFI_PCI_DEVICE_ENABLE;
    break;

  case EfiPciIoAttributeOperationSet:
  case EfiPciIoAttributeOperationEnable:
  case EfiPciIoAttributeOperationDisable:
    // Since we are not a real PCI device no enable/set or disable operations exist.
    return EFI_SUCCESS;
    
  default:
  ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  };
  return EFI_SUCCESS;
}

EFI_STATUS
PciIoGetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL             *This,
  IN  UINT8                          BarIndex,
  OUT UINT64                         *Supports, OPTIONAL
  OUT VOID                           **Resources OPTIONAL
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_STATUS
PciIoSetBarAttributes (
  IN EFI_PCI_IO_PROTOCOL              *This,
  IN     UINT64                       Attributes,
  IN     UINT8                        BarIndex,
  IN OUT UINT64                       *Offset,
  IN OUT UINT64                       *Length
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

EFI_PCI_IO_PROTOCOL PciIoTemplate = 
{
  PciIoPollMem,
  PciIoPollIo,
  PciIoMemRead,
  PciIoMemWrite,
  PciIoIoRead,
  PciIoIoWrite,
  PciIoPciRead,
  PciIoPciWrite,
  PciIoCopyMem,
  PciIoMap,
  PciIoUnmap,
  PciIoAllocateBuffer,
  PciIoFreeBuffer,
  PciIoFlush,
  PciIoGetLocation,
  PciIoAttributes,
  PciIoGetBarAttributes,
  PciIoSetBarAttributes,
  0,
  0
};

EFI_STATUS
EFIAPI
PciEmulationEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;
  EFI_PCI_IO_PRIVATE_DATA *Private;
  UINT8                   CapabilityLength;
  UINT8                   PhysicalPorts;
  UINTN                   Count;

  // Get the Cpu protocol for later use
  Status = gBS->LocateProtocol(&gEfiCpuArchProtocolGuid, NULL, (VOID **)&gCpu);
  ASSERT_EFI_ERROR(Status);

  //Configure USB host for OMAP3530.
  ConfigureUSBHost();

  // Create a private structure
  Private = AllocatePool(sizeof(EFI_PCI_IO_PRIVATE_DATA));
  if (Private == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }
  
  Private->Signature              = EFI_PCI_IO_PRIVATE_DATA_SIGNATURE;  // Fill in signature
  Private->RootBridge.Signature   = PCI_ROOT_BRIDGE_SIGNATURE;          // Fake Root Bridge structure needs a signature too
  Private->RootBridge.MemoryStart = USB_EHCI_HCCAPBASE;                 // Get the USB capability register base
  Private->Segment                = 0;                                  // Default to segment zero

  // Find out the capability register length and number of physical ports.
  CapabilityLength = MmioRead8(Private->RootBridge.MemoryStart);
  PhysicalPorts    = (MmioRead32(Private->RootBridge.MemoryStart + 0x4)) & 0x0000000F;

  // Calculate the total size of the USB registers.
  Private->RootBridge.MemorySize = CapabilityLength + (HOST_CONTROLLER_OPERATION_REG_SIZE + ((4 * PhysicalPorts) - 1));

  // Enable Port Power bit in Port status and control registers in EHCI register space.
  // Port Power Control (PPC) bit in the HCSPARAMS register is already set which indicates
  // host controller implementation includes port power control.
  for (Count = 0; Count < PhysicalPorts; Count++) {
    MmioOr32((Private->RootBridge.MemoryStart + CapabilityLength + HOST_CONTROLLER_OPERATION_REG_SIZE + 4*Count), 0x00001000);
  }

  // Create fake PCI config space.
  Private->ConfigSpace = AllocateZeroPool(sizeof(PCI_TYPE00));
  if (Private->ConfigSpace == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    FreePool(Private);
    return Status;
  }

  // Configure PCI config space
  Private->ConfigSpace->Hdr.VendorId = 0x3530;
  Private->ConfigSpace->Hdr.DeviceId = 0x3530;
  Private->ConfigSpace->Hdr.ClassCode[0] = 0x20;
  Private->ConfigSpace->Hdr.ClassCode[1] = 0x03;
  Private->ConfigSpace->Hdr.ClassCode[2] = 0x0C;
  Private->ConfigSpace->Device.Bar[0] = Private->RootBridge.MemoryStart;

  Handle = NULL;

  // Unique device path.
  CopyMem(&Private->DevicePath, &PciIoDevicePathTemplate, sizeof(PciIoDevicePathTemplate));
  Private->DevicePath.AcpiDevicePath.UID = 0;
  
  // Copy protocol structure
  CopyMem(&Private->PciIoProtocol, &PciIoTemplate, sizeof(PciIoTemplate));

  Status = gBS->InstallMultipleProtocolInterfaces(&Handle,
                                                  &gEfiPciIoProtocolGuid,       &Private->PciIoProtocol,
                                                  &gEfiDevicePathProtocolGuid,  &Private->DevicePath,
                                                  NULL);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "PciEmulationEntryPoint InstallMultipleProtocolInterfaces() failed.\n"));
  }

  return Status;
}

