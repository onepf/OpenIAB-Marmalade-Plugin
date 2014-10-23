#ifndef MARMALADENUI_H
#define MARMALADENUI_H

#define WHISPER_BLOB

using namespace std;
using namespace IwNUI;

class ExampleUI
{
public:
	ExampleUI();
	void Update();
	void SetStatusText(const string &msg);
	void SetConsumableText(const string &msg);
	void Log(const string &msg);

	void EnableShopButtons(bool enable);

	inline CButtonPtr initButton() { return _initButton; }
	inline CButtonPtr consumeButton() { return _consumeButton; }
	inline CButtonPtr nonConsButton() { return _nonConsButton; }
	inline CButtonPtr consButton() { return _consButton; }
	inline CButtonPtr subsButton() { return _subsButton; }

private:
	CAppPtr app;
	CViewPtr view;
	CLabelPtr statusText;
	CLabelPtr consumableText;
	CListBoxPtr logText;
	CStringArray listBoxItems;

	CButtonPtr _initButton;
	CButtonPtr _consumeButton;
	CButtonPtr _nonConsButton;
	CButtonPtr _consButton;
	CButtonPtr _subsButton;
};

// helper function
extern std::string string_format(const std::string fmt, ...);

extern bool OnInitClick(void* data, CButton* button);
extern bool OnConsumeClick(void* data, CButton* button);
extern bool OnPurchaseNonConsumableClick(void* data, CButton* button);
extern bool OnPurchaseConsumableClick(void* data, CButton* button);
extern bool OnPurchaseSubscriptionClick(void* data, CButton* button);

#endif // defined MARMALADENUI_H