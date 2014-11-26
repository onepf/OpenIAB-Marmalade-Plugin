/*
 * Internal header for the openiab extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef OPENIAB_INTERNAL_H
#define OPENIAB_INTERNAL_H

#include "s3eTypes.h"
#include "openiab.h"
#include "openiab_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult openiabInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult openiabInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void openiabTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void openiabTerminate_platform();
void mapSku_platform(const char* sku, const char* storeName, const char* storeSku);

OpenIabSkuDetails* getSkuDetails_platform(const char* sku);

OpenIabSkuDetails** getSkuListDetails_platform(const char** skuList, int skuListCount);

OpenIabPurchase** getPurchases_platform();

void init_platform(OpenIabOptions* options, const char** skuList, int skuListCount);

void purchaseProduct_platform(const char* sku, const char* payload);

void purchaseSubscription_platform(const char* sku, const char* payload);

void consume_platform(const char* sku);

OpenIabStoreNames* openiabStoreNames_platform();

int getPurchasesCount_platform();

#endif /* !OPENIAB_INTERNAL_H */