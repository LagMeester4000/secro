#pragma once
#include "KraNetInput.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct KraNetSession KraNetSession;

// Initialize the library
KraNetSession* KraNetInit(void* ExternalData, void(*UpdateFunc)(void*, KraNetInput, KraNetInput));

// Clean up the library
void KraNetExit(KraNetSession* Net);

// Update the netcode
void KraNetUpdate(KraNetSession* Net, KraNetInput LocalInput);


#ifdef __cplusplus
};
#endif