#include "odometer.h"

#include <stddef.h>
#include <string.h>

#define ODO_SAVE_METRES        100u
#define ODO_SAVE_SEC          60u
#define ODO_SLOT_COUNT        8u
#define ODO_SLOT_SIZE         16u
#define ODO_SLOT_BASE         0x0040u
#define ODO_SLOT_MAGIC        0x4F44u
#define ODO_SLOT_CSUM_OFFSET  15u

typedef struct
{
    uint16_t slotMagic;
    uint32_t seq;
    uint32_t odoMetres;
    uint32_t tripMetres;
    uint8_t  reserved;
    uint8_t  slotCsum;
} OdometerSlot_t;

static const OdometerStorage_t *s_storage;
static uint32_t s_totalMetres;
static uint32_t s_tripMetres;
static uint32_t s_tripSeconds;
static uint32_t s_savedMetres;
static uint32_t s_savedSeconds;
static uint32_t s_lastSeq;
static uint8_t s_nextSlot;
static uint8_t s_initialized;
static uint8_t s_dirty;

static uint8_t Odometer_CalcChecksum(const uint8_t *buffer, uint16_t length)
{
    uint16_t sum = 0u;
    uint16_t index;

    for (index = 0u; index < length; index++)
    {
        sum = (uint16_t)(sum + buffer[index]);
    }

    return (uint8_t)(-sum);
}

static uint8_t Odometer_ValidateSlot(const uint8_t *buffer, uint16_t length)
{
    const OdometerSlot_t *slot = (const OdometerSlot_t *)buffer;
    uint8_t checksum;
    uint8_t expected;

    if (length < 16u)
    {
        return 0u;
    }

    if (slot->slotMagic != ODO_SLOT_MAGIC)
    {
        return 0u;
    }

    checksum = buffer[ODO_SLOT_CSUM_OFFSET];
    expected = Odometer_CalcChecksum(buffer, ODO_SLOT_CSUM_OFFSET);
    if (checksum != expected)
    {
        return 0u;
    }

    return 1u;
}

static uint8_t Odometer_LoadBestSlot(void)
{
    uint8_t slot;
    uint8_t bestSlot = 0u;
    uint8_t valid = 0u;
    uint8_t buffer[16];
    uint32_t bestSeq = 0u;

    for (slot = 0u; slot < ODO_SLOT_COUNT; slot++)
    {
        uint16_t address = (uint16_t)(ODO_SLOT_BASE + slot * ODO_SLOT_SIZE);
        if (s_storage == NULL || s_storage->readBytes == NULL)
        {
            return 0u;
        }

        if (s_storage->readBytes(address, buffer, sizeof(buffer)) != 0u)
        {
            continue;
        }

        if (Odometer_ValidateSlot(buffer, sizeof(buffer)) == 0u)
        {
            continue;
        }

        if (valid == 0u || ((const OdometerSlot_t *)buffer)->seq > bestSeq)
        {
            bestSeq = ((const OdometerSlot_t *)buffer)->seq;
            bestSlot = slot;
            valid = 1u;
        }
    }

    if (valid == 0u)
    {
        s_totalMetres = 0u;
        s_tripMetres = 0u;
        s_tripSeconds = 0u;
        s_savedMetres = 0u;
        s_savedSeconds = 0u;
        s_lastSeq = 0u;
        s_nextSlot = 0u;
        return 0u;
    }

    if (s_storage->readBytes((uint16_t)(ODO_SLOT_BASE + bestSlot * ODO_SLOT_SIZE), buffer, sizeof(buffer)) != 0u)
    {
        return 0u;
    }

    {
        const OdometerSlot_t *slotData = (const OdometerSlot_t *)buffer;
        s_totalMetres = slotData->odoMetres;
        s_tripMetres = slotData->tripMetres;
        s_tripSeconds = 0u;
        s_savedMetres = s_totalMetres;
        s_savedSeconds = 0u;
        s_lastSeq = slotData->seq;
        s_nextSlot = (uint8_t)((bestSlot + 1u) % ODO_SLOT_COUNT);
    }

    return 1u;
}

static void Odometer_BuildSlotBuffer(uint8_t *buffer, uint32_t seq, uint32_t totalMetres, uint32_t tripMetres)
{
    OdometerSlot_t *slotData = (OdometerSlot_t *)buffer;

    slotData->slotMagic = ODO_SLOT_MAGIC;
    slotData->seq = seq;
    slotData->odoMetres = totalMetres;
    slotData->tripMetres = tripMetres;
    slotData->reserved = 0u;
    slotData->slotCsum = Odometer_CalcChecksum(buffer, ODO_SLOT_CSUM_OFFSET);
}

static uint8_t Odometer_WriteSlotWithRetry(uint8_t slot, uint32_t seq, uint32_t totalMetres, uint32_t tripMetres)
{
    uint8_t buffer[16];
    uint8_t readback[16];
    uint16_t address;
    uint8_t attempt;

    Odometer_BuildSlotBuffer(buffer, seq, totalMetres, tripMetres);

    for (attempt = 0u; attempt < 2u; attempt++)
    {
        address = (uint16_t)(ODO_SLOT_BASE + slot * ODO_SLOT_SIZE);
        if (s_storage == NULL || s_storage->writeBytes == NULL)
        {
            return 0u;
        }

        if (s_storage->writeBytes(address, buffer, sizeof(buffer)) != 0u)
        {
            continue;
        }

        if (s_storage->readBytes == NULL)
        {
            return 0u;
        }

        if (s_storage->readBytes(address, readback, sizeof(readback)) != 0u)
        {
            continue;
        }

        if (memcmp(readback, buffer, sizeof(buffer)) == 0)
        {
            return 1u;
        }
    }

    return 0u;
}

void Odometer_Init(const OdometerStorage_t *storage)
{
    s_storage = storage;
    s_totalMetres = 0u;
    s_tripMetres = 0u;
    s_tripSeconds = 0u;
    s_savedMetres = 0u;
    s_savedSeconds = 0u;
    s_lastSeq = 0u;
    s_nextSlot = 0u;
    s_initialized = 0u;
    s_dirty = 0u;

    if (Odometer_LoadBestSlot() != 0u)
    {
        s_initialized = 1u;
    }
    else
    {
        s_initialized = 1u;
    }
}

void Odometer_AddDistance(uint32_t mm)
{
    uint32_t metres;

    if (s_initialized == 0u)
    {
        return;
    }

    if (mm >= 1000u)
    {
        metres = mm / 1000u;
    }
    else
    {
        metres = 0u;
    }

    if (metres != 0u)
    {
        if (s_totalMetres > (0xFFFFFFFFu - metres))
        {
            s_totalMetres = 0xFFFFFFFFu;
        }
        else
        {
            s_totalMetres += metres;
        }

        if (s_tripMetres > (0xFFFFFFFFu - metres))
        {
            s_tripMetres = 0xFFFFFFFFu;
        }
        else
        {
            s_tripMetres += metres;
        }

        if ((s_totalMetres - s_savedMetres) >= ODO_SAVE_METRES)
        {
            s_dirty = 1u;
        }
    }
}

void Odometer_TickSecond(void)
{
    if (s_initialized == 0u)
    {
        return;
    }

    s_tripSeconds++;
    s_savedSeconds++;

    if (s_savedSeconds >= ODO_SAVE_SEC)
    {
        s_dirty = 1u;
    }
}

void Odometer_SaveNow(void)
{
    uint32_t nextSeq;

    if (s_initialized == 0u || s_storage == NULL)
    {
        return;
    }

    if (s_dirty == 0u && (s_totalMetres == s_savedMetres))
    {
        return;
    }

    nextSeq = s_lastSeq + 1u;
    if (Odometer_WriteSlotWithRetry(s_nextSlot, nextSeq, s_totalMetres, s_tripMetres) != 0u)
    {
        s_lastSeq = nextSeq;
        s_savedMetres = s_totalMetres;
        s_savedSeconds = 0u;
        s_nextSlot = (uint8_t)((s_nextSlot + 1u) % ODO_SLOT_COUNT);
        s_dirty = 0u;
    }
}

void Odometer_OnKeyOff(void)
{
    Odometer_SaveNow();
}

void Odometer_ResetTrip(void)
{
    if (s_initialized == 0u)
    {
        return;
    }

    s_tripMetres = 0u;
    s_tripSeconds = 0u;
    s_dirty = 1u;
}

uint32_t Odometer_GetTotal(void)
{
    return s_totalMetres;
}

uint32_t Odometer_GetTrip(void)
{
    return s_tripMetres;
}

uint32_t Odometer_GetTripSeconds(void)
{
    return s_tripSeconds;
}

uint16_t Odometer_GetTripAvgKmh(void)
{
    if (s_tripSeconds == 0u)
    {
        return 0u;
    }

    return (uint16_t)((s_tripMetres * 3600u) / (s_tripSeconds * 1000u));
}
