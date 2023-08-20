//------------------------------------------------------------------------------------------------
//! Categories of prefabs and/or nested categories of prefabs.
[BaseContainerProps()]
class SCR_LAGPrefabsCategory : SCR_LAGBasePrefabCategory
{
	[Attribute("", UIWidgets.EditBox, "World Editor display name, has no real usage")]
    protected string m_sName;
	
	[Attribute("", UIWidgets.Object, "Categories of prefabs and/or nested categories of prefabs")]
	protected ref array<ref SCR_LAGBasePrefabCategory> m_aPrefabsCategories;
	
	//------------------------------------------------------------------------------------------------
	override void Spawn(int numberOfPrefabsToSpawn, SCR_LAGPrefabsSpawner prefabsSpawner, bool isRespawnMode = false)
	{
		prefabsSpawner.SpawnPrefabsCategories(numberOfPrefabsToSpawn, m_aPrefabsCategories);
	}
};