#pragma once

using namespace System;

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

private:
	System::String ^myVolumeName;
	System::String ^mySerialNumber;
	System::String ^myFileSystemName;
	DWORD myFlags;
	DWORD myStatus;
};

