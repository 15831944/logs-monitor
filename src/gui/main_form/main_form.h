#pragma once
#include "util/window_ex.h"
#include "base/memory/singleton.h"
#include "module/file_manager/file_instance.h"

#include "gui/main_form/log_file_list/log_file_item.h"

using namespace ui;

class CaptureFileInfo;

typedef std::map<std::wstring, std::shared_ptr<CaptureFileInfo>> CaptureFileList;

class MainForm : public nim_comp::WindowEx
{
public:
    MainForm();
    ~MainForm();

    virtual std::wstring GetSkinFolder() override;
    virtual std::wstring GetSkinFile() override;
    virtual std::wstring GetWindowClassName() const override;
    virtual std::wstring GetWindowId() const override;

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

    virtual void InitWindow() override;

public:
	static const LPCTSTR kClassName;
	static const LPCTSTR kSkinFolder;
	static const LPCTSTR kSkinFile;

	static const LPCTSTR kFindString;
	static const LPCTSTR kReplaceString;

public:
	void RemoveTrackFile(const std::wstring& file);

private:
	void OnFileSelected(BOOL result, std::wstring file_path);
	void OnLogFileChanged(const std::wstring& log_file, const std::string& data, bool append = true);

private:
    bool Notify(EventArgs* msg);
    bool OnClicked(EventArgs* msg);
	bool OnSelChanged(EventArgs* msg);
	void OnWndSizeMax(bool max);

private:
	ListBox*		list_logs_;
	Button*			btn_hide_loglist_;
	Button*			btn_show_loglist_;
	Box*			box_container_;
	Box*			box_side_bar_;

	CaptureFileList	capture_file_list_;
};

class KeywordInfo
{
public:
	void SetTextColor(const std::wstring& color) { color_text_ = color; }
	std::wstring& GetTextColor() { return color_text_; }

	void SetFullLine(bool full_line) { full_line_ = full_line; }
	bool GetFullLine() { return full_line_; }

	void SetKeyword(const std::wstring& keyword) { keyword_ = keyword; }
	std::wstring& GetKeyword(){ return keyword_; }

private:
	std::wstring		keyword_;				// �ؼ���
	std::wstring		color_text_;			// �ؼ�����ɫ��ɫ�������� global ���Ѿ������
	bool				full_line_ = false;		// �Ƿ����һ����
};

class CaptureFileInfo
{
public:
	std::shared_ptr<FileInstance>			file_instance_;		// �ļ�״̬����
	std::vector<KeywordInfo>				keywords_filter_;	// �ؼ����б�
	RichEdit*								rich_edit_;			// �������ĸ��ı��ؼ�
};
