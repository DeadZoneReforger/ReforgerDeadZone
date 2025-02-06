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
	
	
	override void OnControllableDestroyed(notnull SCR_InstigatorContextData instigatorContextData)
	{
		IEntity entity = instigatorContextData.GetVictimEntity();
		
		//~	No entity destroyed or entity not a character
		if (!entity || !ChimeraCharacter.Cast(entity))
			return;
		
		bool isUnlimitedEditor = false;
		
		//~ Check if player has unlimited editor and if the editor is open
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		if (editorManager)
			isUnlimitedEditor = !editorManager.IsLimited();
		
		//~ Killfeed is disabled and unlimited editor is not open
		if (!isUnlimitedEditor && m_iKillFeedType == EKillFeedType.DISABLED)
			return;
		
		IEntity killerEntity = instigatorContextData.GetKillerEntity();
		
		int victimPlayerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(entity);
		
		//~ Entity is not a player so do not show notification
		if (victimPlayerId <= 0)
			return;
		
		int localPlayerID = SCR_PlayerController.GetLocalPlayerId();
		Faction localPlayerFaction = m_FactionManager.GetLocalPlayerFaction();
		
		//~ No local faction and player is not GM so don't show killfeed
		if (!localPlayerFaction && !isUnlimitedEditor)
			return;
		
		//~ Check if the player can show the notification of a player dying
		if (localPlayerFaction && !isUnlimitedEditor && m_iReceiveKillFeedType != EKillFeedReceiveType.ALL)
		{
			//~ No faction manager so don't show killfeed?
			if (!m_FactionManager)
				return;
			
			//~ Get victim faction
			Faction victimFaction;
			if (victimPlayerId > 0)
			{
				 victimFaction = m_FactionManager.GetPlayerFaction(victimPlayerId);
			}
			else if (entity)
			{
				SCR_FactionAffiliationComponent FactionAffiliation = SCR_FactionAffiliationComponent.Cast(entity.FindComponent(SCR_FactionAffiliationComponent));
				if (FactionAffiliation)
					victimFaction = FactionAffiliation.GetAffiliatedFaction();
			}
			
			//~ Is victim faction friendly to local faction
			SCR_Faction scrLocalPlayerFaction = SCR_Faction.Cast(localPlayerFaction);
			bool victimIsFriendly = (scrLocalPlayerFaction && scrLocalPlayerFaction.DoCheckIfFactionFriendly(scrLocalPlayerFaction)) || (!scrLocalPlayerFaction && localPlayerFaction.IsFactionFriendly(victimFaction));
			
			switch (m_iReceiveKillFeedType)
			{
				//~ check if in group
				case EKillFeedReceiveType.GROUP_ONLY :
				{
					//~ Check if local player is not the same as killed otherwise they are always in the same group
					if (localPlayerID != victimPlayerId)
					{
						//~ Factions not friendly so don't show killfeed
						if (!victimIsFriendly)
							return;
						
						//~ No group manager so don't send
						SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
						if (!groupManager)
							return;
						
						SCR_AIGroup localPlayerGroup = groupManager.GetPlayerGroup(localPlayerID);
						
						//~ If not in a group or not in the same group do not send
						if (!localPlayerGroup || !localPlayerGroup.IsPlayerInGroup(victimPlayerId))
							return;
					}
					
					break;
				}

				//~ Check if the same faction
				case EKillFeedReceiveType.SAME_FACTION_ONLY :
				{
					//~ Check if local player is not the same as killed otherwise they are always the same faction
					if (localPlayerID != victimPlayerId)
					{
						//~ If no local faction or if not the same faction do not show killfeed
						if (!localPlayerFaction || localPlayerFaction != victimFaction)
							return;
						
						//~ If Faction is hostile towards itself still do not show killfeed (Deathmatch)
						if (!victimIsFriendly)
							return;
					}
					
					break;
				}

				//~ Check if allies
				case EKillFeedReceiveType.ALLIES_ONLY :
				{
					//~ Check if local player is not the same as killed otherwise they are always allied
					if (localPlayerID != victimPlayerId)
					{
						//~ Factions not friendly so don't show killfeed
						if (!victimIsFriendly)
							return;
					}
					
					break;
				}

				//~ Check if enemies
				case EKillFeedReceiveType.ENEMIES_ONLY :
				{
					//~ If local player killed it is never an enemy
					if (localPlayerID == victimPlayerId)
						return;
					
					//~ Factions friendly so don't show killfeed
					if (victimIsFriendly)
						return;
					
					break;
				}
			}
		}
		
		SCR_ECharacterControlType victimControlType = instigatorContextData.GetVictimCharacterControlType();
		SCR_ECharacterControlType killerControlType = instigatorContextData.GetKillerCharacterControlType();
		
		int killerPlayerID =  instigatorContextData.GetKillerPlayerID();
		
		//~ Killed by GM so send GM notification
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_UNLIMITED_EDITOR))
		{
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
			{
				//~ Player killed by GM but GM not known
				if (killerPlayerID <= 0)
				{
					//SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_KILLED_BY_EDITOR, victimPlayerId);
					SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
				}
				//~ Player killed by GM and GM is known
				else 
					SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_KILLED_BY_EDITOR_PLAYER, victimPlayerId, killerPlayerID);
			}
			else 
			{
				//~ Possessed AI killed by GM but GM not known
				if (killerPlayerID <= 0)
				{
					//SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_KILLED_BY_EDITOR, victimPlayerId);
					SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
				}
				//~ Possessed AI killed by GM and GM is known
				else 
					SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_KILLED_BY_EDITOR_PLAYER, victimPlayerId, killerPlayerID);
			}
			
			return;
		}
		
		//~ Never show killer so simply show player died if limited editor
		if (!isUnlimitedEditor && m_iKillFeedType == EKillFeedType.UNKNOWN_KILLER)
		{
			//Player died
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
				SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
			//Possessed AI died
			else 
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_DIED, victimPlayerId);
			
			return;
		}
		
		//~ Death notification	
		//~ Suicide	
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.SUICIDE))
		{
			//Player Suicide
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
				SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
			//Possessed Suicide
			else 
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_DIED, victimPlayerId);
			
			return;
		}
		//If killed by other player or player that is has an unlimited editor
		if (killerControlType == SCR_ECharacterControlType.PLAYER || killerControlType == SCR_ECharacterControlType.UNLIMITED_EDITOR)
		{
			//////////////////////////
				IEntity victimEntity = instigatorContextData.GetVictimEntity();
		
				//~	No entity destroyed or entity not a character
				if (!victimEntity || !ChimeraCharacter.Cast(victimEntity))
					return;
			
				//int victimPlayerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(victimEntity);

				//IEntity killerEntity = instigatorContextData.GetKillerEntity();
		
				if (!killerEntity || !ChimeraCharacter.Cast(killerEntity))
					return;
			
				int killerPlayerId = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(victimEntity);

				//~ Entity is not a player so do not show notification
				if (killerPlayerId <= 0)
					return;
			
				string playerName = GetGame().GetPlayerManager().GetPlayerName(victimPlayerId);
				string killerUID = DeadZoneCore.GetPlayerUID(killerPlayerId);
				string playerUID = DeadZoneCore.GetPlayerUID(victimPlayerId);
				string killerName = GetGame().GetPlayerManager().GetPlayerName(killerPlayerId);
				string message = string.Format("DZ::KILLFEED::Player <%1>(UID:%2) killed by player <%3>(UID: %4)", playerName, playerUID, killerName, killerUID);
				DeadZoneCore.Log(DZ_LogType.KILLFEED, message ); 	
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
				SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_KILLED_PLAYER, killerPlayerID, victimPlayerId);
			else 
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.PLAYER_KILLED_POSSESSED_AI, killerPlayerID, victimPlayerId);
			
			return;
		}
		
		//~ Get RPL component if AI or possessed AI
		RplId killerRplId;
		if ((killerControlType == SCR_ECharacterControlType.AI || killerControlType == SCR_ECharacterControlType.POSSESSED_AI) && killerEntity)
		{
			SCR_EditableCharacterComponent editableCharacterComponent = SCR_EditableCharacterComponent.Cast(killerEntity.FindComponent(SCR_EditableCharacterComponent));
			if (editableCharacterComponent)
				killerRplId = Replication.FindId(editableCharacterComponent);
			
			//~ Invalid killer so show that player simply died
			if (!killerRplId.IsValid())
			{
				//Player died
				if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
					SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
				//Possessed AI died
				else 
					SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_DIED, victimPlayerId);
				
				return;
			}
		}
		
		//~ Killed by possessed AI
		if (killerControlType == SCR_ECharacterControlType.POSSESSED_AI)
		{
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
			{
				SCR_NotificationsComponent.SendLocalLimitedEditor(ENotification.AI_KILLED_PLAYER, killerRplId, victimPlayerId);
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_KILLED_PLAYER, killerPlayerID, victimPlayerId);
			}
			else 
			{
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_KILLED_POSSESSED_AI, killerPlayerID, victimPlayerId);
			}
			
			return;
		}
		//~ Killed by AI
		if (killerControlType == SCR_ECharacterControlType.AI)
		{
			if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
				SCR_NotificationsComponent.SendLocal(ENotification.AI_KILLED_PLAYER, killerRplId, victimPlayerId);
			else 
				SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.AI_KILLED_POSSESSED_AI, killerRplId, victimPlayerId);

			return;
		}
		
		//~ Unknown killer (Fallback)
		if (victimControlType != SCR_ECharacterControlType.POSSESSED_AI)
			SCR_NotificationsComponent.SendLocal(ENotification.PLAYER_DIED, victimPlayerId);
		else 
			SCR_NotificationsComponent.SendLocalUnlimitedEditor(ENotification.POSSESSED_AI_DIED, victimPlayerId);
	}

}
