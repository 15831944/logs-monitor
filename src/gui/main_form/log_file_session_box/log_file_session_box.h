#pragma once
#include "module/file_manager/file_instance.h"
#include "gui/main_form/log_file_session_box/keyword_item.h"

using namespace ui;

class KeywordInfo;

class LogFileSessionBox : public VBox
{
public:
	LogFileSessionBox();
	~LogFileSessionBox();

	void InitControl(const std::wstring& file_path, ListBox* log_file_list);
	void Clear();

	void StartCapture();
	void StopCapture();

	bool OnClicked(EventArgs* msg);
	void OnFileChangeCallback(const std::wstring& log_file, const std::string& data, bool append = true);

	static const LPCTSTR kFindString;
	static const LPCTSTR kReplaceString;

private:
	bool AddKeyword();

private:
	RichEdit*	keyword_input_ = nullptr;
	RichEdit*	log_content_ = nullptr;
	ListBox*	keyword_list_ = nullptr;
	Button*		keyword_add_ = nullptr;

	// data
	std::shared_ptr<FileInstance>	log_instance_;			// �ļ�״̬����
	std::vector<KeywordItem*>		keyword_filter_list_;
};