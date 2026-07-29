#include "display.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static DisplayPage_t s_page;

static void FillBlankLine(char *line)
{
    memset(line, ' ', 16);
    line[16] = '\0';
}

static void RenderMainPage(const DisplayData_t *data, DisplayLines_t *lines)
{
    FillBlankLine(lines->line1);
    FillBlankLine(lines->line2);

    snprintf(lines->line1, sizeof(lines->line1), "SPD:%3u km/h RPM:%4u", data->speedKmh, data->rpm);
    snprintf(lines->line2, sizeof(lines->line2), "F:%3u%%", data->fuelPct);
}

static void RenderTripPage(const DisplayData_t *data, DisplayLines_t *lines)
{
    FillBlankLine(lines->line1);
    FillBlankLine(lines->line2);

    snprintf(lines->line1, sizeof(lines->line1), "TRIP:%4lu km", (unsigned long)(data->tripMetres / 1000u));
    snprintf(lines->line2, sizeof(lines->line2), "AVG:%2u T:%02lu:%02lu", data->avgSpeedKmh,
             (unsigned long)(data->tripSeconds / 60u),
             (unsigned long)(data->tripSeconds % 60u));
}

static void RenderEnginePage(const DisplayData_t *data, DisplayLines_t *lines)
{
    FillBlankLine(lines->line1);
    FillBlankLine(lines->line2);

    snprintf(lines->line1, sizeof(lines->line1), "RPM:%4u C:%3dC", data->rpm, data->coolantC);
    snprintf(lines->line2, sizeof(lines->line2), "OIL:%1u.%1u bar", data->oilBarX10 / 10u, data->oilBarX10 % 10u);
}

static void RenderElectricalPage(const DisplayData_t *data, DisplayLines_t *lines)
{
    FillBlankLine(lines->line1);
    FillBlankLine(lines->line2);

    snprintf(lines->line1, sizeof(lines->line1), "BATT:%4uV", data->battmV / 1000u);
    snprintf(lines->line2, sizeof(lines->line2), "ODO:%6lu km", (unsigned long)(data->odoMetres / 1000u));
}

static void RenderDiagPage(const DisplayData_t *data, DisplayLines_t *lines)
{
    FillBlankLine(lines->line1);
    FillBlankLine(lines->line2);

    snprintf(lines->line1, sizeof(lines->line1), "WARN:0x%04X", data->warnMask);
    snprintf(lines->line2, sizeof(lines->line2), "ST:%s UP:%lu", "RUN", (unsigned long)data->ignitionSec);
}

void Display_Init(void)
{
    s_page = PG_MAIN;
}

void Display_Update(const DisplayData_t *data, DisplayLines_t *lines)
{
    if (data == NULL || lines == NULL)
    {
        return;
    }

    if (data->state == CS_BULBCHECK)
    {
        snprintf(lines->line1, sizeof(lines->line1), "BULB CHECK");
        lines->line2[0] = '\0';
        return;
    }

    if (data->state == CS_CRANKING)
    {
        snprintf(lines->line1, sizeof(lines->line1), "CRANKING...");
        lines->line2[0] = '\0';
        return;
    }

    if (data->state == CS_STALLED)
    {
        snprintf(lines->line1, sizeof(lines->line1), "ENGINE STOPPED");
        lines->line2[0] = '\0';
        return;
    }

    if (data->state == CS_LIMP_HOME)
    {
        snprintf(lines->line1, sizeof(lines->line1), "!! STOP ENGINE !!");
        lines->line2[0] = '\0';
        return;
    }

    switch (s_page)
    {
        case PG_MAIN:
            RenderMainPage(data, lines);
            break;

        case PG_TRIP:
            RenderTripPage(data, lines);
            break;

        case PG_ENGINE:
            RenderEnginePage(data, lines);
            break;

        case PG_ELECTRICAL:
            RenderElectricalPage(data, lines);
            break;

        case PG_DIAG:
            RenderDiagPage(data, lines);
            break;

        default:
            RenderMainPage(data, lines);
            break;
    }
}

void Display_SetPage(DisplayPage_t page)
{
    if (page >= PG_MAIN && page <= PG_DIAG)
    {
        s_page = page;
    }
}

DisplayPage_t Display_GetPage(void)
{
    return s_page;
}

void Display_NextPage(void)
{
    if (s_page == PG_DIAG)
    {
        s_page = PG_MAIN;
    }
    else
    {
        s_page = (DisplayPage_t)(s_page + 1);
    }
}
