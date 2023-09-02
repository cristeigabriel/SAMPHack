static constexpr DWORD g_chatContext = 0x26EB80;

template<typename... Args>
inline void CData::WriteToChat(const char* fmt, Args&& ...args) const
{
	DWORD dwChatContext = *(DWORD*)((DWORD)(m_hmSampDll)+g_chatContext);
	if (dwChatContext)
		m_fnWriteToChat(dwChatContext, fmt, FWD(args)...);
}
