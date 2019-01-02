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

	{
		WIN32_FILE_ATTRIBUTE_DATA info;
		BOOL ok(GetFileAttributesExW(path.c_str(),
			GetFileExInfoStandard,
			static_cast<void *>(&info)));

		System::DateTime^ creationDate;
		SYSTEMTIME systemtime;
		if (ok)
		{
			FileTimeToSystemTime(&(info.ftCreationTime), &systemtime);
			creationDate = gcnew System::DateTime((long long)(info.ftCreationTime.dwLowDateTime) | ((long long)(info.ftCreationTime.dwHighDateTime) << 32));
		}
		else
		{
			DWORD status = ::GetLastError();
			_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
		}
	}

	{
		wchar_t volumeName[1024];
		wchar_t fsName[1024];
		DWORD vsn;
		DWORD fsflags;
		BOOL ok(GetVolumeInformationW(path.c_str(),
			volumeName, 1024,
			&vsn,
			nullptr,
			&fsflags,
			fsName, 1024));

		if (ok)
		{

		}
		else
		{
			DWORD status = ::GetLastError();
			_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
		}
	}

	{
		ULARGE_INTEGER free;
		ULARGE_INTEGER total;
		ULARGE_INTEGER totalfree;
		BOOL ok(GetDiskFreeSpaceExW(path.c_str(), &free, &total, &totalfree));
		if (ok)
		{

		}
		else
		{
			DWORD status = ::GetLastError();
			_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
		}

		UINT type(GetDriveTypeW(path.c_str()));

		unsigned long long freeValue = free.QuadPart;

		_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "Type %lu", type);
	}

	{
		HANDLE handle = CreateFileW(path.c_str(),
			GENERIC_READ, // GENERIC_READ, GENERIC_WRITE
			FILE_SHARE_READ || FILE_SHARE_WRITE || FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if (handle != INVALID_HANDLE_VALUE)
		{
			FILE_BASIC_INFO info;
			BOOL ok(GetFileInformationByHandleEx(handle, FileBasicInfo, static_cast<void *>(&info), sizeof(info)));

			CloseHandle(handle);
		}
		else
		{
			DWORD status = ::GetLastError();
			_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu", status);
		}
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
		std::wstring &thisItem(volumes[item]);
		String^ thisVolume = gcnew String(thisItem.c_str(), 0, thisItem.size());
		results[item] = thisVolume;
	}

	return (results);
}

array<String^>^ VolumeInformation::GetMountPoints(String^ volumeGuid) 
{
	std::wstring path;
	MarshalString(volumeGuid, path);

	std::vector<std::wstring> chunks;

	DWORD bufferSize = 0;
	BOOL ok(GetVolumePathNamesForVolumeNameW(path.c_str(), nullptr, 0, &bufferSize));
	DWORD status(ERROR_SUCCESS);
	if (!ok)
	{
		status = ::GetLastError();
		wchar_t *strErrorMessage = nullptr;
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			status,
			0,
			(LPWSTR)&strErrorMessage,
			0,
			NULL);
		_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "VolumeInformation", "%lu \"%S\"", status, strErrorMessage);
		if (strErrorMessage != nullptr)
		{
			LocalFree(strErrorMessage);
		}
		

		// This is expected here as we pass in no buffer initially.
		if (status == ERROR_MORE_DATA)
		{
			wchar_t *buffer = new wchar_t[bufferSize + 8];

			BOOL ok(GetVolumePathNamesForVolumeNameW(path.c_str(), buffer, bufferSize, &bufferSize));

			{
				wchar_t *scan(buffer);

				size_t chunkSize(wcslen(scan));
				while (chunkSize > 0)
				{
					std::wstring thisChunk(scan);
					chunks.push_back(thisChunk);

					scan += chunkSize + 1;

					chunkSize = wcslen(scan);
				}
			}

			delete[] buffer;
		}
	}

	array<String^>^ result = gcnew array<String^>(chunks.size());

	for (size_t item = 0; item < chunks.size(); item++)
	{
		result[item] = gcnew String(chunks[item].c_str(), 0, chunks[item].length());
	}

	return result;
}