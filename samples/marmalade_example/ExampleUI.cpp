#include "IwDebug.h"
#include "s3e.h"
#include "IwNUI.h"
#include "ExampleUI.h"

const CColour COLOUR_GREY       (0xffC9C0BC);

bool ListBoxSelect(void *pData, CListBox* list, int item)
{
	list->SetAttribute("selected",0); // reset selection
	return true;
}

ExampleUI::ExampleUI()
{
    app = CreateApp();
    CWindowPtr window = CreateWindow();
    app->AddWindow(window);
    CViewPtr view = CreateView("canvas");  
    CViewPtr grayView = CreateView(CAttributes()
                                   .Set("name",        "grayView")
                                   .Set("x1",          0)
                                   .Set("y1",          0)
                                   .Set("width",       "100%")
                                   .Set("height",      "100%")
                                   .Set("backgroundColour", COLOUR_GREY)); 
    view->AddChild(grayView);   
	
	// buttons
    _initButton = CreateButton(CAttributes()
                                      .Set("name",    "initButton")
                                      .Set("caption", "Initialize OpenIAB")
                                      .Set("x1", "5%")
                                      .Set("y1", "5%")
                                      .Set("alignW",  "left")
									  .Set("visible", true));   
	_initButton->SetEventHandler("click", (void*)this, &OnInitClick);

	_consumeButton = CreateButton(CAttributes()
									.Set("name",    "consumeButton")
									.Set("caption", "Consume Coin")
									.Set("x1", "5%")
									.Set("y1", "15%")
									.Set("alignW", "left")
									.Set("enabled", false)
									.Set("visible", true));
	_consumeButton->SetEventHandler("click", (void*)this, &OnConsumeClick);

    _nonConsButton = CreateButton(CAttributes()
                                      .Set("name",    "nonConsButton")
                                      .Set("caption", "Purchase Hat")
                                      .Set("x1", "50%")
                                      .Set("y1", "5%")
                                      .Set("alignW",  "left")
									  .Set("enabled", false)
									  .Set("visible", true));  
	_nonConsButton->SetEventHandler("click", (void*)this, &OnPurchaseNonConsumableClick);
    
	_consButton = CreateButton(CAttributes()
                                      .Set("name",    "consButton")
                                      .Set("caption", "Purchase Coin")
                                      .Set("x1", "50%")
                                      .Set("y1", "15%")
                                      .Set("alignW",  "left")
									  .Set("enabled", false)
									  .Set("visible", true));      
	_consButton->SetEventHandler("click", (void*)this, &OnPurchaseConsumableClick);
	
	_subsButton = CreateButton(CAttributes()
                                      .Set("name",    "subsButton")
                                      .Set("caption", "Purchase Subscription")
                                      .Set("x1", "50%")
                                      .Set("y1", "25%")
                                      .Set("alignW",  "left")
									  .Set("enabled", false)
									  .Set("visible", true)); 
	_subsButton->SetEventHandler("click", (void*)this, &OnPurchaseSubscriptionClick);


	// text fields
    statusText = CreateLabel(CAttributes()
                            .Set("caption","")
                            .Set("x1", "5%")
							.Set("x2", "95%")
                            .Set("y1", "60%")
                            .Set("height","10%")
							.Set("alignW","left")
                            );      
	consumableText = CreateLabel(CAttributes()
                            .Set("caption","")
                            .Set("x1", "5%")
							.Set("x2", "95%")
                            .Set("y1", "50%")
                            .Set("height","10%")
							.Set("alignW","left")
                            ); 

	// log list box
	logText = CreateListBox(CAttributes()
                            .Set("x1", "5%")
							.Set("x2", "95%")
                            .Set("y1", "80%")
							.Set("alignW","left")
                            );   
	logText->SetEventHandler("selectitem",(void*)this,&ListBoxSelect);

	listBoxItems.AddString(CString("View Log"));
	logText->SetAttribute("listBoxItems",listBoxItems);

	view->AddChild(_initButton);
    view->AddChild(_consumeButton);
    view->AddChild(_nonConsButton);
    view->AddChild(_consButton);
    view->AddChild(_subsButton);
 
	view->AddChild(statusText);
	view->AddChild(consumableText);
    view->AddChild(logText);

	SetStatusText("");
    window->SetChild(view);   
    app->ShowWindow(window);  
}

void ExampleUI::SetStatusText(const string &msg)
{
	statusText->SetAttribute("caption",msg.c_str());
}

void ExampleUI::SetConsumableText(const string &msg)
{
	string str = "Consumable product ID : " + msg;
	consumableText->SetAttribute("caption",str.c_str());
}

void ExampleUI::Update()
{
	app->Update();
}

void ExampleUI::EnableShopButtons(bool enable)
{
	_nonConsButton->SetAttribute("enabled", enable);
	_consButton->SetAttribute("enabled",    enable);
	_subsButton->SetAttribute("enabled",    enable);
}

void ExampleUI::Log(const string &msg)
{
	IwTrace(OPENIAB_DEMO, (msg.c_str()));
	listBoxItems.AddString(CString(msg.c_str()));
	logText->SetAttribute("listBoxItems",listBoxItems);
	SetStatusText(msg);
}

// helper function
std::string string_format(const std::string fmt, ...) {
    int size = 100;
    std::string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}