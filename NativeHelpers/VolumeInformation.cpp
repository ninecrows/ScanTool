#include "stdafx.h"
#include "VolumeInformation.h"

VolumeInformation::VolumeInformation(System::String ^myRootPath)
{
	std::wstring path = msclr::interop::marshal_as<std::wstring>(myRootPath);

	wchar_t volumeNameBuffer[MAX_PATH + 1];
	wchar_t serialNumberBuffer[64];
	wchar_t filesSystemName[MAX_PATH + 1];
	DWORD flags;
	DWORD status;
	DWORD serial;
	DWORD pathLength;

	BOOL ok = GetVolumeInformationW(path.c_str(),
		volumeNameBuffer, MAX_PATH + 1,
		&serial,
		&pathLength,
		&flags,
		filesSystemName, MAX_PATH + 1);
	if (ok)
	{
		myStatus = ERROR_SUCCESS;

		myVolumeName = gcnew System::String(volumeNameBuffer);
		myFileSystemName = gcnew System::String(filesSystemName);
		_snwprintf(serialNumberBuffer, 64, L"%08lx", serial);
		mySerialNumber = gcnew System::String(serialNumberBuffer);
	}
	else
	{
		myStatus = GetLastError();

		myVolumeName = nullptr;
		myFileSystemName = nullptr;
		mySerialNumber = nullptr;
	}
}
