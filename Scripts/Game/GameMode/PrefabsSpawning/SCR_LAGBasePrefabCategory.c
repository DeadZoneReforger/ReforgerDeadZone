//------------------------------------------------------------------------------------------------
//! Base class for Prefab Spawning.
[BaseContainerProps()]
class SCR_LAGBasePrefabCategory
{
	[Attribute("", UIWidgets.EditBox, "Ratio of how many prefabs to spawn in comparison to others prefab categories", "0 100")]
    protected int m_iRatio;
	
	int GetRatio()
	{
		return m_iRatio;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Interface for spawning prefabs, used in SCR_BasePrefabCategory Subclasses.
	void Spawn(int numberOfPrefabsToSpawn, SCR_LAGPrefabsSpawner prefabsSpawner, bool isRespawnMode = false)
	{
		Debug.Error("SpawnPrefab in SCR_BasePrefabCategory called. This is interface usage only, call it in SCR_BasePrefabCategory sublasses only. ");
	}
};