class DZ_GlobalChatChannel : BaseChatChannel
{
	//------------------------------------------------------------------------------------------------
	override bool ProcessMessage(BaseChatComponent sender, string message, bool isAuthority)
	{
		int playerId = DeadZoneCore.GetPlayerIdFromChatComponent(sender);

		string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
		string uid = DeadZoneCore.GetPlayerUID(playerId);

		DeadZoneCore.Log(DZ_LogType.CHAT_MESSAGE, string.Format("DZ::CHAT::<ID:%1><%2>(UID: %3): %4",playerId, playerName, uid, message));
		
		
		return checkMessageContent(message);
		
	}
	
	

	//------------------------------------------------------------------------------------------------
	override bool IsDelivering(BaseChatComponent sender, BaseChatComponent receiver)
	{
		return true;
	}


	//------------------------------------------------------------------------------------------------
	override bool IsAvailable(BaseChatComponent sender)
	{

		SCR_PlayerController senderPC =SCR_PlayerController.Cast(sender.GetOwner());
		if (!senderPC)
		{
			return false;
		}
		
		
		return true;

	}
	
	bool checkMessageContent(string message)
	{
	
		array<string> myList = {
			"your address",
            "your ip",
            "shut up",
            "nobody asked",
            "retard",
            "retarded",
            "fuckwit",
            "moron",
            "your stupid",
			"eat shit",
            "you are stupid",
            "nigga",
            "nigger",
            "niga", 
            "niger",
            "asshole",
            "fck you",
            "fuck your mom", 
            "fuck you", 
            "fck your ass",
            "fuck your ass", 
            "fat fucks",
            "midget",
            "whore",
            "bitch",
            "biatch",
            "tranny",
            "nazi", 
            "hitler", 
            "clit",
            "cunt", 
            "kys", 
            "pussy",
            "molested",
            "molest",
            "cock sucker",
            "cocksucker",
            "beaner",
            "jigaboo",
            "son of a bitch",
            "bitch",
            "hurensohn",
            "hurnsohn",
            "huren sohn",
            "hure",
            "sau",
            "hundesohn",
            "wixer",
            "wixxer",
            "arschloch",
            "depp",
            "drecksau",
			"negro",
			"pussies",
			"faggot",
			"virgin",
            "fick dich"

		};
		
		message.ToLower();
		foreach(string word: myList)
		{
			word.ToLower();
			if(message.Contains(word))
			{
				
		        SendWarningHint();   
				return false;
			}		
		}
		
		return true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void SendWarningHint()
    {
        SCR_HintManagerComponent.GetInstance().ShowCustomHint("We do not allow toxicty please watch what you say. any attempt to bypass will result in a ban.", "WARNING - BAD WORDING", 15.0);
        
    }
};