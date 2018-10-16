#include "stdafx.h"
#include "resource.h"
#include "main_form.h"

#include "shared/modal_wnd/file_dialog_ex.h"
#include "shared/ui/msgbox.h"

#include "gui/main_form/log_file_list/log_file_item.h"

const LPCTSTR MainForm::kClassName = _T("main_form");
const LPCTSTR MainForm::kSkinFolder = _T("main_form");
const LPCTSTR MainForm::kSkinFile = _T("main_form.xml");

const LPCTSTR MainForm::kFindString = L"\r\r";
const LPCTSTR MainForm::kReplaceString = L"\r";

MainForm::MainForm()
{
}


MainForm::~MainForm()
{
}

std::wstring MainForm::GetSkinFolder()
{
	return kSkinFolder;
}

std::wstring MainForm::GetSkinFile()
{
	return kSkinFile;
}

std::wstring MainForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MainForm::GetWindowId() const
{
	return kClassName;
}

LRESULT MainForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
		{
			OnWndSizeMax(false);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			OnWndSizeMax(true);
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MainForm::InitWindow()
{
	// ���ô�����Ϣ
	SetIcon(IDI_LOGS_MONITOR);
	SetTaskbarTitle(L"Logs Monitor");

	m_pRoot->AttachBubbledEvent(kEventAll, nbase::Bind(&MainForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(kEventClick, nbase::Bind(&MainForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(kEventSelect, nbase::Bind(&MainForm::OnSelChanged, this, std::placeholders::_1));

	box_side_bar_		= dynamic_cast<Box*>(FindControl(L"box_side_bar"));
	list_logs_			= dynamic_cast<ListBox*>(FindControl(L"list_logs"));
	rich_edit_			= dynamic_cast<RichEdit*>(FindControl(L"log_edit"));
	box_container_		= dynamic_cast<HBox*>(FindControl(L"box_container"));

	btn_hide_loglist_	= dynamic_cast<Button*>(FindControl(L"btn_hide_loglist"));
	btn_show_loglist_	= dynamic_cast<Button*>(FindControl(L"btn_show_loglist"));
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MainForm::RemoveTrackFile(const std::wstring& file)
{
	capture_file_list_.erase(file);
}

void MainForm::OnFileSelected(BOOL result, std::wstring file_path)
{
	if (result)
	{
		// ����ļ��Ƿ��Ѿ�����
		auto capture_file_iter = capture_file_list_.find(file_path);
		if (capture_file_iter != capture_file_list_.end())
		{
			ShowMsgBox(GetHWND(), nullptr, L"����ļ��Ѿ��ڼ���б�����", false, L"��ʾ", false, L"ȷ��", false, L"", false);
			return;
		}

		// ������ӵ��ļ�������س�Ա
		std::shared_ptr<CaptureFileInfo> capture_file_info(new CaptureFileInfo);
		capture_file_info->file_instance_.reset(new FileInstance(file_path.c_str()));
		capture_file_info->rich_edit_ = new RichEdit;
		ui::GlobalManager::FillBoxWithCache(capture_file_info->rich_edit_, L"main_form/rich_edit_template.xml");

		// ��ԭ���б������м�ص� richedit ������Ϊ����
		for (auto capture_file_info : capture_file_list_)
		{
			capture_file_info.second->rich_edit_->SetVisible(false);
		}

		// ������ӽ����� richedit ��ӵ������в���ʾ
		box_container_->Add(capture_file_info->rich_edit_);

		// �½�һ���б���
		LogFileItem* item = new LogFileItem;
		item->InitControl(file_path, capture_file_info, list_logs_);

		// ��ʼ�����ļ����
		capture_file_info->file_instance_->StartCapture(nbase::Bind(&MainForm::OnLogFileChanged, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		capture_file_list_[file_path] = capture_file_info;
	}
}

void MainForm::OnLogFileChanged(const std::wstring& log_file, const std::string& data, bool append/* = true*/)
{
	auto capture_file_info = capture_file_list_.find(log_file);
	if (capture_file_info != capture_file_list_.end())
	{
		auto rich_edit = capture_file_info->second->rich_edit_;

		// Convert MBCS to UTF8
		std::wstring utf8_str;
		ui::StringHelper::MBCSToUnicode(data, utf8_str, CP_UTF8);

		// Replace \r\r to\r
		nbase::StringReplaceAll(kFindString, kReplaceString, utf8_str);

		if (append)
		{
			rich_edit->AppendText(utf8_str);
		}
		else
		{
			rich_edit->SetText(utf8_str);
		}
		rich_edit->EndDown();
	}
}

bool MainForm::Notify(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	return true;
}

bool MainForm::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"btn_new_file")
	{
		std::wstring file_type = L"��־�ļ� (*.*)";
		LPCTSTR filter = L"*.*";
		std::map<LPCTSTR, LPCTSTR> filters;
		filters[file_type.c_str()] = filter;

		CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&MainForm::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);

		CFileDialogEx* file_dlg = new CFileDialogEx();
		file_dlg->SetFilter(filters);
		file_dlg->SetMultiSel(TRUE);
		file_dlg->SetParentWnd(this->GetHWND());
		file_dlg->AyncShowOpenFileDlg(cb);
	}
	else if (name == L"btn_refresh_all")
	{
		for (auto capture_file_info : capture_file_list_)
		{
			capture_file_info.second->file_instance_->ClearFile();
			capture_file_info.second->rich_edit_->SetText(L"");
		}
	}
	else if (name == L"btn_remove_all")
	{
		for (auto capture_file_info : capture_file_list_)
		{
			// ���б������ɾ��
			capture_file_info.second->file_instance_->StopCapture();
			capture_file_info.second->rich_edit_->SetText(L"");
			capture_file_info.second->rich_edit_->SetVisible(false);
		}
		capture_file_list_.clear();
		list_logs_->RemoveAll();
	}
	else if (name == L"btn_hide_loglist")
	{
		box_side_bar_->SetVisible(false);
		btn_hide_loglist_->SetVisible(false);
		btn_show_loglist_->SetVisible(true);
	}
	else if (name == L"btn_show_loglist")
	{
		box_side_bar_->SetVisible(true);
		btn_hide_loglist_->SetVisible(true);
		btn_show_loglist_->SetVisible(false);
	}
	return true;
}

bool MainForm::OnSelChanged(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"log_file_item")
	{
		std::wstring item_data = msg->pSender->GetDataID();
		for (auto capture_file_info : capture_file_list_)
		{
			if (capture_file_info.first == item_data)
			{
				capture_file_info.second->rich_edit_->SetVisible(true);
				capture_file_info.second->rich_edit_->EndDown();
			}
			else
			{
				capture_file_info.second->rich_edit_->SetVisible(false);
			}
		}
	}
	return true;
}

void MainForm::OnWndSizeMax(bool max)
{
	if (!m_pRoot)
	{
		return;
	}

	FindControl(L"btn_wnd_max")->SetVisible(!max);
	FindControl(L"btn_wnd_restore")->SetVisible(max);
}
