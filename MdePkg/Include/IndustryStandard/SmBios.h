/** @file
  Industry Standard Definitions of SMBIOS Table Specification v2.6.1

Copyright (c) 2006 - 2010, Intel Corporation.  All rights reserved<BR>
This program and the accompanying materials are licensed and made available under 
the terms and conditions of the BSD License that accompanies this distribution.  
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.                                          
    
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __SMBIOS_STANDARD_H__
#define __SMBIOS_STANDARD_H__

///
/// Reference SMBIOS 2.6, chapter 3.1.2.
/// For v2.1 and later, handle values in the range 0FF00h to 0FFFFh are reserved for
/// use by this specification.
///
#define SMBIOS_HANDLE_RESERVED_BEGIN 0xFF00

///
/// Reference SMBIOS 2.6, chapter 3.1.3.
/// Each text string is limited to 64 significant characters due to system MIF limitations.
///
#define SMBIOS_STRING_MAX_LENGTH     64

///
/// Inactive type is added from SMBIOS 2.2. Reference SMBIOS 2.6, chapter 3.3.43.
/// Upper-level software that interprets the SMBIOS structure-table should bypass an 
/// Inactive structure just like a structure type that the software does not recognize.
///
#define SMBIOS_TYPE_INACTIVE         0x007E    

///
/// End-of-table type is added from SMBIOS 2.2. Reference SMBIOS 2.6, chapter 3.3.44.
/// The end-of-table indicator is used in the last physical structure in a table
///
#define SMBIOS_TYPE_END_OF_TABLE     0x007F

///
/// Smbios Table Entry Point Structure.
///
#pragma pack(1)
typedef struct {
  UINT8   AnchorString[4];
  UINT8   EntryPointStructureChecksum;
  UINT8   EntryPointLength;
  UINT8   MajorVersion;
  UINT8   MinorVersion;
  UINT16  MaxStructureSize;
  UINT8   EntryPointRevision;
  UINT8   FormattedArea[5];
  UINT8   IntermediateAnchorString[5];
  UINT8   IntermediateChecksum;
  UINT16  TableLength;
  UINT32  TableAddress;
  UINT16  NumberOfSmbiosStructures;
  UINT8   SmbiosBcdRevision;
} SMBIOS_TABLE_ENTRY_POINT;

///
/// The Smbios structure header.
///
typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Handle;
} SMBIOS_STRUCTURE;

///
/// String Number for a Null terminated string, 00h stands for no string available.
///
typedef UINT8 SMBIOS_TABLE_STRING;

///
/// BIOS Characteristics  
/// Defines which functions the BIOS supports. PCI, PCMCIA, Flash, etc. 
///
typedef struct {
  UINT32  Reserved                          :2;  ///< Bits 0-1.
  UINT32  Unknown                           :1; 
  UINT32  BiosCharacteristicsNotSupported   :1; 
  UINT32  IsaIsSupported                    :1;  
  UINT32  McaIsSupported                    :1;
  UINT32  EisaIsSupported                   :1;
  UINT32  PciIsSupported                    :1;
  UINT32  PcmciaIsSupported                 :1;
  UINT32  PlugAndPlayIsSupported            :1;
  UINT32  ApmIsSupported                    :1;
  UINT32  BiosIsUpgradable                  :1;
  UINT32  BiosShadowingAllowed              :1;
  UINT32  VlVesaIsSupported                 :1;
  UINT32  EscdSupportIsAvailable            :1;
  UINT32  BootFromCdIsSupported             :1;
  UINT32  SelectableBootIsSupported         :1;
  UINT32  RomBiosIsSocketed                 :1;
  UINT32  BootFromPcmciaIsSupported         :1;
  UINT32  EDDSpecificationIsSupported       :1;
  UINT32  JapaneseNecFloppyIsSupported      :1;
  UINT32  JapaneseToshibaFloppyIsSupported  :1;
  UINT32  Floppy525_360IsSupported          :1;
  UINT32  Floppy525_12IsSupported           :1;
  UINT32  Floppy35_720IsSupported           :1;
  UINT32  Floppy35_288IsSupported           :1;
  UINT32  PrintScreenIsSupported            :1;
  UINT32  Keyboard8042IsSupported           :1;
  UINT32  SerialIsSupported                 :1;
  UINT32  PrinterIsSupported                :1;
  UINT32  CgaMonoIsSupported                :1;
  UINT32  NecPc98                           :1;
  UINT32  ReservedForVendor                 :32; ///< Bits 32-63. Bits 32-47 reserved for BIOS vendor 
                                                 ///< and bits 48-63 reserved for System Vendor. 
} MISC_BIOS_CHARACTERISTICS;

///
/// BIOS Characteristics Extension Byte 1 .
/// This information, available for SMBIOS version 2.1 and later, appears at offset 12h 
/// within the BIOS Information  structure.
///
typedef struct {
  UINT8  AcpiIsSupported                   :1;
  UINT8  UsbLegacyIsSupported              :1; 
  UINT8  AgpIsSupported                    :1; 
  UINT8  I20BootIsSupported                :1;
  UINT8  Ls120BootIsSupported              :1;
  UINT8  AtapiZipDriveBootIsSupported      :1;
  UINT8  Boot1394IsSupported               :1;
  UINT8  SmartBatteryIsSupported           :1;
} MBCE_BIOS_RESERVED;

///
/// BIOS Characteristics Extension Byte 2.
/// This information, available for SMBIOS version 2.3 and later, appears at offset 13h 
/// within the BIOS Information structure.
///
typedef struct {
  UINT8  BiosBootSpecIsSupported              :1;
  UINT8  FunctionKeyNetworkBootIsSupported    :1; 
  UINT8  TargetContentDistributionEnabled     :1; 
  UINT8  ExtensionByte2Reserved               :1;
} MBCE_SYSTEM_RESERVED;

///
/// BIOS Characteristics Extension Bytes.
///
typedef struct {
  MBCE_BIOS_RESERVED    BiosReserved;
  MBCE_SYSTEM_RESERVED  SystemReserved;
  UINT8                 Reserved;
} MISC_BIOS_CHARACTERISTICS_EXTENSION;

///
/// BIOS Information (Type 0).
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       Vendor;
  SMBIOS_TABLE_STRING       BiosVersion;
  UINT16                    BiosSegment;
  SMBIOS_TABLE_STRING       BiosReleaseDate;
  UINT8                     BiosSize;
  MISC_BIOS_CHARACTERISTICS BiosCharacteristics;
  UINT8                     BIOSCharacteristicsExtensionBytes[2];
  UINT8                     SystemBiosMajorRelease;
  UINT8                     SystemBiosMinorRelease;
  UINT8                     EmbeddedControllerFirmwareMajorRelease;
  UINT8                     EmbeddedControllerFirmwareMinorRelease;
} SMBIOS_TABLE_TYPE0;

///
///  System Wake-up Type.
///
typedef enum {  
  SystemWakeupTypeReserved         = 0x00,
  SystemWakeupTypeOther            = 0x01,
  SystemWakeupTypeUnknown          = 0x02,
  SystemWakeupTypeApmTimer         = 0x03,
  SystemWakeupTypeModemRing        = 0x04,
  SystemWakeupTypeLanRemote        = 0x05,
  SystemWakeupTypePowerSwitch      = 0x06,
  SystemWakeupTypePciPme           = 0x07,
  SystemWakeupTypeAcPowerRestored  = 0x08
} MISC_SYSTEM_WAKEUP_TYPE;

///
/// System Information (Type 1).
/// 
/// The information in this structure defines attributes of the overall system and is 
/// intended to be associated with the Component ID group of the system's MIF.
/// An SMBIOS implementation is associated with a single system instance and contains 
/// one and only one System Information (Type 1) structure.
///
typedef struct {
  SMBIOS_STRUCTURE        Hdr;
  SMBIOS_TABLE_STRING     Manufacturer;
  SMBIOS_TABLE_STRING     ProductName;
  SMBIOS_TABLE_STRING     Version;
  SMBIOS_TABLE_STRING     SerialNumber;
  GUID                    Uuid;
  UINT8                   WakeUpType;           ///< The enumeration value from MISC_SYSTEM_WAKEUP_TYPE.
  SMBIOS_TABLE_STRING     SKUNumber;
  SMBIOS_TABLE_STRING     Family;
} SMBIOS_TABLE_TYPE1;

///
///  Base Board - Feature Flags. 
///
typedef struct {
  UINT8  Motherboard           :1;
  UINT8  RequiresDaughterCard  :1;
  UINT8  Removable             :1;
  UINT8  Replaceable           :1;
  UINT8  HotSwappable          :1;
  UINT8  Reserved              :3;
} BASE_BOARD_FEATURE_FLAGS;

///
///  Base Board - Board Type.
///
typedef enum {  
  BaseBoardTypeUnknown                  = 0x1,
  BaseBoardTypeOther                    = 0x2,
  BaseBoardTypeServerBlade              = 0x3,
  BaseBoardTypeConnectivitySwitch       = 0x4,
  BaseBoardTypeSystemManagementModule   = 0x5,
  BaseBoardTypeProcessorModule          = 0x6,
  BaseBoardTypeIOModule                 = 0x7,
  BaseBoardTypeMemoryModule             = 0x8,
  BaseBoardTypeDaughterBoard            = 0x9,
  BaseBoardTypeMotherBoard              = 0xA,
  BaseBoardTypeProcessorMemoryModule    = 0xB,
  BaseBoardTypeProcessorIOModule        = 0xC,
  BaseBoardTypeInterconnectBoard        = 0xD
} BASE_BOARD_TYPE;

///
/// Base Board (or Module) Information (Type 2).
///
/// The information in this structure defines attributes of a system baseboard - 
/// for example a motherboard, planar, or server blade or other standard system module.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       Manufacturer;
  SMBIOS_TABLE_STRING       ProductName;
  SMBIOS_TABLE_STRING       Version;
  SMBIOS_TABLE_STRING       SerialNumber;
  SMBIOS_TABLE_STRING       AssetTag;
  BASE_BOARD_FEATURE_FLAGS  FeatureFlag;
  SMBIOS_TABLE_STRING       LocationInChassis;
  UINT16                    ChassisHandle;
  UINT8                     BoardType;              ///< The enumeration value from BASE_BOARD_TYPE.
  UINT8                     NumberOfContainedObjectHandles;
  UINT16                    ContainedObjectHandles[1];
} SMBIOS_TABLE_TYPE2;

///
/// System Enclosure or Chassis Types
///
typedef enum {  
  MiscChassisTypeOther                = 0x01,
  MiscChassisTypeUnknown              = 0x02,
  MiscChassisTypeDeskTop              = 0x03,
  MiscChassisTypeLowProfileDesktop    = 0x04,
  MiscChassisTypePizzaBox             = 0x05,
  MiscChassisTypeMiniTower            = 0x06,
  MiscChassisTypeTower                = 0x07,
  MiscChassisTypePortable             = 0x08,
  MiscChassisTypeLapTop               = 0x09,
  MiscChassisTypeNotebook             = 0x0A,
  MiscChassisTypeHandHeld             = 0x0B,
  MiscChassisTypeDockingStation       = 0x0C,
  MiscChassisTypeAllInOne             = 0x0D,
  MiscChassisTypeSubNotebook          = 0x0E,
  MiscChassisTypeSpaceSaving          = 0x0F,
  MiscChassisTypeLunchBox             = 0x10,
  MiscChassisTypeMainServerChassis    = 0x11,
  MiscChassisTypeExpansionChassis     = 0x12,
  MiscChassisTypeSubChassis           = 0x13,
  MiscChassisTypeBusExpansionChassis  = 0x14,
  MiscChassisTypePeripheralChassis    = 0x15,
  MiscChassisTypeRaidChassis          = 0x16,
  MiscChassisTypeRackMountChassis     = 0x17,
  MiscChassisTypeSealedCasePc         = 0x18,
  MiscChassisMultiSystemChassis       = 0x19,
  MiscChassisCompactPCI               = 0x1A,
  MiscChassisAdvancedTCA              = 0x1B,
  MiscChassisBlade                    = 0x1C,
  MiscChassisBladeEnclosure           = 0x1D
} MISC_CHASSIS_TYPE;

///
/// System Enclosure or Chassis States .
///
typedef enum {  
  ChassisStateOther           = 0x01,
  ChassisStateUnknown         = 0x02,
  ChassisStateSafe            = 0x03,
  ChassisStateWarning         = 0x04,
  ChassisStateCritical        = 0x05,
  ChassisStateNonRecoverable  = 0x06
} MISC_CHASSIS_STATE;

///
/// System Enclosure or Chassis Security Status.
///
typedef enum {  
  ChassisSecurityStatusOther                          = 0x01,
  ChassisSecurityStatusUnknown                        = 0x02,
  ChassisSecurityStatusNone                           = 0x03,
  ChassisSecurityStatusExternalInterfaceLockedOut     = 0x04,
  ChassisSecurityStatusExternalInterfaceLockedEnabled = 0x05
} MISC_CHASSIS_SECURITY_STATE;

///
/// Contained Element record
///
typedef struct {
  UINT8                 ContainedElementType;
  UINT8                 ContainedElementMinimum;
  UINT8                 ContainedElementMaximum;
} CONTAINED_ELEMENT;


///
/// System Enclosure or Chassis (Type 3).
///
/// The information in this structure defines attributes of the system's mechanical enclosure(s).  
/// For example, if a system included a separate enclosure for its peripheral devices, 
/// two structures would be returned: one for the main, system enclosure and the second for
/// the peripheral device enclosure.  The additions to this structure in v2.1 of this specification
/// support the population of the CIM_Chassis class. 
///
typedef struct {
  SMBIOS_STRUCTURE            Hdr;
  SMBIOS_TABLE_STRING         Manufacturer;
  UINT8                       Type;
  SMBIOS_TABLE_STRING         Version;
  SMBIOS_TABLE_STRING         SerialNumber;
  SMBIOS_TABLE_STRING         AssetTag;
  UINT8                       BootupState;            ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       PowerSupplyState;       ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       ThermalState;           ///< The enumeration value from MISC_CHASSIS_STATE.
  UINT8                       SecurityStatus;         ///< The enumeration value from MISC_CHASSIS_SECURITY_STATE.
  UINT8                       OemDefined[4];
  UINT8                       Height;
  UINT8                       NumberofPowerCords;
  UINT8                       ContainedElementCount;
  UINT8                       ContainedElementRecordLength;
  CONTAINED_ELEMENT           ContainedElements[1];
} SMBIOS_TABLE_TYPE3;

///
/// Processor Information - Processor Type.
///
typedef enum {
  ProcessorOther   = 0x01,
  ProcessorUnknown = 0x02,
  CentralProcessor = 0x03,
  MathProcessor    = 0x04,
  DspProcessor     = 0x05,
  VideoProcessor   = 0x06
} PROCESSOR_TYPE_DATA;

///
/// Processor Information - Processor Family.
///
typedef enum {
  ProcessorFamilyOther                  = 0x01, 
  ProcessorFamilyUnknown                = 0x02,
  ProcessorFamily8086                   = 0x03, 
  ProcessorFamily80286                  = 0x04,
  ProcessorFamilyIntel386               = 0x05, 
  ProcessorFamilyIntel486               = 0x06,
  ProcessorFamily8087                   = 0x07,
  ProcessorFamily80287                  = 0x08,
  ProcessorFamily80387                  = 0x09, 
  ProcessorFamily80487                  = 0x0A,
  ProcessorFamilyPentium                = 0x0B, 
  ProcessorFamilyPentiumPro             = 0x0C,
  ProcessorFamilyPentiumII              = 0x0D,
  ProcessorFamilyPentiumMMX             = 0x0E,
  ProcessorFamilyCeleron                = 0x0F,
  ProcessorFamilyPentiumIIXeon          = 0x10,
  ProcessorFamilyPentiumIII             = 0x11, 
  ProcessorFamilyM1                     = 0x12,
  ProcessorFamilyM2                     = 0x13,
  ProcessorFamilyM1Reserved2            = 0x14,
  ProcessorFamilyM1Reserved3            = 0x15,
  ProcessorFamilyM1Reserved4            = 0x16,
  ProcessorFamilyM1Reserved5            = 0x17,
  ProcessorFamilyAmdDuron               = 0x18,
  ProcessorFamilyK5                     = 0x19, 
  ProcessorFamilyK6                     = 0x1A,
  ProcessorFamilyK6_2                   = 0x1B,
  ProcessorFamilyK6_3                   = 0x1C,
  ProcessorFamilyAmdAthlon              = 0x1D,
  ProcessorFamilyAmd29000               = 0x1E,
  ProcessorFamilyK6_2Plus               = 0x1F,
  ProcessorFamilyPowerPC                = 0x20,
  ProcessorFamilyPowerPC601             = 0x21,
  ProcessorFamilyPowerPC603             = 0x22,
  ProcessorFamilyPowerPC603Plus         = 0x23,
  ProcessorFamilyPowerPC604             = 0x24,
  ProcessorFamilyPowerPC620             = 0x25,
  ProcessorFamilyPowerPCx704            = 0x26,
  ProcessorFamilyPowerPC750             = 0x27,
  ProcessorFamilyIntelCoreDuo           = 0x28,
  ProcessorFamilyIntelCoreDuoMobile     = 0x29,
  ProcessorFamilyIntelCoreSoloMobile    = 0x2A,
  ProcessorFamilyIntelAtom              = 0x2B,
  ProcessorFamilyAlpha3                 = 0x30,
  ProcessorFamilyAlpha21064             = 0x31,
  ProcessorFamilyAlpha21066             = 0x32,
  ProcessorFamilyAlpha21164             = 0x33,
  ProcessorFamilyAlpha21164PC           = 0x34,
  ProcessorFamilyAlpha21164a            = 0x35,
  ProcessorFamilyAlpha21264             = 0x36,
  ProcessorFamilyAlpha21364             = 0x37,
  ProcessorFamilyMips                   = 0x40,
  ProcessorFamilyMIPSR4000              = 0x41,
  ProcessorFamilyMIPSR4200              = 0x42,
  ProcessorFamilyMIPSR4400              = 0x43,
  ProcessorFamilyMIPSR4600              = 0x44,
  ProcessorFamilyMIPSR10000             = 0x45,
  ProcessorFamilySparc                  = 0x50,
  ProcessorFamilySuperSparc             = 0x51,
  ProcessorFamilymicroSparcII           = 0x52,
  ProcessorFamilymicroSparcIIep         = 0x53,
  ProcessorFamilyUltraSparc             = 0x54,
  ProcessorFamilyUltraSparcII           = 0x55,
  ProcessorFamilyUltraSparcIIi          = 0x56,
  ProcessorFamilyUltraSparcIII          = 0x57,
  ProcessorFamilyUltraSparcIIIi         = 0x58,
  ProcessorFamily68040                  = 0x60,
  ProcessorFamily68xxx                  = 0x61,
  ProcessorFamily68000                  = 0x62,
  ProcessorFamily68010                  = 0x63,
  ProcessorFamily68020                  = 0x64,
  ProcessorFamily68030                  = 0x65,
  ProcessorFamilyHobbit                 = 0x70,
  ProcessorFamilyCrusoeTM5000           = 0x78,
  ProcessorFamilyCrusoeTM3000           = 0x79,
  ProcessorFamilyEfficeonTM8000         = 0x7A,
  ProcessorFamilyWeitek                 = 0x80,
  ProcessorFamilyItanium                = 0x82,
  ProcessorFamilyAmdAthlon64            = 0x83,
  ProcessorFamilyAmdOpteron             = 0x84,
  ProcessorFamilyAmdSempron             = 0x85,
  ProcessorFamilyAmdTurion64Mobile      = 0x86,
  ProcessorFamilyDualCoreAmdOpteron     = 0x87,
  ProcessorFamilyAmdAthlon64X2DualCore  = 0x88,
  ProcessorFamilyAmdTurion64X2Mobile    = 0x89,
  ProcessorFamilyQuadCoreAmdOpteron     = 0x8A,
  ProcessorFamilyThirdGenerationAmdOpteron = 0x8B,
  ProcessorFamilyAmdPhenomFxQuadCore    = 0x8C,
  ProcessorFamilyAmdPhenomX4QuadCore    = 0x8D,
  ProcessorFamilyAmdPhenomX2DualCore    = 0x8E,
  ProcessorFamilyAmdAthlonX2DualCore    = 0x8F,  
  ProcessorFamilyPARISC                 = 0x90,
  ProcessorFamilyPaRisc8500             = 0x91,
  ProcessorFamilyPaRisc8000             = 0x92,
  ProcessorFamilyPaRisc7300LC           = 0x93,
  ProcessorFamilyPaRisc7200             = 0x94,
  ProcessorFamilyPaRisc7100LC           = 0x95,
  ProcessorFamilyPaRisc7100             = 0x96,
  ProcessorFamilyV30                    = 0xA0,
  ProcessorFamilyQuadCoreIntelXeon3200Series  = 0xA1,
  ProcessorFamilyDualCoreIntelXeon3000Series  = 0xA2,
  ProcessorFamilyQuadCoreIntelXeon5300Series  = 0xA3,
  ProcessorFamilyDualCoreIntelXeon5100Series  = 0xA4,
  ProcessorFamilyDualCoreIntelXeon5000Series  = 0xA5,
  ProcessorFamilyDualCoreIntelXeonLV          = 0xA6,
  ProcessorFamilyDualCoreIntelXeonULV         = 0xA7,
  ProcessorFamilyDualCoreIntelXeon7100Series  = 0xA8,
  ProcessorFamilyQuadCoreIntelXeon5400Series  = 0xA9,
  ProcessorFamilyQuadCoreIntelXeon            = 0xAA,
  ProcessorFamilyDualCoreIntelXeon5200Series  = 0xAB,
  ProcessorFamilyDualCoreIntelXeon7200Series  = 0xAC,
  ProcessorFamilyQuadCoreIntelXeon7300Series  = 0xAD,
  ProcessorFamilyQuadCoreIntelXeon7400Series  = 0xAE,
  ProcessorFamilyMultiCoreIntelXeon7400Series = 0xAF,
  ProcessorFamilyPentiumIIIXeon         = 0xB0,
  ProcessorFamilyPentiumIIISpeedStep    = 0xB1,
  ProcessorFamilyPentium4               = 0xB2,
  ProcessorFamilyIntelXeon              = 0xB3,
  ProcessorFamilyAS400                  = 0xB4,
  ProcessorFamilyIntelXeonMP            = 0xB5,
  ProcessorFamilyAMDAthlonXP            = 0xB6,
  ProcessorFamilyAMDAthlonMP            = 0xB7,
  ProcessorFamilyIntelItanium2          = 0xB8,
  ProcessorFamilyIntelPentiumM          = 0xB9,
  ProcessorFamilyIntelCeleronD          = 0xBA,
  ProcessorFamilyIntelPentiumD          = 0xBB,
  ProcessorFamilyIntelPentiumEx         = 0xBC,
  ProcessorFamilyIntelCoreSolo          = 0xBD,  ///< SMBIOS spec 2.6 correct this value
  ProcessorFamilyReserved               = 0xBE,
  ProcessorFamilyIntelCore2             = 0xBF,
  ProcessorFamilyIntelCore2Solo         = 0xC0,
  ProcessorFamilyIntelCore2Extreme      = 0xC1,
  ProcessorFamilyIntelCore2Quad         = 0xC2,
  ProcessorFamilyIntelCore2ExtremeMobile = 0xC3,
  ProcessorFamilyIntelCore2DuoMobile    = 0xC4,
  ProcessorFamilyIntelCore2SoloMobile   = 0xC5,
  ProcessorFamilyIntelCoreI7            = 0xC6,
  ProcessorFamilyDualCoreIntelCeleron   = 0xC7,  
  ProcessorFamilyIBM390                 = 0xC8,
  ProcessorFamilyG4                     = 0xC9,
  ProcessorFamilyG5                     = 0xCA,
  ProcessorFamilyG6                     = 0xCB,
  ProcessorFamilyzArchitectur           = 0xCC,
  ProcessorFamilyViaC7M                 = 0xD2,
  ProcessorFamilyViaC7D                 = 0xD3,
  ProcessorFamilyViaC7                  = 0xD4,
  ProcessorFamilyViaEden                = 0xD5,
  ProcessorFamilyMultiCoreIntelXeon           = 0xD6,
  ProcessorFamilyDualCoreIntelXeon3Series     = 0xD7,
  ProcessorFamilyQuadCoreIntelXeon3Series     = 0xD8,
  ProcessorFamilyDualCoreIntelXeon5Series     = 0xDA,
  ProcessorFamilyQuadCoreIntelXeon5Series     = 0xDB,
  ProcessorFamilyDualCoreIntelXeon7Series     = 0xDD,
  ProcessorFamilyQuadCoreIntelXeon7Series     = 0xDE,
  ProcessorFamilyMultiCoreIntelXeon7Series    = 0xDF,
  ProcessorFamilyEmbeddedAmdOpteronQuadCore   = 0xE6,
  ProcessorFamilyAmdPhenomTripleCore          = 0xE7,
  ProcessorFamilyAmdTurionUltraDualCoreMobile = 0xE8,
  ProcessorFamilyAmdTurionDualCoreMobile      = 0xE9,
  ProcessorFamilyAmdAthlonDualCore            = 0xEA,
  ProcessorFamilyAmdSempronSI                 = 0xEB,
  ProcessorFamilyi860                   = 0xFA,
  ProcessorFamilyi960                   = 0xFB,
  ProcessorFamilyIndicatorFamily2       = 0xFE,
  ProcessorFamilyReserved1              = 0xFF
} PROCESSOR_FAMILY_DATA;

///
/// Processor Information - Voltage. 
///
typedef struct {
  UINT8  ProcessorVoltageCapability5V        :1; 
  UINT8  ProcessorVoltageCapability3_3V      :1;  
  UINT8  ProcessorVoltageCapability2_9V      :1;  
  UINT8  ProcessorVoltageCapabilityReserved  :1; ///< Bit 3, must be zero.
  UINT8  ProcessorVoltageReserved            :3; ///< Bits 4-6, must be zero.
  UINT8  ProcessorVoltageIndicateLegacy      :1;
} PROCESSOR_VOLTAGE;

///
/// Processor Information - Processor Upgrade.
///
typedef enum {
  ProcessorUpgradeOther         = 0x01,
  ProcessorUpgradeUnknown       = 0x02,
  ProcessorUpgradeDaughterBoard = 0x03,
  ProcessorUpgradeZIFSocket     = 0x04,
  ProcessorUpgradePiggyBack     = 0x05, ///< Replaceable.
  ProcessorUpgradeNone          = 0x06,
  ProcessorUpgradeLIFSocket     = 0x07,
  ProcessorUpgradeSlot1         = 0x08,
  ProcessorUpgradeSlot2         = 0x09,
  ProcessorUpgrade370PinSocket  = 0x0A,
  ProcessorUpgradeSlotA         = 0x0B,
  ProcessorUpgradeSlotM         = 0x0C,
  ProcessorUpgradeSocket423     = 0x0D,
  ProcessorUpgradeSocketA       = 0x0E, ///< Socket 462.
  ProcessorUpgradeSocket478     = 0x0F,
  ProcessorUpgradeSocket754     = 0x10,
  ProcessorUpgradeSocket940     = 0x11,
  ProcessorUpgradeSocket939     = 0x12,
  ProcessorUpgradeSocketmPGA604 = 0x13,
  ProcessorUpgradeSocketLGA771  = 0x14,
  ProcessorUpgradeSocketLGA775  = 0x15,
  ProcessorUpgradeSocketS1      = 0x16,
  ProcessorUpgradeAM2           = 0x17,
  ProcessorUpgradeF1207         = 0x18,
  ProcessorSocketLGA1366        = 0x19
} PROCESSOR_UPGRADE;

///
/// Processor ID Field Description
///
typedef struct {
  UINT32  ProcessorSteppingId:4;
  UINT32  ProcessorModel:     4;
  UINT32  ProcessorFamily:    4;
  UINT32  ProcessorType:      2;
  UINT32  ProcessorReserved1: 2;
  UINT32  ProcessorXModel:    4;
  UINT32  ProcessorXFamily:   8;
  UINT32  ProcessorReserved2: 4;
} PROCESSOR_SIGNATURE;

typedef struct {
  UINT32  ProcessorFpu       :1;
  UINT32  ProcessorVme       :1;
  UINT32  ProcessorDe        :1;
  UINT32  ProcessorPse       :1;
  UINT32  ProcessorTsc       :1;
  UINT32  ProcessorMsr       :1;
  UINT32  ProcessorPae       :1;
  UINT32  ProcessorMce       :1;
  UINT32  ProcessorCx8       :1;
  UINT32  ProcessorApic      :1;
  UINT32  ProcessorReserved1 :1;
  UINT32  ProcessorSep       :1;
  UINT32  ProcessorMtrr      :1;
  UINT32  ProcessorPge       :1;
  UINT32  ProcessorMca       :1;
  UINT32  ProcessorCmov      :1;
  UINT32  ProcessorPat       :1;
  UINT32  ProcessorPse36     :1;
  UINT32  ProcessorPsn       :1;
  UINT32  ProcessorClfsh     :1;
  UINT32  ProcessorReserved2 :1;
  UINT32  ProcessorDs        :1;
  UINT32  ProcessorAcpi      :1;
  UINT32  ProcessorMmx       :1;
  UINT32  ProcessorFxsr      :1;
  UINT32  ProcessorSse       :1;
  UINT32  ProcessorSse2      :1;
  UINT32  ProcessorSs        :1;
  UINT32  ProcessorReserved3 :1;
  UINT32  ProcessorTm        :1;
  UINT32  ProcessorReserved4 :2;
} PROCESSOR_FEATURE_FLAGS;

typedef struct {
  PROCESSOR_SIGNATURE     Signature;
  PROCESSOR_FEATURE_FLAGS FeatureFlags;
} PROCESSOR_ID_DATA;

///
/// Processor Information (Type 4).
///
/// The information in this structure defines the attributes of a single processor; 
/// a separate structure instance is provided for each system processor socket/slot. 
/// For example, a system with an IntelDX2 processor would have a single 
/// structure instance, while a system with an IntelSX2 processor would have a structure
/// to describe the main CPU, and a second structure to describe the 80487 co-processor. 
///
typedef struct { 
  SMBIOS_STRUCTURE      Hdr;
  SMBIOS_TABLE_STRING   Socket;
  UINT8                 ProcessorType;          ///< The enumeration value from PROCESSOR_TYPE_DATA.
  UINT8                 ProcessorFamily;        ///< The enumeration value from PROCESSOR_FAMILY_DATA.
  SMBIOS_TABLE_STRING   ProcessorManufacture;
  PROCESSOR_ID_DATA     ProcessorId;
  SMBIOS_TABLE_STRING   ProcessorVersion;
  PROCESSOR_VOLTAGE     Voltage;
  UINT16                ExternalClock;
  UINT16                MaxSpeed;
  UINT16                CurrentSpeed;
  UINT8                 Status;
  UINT8                 ProcessorUpgrade;      ///< The enumeration value from PROCESSOR_UPGRADE.
  UINT16                L1CacheHandle;
  UINT16                L2CacheHandle;
  UINT16                L3CacheHandle;
  SMBIOS_TABLE_STRING   SerialNumber;
  SMBIOS_TABLE_STRING   AssetTag;
  SMBIOS_TABLE_STRING   PartNumber;
  //
  // Add for smbios 2.5
  //
  UINT8                 CoreCount;
  UINT8                 EnabledCoreCount;
  UINT8                 ThreadCount;
  UINT16                ProcessorCharacteristics;
  //
  // Add for smbios 2.6
  //
  UINT16                ProcessorFamily2;
} SMBIOS_TABLE_TYPE4;

///
/// Memory Controller Error Detecting Method.
///
typedef enum {  
  ErrorDetectingMethodOther   = 0x01,
  ErrorDetectingMethodUnknown = 0x02,
  ErrorDetectingMethodNone    = 0x03,
  ErrorDetectingMethodParity  = 0x04,
  ErrorDetectingMethod32Ecc   = 0x05,
  ErrorDetectingMethod64Ecc   = 0x06,
  ErrorDetectingMethod128Ecc  = 0x07,
  ErrorDetectingMethodCrc     = 0x08
} MEMORY_ERROR_DETECT_METHOD;

///
/// Memory Controller Error Correcting Capability.
///
typedef struct {
  UINT8  Other                 :1;
  UINT8  Unknown               :1;
  UINT8  None                  :1;
  UINT8  SingleBitErrorCorrect :1;
  UINT8  DoubleBitErrorCorrect :1;
  UINT8  ErrorScrubbing        :1;
  UINT8  Reserved              :2;
} MEMORY_ERROR_CORRECT_CAPABILITY;

///
/// Memory Controller Information - Interleave Support.
///
typedef enum {  
  MemoryInterleaveOther      = 0x01,
  MemoryInterleaveUnknown    = 0x02,
  MemoryInterleaveOneWay     = 0x03,
  MemoryInterleaveTwoWay     = 0x04,
  MemoryInterleaveFourWay    = 0x05,
  MemoryInterleaveEightWay   = 0x06,
  MemoryInterleaveSixteenWay = 0x07
} MEMORY_SUPPORT_INTERLEAVE_TYPE;

///
/// Memory Controller Information - Memory Speeds.
///
typedef struct {
  UINT16     Other    :1;
  UINT16     Unknown  :1;
  UINT16     SeventyNs:1;
  UINT16     SixtyNs  :1;
  UINT16     FiftyNs  :1;
  UINT16     Reserved :11;
} MEMORY_SPEED_TYPE;

///
/// Memory Controller Information (Type 5, Obsolete).
///
/// The information in this structure defines the attributes of the system's memory controller(s) 
/// and the supported attributes of any memory-modules present in the sockets controlled by 
/// this controller. 
/// Note: This structure, and its companion Memory Module Information (Type 6, Obsolete), 
/// are obsolete starting with version 2.1 of this specification. The Physical Memory Array (Type 16)
/// and Memory Device (Type 17) structures should be used instead.  BIOS providers might
/// choose to implement both memory description types to allow existing DMI browsers
/// to properly display the system's memory attributes.
///
typedef struct {
  SMBIOS_STRUCTURE                Hdr;
  UINT8                           ErrDetectMethod;            ///< The enumeration value from MEMORY_ERROR_DETECT_METHOD.
  MEMORY_ERROR_CORRECT_CAPABILITY ErrCorrectCapability;
  UINT8                           SupportInterleave;          ///< The enumeration value from MEMORY_SUPPORT_INTERLEAVE_TYPE.
  UINT8                           CurrentInterleave;          ///< The enumeration value from MEMORY_SUPPORT_INTERLEAVE_TYPE .     
  UINT8                           MaxMemoryModuleSize;
  MEMORY_SPEED_TYPE               SupportSpeed;
  UINT16                          SupportMemoryType;
  UINT8                           MemoryModuleVoltage;
  UINT8                           AssociatedMemorySlotNum;
  UINT16                          MemoryModuleConfigHandles[1];
} SMBIOS_TABLE_TYPE5;

///
/// Memory Module Information - Memory Types
///
typedef struct {
  UINT16  Other       :1;
  UINT16  Unknown     :1;
  UINT16  Standard    :1;
  UINT16  FastPageMode:1;
  UINT16  Edo         :1;
  UINT16  Parity      :1;
  UINT16  Ecc         :1;
  UINT16  Simm        :1;
  UINT16  Dimm        :1;
  UINT16  BurstEdo    :1;
  UINT16  Sdram       :1;
  UINT16  Reserved    :5;
} MEMORY_CURRENT_TYPE;

///
/// Memory Module Information - Memory Size.
///
typedef struct {
  UINT8   InstalledOrEnabledSize  :7; ///< Size (n), where 2**n is the size in MB.
  UINT8   SingleOrDoubleBank      :1;
} MEMORY_INSTALLED_ENABLED_SIZE;

///
/// Memory Module Information (Type 6, Obsolete)
///
/// One Memory Module Information structure is included for each memory-module socket 
/// in the system.  The structure describes the speed, type, size, and error status
/// of each system memory module.  The supported attributes of each module are described 
/// by the "owning" Memory Controller Information structure.  
/// Note:  This structure, and its companion Memory Controller Information (Type 5, Obsolete), 
/// are obsolete starting with version 2.1 of this specification. The Physical Memory Array (Type 16)
/// and Memory Device (Type 17) structures should be used instead.
///
typedef struct {
  SMBIOS_STRUCTURE              Hdr;
  SMBIOS_TABLE_STRING           SocketDesignation;
  UINT8                         BankConnections;
  UINT8                         CurrentSpeed;
  MEMORY_CURRENT_TYPE           CurrentMemoryType;
  MEMORY_INSTALLED_ENABLED_SIZE InstalledSize;
  MEMORY_INSTALLED_ENABLED_SIZE EnabledSize;
  UINT8                         ErrorStatus;
} SMBIOS_TABLE_TYPE6;

///
/// Cache Information - SRAM Type.
///
typedef struct {
  UINT16  Other         :1;
  UINT16  Unknown       :1;
  UINT16  NonBurst      :1;
  UINT16  Burst         :1;
  UINT16  PipelineBurst :1;
  UINT16  Asynchronous  :1;
  UINT16  Synchronous   :1;
  UINT16  Reserved      :9;
} CACHE_SRAM_TYPE_DATA;

///
/// Cache Information - Error Correction Type.
///
typedef enum {
  CacheErrorOther     = 0x01,
  CacheErrorUnknown   = 0x02,
  CacheErrorNone      = 0x03,
  CacheErrorParity    = 0x04,
  CacheErrorSingleBit = 0x05, ///< ECC
  CacheErrorMultiBit  = 0x06  ///< ECC
} CACHE_ERROR_TYPE_DATA;

///
/// Cache Information - System Cache Type. 
///
typedef enum {
  CacheTypeOther       = 0x01,
  CacheTypeUnknown     = 0x02,
  CacheTypeInstruction = 0x03,
  CacheTypeData        = 0x04,
  CacheTypeUnified     = 0x05
} CACHE_TYPE_DATA;

///
/// Cache Information - Associativity. 
///
typedef enum {
  CacheAssociativityOther        = 0x01,
  CacheAssociativityUnknown      = 0x02,
  CacheAssociativityDirectMapped = 0x03,
  CacheAssociativity2Way         = 0x04,
  CacheAssociativity4Way         = 0x05,
  CacheAssociativityFully        = 0x06,
  CacheAssociativity8Way         = 0x07,
  CacheAssociativity16Way        = 0x08,
  CacheAssociativity12Way        = 0x09,
  CacheAssociativity24Way        = 0x0A,
  CacheAssociativity32Way        = 0x0B,
  CacheAssociativity48Way        = 0x0C,
  CacheAssociativity64Way        = 0x0D
} CACHE_ASSOCIATIVITY_DATA;

///
/// Cache Information (Type 7).
///
/// The information in this structure defines the attributes of CPU cache device in the system. 
/// One structure is specified for each such device, whether the device is internal to
/// or external to the CPU module.  Cache modules can be associated with a processor structure
/// in one or two ways, depending on the SMBIOS version.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       SocketDesignation;
  UINT16                    CacheConfiguration;
  UINT16                    MaximumCacheSize;
  UINT16                    InstalledSize;
  CACHE_SRAM_TYPE_DATA      SupportedSRAMType;
  CACHE_SRAM_TYPE_DATA      CurrentSRAMType;
  UINT8                     CacheSpeed;
  UINT8                     ErrorCorrectionType;            ///< The enumeration value from CACHE_ERROR_TYPE_DATA.
  UINT8                     SystemCacheType;                ///< The enumeration value from CACHE_TYPE_DATA.
  UINT8                     Associativity;                  ///< The enumeration value from CACHE_ASSOCIATIVITY_DATA.
} SMBIOS_TABLE_TYPE7;

///
/// Port Connector Information - Connector Types. 
///
typedef enum {
  PortConnectorTypeNone                   = 0x00,
  PortConnectorTypeCentronics             = 0x01,
  PortConnectorTypeMiniCentronics         = 0x02,
  PortConnectorTypeProprietary            = 0x03,
  PortConnectorTypeDB25Male               = 0x04,
  PortConnectorTypeDB25Female             = 0x05,
  PortConnectorTypeDB15Male               = 0x06,
  PortConnectorTypeDB15Female             = 0x07,
  PortConnectorTypeDB9Male                = 0x08,
  PortConnectorTypeDB9Female              = 0x09,
  PortConnectorTypeRJ11                   = 0x0A,
  PortConnectorTypeRJ45                   = 0x0B,
  PortConnectorType50PinMiniScsi          = 0x0C,
  PortConnectorTypeMiniDin                = 0x0D,
  PortConnectorTypeMicriDin               = 0x0E,
  PortConnectorTypePS2                    = 0x0F,
  PortConnectorTypeInfrared               = 0x10,
  PortConnectorTypeHpHil                  = 0x11,
  PortConnectorTypeUsb                    = 0x12,
  PortConnectorTypeSsaScsi                = 0x13,
  PortConnectorTypeCircularDin8Male       = 0x14,
  PortConnectorTypeCircularDin8Female     = 0x15,
  PortConnectorTypeOnboardIde             = 0x16,
  PortConnectorTypeOnboardFloppy          = 0x17,
  PortConnectorType9PinDualInline         = 0x18,
  PortConnectorType25PinDualInline        = 0x19,
  PortConnectorType50PinDualInline        = 0x1A,
  PortConnectorType68PinDualInline        = 0x1B,
  PortConnectorTypeOnboardSoundInput      = 0x1C,
  PortConnectorTypeMiniCentronicsType14   = 0x1D,
  PortConnectorTypeMiniCentronicsType26   = 0x1E,
  PortConnectorTypeHeadPhoneMiniJack      = 0x1F,
  PortConnectorTypeBNC                    = 0x20,
  PortConnectorType1394                   = 0x21,
  PortConnectorTypePC98                   = 0xA0,
  PortConnectorTypePC98Hireso             = 0xA1,
  PortConnectorTypePCH98                  = 0xA2,
  PortConnectorTypePC98Note               = 0xA3,
  PortConnectorTypePC98Full               = 0xA4,
  PortConnectorTypeOther                  = 0xFF
} MISC_PORT_CONNECTOR_TYPE;

///
/// Port Connector Information - Port Types 
///
typedef enum {
  PortTypeNone                      = 0x00,
  PortTypeParallelXtAtCompatible    = 0x01,
  PortTypeParallelPortPs2           = 0x02,
  PortTypeParallelPortEcp           = 0x03,
  PortTypeParallelPortEpp           = 0x04,
  PortTypeParallelPortEcpEpp        = 0x05,
  PortTypeSerialXtAtCompatible      = 0x06,
  PortTypeSerial16450Compatible     = 0x07,
  PortTypeSerial16550Compatible     = 0x08,
  PortTypeSerial16550ACompatible    = 0x09,
  PortTypeScsi                      = 0x0A,
  PortTypeMidi                      = 0x0B,
  PortTypeJoyStick                  = 0x0C,
  PortTypeKeyboard                  = 0x0D,
  PortTypeMouse                     = 0x0E,
  PortTypeSsaScsi                   = 0x0F,
  PortTypeUsb                       = 0x10,
  PortTypeFireWire                  = 0x11,
  PortTypePcmciaTypeI               = 0x12,
  PortTypePcmciaTypeII              = 0x13,
  PortTypePcmciaTypeIII             = 0x14,
  PortTypeCardBus                   = 0x15,
  PortTypeAccessBusPort             = 0x16,
  PortTypeScsiII                    = 0x17,
  PortTypeScsiWide                  = 0x18,
  PortTypePC98                      = 0x19,
  PortTypePC98Hireso                = 0x1A,
  PortTypePCH98                     = 0x1B,
  PortTypeVideoPort                 = 0x1C,
  PortTypeAudioPort                 = 0x1D,
  PortTypeModemPort                 = 0x1E,
  PortTypeNetworkPort               = 0x1F,
  PortType8251Compatible            = 0xA0,
  PortType8251FifoCompatible        = 0xA1,
  PortTypeOther                     = 0xFF
} MISC_PORT_TYPE;

///
/// Port Connector Information (Type 8).
///
/// The information in this structure defines the attributes of a system port connector, 
/// e.g. parallel, serial, keyboard, or mouse ports.  The port's type and connector information 
/// are provided. One structure is present for each port provided by the system.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  SMBIOS_TABLE_STRING       InternalReferenceDesignator;
  UINT8                     InternalConnectorType;          ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  SMBIOS_TABLE_STRING       ExternalReferenceDesignator;
  UINT8                     ExternalConnectorType;          ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  UINT8                     PortType;                       ///< The enumeration value from MISC_PORT_TYPE.
} SMBIOS_TABLE_TYPE8;

///
/// System Slots - Slot Type
///
typedef enum {
  SlotTypeOther                        = 0x01,
  SlotTypeUnknown                      = 0x02,
  SlotTypeIsa                          = 0x03,
  SlotTypeMca                          = 0x04,
  SlotTypeEisa                         = 0x05,
  SlotTypePci                          = 0x06,
  SlotTypePcmcia                       = 0x07,
  SlotTypeVlVesa                       = 0x08,
  SlotTypeProprietary                  = 0x09,
  SlotTypeProcessorCardSlot            = 0x0A,
  SlotTypeProprietaryMemoryCardSlot    = 0x0B,
  SlotTypeIORiserCardSlot              = 0x0C,
  SlotTypeNuBus                        = 0x0D,
  SlotTypePci66MhzCapable              = 0x0E,
  SlotTypeAgp                          = 0x0F,
  SlotTypeApg2X                        = 0x10,
  SlotTypeAgp4X                        = 0x11,
  SlotTypePciX                         = 0x12,
  SlotTypeAgp4x                        = 0x13,
  SlotTypePC98C20                      = 0xA0,
  SlotTypePC98C24                      = 0xA1,
  SlotTypePC98E                        = 0xA2,
  SlotTypePC98LocalBus                 = 0xA3,
  SlotTypePC98Card                     = 0xA4,
  SlotTypePciExpress                   = 0xA5,
  SlotTypePciExpressX1                 = 0xA6,
  SlotTypePciExpressX2                 = 0xA7,
  SlotTypePciExpressX4                 = 0xA8,
  SlotTypePciExpressX8                 = 0xA9,
  SlotTypePciExpressX16                = 0xAA,
  SlotTypePciExpressGen2               = 0xAB,
  SlotTypePciExpressGen2X1             = 0xAC,
  SlotTypePciExpressGen2X2             = 0xAD,
  SlotTypePciExpressGen2X4             = 0xAE,
  SlotTypePciExpressGen2X8             = 0xAF,
  SlotTypePciExpressGen2X16            = 0xB0
} MISC_SLOT_TYPE;

///
/// System Slots - Slot Data Bus Width.
///
typedef enum {
  SlotDataBusWidthOther      = 0x01,
  SlotDataBusWidthUnknown    = 0x02,
  SlotDataBusWidth8Bit       = 0x03,
  SlotDataBusWidth16Bit      = 0x04,
  SlotDataBusWidth32Bit      = 0x05,
  SlotDataBusWidth64Bit      = 0x06,
  SlotDataBusWidth128Bit     = 0x07,
  SlotDataBusWidth1X         = 0x08, ///< Or X1
  SlotDataBusWidth2X         = 0x09, ///< Or X2
  SlotDataBusWidth4X         = 0x0A, ///< Or X4
  SlotDataBusWidth8X         = 0x0B, ///< Or X8
  SlotDataBusWidth12X        = 0x0C, ///< Or X12
  SlotDataBusWidth16X        = 0x0D, ///< Or X16
  SlotDataBusWidth32X        = 0x0E  ///< Or X32
} MISC_SLOT_DATA_BUS_WIDTH;

///
/// System Slots - Current Usage.
///
typedef enum {
  SlotUsageOther     = 0x01,
  SlotUsageUnknown   = 0x02,
  SlotUsageAvailable = 0x03,
  SlotUsageInUse     = 0x04
} MISC_SLOT_USAGE;

///
/// System Slots - Slot Length. 
///
typedef enum {
  SlotLengthOther   = 0x01,
  SlotLengthUnknown = 0x02,
  SlotLengthShort   = 0x03,
  SlotLengthLong    = 0x04
} MISC_SLOT_LENGTH;

///
/// System Slots - Slot Characteristics 1. 
///
typedef struct {
  UINT8  CharacteristicsUnknown  :1;
  UINT8  Provides50Volts         :1;
  UINT8  Provides33Volts         :1;
  UINT8  SharedSlot              :1;
  UINT8  PcCard16Supported       :1;
  UINT8  CardBusSupported        :1;
  UINT8  ZoomVideoSupported      :1;
  UINT8  ModemRingResumeSupported:1;
} MISC_SLOT_CHARACTERISTICS1;
///
/// System Slots - Slot Characteristics 2.  
///
typedef struct {
  UINT8  PmeSignalSupported      :1;
  UINT8  HotPlugDevicesSupported :1;
  UINT8  SmbusSignalSupported    :1;
  UINT8  Reserved                :5;  ///< Set to 0.
} MISC_SLOT_CHARACTERISTICS2;

///
/// System Slots (Type 9)
///
/// The information in this structure defines the attributes of a system slot. 
/// One structure is provided for each slot in the system.
///
///
typedef struct {
  SMBIOS_STRUCTURE            Hdr;
  SMBIOS_TABLE_STRING         SlotDesignation;
  UINT8                       SlotType;                 ///< The enumeration value from MISC_SLOT_TYPE.
  UINT8                       SlotDataBusWidth;         ///< The enumeration value from MISC_SLOT_DATA_BUS_WIDTH.
  UINT8                       CurrentUsage;             ///< The enumeration value from MISC_SLOT_USAGE.
  UINT8                       SlotLength;               ///< The enumeration value from MISC_SLOT_LENGTH.
  UINT16                      SlotID;
  MISC_SLOT_CHARACTERISTICS1  SlotCharacteristics1;
  MISC_SLOT_CHARACTERISTICS2  SlotCharacteristics2;
  //
  // Add for smbios 2.6
  //
  UINT16                      SegmentGroupNum;
  UINT8                       BusNum;
  UINT8                       DevFuncNum;
} SMBIOS_TABLE_TYPE9;

///
/// On Board Devices Information - Device Types. 
///
typedef enum {
  OnBoardDeviceTypeOther          = 0x01,
  OnBoardDeviceTypeUnknown        = 0x02,
  OnBoardDeviceTypeVideo          = 0x03,
  OnBoardDeviceTypeScsiController = 0x04,
  OnBoardDeviceTypeEthernet       = 0x05,
  OnBoardDeviceTypeTokenRing      = 0x06,
  OnBoardDeviceTypeSound          = 0x07
} MISC_ONBOARD_DEVICE_TYPE;

///
/// Device Item Entry
///
typedef struct {
  UINT8                     DeviceType;             ///< Bit [6:0] - enumeration type of device from MISC_ONBOARD_DEVICE_TYPE.
                                                    ///< Bit 7     - 1 : device enabled, 0 : device disabled.
  SMBIOS_TABLE_STRING       DescriptionString;
} DEVICE_STRUCT;

///
/// On Board Devices Information (Type 10, obsolete).
///
/// Note: This structure is obsolete starting with version 2.6 specification; the Onboard Devices Extended 
/// Information (Type 41) structure should be used instead . BIOS providers can choose to implement both 
/// types to allow existing SMBIOS browsers to properly display the system's onboard devices information.  
/// The information in this structure defines the attributes of devices that are onboard (soldered onto) 
/// a system element, usually the baseboard.  In general, an entry in this table implies that the BIOS
/// has some level of control over the enabling of the associated device for use by the system.
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  DEVICE_STRUCT         Device[1];
} SMBIOS_TABLE_TYPE10;

///
/// OEM Strings (Type 11).
/// This structure contains free form strings defined by the OEM. Examples of this are: 
/// Part Numbers for Reference Documents for the system, contact information for the manufacturer, etc. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 StringCount;
} SMBIOS_TABLE_TYPE11;

///
/// System Configuration Options (Type 12).
///
/// This structure contains information required to configure the base board's Jumpers and Switches. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 StringCount;
} SMBIOS_TABLE_TYPE12;


///
/// BIOS Language Information (Type 13).
///
/// The information in this structure defines the installable language attributes of the BIOS.  
/// 
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 InstallableLanguages;
  UINT8                 Flags;
  UINT8                 Reserved[15];
  SMBIOS_TABLE_STRING   CurrentLanguages;
} SMBIOS_TABLE_TYPE13;

///
/// System Event Log - Event Log Types.
/// 
typedef enum {
  EventLogTypeReserved         = 0x00,
  EventLogTypeSingleBitECC     = 0x01,
  EventLogTypeMultiBitECC      = 0x02,
  EventLogTypeParityMemErr     = 0x03,
  EventLogTypeBusTimeOut       = 0x04,
  EventLogTypeIOChannelCheck   = 0x05,
  EventLogTypeSoftwareNMI      = 0x06,
  EventLogTypePOSTMemResize    = 0x07,
  EventLogTypePOSTErr          = 0x08,
  EventLogTypePCIParityErr     = 0x09,
  EventLogTypePCISystemErr     = 0x0A,
  EventLogTypeCPUFailure       = 0x0B,
  EventLogTypeEISATimeOut      = 0x0C,
  EventLogTypeMemLogDisabled   = 0x0D,
  EventLogTypeLoggingDisabled  = 0x0E,
  EventLogTypeSysLimitExce     = 0x10,
  EventLogTypeAsyncHWTimer     = 0x11,
  EventLogTypeSysConfigInfo    = 0x12,
  EventLogTypeHDInfo           = 0x13,
  EventLogTypeSysReconfig      = 0x14,
  EventLogTypeUncorrectCPUErr  = 0x15,
  EventLogTypeAreaResetAndClr  = 0x16,
  EventLogTypeSystemBoot       = 0x17,
  EventLogTypeUnused           = 0x18, ///< 0x18 - 0x7F
  EventLogTypeAvailForSys      = 0x80, ///< 0x80 - 0xFE
  EventLogTypeEndOfLog         = 0xFF
} EVENT_LOG_TYPE_DATA;

///
/// System Event Log - Variable Data Format Types. 
/// 
typedef enum {
  EventLogVariableNone                        = 0x00,
  EventLogVariableHandle                      = 0x01,
  EventLogVariableMutilEvent                  = 0x02,
  EventLogVariableMutilEventHandle            = 0x03,
  EventLogVariablePOSTResultBitmap            = 0x04,
  EventLogVariableSysManagementType           = 0x05,
  EventLogVariableMutliEventSysManagmentType  = 0x06,                               
  EventLogVariableUnused                      = 0x07,
  EventLogVariableOEMAssigned                 = 0x80
} EVENT_LOG_VARIABLE_DATA;

///
/// Group Item Entry
///
typedef struct {
  UINT8                 ItemType;
  UINT16                ItemHandle;
} GROUP_STRUCT;

///
/// Event Log Type Descriptors
///
typedef struct {
  UINT8                 LogType;                    ///< The enumeration value from EVENT_LOG_TYPE_DATA.
  UINT8                 DataFormatType;
} EVENT_LOG_TYPE;

///
/// Group Associations (Type 14).
///
/// The Group Associations structure is provided for OEMs who want to specify 
/// the arrangement or hierarchy of certain components (including other Group Associations) 
/// within the system. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  SMBIOS_TABLE_STRING   GroupName;
  GROUP_STRUCT          Group[1];
} SMBIOS_TABLE_TYPE14;

///
/// System Event Log (Type 15).
///
/// The presence of this structure within the SMBIOS data returned for a system indicates 
/// that the system supports an event log.  An event log is a fixed-length area within a 
/// non-volatile storage element, starting with a fixed-length (and vendor-specific) header 
/// record, followed by one or more variable-length log records. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT16                LogAreaLength;
  UINT16                LogHeaderStartOffset;
  UINT16                LogDataStartOffset;
  UINT8                 AccessMethod;
  UINT8                 LogStatus;
  UINT32                LogChangeToken;
  UINT32                AccessMethodAddress;
  UINT8                 LogHeaderFormat;
  UINT8                 NumberOfSupportedLogTypeDescriptors;
  UINT8                 LengthOfLogTypeDescriptor;
  EVENT_LOG_TYPE        EventLogTypeDescriptors[1];
} SMBIOS_TABLE_TYPE15;

///
/// Physical Memory Array - Location.
///
typedef enum {
  MemoryArrayLocationOther                 = 0x01,
  MemoryArrayLocationUnknown               = 0x02,
  MemoryArrayLocationSystemBoard           = 0x03,
  MemoryArrayLocationIsaAddonCard          = 0x04,
  MemoryArrayLocationEisaAddonCard         = 0x05,
  MemoryArrayLocationPciAddonCard          = 0x06,
  MemoryArrayLocationMcaAddonCard          = 0x07,
  MemoryArrayLocationPcmciaAddonCard       = 0x08,
  MemoryArrayLocationProprietaryAddonCard  = 0x09,
  MemoryArrayLocationNuBus                 = 0x0A,
  MemoryArrayLocationPc98C20AddonCard      = 0xA0,
  MemoryArrayLocationPc98C24AddonCard      = 0xA1,
  MemoryArrayLocationPc98EAddonCard        = 0xA2,
  MemoryArrayLocationPc98LocalBusAddonCard = 0xA3
} MEMORY_ARRAY_LOCATION;

///
/// Physical Memory Array - Use.
///
typedef enum {
  MemoryArrayUseOther                      = 0x01,
  MemoryArrayUseUnknown                    = 0x02,
  MemoryArrayUseSystemMemory               = 0x03,
  MemoryArrayUseVideoMemory                = 0x04,
  MemoryArrayUseFlashMemory                = 0x05,
  MemoryArrayUseNonVolatileRam             = 0x06,
  MemoryArrayUseCacheMemory                = 0x07
} MEMORY_ARRAY_USE;

///
/// Physical Memory Array - Error Correction Types. 
///
typedef enum {
  MemoryErrorCorrectionOther               = 0x01,
  MemoryErrorCorrectionUnknown             = 0x02,
  MemoryErrorCorrectionNone                = 0x03,
  MemoryErrorCorrectionParity              = 0x04,
  MemoryErrorCorrectionSingleBitEcc        = 0x05,
  MemoryErrorCorrectionMultiBitEcc         = 0x06,
  MemoryErrorCorrectionCrc                 = 0x07
} MEMORY_ERROR_CORRECTION;

///
/// Physical Memory Array (Type 16).
///
/// This structure describes a collection of memory devices that operate 
/// together to form a memory address space. 
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT8                     Location;                       ///< The enumeration value from MEMORY_ARRAY_LOCATION.
  UINT8                     Use;                            ///< The enumeration value from MEMORY_ARRAY_USE.
  UINT8                     MemoryErrorCorrection;          ///< The enumeration value from MEMORY_ERROR_CORRECTION.
  UINT32                    MaximumCapacity;
  UINT16                    MemoryErrorInformationHandle;
  UINT16                    NumberOfMemoryDevices;
} SMBIOS_TABLE_TYPE16;

///
/// Memory Device - Form Factor.
///
typedef enum {
  MemoryFormFactorOther                    = 0x01,
  MemoryFormFactorUnknown                  = 0x02,
  MemoryFormFactorSimm                     = 0x03,
  MemoryFormFactorSip                      = 0x04,
  MemoryFormFactorChip                     = 0x05,
  MemoryFormFactorDip                      = 0x06,
  MemoryFormFactorZip                      = 0x07,
  MemoryFormFactorProprietaryCard          = 0x08,
  MemoryFormFactorDimm                     = 0x09,
  MemoryFormFactorTsop                     = 0x0A,
  MemoryFormFactorRowOfChips               = 0x0B,
  MemoryFormFactorRimm                     = 0x0C,
  MemoryFormFactorSodimm                   = 0x0D,
  MemoryFormFactorSrimm                    = 0x0E,
  MemoryFormFactorFbDimm                   = 0x0F
} MEMORY_FORM_FACTOR;

///
/// Memory Device - Type
///
typedef enum {
  MemoryTypeOther                          = 0x01,
  MemoryTypeUnknown                        = 0x02,
  MemoryTypeDram                           = 0x03,
  MemoryTypeEdram                          = 0x04,
  MemoryTypeVram                           = 0x05,
  MemoryTypeSram                           = 0x06,
  MemoryTypeRam                            = 0x07,
  MemoryTypeRom                            = 0x08,
  MemoryTypeFlash                          = 0x09,
  MemoryTypeEeprom                         = 0x0A,
  MemoryTypeFeprom                         = 0x0B,
  MemoryTypeEprom                          = 0x0C,
  MemoryTypeCdram                          = 0x0D,
  MemoryType3Dram                          = 0x0E,
  MemoryTypeSdram                          = 0x0F,
  MemoryTypeSgram                          = 0x10,
  MemoryTypeRdram                          = 0x11,
  MemoryTypeDdr                            = 0x12,
  MemoryTypeDdr2                           = 0x13,
  MemoryTypeDdr2FbDimm                     = 0x14,
  MemoryTypeDdr3                           = 0x18,
  MemoryTypeFbd2                           = 0x19
} MEMORY_DEVICE_TYPE;

typedef struct {
  UINT16    Reserved        :1;
  UINT16    Other           :1;
  UINT16    Unknown         :1;
  UINT16    FastPaged       :1;
  UINT16    StaticColumn    :1;
  UINT16    PseudoStatic    :1;
  UINT16    Rambus          :1;
  UINT16    Synchronous     :1;
  UINT16    Cmos            :1;
  UINT16    Edo             :1;
  UINT16    WindowDram      :1;
  UINT16    CacheDram       :1;
  UINT16    Nonvolatile     :1;
  UINT16    Reserved1       :3;
} MEMORY_DEVICE_TYPE_DETAIL;

///
/// Memory Device (Type 17).
///
/// This structure describes a single memory device that is part of 
/// a larger Physical Memory Array (Type 16).
/// Note:  If a system includes memory-device sockets, the SMBIOS implementation 
/// includes a Memory Device structure instance for each slot, whether or not the 
/// socket is currently populated.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT16                    MemoryArrayHandle;
  UINT16                    MemoryErrorInformationHandle;
  UINT16                    TotalWidth;
  UINT16                    DataWidth;
  UINT16                    Size;
  UINT8                     FormFactor;                     ///< The enumeration value from MEMORY_FORM_FACTOR.
  UINT8                     DeviceSet;
  SMBIOS_TABLE_STRING       DeviceLocator;
  SMBIOS_TABLE_STRING       BankLocator;
  UINT8                     MemoryType;                     ///< The enumeration value from MEMORY_DEVICE_TYPE.
  MEMORY_DEVICE_TYPE_DETAIL TypeDetail;
  UINT16                    Speed;
  SMBIOS_TABLE_STRING       Manufacturer;
  SMBIOS_TABLE_STRING       SerialNumber;
  SMBIOS_TABLE_STRING       AssetTag;
  SMBIOS_TABLE_STRING       PartNumber;
  //
  // Add for smbios 2.6
  //  
  UINT8                 Attributes;
} SMBIOS_TABLE_TYPE17;

///
/// 32-bit Memory Error Information - Error Type. 
///
typedef enum {  
  MemoryErrorOther             = 0x01,
  MemoryErrorUnknown           = 0x02,
  MemoryErrorOk                = 0x03,
  MemoryErrorBadRead           = 0x04,
  MemoryErrorParity            = 0x05,
  MemoryErrorSigleBit          = 0x06,
  MemoryErrorDoubleBit         = 0x07,
  MemoryErrorMultiBit          = 0x08,
  MemoryErrorNibble            = 0x09,
  MemoryErrorChecksum          = 0x0A,
  MemoryErrorCrc               = 0x0B,
  MemoryErrorCorrectSingleBit  = 0x0C,
  MemoryErrorCorrected         = 0x0D,
  MemoryErrorUnCorrectable     = 0x0E
} MEMORY_ERROR_TYPE;

///
/// 32-bit Memory Error Information - Error Granularity. 
///
typedef enum {  
  MemoryGranularityOther               = 0x01,
  MemoryGranularityOtherUnknown        = 0x02,
  MemoryGranularityDeviceLevel         = 0x03,
  MemoryGranularityMemPartitionLevel   = 0x04
} MEMORY_ERROR_GRANULARITY;

///
/// 32-bit Memory Error Information - Error Operation. 
///
typedef enum {  
  MemoryErrorOperationOther            = 0x01,
  MemoryErrorOperationUnknown          = 0x02,
  MemoryErrorOperationRead             = 0x03,
  MemoryErrorOperationWrite            = 0x04,
  MemoryErrorOperationPartialWrite     = 0x05
} MEMORY_ERROR_OPERATION;

///
/// 32-bit Memory Error Information (Type 18).
/// 
/// This structure identifies the specifics of an error that might be detected 
/// within a Physical Memory Array.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT8                     ErrorType;                  ///< The enumeration value from MEMORY_ERROR_TYPE.
  UINT8                     ErrorGranularity;           ///< The enumeration value from MEMORY_ERROR_GRANULARITY.
  UINT8                     ErrorOperation;             ///< The enumeration value from MEMORY_ERROR_OPERATION.
  UINT32                    VendorSyndrome;
  UINT32                    MemoryArrayErrorAddress;
  UINT32                    DeviceErrorAddress;
  UINT32                    ErrorResolution;
} SMBIOS_TABLE_TYPE18;

///
/// Memory Array Mapped Address (Type 19).
///
/// This structure provides the address mapping for a Physical Memory Array.  
/// One structure is present for each contiguous address range described.
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT32                StartingAddress;
  UINT32                EndingAddress;
  UINT16                MemoryArrayHandle;
  UINT8                 PartitionWidth;
} SMBIOS_TABLE_TYPE19;

///
/// Memory Device Mapped Address (Type 20).
///
/// This structure maps memory address space usually to a device-level granularity.  
/// One structure is present for each contiguous address range described. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT32                StartingAddress;
  UINT32                EndingAddress;
  UINT16                MemoryDeviceHandle;
  UINT16                MemoryArrayMappedAddressHandle;
  UINT8                 PartitionRowPosition;
  UINT8                 InterleavePosition;
  UINT8                 InterleavedDataDepth;
} SMBIOS_TABLE_TYPE20;

///
/// Built-in Pointing Device - Type
///
typedef enum {
  PointingDeviceTypeOther         = 0x01,
  PointingDeviceTypeUnknown       = 0x02,
  PointingDeviceTypeMouse         = 0x03,
  PointingDeviceTypeTrackBall     = 0x04,
  PointingDeviceTypeTrackPoint    = 0x05,
  PointingDeviceTypeGlidePoint    = 0x06,
  PointingDeviceTouchPad          = 0x07,
  PointingDeviceTouchScreen       = 0x08,
  PointingDeviceOpticalSensor     = 0x09
} BUILTIN_POINTING_DEVICE_TYPE;

///
/// Built-in Pointing Device - Interface.
///
typedef enum {
  PointingDeviceInterfaceOther              = 0x01,
  PointingDeviceInterfaceUnknown            = 0x02,
  PointingDeviceInterfaceSerial             = 0x03,
  PointingDeviceInterfacePs2                = 0x04,
  PointingDeviceInterfaceInfrared           = 0x05,
  PointingDeviceInterfaceHpHil              = 0x06,
  PointingDeviceInterfaceBusMouse           = 0x07,
  PointingDeviceInterfaceADB                = 0x08,
  PointingDeviceInterfaceBusMouseDB9        = 0xA0,
  PointingDeviceInterfaceBusMouseMicroDin   = 0xA1,
  PointingDeviceInterfaceUsb                = 0xA2
} BUILTIN_POINTING_DEVICE_INTERFACE;

///
/// Built-in Pointing Device (Type 21).
///
/// This structure describes the attributes of the built-in pointing device for the 
/// system. The presence of this structure does not imply that the built-in
/// pointing device is active for the system's use! 
///
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  UINT8                             Type;                   ///< The enumeration value from BUILTIN_POINTING_DEVICE_TYPE.
  UINT8                             Interface;              ///< The enumeration value from BUILTIN_POINTING_DEVICE_INTERFACE.
  UINT8                             NumberOfButtons;
} SMBIOS_TABLE_TYPE21;

///
/// Portable Battery - Device Chemistry
///
typedef enum {  
  PortableBatteryDeviceChemistryOther               = 0x01,
  PortableBatteryDeviceChemistryUnknown             = 0x02,
  PortableBatteryDeviceChemistryLeadAcid            = 0x03,
  PortableBatteryDeviceChemistryNickelCadmium       = 0x04,
  PortableBatteryDeviceChemistryNickelMetalHydride  = 0x05,
  PortableBatteryDeviceChemistryLithiumIon          = 0x06,
  PortableBatteryDeviceChemistryZincAir             = 0x07,
  PortableBatteryDeviceChemistryLithiumPolymer      = 0x08
} PORTABLE_BATTERY_DEVICE_CHEMISTRY;

///
/// Portable Battery (Type 22).
///
/// This structure describes the attributes of the portable battery(s) for the system.  
/// The structure contains the static attributes for the group.  Each structure describes 
/// a single battery pack's attributes.
///
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  SMBIOS_TABLE_STRING               Location;
  SMBIOS_TABLE_STRING               Manufacturer;
  SMBIOS_TABLE_STRING               ManufactureDate;
  SMBIOS_TABLE_STRING               SerialNumber;
  SMBIOS_TABLE_STRING               DeviceName;
  UINT8                             DeviceChemistry;              ///< The enumeration value from PORTABLE_BATTERY_DEVICE_CHEMISTRY.
  UINT16                            DeviceCapacity;
  UINT16                            DesignVoltage;
  SMBIOS_TABLE_STRING               SBDSVersionNumber;
  UINT8                             MaximumErrorInBatteryData;
  UINT16                            SBDSSerialNumber;
  UINT16                            SBDSManufactureDate;
  SMBIOS_TABLE_STRING               SBDSDeviceChemistry;
  UINT8                             DesignCapacityMultiplier;
  UINT32                            OEMSpecific;
} SMBIOS_TABLE_TYPE22;

///
/// System Reset (Type 23)
///
/// This structure describes whether Automatic System Reset functions enabled (Status).  
/// If the system has a watchdog Timer and the timer is not reset (Timer Reset)
/// before the Interval elapses, an automatic system reset will occur. The system will re-boot 
/// according to the Boot Option. This function may repeat until the Limit is reached, at which time 
/// the system will re-boot according to the Boot Option at Limit.  
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 Capabilities;
  UINT16                ResetCount;
  UINT16                ResetLimit;
  UINT16                TimerInterval;
  UINT16                Timeout;
} SMBIOS_TABLE_TYPE23;

///
/// Hardware Security (Type 24).
///
/// This structure describes the system-wide hardware security settings. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 HardwareSecuritySettings;
} SMBIOS_TABLE_TYPE24;

///
/// System Power Controls (Type 25).
///
/// This structure describes the attributes for controlling the main power supply to the system. 
/// Software that interprets this structure uses the month, day, hour, minute, and second values 
/// to determine the number of seconds until the next power-on of the system.  The presence of  
/// this structure implies that a timed power-on facility is available for the system. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 NextScheduledPowerOnMonth;
  UINT8                 NextScheduledPowerOnDayOfMonth;
  UINT8                 NextScheduledPowerOnHour;
  UINT8                 NextScheduledPowerOnMinute;
  UINT8                 NextScheduledPowerOnSecond;
} SMBIOS_TABLE_TYPE25;

///
/// Voltage Probe - Location and Status.
///
typedef struct {
  UINT8 VoltageProbeSite      :5;
  UINT8  VoltageProbeStatus   :3;
} MISC_VOLTAGE_PROBE_LOCATION;

///
/// Voltage Probe (Type 26)
///
/// This describes the attributes for a voltage probe in the system.   
/// Each structure describes a single voltage probe.
///
typedef struct {
  SMBIOS_STRUCTURE              Hdr;
  SMBIOS_TABLE_STRING           Description;
  MISC_VOLTAGE_PROBE_LOCATION   LocationAndStatus;
  UINT16                        MaximumValue;
  UINT16                        MinimumValue;
  UINT16                        Resolution;
  UINT16                        Tolerance;
  UINT16                        Accuracy;
  UINT32                        OEMDefined;
  UINT16                        NominalValue;
} SMBIOS_TABLE_TYPE26;

///
/// Cooling Device - Device Type and Status.
///
typedef struct {
  UINT8 CoolingDevice       :5;
  UINT8 CoolingDeviceStatus :3;
} MISC_COOLING_DEVICE_TYPE;

///
/// Cooling Device (Type 27)
///
/// This structure describes the attributes for a cooling device in the system.  
/// Each structure describes a single cooling device. 
/// 
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  UINT16                            TemperatureProbeHandle;
  MISC_COOLING_DEVICE_TYPE          DeviceTypeAndStatus;
  UINT8                             CoolingUnitGroup;
  UINT32                            OEMDefined;
  UINT16                            NominalSpeed;
} SMBIOS_TABLE_TYPE27;

///
/// Temperature Probe - Location and Status.
///
typedef struct {
  UINT8 TemperatureProbeSite   :5;
  UINT8 TemperatureProbeStatus :3;
} MISC_TEMPERATURE_PROBE_LOCATION;

///
/// Temperature Probe (Type 28).
///
/// This structure describes the attributes for a temperature probe in the system.  
/// Each structure describes a single temperature probe. 
///
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  SMBIOS_TABLE_STRING               Description;
  MISC_TEMPERATURE_PROBE_LOCATION   LocationAndStatus;
  UINT16                            MaximumValue;
  UINT16                            MinimumValue;
  UINT16                            Resolution;
  UINT16                            Tolerance;
  UINT16                            Accuracy;
  UINT32                            OEMDefined;
  UINT16                            NominalValue;
} SMBIOS_TABLE_TYPE28;

///
/// Electrical Current Probe - Location and Status.
///
typedef struct {
  UINT8 ElectricalCurrentProbeSite   :5;
  UINT8 ElectricalCurrentProbeStatus :3;
} MISC_ELECTRICAL_CURRENT_PROBE_LOCATION;

///
/// Electrical Current Probe (Type 29).
///
/// This structure describes the attributes for an electrical current probe in the system.
/// Each structure describes a single electrical current probe. 
///
typedef struct {
  SMBIOS_STRUCTURE                        Hdr;
  SMBIOS_TABLE_STRING                     Description;
  MISC_ELECTRICAL_CURRENT_PROBE_LOCATION  LocationAndStatus;
  UINT16                                  MaximumValue;
  UINT16                                  MinimumValue;
  UINT16                                  Resolution;
  UINT16                                  Tolerance;
  UINT16                                  Accuracy;
  UINT32                                  OEMDefined;
  UINT16                                  NominalValue;
} SMBIOS_TABLE_TYPE29;

///
/// Out-of-Band Remote Access (Type 30).
///
/// This structure describes the attributes and policy settings of a hardware facility 
/// that may be used to gain remote access to a hardware system when the operating system 
/// is not available due to power-down status, hardware failures, or boot failures. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  SMBIOS_TABLE_STRING   ManufacturerName;
  UINT8                 Connections;
} SMBIOS_TABLE_TYPE30;

///
/// Boot Integrity Services (BIS) Entry Point (Type 31).
///
/// Structure type 31 (decimal) is reserved for use by the Boot Integrity Services (BIS). 
/// 
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 Checksum;
  UINT8                 Reserved1;
  UINT16                Reserved2;
  UINT32                BisEntry16;
  UINT32                BisEntry32;
  UINT64                Reserved3;
  UINT32                Reserved4;
} SMBIOS_TABLE_TYPE31;

///
/// System Boot Information - System Boot Status.
///
typedef enum {
  BootInformationStatusNoError                  = 0x00,
  BootInformationStatusNoBootableMedia          = 0x01,
  BootInformationStatusNormalOSFailedLoading    = 0x02,
  BootInformationStatusFirmwareDetectedFailure  = 0x03,
  BootInformationStatusOSDetectedFailure        = 0x04,
  BootInformationStatusUserRequestedBoot        = 0x05,
  BootInformationStatusSystemSecurityViolation  = 0x06,
  BootInformationStatusPreviousRequestedImage   = 0x07,
  BootInformationStatusWatchdogTimerExpired     = 0x08,
  BootInformationStatusStartReserved            = 0x09,
  BootInformationStatusStartOemSpecific         = 0x80,
  BootInformationStatusStartProductSpecific     = 0xC0
} MISC_BOOT_INFORMATION_STATUS_DATA_TYPE;

///
/// System Boot Information (Type 32).
///
/// The client system firmware, e.g. BIOS, communicates the System Boot Status to the 
/// client's Pre-boot Execution Environment (PXE) boot image or OS-present management 
/// application via this structure. When used in the PXE environment, for example, 
/// this code identifies the reason the PXE was initiated and can be used by boot-image 
/// software to further automate an enterprise's PXE sessions.  For example, an enterprise  
/// could choose to automatically download a hardware-diagnostic image to a client whose 
/// reason code indicated either a firmware- or operating system-detected hardware failure.
///
typedef struct {
  SMBIOS_STRUCTURE                        Hdr;
  UINT8                                   Reserved[6];
  UINT8                                   BootStatus;     ///< The enumeration value from MISC_BOOT_INFORMATION_STATUS_DATA_TYPE.
} SMBIOS_TABLE_TYPE32;

///
/// 64-bit Memory Error Information (Type 33).
///
/// This structure describes an error within a Physical Memory Array, 
/// when the error address is above 4G (0xFFFFFFFF).
///    
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT8                     ErrorType;                    ///< The enumeration value from MEMORY_ERROR_TYPE.
  UINT8                     ErrorGranularity;             ///< The enumeration value from MEMORY_ERROR_GRANULARITY.
  UINT8                     ErrorOperation;               ///< The enumeration value from MEMORY_ERROR_OPERATION.
  UINT32                    VendorSyndrome;
  UINT64                    MemoryArrayErrorAddress;
  UINT64                    DeviceErrorAddress;
  UINT32                    ErrorResolution;
} SMBIOS_TABLE_TYPE33;

///
/// Management Device -  Type. 
///
typedef enum {
  ManagementDeviceTypeOther      = 0x01,
  ManagementDeviceTypeUnknown    = 0x02,
  ManagementDeviceTypeLm75       = 0x03,
  ManagementDeviceTypeLm78       = 0x04,
  ManagementDeviceTypeLm79       = 0x05,
  ManagementDeviceTypeLm80       = 0x06,
  ManagementDeviceTypeLm81       = 0x07,
  ManagementDeviceTypeAdm9240    = 0x08,
  ManagementDeviceTypeDs1780     = 0x09,
  ManagementDeviceTypeMaxim1617  = 0x0A,
  ManagementDeviceTypeGl518Sm    = 0x0B,
  ManagementDeviceTypeW83781D    = 0x0C,
  ManagementDeviceTypeHt82H791   = 0x0D
} MISC_MANAGEMENT_DEVICE_TYPE;

///
/// Management Device -  Address Type. 
///
typedef enum {
  ManagementDeviceAddressTypeOther   = 0x01,
  ManagementDeviceAddressTypeUnknown = 0x02,
  ManagementDeviceAddressTypeIOPort  = 0x03,
  ManagementDeviceAddressTypeMemory  = 0x04,
  ManagementDeviceAddressTypeSmbus   = 0x05
} MISC_MANAGEMENT_DEVICE_ADDRESS_TYPE;

///
/// Management Device (Type 34).
///
/// The information in this structure defines the attributes of a Management Device.  
/// A Management Device might control one or more fans or voltage, current, or temperature
/// probes as defined by one or more Management Device Component structures.
///
typedef struct {
  SMBIOS_STRUCTURE                      Hdr;
  SMBIOS_TABLE_STRING                   Description;
  UINT8                                 Type;                     ///< The enumeration value from MISC_MANAGEMENT_DEVICE_TYPE.
  UINT32                                Address;
  UINT8                                 AddressType;              ///< The enumeration value from MISC_MANAGEMENT_DEVICE_ADDRESS_TYPE.
} SMBIOS_TABLE_TYPE34;

///
/// Management Device Component (Type 35)
///
/// This structure associates a cooling device or environmental probe with structures 
/// that define the controlling hardware device and (optionally) the component's thresholds. 
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  SMBIOS_TABLE_STRING   Description;
  UINT16                ManagementDeviceHandle;
  UINT16                ComponentHandle;
  UINT16                ThresholdHandle;
} SMBIOS_TABLE_TYPE35;

///
/// Management Device Threshold Data (Type 36).
///
/// The information in this structure defines threshold information for 
/// a component (probe or cooling-unit) contained within a Management Device.  
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT16                LowerThresholdNonCritical;
  UINT16                UpperThresholdNonCritical;
  UINT16                LowerThresholdCritical;
  UINT16                UpperThresholdCritical;
  UINT16                LowerThresholdNonRecoverable;
  UINT16                UpperThresholdNonRecoverable;
} SMBIOS_TABLE_TYPE36;

///
/// Memory Channel Entry.
///
typedef struct {
  UINT8                 DeviceLoad;
  UINT16                DeviceHandle;
} MEMORY_DEVICE;

///
/// Memory Channel - Channel Type.
///
typedef enum {
  MemoryChannelTypeOther       = 0x01,
  MemoryChannelTypeUnknown     = 0x02,
  MemoryChannelTypeRambus      = 0x03,
  MemoryChannelTypeSyncLink    = 0x04
} MEMORY_CHANNEL_TYPE;

///
/// Memory Channel (Type 37)
///
/// The information in this structure provides the correlation between a Memory Channel
/// and its associated Memory Devices.  Each device presents one or more loads to the channel. 
/// The sum of all device loads cannot exceed the channel's defined maximum.
///
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 ChannelType;
  UINT8                 MaximumChannelLoad;
  UINT8                 MemoryDeviceCount;
  MEMORY_DEVICE         MemoryDevice[1];
} SMBIOS_TABLE_TYPE37;

///
/// IPMI Device Information - BMC Interface Type
///
typedef enum {
  IPMIDeviceInfoInterfaceTypeUnknown       = 0x00,
  IPMIDeviceInfoInterfaceTypeKCS           = 0x01, ///< The Keyboard Controller Style.
  IPMIDeviceInfoInterfaceTypeSMIC          = 0x02, ///< The Server Management Interface Chip.
  IPMIDeviceInfoInterfaceTypeBT            = 0x03, ///< The Block Transfer
  IPMIDeviceInfoInterfaceTypeReserved      = 0x04
} BMC_INTERFACE_TYPE;

///
/// IPMI Device Information (Type 38).
///
/// The information in this structure defines the attributes of an 
/// Intelligent Platform Management Interface (IPMI) Baseboard Management Controller (BMC).
/// 
typedef struct {
  SMBIOS_STRUCTURE      Hdr;
  UINT8                 InterfaceType;              ///< The enumeration value from BMC_INTERFACE_TYPE.
  UINT8                 IPMISpecificationRevision;
  UINT8                 I2CSlaveAddress;
  UINT8                 NVStorageDeviceAddress;
  UINT64                BaseAddress;
  UINT8                 BaseAddressModifier_InterruptInfo;
  UINT8                 InterruptNumber;
} SMBIOS_TABLE_TYPE38;

///
/// System Power Supply - Power Supply Characteristics.
///
typedef struct {
  UINT16                        PowerSupplyHotReplaceable:1;
  UINT16                        PowerSupplyPresent       :1;
  UINT16                        PowerSupplyUnplugged     :1;
  UINT16                        InputVoltageRangeSwitch  :4;
  UINT16                        PowerSupplyStatus        :3;
  UINT16                        PowerSupplyType          :4;
  UINT16                        Reserved                 :2;
} SYS_POWER_SUPPLY_CHARACTERISTICS;

///
/// System Power Supply (Type 39).
///
/// This structure identifies attributes of a system power supply.  One instance
/// of this record is present for each possible power supply in a system.  
///
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  UINT8                             PowerUnitGroup;
  SMBIOS_TABLE_STRING               Location;
  SMBIOS_TABLE_STRING               DeviceName;
  SMBIOS_TABLE_STRING               Manufacturer;
  SMBIOS_TABLE_STRING               SerialNumber;
  SMBIOS_TABLE_STRING               AssetTagNumber;
  SMBIOS_TABLE_STRING               ModelPartNumber;
  SMBIOS_TABLE_STRING               RevisionLevel;
  UINT16                            MaxPowerCapacity;
  SYS_POWER_SUPPLY_CHARACTERISTICS  PowerSupplyCharacteristics;
  UINT16                            InputVoltageProbeHandle;
  UINT16                            CoolingDeviceHandle;
  UINT16                            InputCurrentProbeHandle;
} SMBIOS_TABLE_TYPE39;

///
/// Additional Information Entry Format. 
///
typedef struct {                       
  UINT8                   EntryLength; 
  UINT16                  ReferencedHandle;
  UINT8                   ReferencedOffset;
  SMBIOS_TABLE_STRING     EntryString;
  UINT8                   Value[1];
}ADDITIONAL_INFORMATION_ENTRY;

///
/// Additional Information (Type 40).
///
/// This structure is intended to provide additional information for handling unspecified 
/// enumerated values and interim field updates in another structure. 
///
typedef struct {
  SMBIOS_STRUCTURE                      Hdr;
  UINT8                                 NumberOfAdditionalInformationEntries;
  ADDITIONAL_INFORMATION_ENTRY          AdditionalInfoEntries[1];  
} SMBIOS_TABLE_TYPE40;

///
/// Onboard Devices Extended Information - Onboard Device Types.
///
typedef enum{
  OnBoardDeviceExtendedTypeOther          = 0x01,
  OnBoardDeviceExtendedTypeUnknown        = 0x02,
  OnBoardDeviceExtendedTypeVideo          = 0x03,
  OnBoardDeviceExtendedTypeScsiController = 0x04,
  OnBoardDeviceExtendedTypeEthernet       = 0x05,
  OnBoardDeviceExtendedTypeTokenRing      = 0x06,
  OnBoardDeviceExtendedTypeSound          = 0x07,
  OnBoardDeviceExtendedTypePATAController = 0x08,
  OnBoardDeviceExtendedTypeSATAController = 0x09,
  OnBoardDeviceExtendedTypeSASController  = 0x0A
} ONBOARD_DEVICE_EXTENDED_INFO_TYPE;

///
/// Onboard Devices Extended Information (Type 41).
///
/// The information in this structure defines the attributes of devices that 
/// are onboard (soldered onto) a system element, usually the baseboard.  
/// In general, an entry in this table implies that the BIOS has some level of 
/// control over the enabling of the associated device for use by the system. 
///
typedef struct {
  SMBIOS_STRUCTURE                  Hdr;
  SMBIOS_TABLE_STRING               ReferenceDesignation;
  UINT8                             DeviceType;             ///< The enumeration value from ONBOARD_DEVICE_EXTENDED_INFO_TYPE
  UINT8                             DeviceTypeInstance;
  UINT16                            SegmentGroupNum;
  UINT8                             BusNum;
  UINT8                             DevFuncNum;  
} SMBIOS_TABLE_TYPE41;

///
/// Inactive (Type 126)
///
typedef struct {
  SMBIOS_STRUCTURE   Hdr;
} SMBIOS_TABLE_TYPE126;

///
/// End-of-Table (Type 127)
///
typedef struct {
  SMBIOS_STRUCTURE   Hdr;
} SMBIOS_TABLE_TYPE127;

///
/// Union of all the possible SMBIOS record types.
///
typedef union {
  SMBIOS_STRUCTURE      *Hdr;
  SMBIOS_TABLE_TYPE0    *Type0;
  SMBIOS_TABLE_TYPE1    *Type1;
  SMBIOS_TABLE_TYPE2    *Type2;
  SMBIOS_TABLE_TYPE3    *Type3;
  SMBIOS_TABLE_TYPE4    *Type4;
  SMBIOS_TABLE_TYPE5    *Type5;
  SMBIOS_TABLE_TYPE6    *Type6;
  SMBIOS_TABLE_TYPE7    *Type7;
  SMBIOS_TABLE_TYPE8    *Type8;
  SMBIOS_TABLE_TYPE9    *Type9;
  SMBIOS_TABLE_TYPE10   *Type10;
  SMBIOS_TABLE_TYPE11   *Type11;
  SMBIOS_TABLE_TYPE12   *Type12;
  SMBIOS_TABLE_TYPE13   *Type13;
  SMBIOS_TABLE_TYPE14   *Type14;
  SMBIOS_TABLE_TYPE15   *Type15;
  SMBIOS_TABLE_TYPE16   *Type16;
  SMBIOS_TABLE_TYPE17   *Type17;
  SMBIOS_TABLE_TYPE18   *Type18;
  SMBIOS_TABLE_TYPE19   *Type19;
  SMBIOS_TABLE_TYPE20   *Type20;
  SMBIOS_TABLE_TYPE21   *Type21;
  SMBIOS_TABLE_TYPE22   *Type22;
  SMBIOS_TABLE_TYPE23   *Type23;
  SMBIOS_TABLE_TYPE24   *Type24;
  SMBIOS_TABLE_TYPE25   *Type25;
  SMBIOS_TABLE_TYPE26   *Type26;
  SMBIOS_TABLE_TYPE27   *Type27;
  SMBIOS_TABLE_TYPE28   *Type28;
  SMBIOS_TABLE_TYPE29   *Type29;
  SMBIOS_TABLE_TYPE30   *Type30;
  SMBIOS_TABLE_TYPE31   *Type31;
  SMBIOS_TABLE_TYPE32   *Type32;
  SMBIOS_TABLE_TYPE33   *Type33;
  SMBIOS_TABLE_TYPE34   *Type34;
  SMBIOS_TABLE_TYPE35   *Type35;
  SMBIOS_TABLE_TYPE36   *Type36;
  SMBIOS_TABLE_TYPE37   *Type37;
  SMBIOS_TABLE_TYPE38   *Type38;
  SMBIOS_TABLE_TYPE39   *Type39;
  SMBIOS_TABLE_TYPE40   *Type40;
  SMBIOS_TABLE_TYPE41   *Type41;
  SMBIOS_TABLE_TYPE126  *Type126;
  SMBIOS_TABLE_TYPE127  *Type127;
  UINT8                 *Raw;
} SMBIOS_STRUCTURE_POINTER;

#pragma pack()

#endif
