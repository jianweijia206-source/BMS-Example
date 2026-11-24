#include <stdio.h>

/* Simple, hardware-independent BMS example for Keil C51 / 8051. */
#define CELL_COUNT             4U
#define CELL_OVERVOLTAGE_MV    4200U
#define CELL_UNDERVOLTAGE_MV   2800U
#define PACK_OVERCURRENT_MA    20000L
#define MAX_TEMPERATURE_C      60

typedef struct {
    unsigned int cell_mv[CELL_COUNT];
    long current_ma;       /* Positive: discharging; negative: charging. */
    int temperature_c;
    unsigned int soc_percent;
    unsigned char fault;
} BmsState;

enum {
    BMS_FAULT_NONE = 0,
    BMS_FAULT_OVERVOLTAGE = 1,
    BMS_FAULT_UNDERVOLTAGE = 2,
    BMS_FAULT_OVERCURRENT = 3,
    BMS_FAULT_OVERTEMPERATURE = 4
};

/* Replace this demo data with ADC readings and a temperature sensor driver. */
static void ReadSensors(BmsState *state)
{
    unsigned char i;
    for (i = 0; i < CELL_COUNT; ++i) {
        state->cell_mv[i] = 3700U;
    }
    state->current_ma = 1250L;
    state->temperature_c = 28;
}

static void UpdateSoc(BmsState *state)
{
    unsigned long total_mv = 0UL;
    unsigned char i;
    for (i = 0; i < CELL_COUNT; ++i) {
        total_mv += state->cell_mv[i];
    }
    /* Demo voltage estimate: 3.0 V = 0%, 4.2 V = 100% per cell. */
    if (total_mv <= (unsigned long)CELL_COUNT * 3000UL) {
        state->soc_percent = 0U;
    } else if (total_mv >= (unsigned long)CELL_COUNT * 4200UL) {
        state->soc_percent = 100U;
    } else {
        state->soc_percent = (unsigned int)
            ((total_mv - (unsigned long)CELL_COUNT * 3000UL) * 100UL /
             ((unsigned long)CELL_COUNT * 1200UL));
    }
}

static void CheckProtection(BmsState *state)
{
    unsigned char i;
    state->fault = BMS_FAULT_NONE;
    for (i = 0; i < CELL_COUNT; ++i) {
        if (state->cell_mv[i] > CELL_OVERVOLTAGE_MV) {
            state->fault = BMS_FAULT_OVERVOLTAGE;
            return;
        }
        if (state->cell_mv[i] < CELL_UNDERVOLTAGE_MV) {
            state->fault = BMS_FAULT_UNDERVOLTAGE;
            return;
        }
    }
    if (state->current_ma > PACK_OVERCURRENT_MA ||
        state->current_ma < -PACK_OVERCURRENT_MA) {
        state->fault = BMS_FAULT_OVERCURRENT;
    } else if (state->temperature_c > MAX_TEMPERATURE_C) {
        state->fault = BMS_FAULT_OVERTEMPERATURE;
    }
}

int main(void)
{
    BmsState battery;
    unsigned char i;
    ReadSensors(&battery);
    UpdateSoc(&battery);
    CheckProtection(&battery);

    printf("BMS demo: SOC=%u%%, pack current=%ld mA, temperature=%d C\\n",
           battery.soc_percent, battery.current_ma, battery.temperature_c);
    for (i = 0; i < CELL_COUNT; ++i) {
        printf("Cell %u: %u mV\\n", (unsigned int)(i + 1U), battery.cell_mv[i]);
    }
    printf("Protection status: %s (code %u)\\n",
           battery.fault == BMS_FAULT_NONE ? "NORMAL" : "FAULT",
           (unsigned int)battery.fault);
    while (1) {
        /* Embedded main loop: sample, update, protect, and communicate. */
        break; /* Keep the host/demo build able to exit. */
    }
    return 0;
}
