#pragma once
#include <stdint.h>

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

struct ACPISDTHeader
{
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
};

struct PACKED RSDPDescriptor
{
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
};

struct PACKED RSDP2Descriptor
{
	char Signature[8];
	uint8_t Checksum;
	char OEMID[6];
	uint8_t Revision;
	uint32_t RsdtAddress;
	uint32_t Length;
	uint64_t XSDTAddress;
	uint8_t ExtendedChecksum;
	uint8_t Reserved[3];
};
