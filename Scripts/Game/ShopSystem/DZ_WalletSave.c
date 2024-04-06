
[EPF_ComponentSaveDataType(ADM_CurrencyComponent), BaseContainerProps()]
class TAG_MyCustomComponentSaveDataClass : EPF_ComponentSaveDataClass
{
};

[EDF_DbName.Automatic()]
class TAG_MyCustomComponentSaveData : EPF_ComponentSaveData
{
    int m_iNumber;

    //------------------------------------------------------------------------------------------------
    override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
    {
        m_iNumber = ADM_CurrencyComponent.Cast(component).GetValue();
        return EPF_EReadResult.OK;
    }


    //------------------------------------------------------------------------------------------------
    override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
    {
        ADM_CurrencyComponent m_iNumber2 = ADM_CurrencyComponent.Cast(component);
        m_iNumber2.SetValue(m_iNumber);
        return EPF_EApplyResult.OK;
    }

    //------------------------------------------------------------------------------------------------
    override bool Equals(notnull EPF_ComponentSaveData other)
    {
        TAG_MyCustomComponentSaveData otherData = TAG_MyCustomComponentSaveData.Cast(other);
        return m_iNumber == otherData.m_iNumber;
    }
};