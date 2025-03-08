#include "monitor.h"
#include "mhelpers.h"
#include "adc.h"

#define VBAT_CHAN ADC_Chan_0

#define ADC_SCALE (3.3 / 4096.0)

#define VBAT_BRIDGE_SCALE (12.06 / 2.334)

static struct
{
    RMA_t vbat;
} _g_monState =
    {
        .vbat = {.windowSz = (FUSION_FREQ / 10)},
};

/******************************************************************************/

extern int _dbg;

extern void __dbg_hook(int arg);

/******************************************************************************/

static void _Monitor_ConverVbat(uint32_t vbat);

float Monitor_GetVbat()
{
    ADC_Read(ADC_DEV, VBAT_CHAN, _Monitor_ConverVbat);

    return 0;
}

/******************************************************************************/

static void _Monitor_ConverVbat(uint32_t vbat)
{
    float fvbat = vbat;

    fvbat *= ADC_SCALE;
    fvbat *= VBAT_BRIDGE_SCALE;

    FS_RmaUpdate(&_g_monState.vbat, fvbat);
}