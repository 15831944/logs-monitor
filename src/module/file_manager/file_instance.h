#pragma once
#include <thread>
#include "base/callback/callback.h"

typedef std::function<void(const std::string&, const std::string&)> FileChangedCallback;
typedef std::shared_ptr<std::thread> SharedPtrThread;

class FileInstance : public nbase::SupportWeakCallback
{
public:
	FileInstance(const std::wstring& file_path);
	~FileInstance();

	/**
	 * ���������߳�
	 */
	void StartCapture(FileChangedCallback cb);

	/**
	 * ��δ��ɣ�ֹͣ������߳�
	 */
	bool StopCapture();

	/**
	 * ���ļ����ݹ���
	 */
	bool ClearFile();

private:
	void CaptureFileThread();
	DWORD GetFileSizeBytes();

private:
	std::wstring		file_;						// �ļ�����·��
	SharedPtrThread		file_capture_thread_;		// �����ļ�������߳�
	FileChangedCallback file_changed_callback_;		// �ⲿ���ݽ����Ļص�����

	DWORD				last_file_size_ = 0;		// ��¼���һ�ζ�ȡ�ļ��ı�Ĵ�С��������ļ���λ�ã�
	DWORD				curr_file_size_ = 0;		// ��¼��ǰ�ļ�������ȡ���ļ�λ�ã������һ�ζԱȵó��Ƿ���Ҫ��ȡ������

	bool				file_first_load_ = true;	// �����Ƿ��ǵ�һ�δ��ļ��� load �����ļ�����
	bool				stop_capture_ = false;
};