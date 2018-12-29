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

array<String^>^ VolumeInformation::GetVolumeGuids()
{
	std::vector<std::wstring> volumes;

	size_t bufferSize(1024);
	wchar_t buffer[1024];
	HANDLE volumeSearch(FindFirstVolumeW(buffer, bufferSize));

	// If the search started then we should keep going until we run out of search hits and then close the search handle.
	if (volumeSearch != INVALID_HANDLE_VALUE)
	{
		// Grab this name and save it for later.
		volumes.push_back(buffer);

		// ...and keep running through the volumes list until we reach the end.
		while (FindNextVolumeW(volumeSearch, buffer, bufferSize))
		{
			volumes.push_back(buffer);
		}

		BOOL ok(FindVolumeClose(volumeSearch));
		if (!ok)
		{
			DWORD status = ::GetLastError();
			_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
		}
	}

	// For debugging purposes we'll snag the failure code.
	else
	{
		DWORD status = ::GetLastError();
		_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
	}

	// Create a new array that can contain the list of volume GUIDs that we found and load those strings up into that array.
	array<String^>^ results = gcnew array<String^>(volumes.size());
	for (size_t item = 0; item < volumes.size(); item++)
	{
		String^ thisVolume = gcnew String(volumes[item].c_str());
		results[item] = thisVolume;
	}

	return (results);
}