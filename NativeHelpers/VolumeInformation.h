#pragma once
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

private:
	System::String ^myVolumeName;
	System::String ^mySerialNumber;
	System::String ^myFileSystemName;
	DWORD myFlags;
	DWORD myStatus;
};

