#include "stdafx.h"
#include "log_file_item.h"
#include "module/util/windows_manager.h"

LogFileItem::LogFileItem()
{

}

LogFileItem::~LogFileItem()
{

}

void LogFileItem::InitControl(const std::wstring& file_path, LogFileSessionBox* log_file_session_box, ListBox* log_file_list)
{
	log_file_session_box_ = log_file_session_box;
	file_list_box_ = log_file_list;
	file_path_ = file_path;

	// ����һ�� list item �����ļ�����
	ui::GlobalManager::FillBoxWithCache(this, L"main_form/log_file_item.xml");

	// ���ÿؼ���ʽ�����Լ̳� ListContainerElement ����װ�ڲ�����
	Label* label_file_name = dynamic_cast<Label*>(FindSubControl(L"file_name"));
	label_file_name->SetText(PathFindFileName(file_path.c_str()));
	Label* label_file_path = dynamic_cast<Label*>(FindSubControl(L"file_path"));
	label_file_path->SetText(file_path);

	Button* btn_clear = dynamic_cast<Button*>(FindSubControl(L"clear"));
	btn_clear->AttachClick(nbase::Bind(&LogFileItem::OnClearFile, this, std::placeholders::_1));
	Button* btn_delete = dynamic_cast<Button*>(FindSubControl(L"delete"));
	btn_delete->AttachClick(nbase::Bind(&LogFileItem::OnRemoveItem, this, std::placeholders::_1));
	Button* btn_open_file = dynamic_cast<Button*>(FindSubControl(L"open_file"));
	btn_open_file->AttachClick(nbase::Bind(&LogFileItem::OnOpenFile, this, std::placeholders::_1));
	Button* btn_open_folder = dynamic_cast<Button*>(FindSubControl(L"open_folder"));
	btn_open_folder->AttachClick(nbase::Bind(&LogFileItem::OnOpenFolder, this, std::placeholders::_1));


	SetDataID(file_path);
	SetToolTipText(file_path);

	// ��� list item ���б���
	log_file_list->Add(this);
	log_file_list->SelectItem(log_file_list->GetCount() - 1);
}

void LogFileItem::ShowRichEdit(bool is_show/* = true*/)
{
	log_file_session_box_->SetVisible(is_show);
}

bool LogFileItem::OnClearFile(EventArgs* msg)
{
	log_file_session_box_->Clear();
	return true;
}

bool LogFileItem::OnRemoveItem(EventArgs* msg)
{
	// �ڼ���б���ɾ��
	MainForm* main_form = dynamic_cast<MainForm*>(nim_comp::WindowsManager::GetInstance()->GetWindow(MainForm::kClassName, MainForm::kClassName));
	main_form->RemoveTrackFile(this->GetDataID());

	// ���б������ɾ��
	log_file_session_box_->StopCapture();
	log_file_session_box_->SetVisible(false);
	file_list_box_->Remove(this);

	// ����������ļ�����ʾ�����ļ��� richedit
	if (file_list_box_->GetCount() > 0)
	{
		LogFileItem* item = dynamic_cast<LogFileItem*>(file_list_box_->GetItemAt(0));
		item->ShowRichEdit();
		file_list_box_->SelectItem(0);
	}

	return true;
}

bool LogFileItem::OnOpenFile(EventArgs* msg)
{
	nbase::ThreadManager::PostTask(kThreadGlobalMisc, nbase::Bind(&shared::tools::SafeOpenUrlEx, nbase::UTF16ToUTF8(file_path_), SW_SHOW));
	return true;
}

bool LogFileItem::OnOpenFolder(EventArgs* msg)
{
	std::string file_path;
	shared::FilePathApartDirectory(nbase::UTF16ToUTF8(file_path_).c_str(), file_path);
	nbase::ThreadManager::PostTask(kThreadGlobalMisc, nbase::Bind(&shared::tools::SafeOpenUrlEx, file_path, SW_SHOW));
	return true;
}
