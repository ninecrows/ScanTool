#include "stdafx.h"
#include "NativeHelpers.h"

public ref class NativeHelpers
{
public:
	static String^ GetFileId(String ^ myPath)
	{
		std::wstring path = msclr::interop::marshal_as<std::wstring>(myPath);

		std::wstring result;

		HANDLE myHandle = CreateFileW(path.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr);
		if (myHandle != INVALID_HANDLE_VALUE)
		{
			FILE_ID_INFO info;
			memset(&info, 0, sizeof(info));
			BOOL ok = GetFileInformationByHandleEx(myHandle, FileIdInfo, &info, sizeof(info));
			if (ok)
			{
				// Format the file identifier as a string.
				wchar_t fileId[64];
				memset(fileId, 0, sizeof(fileId));
				for (size_t ct = 0; ct < 16; ct++)
				{
					_snwprintf(fileId + (ct * 2), 3, L"%02x", info.FileId.Identifier[ct]);
				}

				wchar_t buffer[256];
				_snwprintf(buffer, 256, L">%llx:%s", info.VolumeSerialNumber, fileId);				

				result = buffer;
			}
			else
			{
				DWORD status = GetLastError();
				wchar_t buffer[64];
				_snwprintf(buffer, 64, L"*Failed to get file id*0x%08lx", status);
				result = buffer;
			}

			CloseHandle(myHandle);
		}
		else
		{
			DWORD status = GetLastError();
			wchar_t buffer[64];
			_snwprintf(buffer, 64, L"*Failed to open file*0x%08lx", status);
			result = buffer;
		}

		String^ out = gcnew String(result.c_str());
		return (out);
	}
};