class DEUS_SafeZoneClass: ScriptedGameTriggerEntityClass
{
    // This is a ScriptedGameTriggerEntityClass called DEUS_SafeZoneClass.
}

class DEUS_SafeZone: ScriptedGameTriggerEntity
{
	//------------------------------------------------------------------------------------------------
    // Override method for activation. Activates god mode for entity if it is of SCR_ChimeraCharacter type.
    override protected void OnActivate(IEntity ent)
    {
        if (Replication.IsClient())
        {
            return;
        }
        
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
        if(!character) 
        {

            return;
        }

        EnableGodMode(character);
    }

	//------------------------------------------------------------------------------------------------
    // Override method for deactivation. Deactivates god mode for entity if it is of SCR_ChimeraCharacter type.
    override void OnDeactivate(IEntity ent)
    {
        if (Replication.IsClient())
        {
            Print("Error: Attempting to deactivate on client side.");
            return;
        }
        
        SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(ent);
        if(!character) 
        {
            
            return;
        }

        DisableGodMode(character);
    }

	//------------------------------------------------------------------------------------------------
    // Private method to enable god mode for SCR_ChimeraCharacter entity and send player a notification.
    private void EnableGodMode(SCR_ChimeraCharacter character)
    {
        SCR_DamageManagerComponent dmgManager = SCR_DamageManagerComponent.Cast(character.FindComponent(SCR_DamageManagerComponent));
        if(!dmgManager) 
        {
            Print("DamageManagerComponent not found.", LogLevel.ERROR);
            return;
        }
        dmgManager.EnableDamageHandling(false);
        SCR_NotificationsComponent.SendToPlayer(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character), 1939);
    }

	//------------------------------------------------------------------------------------------------
    // Private method to disable god mode for SCR_ChimeraCharacter entity and send player a notification.
    private void DisableGodMode(SCR_ChimeraCharacter character)
    {
        SCR_DamageManagerComponent dmgManager = SCR_DamageManagerComponent.Cast(character.FindComponent(SCR_DamageManagerComponent));
        if(!dmgManager) 
        {
            Print("DamageManagerComponent not found.", LogLevel.ERROR);
            return;
        }
        dmgManager.EnableDamageHandling(true);
        SCR_NotificationsComponent.SendToPlayer(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character), 1940);
    }
}