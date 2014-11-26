/*
Generic implementation of the openiab extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

 
#include "openiab_internal.h"

 s3eResult openiabInit()
{
    //Add any generic initialisation code here
    return openiabInit_platform();
}

void openiabTerminate()
{
    //Add any generic termination code here
    openiabTerminate_platform();
}

void mapSku(const char* sku, const char* storeName, const char* storeSku)
{
	mapSku_platform(sku, storeName, storeSku);
}

OpenIabSkuDetails* getSkuDetails(const char* sku)
{
	return getSkuDetails_platform(sku);
}

OpenIabSkuDetails** getSkuListDetails(const char** skuList, int skuListCount)
{
	return getSkuListDetails_platform(skuList, skuListCount);
}

OpenIabPurchase** getPurchases()
{
    return getPurchases_platform();
}

void init(OpenIabOptions* options, const char** skuList, int skuListCount)
{
	init_platform(options, skuList, skuListCount);
}

void purchaseProduct(const char* sku, const char* payload)
{
	purchaseProduct_platform(sku, payload);
}

void purchaseSubscription(const char* sku, const char* payload)
{
	purchaseSubscription_platform(sku, payload);
}

void consume(const char* sku)
{
	consume_platform(sku);
}

OpenIabStoreNames* openiabStoreNames()
{
	return openiabStoreNames_platform();
}