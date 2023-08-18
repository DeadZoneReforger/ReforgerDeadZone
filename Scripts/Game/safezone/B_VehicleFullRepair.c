class B_VehicleFullRepairClass : ScriptedGameTriggerEntityClass
{

}

class B_VehicleFullRepair: ScriptedGameTriggerEntity
{
	//------------------------------------------------------------------------------------------------
    // Override method for activation. Activates god mode for entity if it is of Vehicle type.
    override protected void OnActivate(IEntity ent)
    {
        if (Replication.IsClient())
        {
            return;
        }
        
        Vehicle character = Vehicle.Cast(ent);
        if(!character) 
        {
            
            return;
        }

        EnableGodMode(character);
    }

	//------------------------------------------------------------------------------------------------
    // Override method for deactivation. Deactivates god mode for entity if it is of Vehicle type.
    override void OnDeactivate(IEntity ent)
    {
        if (Replication.IsClient())
        {
            Print("Error: Attempting to deactivate on client side.");
            return;
        }
        
        Vehicle character = Vehicle.Cast(ent);
        if(!character) 
        {
            
            return;
        }

        DisableGodMode(character);
    }
	
	private void EnableGodMode(Vehicle character)
    {
        SCR_VehicleDamageManagerComponent dmgManager = SCR_VehicleDamageManagerComponent.Cast(character.FindComponent(SCR_VehicleDamageManagerComponent));
        if(!dmgManager) 
        {
            Print("DamageManagerComponent not found.", LogLevel.ERROR);
            return;
        }
        dmgManager.EnableDamageHandling(false);
        SCR_NotificationsComponent.SendToPlayer(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character), 1939);
    }

	//------------------------------------------------------------------------------------------------
    // Private method to disable god mode for SCR_Chimeraheli entity and send player a notification.
    private void DisableGodMode(Vehicle heli)
    {
        SCR_VehicleDamageManagerComponent dmgManager = SCR_VehicleDamageManagerComponent.Cast(heli.FindComponent(SCR_VehicleDamageManagerComponent));
        if(!dmgManager) 
        {
            Print("DamageManagerComponent not found.", LogLevel.ERROR);
            return;
        }
        dmgManager.EnableDamageHandling(true);
        SCR_NotificationsComponent.SendToPlayer(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(heli), 1940);
    }
}