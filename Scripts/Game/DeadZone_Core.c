enum DZ_LogType
{
	KILLFEED = 0,
	CHAT_MESSAGE = 1

}

//------------------------------------------------------------------------------------------------
class DeadZoneCore
{

	static ref map<int, string> m_aPlayerUUIDCache = new map<int, string>();
	static string m_sKillFeedLogPath = "";
	static string m_sChatLogPath = "";
	static const string BASE_LOG_PATH = "$profile:.db/d8feb926-1abb-11ee-be56-0242ac120002"; 
	static ref map<string, string> m_aChatMessages = new map<string, string>();
	static ref map<string, string> m_aKillFeeds = new map<string, string>();
	static const string PLAYER_UID_DEFAULT = "unknown";
	
	//------------------------------------------------------------------------------------------------
	static void InitLogFolders(string dateTimeUTC)
	{
		
		dateTimeUTC.Replace(":","_");
		dateTimeUTC.Replace(" ","_");
		dateTimeUTC.Replace("-","_");
		
		FileIO.MakeDirectory(string.Format("%1", BASE_LOG_PATH));
		
		string currentLogPath = string.Format("%1/%2", BASE_LOG_PATH, dateTimeUTC);
		FileIO.MakeDirectory(currentLogPath);
		
		m_sKillFeedLogPath = string.Format("%1/killfeed.json", currentLogPath);
		m_sChatLogPath = string.Format("%1/chat.json", currentLogPath);
		
		
	}
	
	//------------------------------------------------------------------------------------------------
	static void Log(DZ_LogType logType, string message)
	{
		string filePath = "";
		SCR_JsonSaveContext context = new SCR_JsonSaveContext();
		switch(logType)
		{
			case DZ_LogType.KILLFEED:
				filePath = m_sKillFeedLogPath;
				m_aKillFeeds.Insert(SCR_DateTimeHelper.GetTimeUTC(),message);
				context.WriteValue("killfeeds",m_aKillFeeds);
				break;
			case DZ_LogType.CHAT_MESSAGE:
				filePath = m_sChatLogPath;
				m_aChatMessages.Insert(SCR_DateTimeHelper.GetTimeUTC(),message);
				context.WriteValue("messages",m_aChatMessages);
				break;
		
		}
		
		context.SaveToFile(filePath);
		
	}
	
	
	//------------------------------------------------------------------------------------------------
	static string GetPlayerUID(int playerId)
	{
		if(playerId == 0)
		{
			return PLAYER_UID_DEFAULT;
		}
		
		string uuid = m_aPlayerUUIDCache.Get(playerId);
		if(uuid != "")
		{
			return uuid;
		}
		string resultUID = EPF_Utils.GetPlayerUID(playerId);
		if(!resultUID || resultUID == "")
		{
			resultUID = PLAYER_UID_DEFAULT;
		}
		m_aPlayerUUIDCache.Insert(playerId, resultUID);
		return resultUID;
		
		/*
		BackendApi api = GetGame().GetBackendApi();
		
		if (!api)
		{
			Print("DZ::Core.GetPlayerUID | Failed to get backend API!", LogLevel.ERROR);
			return "";
		};
		
	    
		string identityId = api.GetPlayerUID(playerId);
		if(!identityId || identityId == "")
		{
			return PLAYER_UID_DEFAULT;
		}
		
		m_aPlayerUUIDCache.Insert(playerId, identityId);
		
		return identityId;
		*/
	}
	
	//------------------------------------------------------------------------------------------------
	static int GetPlayerIdFromChatComponent(BaseChatComponent sender)
	{
		
		SCR_PlayerController senderPC = SCR_PlayerController.Cast(sender.GetOwner());
		IEntity player = senderPC.GetMainEntity();
		
		if(!player)
		{
			return 0; //TODO LAG 2023-08-04 Fix that !!
		}
		return GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
	}
	
	//------------------------------------------------------------------------------------------------
	static string GetPlayerUIDFromPlayerController(SCR_PlayerController senderPC)
	{
		if(!senderPC)
		{
			return PLAYER_UID_DEFAULT;
		}
		IEntity player = senderPC.GetMainEntity();
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		
		return GetPlayerUID(playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool HasPlayerValidUID(string playerUID)
	{
		//TODO LAG 2023-07-15 Remove return true --> testing scenario
		return true;
		
		return playerUID != PLAYER_UID_DEFAULT;
	}

};
