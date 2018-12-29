#pragma once
public ref class Helpers
{
public:
	static System::String^ Helpers::GetFileId(System::String ^ myPath);
	static array<System::String^>^ Helpers::GetVolumeGuids();
};

