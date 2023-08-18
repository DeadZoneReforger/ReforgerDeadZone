modded class SCR_ChatPanel : ScriptedWidgetComponent
{
    
    //------------------------------------------------------------------------------------------------
    //! Returns style of this channel. If not resolved, returns system style.
    //! Override to handle DZ_GlobalChatChannel Type
    override protected SCR_ChatMessageStyle GetChannelStyle(BaseChatChannel channel)
    {
        if (channel == null)
            return m_SystemChannelStyle;
        
        SCR_ChatMessageStyle style;
        switch(channel.Type())
        {
            case BaseChatChannel:
            {
                if (channel.GetName() == "Global")
                {
                    style = m_GlobalChannelStyle;
                    break;
                }    
            }
            case DZ_GlobalChatChannel:
                style = m_GlobalChannelStyle;
                break;
            case GroupChatChannel:
                style = m_GroupChannelStyle;
                break;            
            case FactionChatChannel:
                style = m_FactionChannelStyle;
                break;
            case SCR_VehicleChatChannel:
                style = m_VehicleChannelStyle;
                break;
            case LocalChatChannel:
                style = m_LocalChannelStyle;
                break;
            case PrivateMessageChannel:
                style = m_DirectChannelStyle;
                break;
        }
        
        if (!style)
            return m_SystemChannelStyle;
        
        return style;
    }
};