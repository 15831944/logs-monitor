#include "stdafx.h"
#include "log_file_session_box.h"

const LPCTSTR LogFileSessionBox::kFindString = L"\r\r";
const LPCTSTR LogFileSessionBox::kReplaceString = L"\r";

LogFileSessionBox::LogFileSessionBox()
{

}

LogFileSessionBox::~LogFileSessionBox()
{

}

void LogFileSessionBox::InitControl(const std::wstring& file_path, ListBox* log_file_list)
{
	this->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&LogFileSessionBox::OnClicked, this, std::placeholders::_1));

	keyword_rich_edit_ = dynamic_cast<RichEdit*>(FindSubControl(L"keyword_input"));
	rich_edit_ = dynamic_cast<RichEdit*>(FindSubControl(L"log_edit"));
	btn_add_keyword_ = dynamic_cast<Button*>(FindSubControl(L"btn_add_keyword"));

	file_instance_.reset(new FileInstance(file_path.c_str()));
}

void LogFileSessionBox::Clear()
{
	rich_edit_->SetText(L"");
	file_instance_->ClearFile();
}

void LogFileSessionBox::StartCapture()
{
	file_instance_->StartCapture(nbase::Bind(&LogFileSessionBox::OnFileChangeCallback, this, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void LogFileSessionBox::StopCapture()
{
	file_instance_->StopCapture();
}

bool LogFileSessionBox::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"btn_add_keyword")
	{
		KeywordInfo keyword_info;
		keyword_info.SetTextColor(L"link_blue");
		keyword_info.SetKeyword(keyword_rich_edit_->GetText());
		keyword_filter_list_.push_back(keyword_info);

		keyword_rich_edit_->SetText(L"");
	}

	return true;
}

void LogFileSessionBox::OnFileChangeCallback(const std::wstring& log_file, const std::string& data, bool append /*= true*/)
{
	// Convert MBCS to UTF8
	std::wstring utf8_str;
	ui::StringHelper::MBCSToUnicode(data, utf8_str, CP_UTF8);

	// Replace \r\r to \r
	nbase::StringReplaceAll(kFindString, kReplaceString, utf8_str);

	// �ȼ�¼���йؼ��ֵ�λ��
	std::map<int, KeywordInfo> keywords_pos;
	for (auto keyword : keyword_filter_list_)
	{
		std::size_t begin = 0;
		while (begin < utf8_str.length())
		{
			std::size_t pos = utf8_str.find(keyword.GetKeyword(), begin);
			if (std::string::npos != pos)
			{
				// ���ֹؼ���
				std::wstring normal_text = utf8_str.substr(begin, pos - begin);
				std::wstring color_text = utf8_str.substr(pos, keyword.GetKeyword().length());

				// ��¼�ؼ���λ�ú�Ҫ��ȡ�Ĺؼ��ֳ��ȣ����뵽 map ��
				keywords_pos[pos] = keyword;
				begin = pos + keyword.GetKeyword().length();
			}
			else
			{
				break;
			}
		}
	}

	// �����ؼ���λ���б�����Ҫ��ɫ�����ֲ�ֽ��в���
	size_t last_keyword_pos = 0;
	for (auto keyword_pos : keywords_pos)
	{
		auto begin = keyword_pos.first;
		auto length = keyword_pos.second.GetKeyword().length();

		std::wstring normal_text = utf8_str.substr(last_keyword_pos, begin - last_keyword_pos);
		std::wstring color_text = utf8_str.substr(begin, length);

		rich_edit_->AppendText(normal_text);
		rich_edit_->AddColorText(color_text, L"link_blue");

		last_keyword_pos = begin + length;
	}

	// �������ؼ��ֺ�ʣ�������׷�ӵ�������
	if (last_keyword_pos < utf8_str.length())
	{
		rich_edit_->AppendText(utf8_str.substr(last_keyword_pos, utf8_str.length()));
	}

	rich_edit_->EndDown();
}
