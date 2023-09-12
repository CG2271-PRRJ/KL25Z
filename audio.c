#include "audio.h"

void changeNote(uint32_t freq)
{
    uint32_t mod;
    mod = 375000 / (freq * 2);
    TPM0->MOD = mod;
    TPM0_C5V = mod / VOLUME;
}

void stopNote(void)
{
    TPM0_C5V = 0;
}
