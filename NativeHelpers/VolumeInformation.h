#pragma once
#include <string>

using namespace System;

inline void MarshalString(String ^ s, std::wstring& os) {
	using namespace Runtime::InteropServices;
	const wchar_t* chars =
		(const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

public ref class VolumeInformation
{
public:

	VolumeInformation(System::String^ myRootPath);
	

	inline
		System::String^
		SerialNumber()
	{
		return mySerialNumber;
	}

	inline
		System::String^
		VolumeName()
	{
		return myVolumeName;
	}

	inline
		System::String^
		FileSystemName()
	{
		return myFileSystemName;
	}

	inline
		DWORD
		Flags()
	{
		return myFlags;
	}

public:
		static array<String^>^ GetVolumeGuids();

		static array<String^>^ GetMountPoints(String^ volumeGuid);

private:
	System::String ^myVolumeName;
	System::String ^mySerialNumber;
	System::String ^myFileSystemName;
	DWORD myFlags;
	DWORD myStatus;
};

