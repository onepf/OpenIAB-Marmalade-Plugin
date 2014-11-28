#include "openIab.h"
#include "IwDebug.h"
#include "s3e.h"
#include "IwNUI.h"
#include "IwRandom.h"

#include "ExampleUI.h"
#include <IwPropertyString.h>
#include "s3eDebug.h"

// Button click handlers for the UI

const char* hatSku = "sku_hat_inner";
const char* coinSku = "sku_coin_inner";
const char* subSku = "sku_subs_inner";

// note this is the public license key provided by Google, not the one you sign this app with, it's in the developer console under Services & APIs
const char *publicKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAixqRVmOe1akkTkeFS9ot8zERpdYVaziPRRKxMquWg7UUhZvi7YvV5UGVB7XUwP9LQVXndslUZmxj1XU1SeAERdHSfrAk7qcX6vYZJ023nxLXTe5lqLkPj9YQKlA39V5hp5rqi5oH8ZA6o2Eqrz9dISQXK+C84GkUvHggRsxfcdcO4sabWPcKW//io1eoiFANW3gKvXXpaovhWxtuJ/U4uB9t6WJLzUcnRP2eELCsbvucIYJKksIwmN3P22DDRV4ce43Fbld/eJGz8yXkEF6/wIyha6+tepZd6ix4B9RQVZYhEijAWA/yBSU5gMYrnFaC0jIZPmMUCnfglwwv1H+a4QIDAQAB";

bool OnInitClick(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*) data;
	ui->Log("Starting OpenIAB initialization");
	
	// Set options
	OpenIabOptions* options = new OpenIabOptions();

	options->numStores = 1;
	options->storeNames = new const char*[options->numStores];
	options->storeKeys = new const char*[options->numStores];
	options->storeNames[0] = openiabStoreNames()->GOOGLE;
	options->storeKeys[0] = publicKey;

	options->verifyMode = VERIFY_SKIP;
    options->storeSearchStrategy = SEARCH_STRATEGY_INSTALLER_THEN_BEST_FIT;
	
    options->checkInventory = false;
	options->checkInventoryTimeoutMs = 20000;
	options->discoveryTimeoutMs = 10000;

	options->numPreferredStoreNames = 1;
	options->preferredStoreNames = new const char*[options->numPreferredStoreNames];
	options->preferredStoreNames[0] = openiabStoreNames()->GOOGLE;

	const char** skuList = new const char*[3];
	skuList[0] = hatSku;
	skuList[1] = coinSku;
	skuList[2] = subSku;

	// Start up the Store interface
	init(options, skuList, 3);
	return true;
}

bool OnConsumeClick(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*)data;
	ui->Log("Staring consumption");
    consume(coinSku);
    return true;
}

// Purchase a Hat
bool OnPurchaseNonConsumableClick(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*) data;
	ui->Log("Attempting to purchase Hat");
	string randomPayload = string_format("TestPayload%d",IwRandMinMax(1,10000));
	purchaseProduct(hatSku, randomPayload.c_str());
	return true;
}

// Purchase a Coin
bool OnPurchaseConsumableClick(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*) data;
	ui->Log("Attempting to purchase Coin");
	string randomPayload = string_format("TestPayload%d",IwRandMinMax(1,10000));
	purchaseProduct(coinSku, randomPayload.c_str());
	return true;
}

// Purchase a Subscription
bool OnPurchaseSubscriptionClick(void* data, CButton* button)
{
	ExampleUI *ui = (ExampleUI*) data;
	ui->Log("Attempting to purchase Subscription");
	string randomPayload = string_format("TestPayload%d",IwRandMinMax(1,10000));
	purchaseSubscription(subSku, randomPayload.c_str());

	return true;
}

int32 InitCallback(void *systemData, void *userData)
{
	if ((systemData) && (userData))
	{
		ExampleUI* ui = (ExampleUI*) userData;

		OpenIabInitResponse* ir = (OpenIabInitResponse*)systemData;
		if (ir->error)
		{
			string str = string_format("Init failed: %d, %s", (int)ir->status, ir->error);
			ui->Log(str);
		}
		if (ir->status == OPENIAB_RESPONSE_RESULT_OK)
		{
			ui->Log("Init succeded");
			ui->EnableShopButtons(true);
            ui->initButton()->SetAttribute("enabled", false);

            const char** skuList = new const char*[3];
            skuList[0] = hatSku;
            skuList[1] = coinSku;
            skuList[2] = subSku;
            OpenIabSkuDetails** sdList = getSkuListDetails(skuList, 3);
            for (int i = 0; i < 3; ++i)
            {
                ui->Log(string_format("SkuDetails: %s", sdList[i]->productId));
            }

			int count = getPurchasesCount();
			OpenIabPurchase** purchaseList = getPurchases();
			for (int i = 0; i < count; ++i)
			{
				ui->Log(string_format("Purchase: %s", purchaseList[i]->productId));
			}
		}
	}
	return true;
}

int32 PurchaseCallback(void *systemData, void *userData)
{
	if ((systemData) && (userData))
	{
		ExampleUI* ui = (ExampleUI*) userData;
		
		OpenIabPurchaseResponse* pr = (OpenIabPurchaseResponse*)systemData;
		string str;
		if (pr->error) {
			str = string_format("Purchase returned : %d, %s", (int)pr->status, pr->error);
			ui->Log(str);
		}
		if (pr->status == OPENIAB_RESPONSE_RESULT_OK)
		{
			OpenIabPurchase *p = pr->purchase;
			ui->Log(string_format("orderID				: %s", p->orderId));
			ui->Log(string_format("packageName			: %s", p->packageName));
			ui->Log(string_format("productId			: %s", p->productId));
			ui->Log(string_format("purchaseTime			: %d", p->purchaseTime));
			ui->Log(string_format("purchaseState		: %d", p->purchaseState));
			ui->Log(string_format("purchaseToken		: %s", p->purchaseToken));				
			ui->Log(string_format("developerPayload		: %s", p->developerPayload));				
			ui->Log(string_format("originalJson			: %s", p->originalJson));
			ui->Log(string_format("signature			: %s", p->signature));
			ui->Log(string_format("appstore			    : %s", p->appstoreName));

			if (strcmp(hatSku, p->productId) == 0)
			{
				ui->nonConsButton()->SetAttribute("enabled", false);
			} 
			else if (strcmp(coinSku, p->productId) == 0)
			{
				ui->SetConsumableText(coinSku);
				ui->consButton()->SetAttribute("enabled", false);
                ui->consumeButton()->SetAttribute("enabled", true);
				ui->Log("Attempting to consume Coin");
			} 
			else if (strcmp(subSku, p->productId) == 0) 
			{
				ui->subsButton()->SetAttribute("enabled", false);
			}
		}
	}
	return true;
}

int32 ConsumeCallback(void *systemData,void *userData)
{
	if ((systemData) && (userData))
	{
		ExampleUI* ui = (ExampleUI*) userData;
		OpenIabConsumeResponse* cr = (OpenIabConsumeResponse*)systemData;
		string str;
		if (cr->error) {
			str = string_format("Purchase returned : %d, %s", (int)cr->status, cr->error);
			ui->Log(str);
		}
		if (cr->status == OPENIAB_RESPONSE_RESULT_OK) {
			ui->SetConsumableText("None");
		}
		ui->consButton()->SetAttribute("enabled", true);
        ui->consumeButton()->SetAttribute("enabled", false);
	}
	return true;
}

int main()
{
	// Create UI
	ExampleUI *ui = new ExampleUI(); 

    // Register callbacks and pass in our UI pointer
    openiabRegister(OPENIAB_INIT_CALLBACK, InitCallback, ui);
    openiabRegister(OPENIAB_PURCHASE_CALLBACK, PurchaseCallback, ui);
    openiabRegister(OPENIAB_CONSUME_CALLBACK, ConsumeCallback, ui);

    // Map SKUs
    mapSku(hatSku, openiabStoreNames()->GOOGLE, "sku_hat");
    mapSku(coinSku, openiabStoreNames()->GOOGLE, "sku_coin");
    mapSku(subSku, openiabStoreNames()->GOOGLE, "sku_subs");

	// App loop
	while (true)
	{
		ui->Update(); // update the UI
		s3eDeviceYield();
	}

	return 0;
}
