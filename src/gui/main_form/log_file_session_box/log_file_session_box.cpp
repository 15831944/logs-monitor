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
	GlobalManager::FillBoxWithCache(this, L"main_form/log_file_session_box.xml");

	this->AttachBubbledEvent(kEventClick, nbase::Bind(&LogFileSessionBox::OnClicked, this, std::placeholders::_1));

	keyword_list_	= dynamic_cast<ListBox*>(FindSubControl(L"keyword_list"));
	keyword_input_	= dynamic_cast<RichEdit*>(FindSubControl(L"keyword_input"));
	keyword_add_	= dynamic_cast<Button*>(FindSubControl(L"keyword_add"));
	log_content_	= dynamic_cast<RichEdit*>(FindSubControl(L"log_content"));

	log_instance_.reset(new FileInstance(file_path.c_str()));
}

void LogFileSessionBox::Clear()
{
	log_content_->SetText(L"");
	log_instance_->ClearFile();
}

void LogFileSessionBox::StartCapture()
{
	log_instance_->StartCapture(nbase::Bind(&LogFileSessionBox::OnFileChangeCallback, this, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void LogFileSessionBox::StopCapture()
{
	log_instance_->StopCapture();
}

void LogFileSessionBox::RemoveKeyword(KeywordItem* keyword_item)
{
	keyword_filter_list_.remove(keyword_item);
}

bool LogFileSessionBox::OnClicked(EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"keyword_add")
	{
		AddKeyword();
	}

	return true;
}

void LogFileSessionBox::OnFileChangeCallback(const std::wstring& log_file, const std::string& data, bool append /*= true*/)
{
	if (!append)
	{
		log_content_->SetText(L"");
	}

	// Convert MBCS to UTF8
	std::wstring utf8_str;
	ui::StringHelper::MBCSToUnicode(data, utf8_str, CP_UTF8);

	// Replace \r\r to \r
	nbase::StringReplaceAll(kFindString, kReplaceString, utf8_str);

	// �ȼ�¼���йؼ��ֵ�λ��
	std::map<int, KeywordItem*> keywords_pos;
	for (auto keyword : keyword_filter_list_)
	{
		std::size_t begin = 0;
		while (begin < utf8_str.length())
		{
			std::size_t pos = utf8_str.find(keyword->GetKeyword(), begin);
			if (std::string::npos != pos)
			{
				// ���ֹؼ���
				std::wstring normal_text = utf8_str.substr(begin, pos - begin);
				std::wstring color_text = utf8_str.substr(pos, keyword->GetKeyword().length());

				// ��¼�ؼ���λ�ú�Ҫ��ȡ�Ĺؼ��ֳ��ȣ����뵽 map ��
				keywords_pos[pos] = keyword;
				begin = pos + keyword->GetKeyword().length();
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
		auto length = keyword_pos.second->GetKeyword().length();

		std::wstring normal_text = utf8_str.substr(last_keyword_pos, begin - last_keyword_pos);
		std::wstring color_text = utf8_str.substr(begin, length);

		log_content_->AppendText(normal_text);
		log_content_->AddColorText(color_text, L"link_blue");

		last_keyword_pos = begin + length;
	}

	// �������ؼ��ֺ�ʣ�������׷�ӵ�������
	if (last_keyword_pos < utf8_str.length())
	{
		log_content_->AppendText(utf8_str.substr(last_keyword_pos, utf8_str.length()));
	}

	log_content_->EndDown();
}

bool LogFileSessionBox::AddKeyword()
{
	KeywordItem* item = new KeywordItem;
	item->InitControl(keyword_input_->GetText(), keyword_list_, this);
	keyword_filter_list_.push_back(item);
	keyword_input_->SetText(L"");

	return true;
}
