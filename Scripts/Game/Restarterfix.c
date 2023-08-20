modded class PREFIX_Restart extends ScriptedGameTriggerEntity
{
    
    override void OnInit(IEntity owner)
    {
        super.OnInit(owner);
        if (!(Replication.IsServer())) return;
        Print("PREFIX_Restart::EOnInit Restart in " + m_fShutdownAfter + "h", LogLevel.WARNING);
        GetGame().GetCallqueue().CallLater(PrepareMessage, HoursToMillis(m_fShutdownAfter) - MinutesToMillis(5), false, 5);
        GetGame().GetCallqueue().CallLater(PrepareMessage, HoursToMillis(m_fShutdownAfter) - MinutesToMillis(3), false, 3);
        GetGame().GetCallqueue().CallLater(PrepareMessage, HoursToMillis(m_fShutdownAfter) - MinutesToMillis(1), false, 1);
        GetGame().GetCallqueue().CallLater(Shutdown, HoursToMillis(m_fShutdownAfter));
    }
    
    void PrepareMessage(int minutes_left)
    {
        //NOTE: Make shure to place RLP component on map
        if (RplSession.Mode() == RplMode.Dedicated)
        {
            Rpc(SendHint, minutes_left);
            return;
        } 
        
        SendHint(minutes_left);      
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void SendHint(int minutes_left)
    {
        SCR_HintManagerComponent.GetInstance().ShowCustomHint("The Server will restart in " + minutes_left + " minutes. Make sure that you are ready", "SERVER RESTART", 10.0);
        Print("PREFIX_Restart::WarnPlayers warned all Players " + minutes_left + "minutes before restart");
    }
    
    override void Shutdown()
    {
        Print("PREFIX_Restart::Shutdown shutting down Server!", LogLevel.FATAL);
        GetGame().RequestClose();
    }
    
   override float MinutesToMillis(float minutes)
    {
        return minutes * 1000 * 60;
    }
    
    override float HoursToMillis(float hours)
    {
        return MinutesToMillis(hours) * 60;
    }
}
