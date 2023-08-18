modded class SCR_NotificationSenderComponent : SCR_BaseGameModeComponent
{
	
	
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) 
	{
		
		//~ Should never be called for clients but just in case
		if (!GetGameMode().IsMaster())
		{
			return;
		}
		
		SCR_EditorManagerEntity editorManager;
		
		//~ Get editorManager if has any
		SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
		if (core)
		{
			 editorManager = core.GetEditorManager(playerId);
		}

		bool hasUnlimitedEditor = editorManager && !editorManager.IsLimited();
		
		//~ Is GM, Always show GM left notification even if kicked/banned to notify players that the GM has left
		if (hasUnlimitedEditor)
		{
			// Print("DZ::NSC::Entering hasUnlimitedEditor clause..", LogLevel.WARNING);
			SCR_NotificationsComponent.SendToEveryone(ENotification.PLAYER_LEFT, playerId);				
		}

		bool isKickedOrBanned = false;
		
		//~ Check if disconnect cause has message attached to it. If true: show kick/ban reason. If false: only show gm/player left
		if (m_PlayerKickReasonsConfig)
		{
			string groupId, reasonId;
			KickCauseGroup2 groupInt;
			int reasonInt;
			
			//~ Get disconnect message preset
			GetGame().GetFullKickReason(cause, groupInt, reasonInt, groupId, reasonId);
			SCR_ConfigurableDialogUiPreset preset = m_PlayerKickReasonsConfig.FindPreset(groupId + "_" + reasonId);
			
			//~ If has kick/Ban message it will send out a notification
			isKickedOrBanned = preset != null && !preset.m_sMessage.IsEmpty();
		}
		//~ No config
		else 
		{
			//Print("'SCR_NotificationSenderComponent' has no 'm_PlayerKickReasonsConfig'! Make sure it is added else it will never know if a player was kicked!", LogLevel.ERROR);
		}
		
		//~ Is kicked/banned. Will also send ban notification if for some reason there is a timeout attached even if there is no specific kick message
		if (isKickedOrBanned || timeout != 0)
		{
			//~ Player kicked 
			if (timeout == 0)
				SCR_NotificationsComponent.SendToEveryone(ENotification.PLAYER_KICKED, playerId, cause, timeout);	
			//~ Player banned
			else if (timeout < 0)
				SCR_NotificationsComponent.SendToEveryone(ENotification.PLAYER_BANNED_NO_DURATION, playerId, cause, timeout);
			//~ More then 0 so player has temp banned
			else
				SCR_NotificationsComponent.SendToEveryone(ENotification.PLAYER_BANNED, playerId, cause, timeout);
		}		
		//~ Is Not kicked/banned, is player and should send on leave notification. 
		else if (m_bShowPlayerLeftNotification && !hasUnlimitedEditor)
		{
			SCR_NotificationsComponent.SendToEveryone(ENotification.PLAYER_LEFT, playerId);
		}
	}
	
	protected override void OnControllableDestroyedHotfix(IEntity entity, IEntity instigator)
	{		
		//~	No entity destroyed
		if (!entity)
		{
			return;
		}
		
		int playerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(entity);
		if (playerId <= 0)
		{
			return;
		}
		
		
		int killerId;
		if (entity == instigator)
		{
			killerId = playerId;
		}
		else
		{
			killerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(instigator);
		} 
		

		SCR_EditableCharacterComponent killerEditableCharacterComponent;
		//~ If there is a killer and the killer is not the player itself
		if (instigator && killerId <= 0)
		{
			killerEditableCharacterComponent = SCR_EditableCharacterComponent.Cast(instigator.FindComponent(SCR_EditableCharacterComponent));
		
			//~ Killer was not character so get killer in vehicle
			if (!killerEditableCharacterComponent)
				killerEditableCharacterComponent = GetKillerFromVehicle(instigator, instigator.IsInherited(Vehicle));
			
			if (killerEditableCharacterComponent)
				killerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(killerEditableCharacterComponent.GetOwner());
		}

		bool playerIsPossessed = false; 
		bool killerIsPossessed = false; 
		
		//~ Check if player or killer where possessed
		SCR_PossessingManagerComponent possesionManager = SCR_PossessingManagerComponent.GetInstance();
		if (possesionManager)
		{
			playerIsPossessed = possesionManager.IsPossessing(playerId);
			
			if (killerId > 0)
			{
				killerIsPossessed = possesionManager.IsPossessing(killerId);
			}
			
		}
		
		
		
		//Death notification	
		//Suicide	
		if (playerId == killerId || !instigator)
		{			
			//Player Suicide
			if (!playerIsPossessed)
			{
				//DeadZoneCore.Log(DZ_LogType.KILLFEED, string.Format("DZ::KILLFEED::Player <%1> suicide with UID: %2", playerName, playerUID) ); 
				return;
			}
			
			//Possessed Suicide
			//DeadZoneCore.Log(DZ_LogType.KILLFEED, string.Format("DZ::KILLFEED::Player <%1> suicide with UID: %2", playerName, playerUID) ); 
			return;
		}			
		
		//If killed by other player
		if (killerId > 0)
		{
			
			//Player killed player
			if (!playerIsPossessed && !killerIsPossessed)
			{
				string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
				string killerUID = DeadZoneCore.GetPlayerUID(killerId);
				string playerUID = DeadZoneCore.GetPlayerUID(playerId);
				string killerName = GetGame().GetPlayerManager().GetPlayerName(killerId);
				string message = string.Format("DZ::KILLFEED::Player <%1>(UID:%2) killed by player <%3>(UID: %4)", playerName, playerUID, killerName, killerUID);
				DeadZoneCore.Log(DZ_LogType.KILLFEED, message ); 			
			}
			
		}
		
	}
	

}